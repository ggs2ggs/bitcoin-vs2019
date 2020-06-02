﻿// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

struct CExtPubKey {
    unsigned char m_nDepth;
    unsigned char m_vchFingerprint[4];
    unsigned int m_nChild;
    ChainCode m_chaincode;
    CPubKey m_pubkey;

    friend bool operator==(const CExtPubKey &a, const CExtPubKey &b)
    {
        return a.m_nDepth == b.m_nDepth &&
            memcmp(&a.m_vchFingerprint[0], &b.m_vchFingerprint[0], sizeof(m_vchFingerprint)) == 0 &&
            a.m_nChild == b.m_nChild &&
            a.m_chaincode == b.m_chaincode &&
            a.m_pubkey == b.m_pubkey;
    }

    void Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const;
    void Decode(const unsigned char code[BIP32_EXTKEY_SIZE]);
    bool Derive(CExtPubKey& out, unsigned int nChild) const;

    void Serialize(CSizeComputer& s) const
    {
        // Optimized implementation for ::GetSerializeSize that avoids copying.
        s.seek(BIP32_EXTKEY_SIZE + 1); // add one byte for the size (compact int)
    }
    template <typename Stream>
    void Serialize(Stream& s) const
    {
        unsigned int len = BIP32_EXTKEY_SIZE;
        ::WriteCompactSize(s, len);
        unsigned char code[BIP32_EXTKEY_SIZE];
        Encode(code);
        s.write((const char *)&code[0], len);
    }
    template <typename Stream>
    void Unserialize(Stream& s)
    {
        unsigned int len = (unsigned int)::ReadCompactSize(s);
        unsigned char code[BIP32_EXTKEY_SIZE];
        if (len != BIP32_EXTKEY_SIZE)
            throw std::runtime_error("Invalid extended key size\n");
        s.read((char *)&code[0], len);
        Decode(code);
    }

    // CChainParams::EXT_PUBLIC_KEY
    base58string GetBase58stringWithNetworkExtPublicKeyPrefix() const;
};
