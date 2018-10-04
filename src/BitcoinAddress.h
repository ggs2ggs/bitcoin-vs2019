// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include "base58.h"
#include "base58string.h"
class CTxDestination;
class CChainParams;

/** base58-encoded Bitcoin addresses.
* Public-key-hash-addresses have version 0 (or 111 testnet).
* The data vector contains RIPEMD160(SHA256(pubkey)), where pubkey is the serialized public key.
* Script-hash-addresses have version 5 (or 196 testnet).
* The data vector contains RIPEMD160(SHA256(cscript)), where cscript is the serialized redemption script.
*/
// ※ CKeyID は排除。CKeyID 側に処理は移行済み.
class CBitcoinAddress
{
public:
    // bool Set(const CKeyID &id);
    bool IsValid() const;
    bool IsValid(const CChainParams& params) const;

    CBitcoinAddress() {}
    CBitcoinAddress(const base58string& strAddress)
    {
        m_addressString = strAddress;
    }

    CTxDestination Get() const;
    // bool GetKeyID(CKeyID& keyID) const;
    bool IsScript() const;

    bool operator<(const CBitcoinAddress& rhs) const { return this->m_addressString < rhs.m_addressString; }
    bool operator==(const CBitcoinAddress& rhs) const { return this->m_addressString == rhs.m_addressString; }

    bool SetBase58string(const base58string& str)
    {
        m_addressString = str;
        return IsValid();
    }

private:
    // CBitcoinAddress(const CScriptID &dest); // { Set(dest); }
    // CBitcoinAddress(const CKeyID &dest); // CKeyID はもう受け付けない。CKeyID 側で処理をする.
    // CBitcoinAddress(const CTxDestination &dest); // { Set(dest); }
    // bool Set(const CTxDestination &dest);
    // bool Set(const CScriptID &id);

public:
    friend class CTxDestination;

private:
    base58string m_addressString;
};
