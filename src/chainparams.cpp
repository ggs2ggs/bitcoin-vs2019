// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "tinyformat.h"
#include "ChainParamsUtil.h"

static CChainParams* g_pCurrentParams = 0;

CChainParams::CChainParams(NetworkType networkType)
{
    m_networkType = networkType;
    CChainParamsPool::Instance()->RegisterParamsOfNetworkType(m_networkType, this);
}

const CChainParams &Params() {
    assert(g_pCurrentParams);
    return *g_pCurrentParams;
}

CChainParams& Params(NetworkType chain)
{
    CChainParams* p = CChainParamsPool::Instance()->GetParamsOfNetworkType(chain);
    if (!p) {
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
    }
    return *p;
}

void SelectParams(NetworkType network)
{
    SelectBaseParams(network);
    g_pCurrentParams = &Params(network);
}

// 明示的な参照により無理やりグローバル変数のインスタンス化を行わせる.
// これをやらないと特定のグローバル変数が未使用とみなされて最適化か何かの理由で存在が抹消されるっぽい.
// つまりコンストラクタが呼ばれない.
// 本当はこういうの書きたくないんだけど….
class CMainParams;
extern CMainParams g_mainParams;
auto p1 = &g_mainParams;

class CTestNetParams;
extern CTestNetParams g_testNetParams;
auto p2 = &g_testNetParams;

class CRegTestParams;
extern CRegTestParams g_regTestParams;
auto p3 = &g_regTestParams;
