﻿// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "BitcoinAddress.h"

/*
namespace
{
    class CBitcoinAddressVisitor : public boost::static_visitor<bool>
    {
    private:
        CBitcoinAddress* addr;

    public:
        CBitcoinAddressVisitor(CBitcoinAddress* addrIn) : addr(addrIn) {}

        bool operator()(const CKeyID& id) const { assert(0); return false; } // CKeyID 側に移行.
        bool operator()(const CScriptID& id) const { assert(0); return false; } // CScriptID 側に移行.
        bool operator()(const CNoDestination& no) const { return false; }
    };

} // anon namespace
*/

/*
bool CBitcoinAddress::Set(const CTxDestination& dest)
{
    return boost::apply_visitor(CBitcoinAddressVisitor(this), dest);
}
*/

bool CBitcoinAddress::IsValid() const
{
    return IsValid(Params());
}

bool CBitcoinAddress::IsValid(const CChainParams& params) const
{
    bool fCorrectSize = m_data.m_vchData.size() == 20;
    bool fKnownVersion = m_data.m_vchVersion == params.Base58Prefix(CChainParams::PUBKEY_ADDRESS) ||
        m_data.m_vchVersion == params.Base58Prefix(CChainParams::SCRIPT_ADDRESS);
    return fCorrectSize && fKnownVersion;
}

CTxDestination CBitcoinAddress::Get() const
{
    if (!IsValid())
        return CNoDestination();
    uint160 id;
    memcpy(&id, &m_data.m_vchData[0], 20);
    if (m_data.m_vchVersion == Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS))
        return CKeyID(id);
    else if (m_data.m_vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS))
        return CScriptID(id);
    else
        return CNoDestination();
}

// ####### これも CKeyID に移行する候補.
bool CBitcoinAddress::GetKeyID(CKeyID& keyID) const
{
    if (!IsValid() || m_data.m_vchVersion != Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS))
        return false;
    uint160 id;
    memcpy(&id, &m_data.m_vchData[0], 20);
    keyID = CKeyID(id);
    return true;
}

bool CBitcoinAddress::IsScript() const
{
    return IsValid() && m_data.m_vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS);
}
