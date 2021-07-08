﻿// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include "base58.h"
class CTxDestination;

/** base58-encoded Bitcoin addresses.
* Public-key-hash-addresses have version 0 (or 111 testnet).
* The data vector contains RIPEMD160(SHA256(pubkey)), where pubkey is the serialized public key.
* Script-hash-addresses have version 5 (or 196 testnet).
* The data vector contains RIPEMD160(SHA256(cscript)), where cscript is the serialized redemption script.
*/
class CBitcoinAddress {
public:
    bool IsValid() const;
    bool IsValid(const CChainParams &params) const;

    CBitcoinAddress() {}
    CBitcoinAddress(const base58string& strAddress) { m_data.SetBase58string(strAddress); }

    CTxDestination Get() const;
    // bool GetKeyID(CKeyID &keyID) const;
    bool IsScript() const;

    bool operator <  (const CBitcoinAddress& rhs) const { return this->m_data <  rhs.m_data; }
    bool operator == (const CBitcoinAddress& rhs) const { return this->m_data == rhs.m_data; }

    bool SetBase58string(const base58string& str) { return m_data.SetBase58string(str) && IsValid(); }

public:
    friend class CTxDestination;

private:
    CBase58Data m_data;
};
