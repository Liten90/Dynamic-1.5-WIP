// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Myriadcoin Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"

#include "crypto/common.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"

int GetAlgo(int nAlgorithm)
{
    switch (nAlgorithm & IDENTIFIER_ALGO)
    {
        case IDENTIFIER_ARGON2D_MIN:
            return ALGO_ARGON2D_MIN;
        case IDENTIFIER_ARGON2D_LOW:
            return ALGO_ARGON2D_LOW;
        case IDENTIFIER_ARGON2D_MEDIUM:
            return ALGO_ARGON2D_MEDIUM;
        case IDENTIFIER_ARGON2D_HIGH:
            return ALGO_ARGON2D_HIGH;
        case IDENTIFIER_ARGON2D_MAX:
            return ALGO_ARGON2D_MAX;
    }
    return ALGO_ARGON2D_MIN;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetPoWHash(GetAlgo()).ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i].ToString() << "\n";
    }
    return s.str();
}

uint256 CBlockHeader::GetPoWHash(int algo) const
{
	return hash_Argon2d(BEGIN(nVersion), END(nNonce), 1);
}

#ifdef __AVX2__

uint256 CBlockHeader::GetHashWithCtx(void *Matrix, int algo) const
{
	return(hash_Argon2d_ctx(UVOIDBEGIN(nVersion), Matrix, 1));
}

#endif
