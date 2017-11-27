// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Myriadcoin Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DYNAMIC_PRIMITIVES_BLOCK_H
#define DYNAMIC_PRIMITIVES_BLOCK_H

#include "hash.h"
#include "serialize.h"
#include "primitives/transaction.h"
#include "uint256.h"
#include "utilstrencodings.h"

/** Multi-Algo definitions used to encode algorithm in nVersion */
enum {
    ALGO_ARGON2D_MIN  	= 0,
    ALGO_ARGON2D_LOW   	= 1,
    ALGO_ARGON2D_MEDIUM = 2,
    ALGO_ARGON2D_HIGH   = 3,
    ALGO_ARGON2D_MAX    = 4,

    NUM_ALGOS_IMPL
};

const int NUM_ALGOS = 4;

enum
{
	IDENTIFIER_ARGON2D_MIN		 = 0x00,
    IDENTIFIER_ARGON2D_LOW       = 0x01,
    IDENTIFIER_ARGON2D_MEDIUM    = 0x02,
    IDENTIFIER_ARGON2D_HIGH      = 0x03,
    IDENTIFIER_ARGON2D_MAX       = 0x04,
    
    IDENTIFIER_ALGO
};

int GetAlgo(int nAlgorithm);

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
class CBlockHeader
{
public:
    // header
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
    uint32_t nAlgorithm;

    CBlockHeader()
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        READWRITE(nAlgorithm);
    }

    void SetNull()
    {
        nVersion = 0;
        hashPrevBlock.SetNull();
        hashMerkleRoot.SetNull();
        nTime = 0;
        nBits = 0;
        nNonce = 0;
        nAlgorithm = 0;
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const {
		return hash_Argon2d(BEGIN(nVersion), END(nNonce), 1);
	}
    
    uint256 GetPoWHash(int algo) const;
    
#ifdef __AVX2__
	uint256 GetHashWithCtx(void *Matrix, int algo) const;
#endif

    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }
   
    inline int GetAlgo() const
    {
        return ::GetAlgo(nAlgorithm);
    }
    
    /** Encode the algorithm into nVersion */
    inline void SetAlgo(int algo)
    {
        switch(algo)
        {
            case ALGO_ARGON2D_MIN:
				nAlgorithm |= IDENTIFIER_ARGON2D_MIN;
                break;
            case ALGO_ARGON2D_LOW:
                nAlgorithm |= IDENTIFIER_ARGON2D_LOW;
                break;
            case ALGO_ARGON2D_MEDIUM:
                nAlgorithm |= IDENTIFIER_ARGON2D_MEDIUM;
                break;
            case ALGO_ARGON2D_HIGH:
                nAlgorithm |= IDENTIFIER_ARGON2D_HIGH;
                break;
            case ALGO_ARGON2D_MAX:
                nAlgorithm |= IDENTIFIER_ARGON2D_MAX;
                break;
            default:
                break;
        }
	}
};


class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransaction> vtx;

    // memory only
    mutable CTxOut txoutDynode; // Dynode payment
    mutable std::vector<CTxOut> voutSuperblock; // superblock payment
    mutable bool fChecked;

    CBlock()
    {
        SetNull();
    }

    CBlock(const CBlockHeader &header)
    {
        SetNull();
        *((CBlockHeader*)this) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(*(CBlockHeader*)this);
        READWRITE(vtx);
    }

    void SetNull()
    {
        CBlockHeader::SetNull();
        vtx.clear();
        txoutDynode = CTxOut();
        voutSuperblock.clear();
        fChecked = false;
    }

    CBlockHeader GetBlockHeader() const
    {
        CBlockHeader block;
        block.nVersion       = nVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        block.nAlgorithm	 = nAlgorithm;
        return block;
    }

    std::string ToString() const;
};


/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
struct CBlockLocator
{
    std::vector<uint256> vHave;

    CBlockLocator() {}

    CBlockLocator(const std::vector<uint256>& vHaveIn)
    {
        vHave = vHaveIn;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    }

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull() const
    {
        return vHave.empty();
    }
};

#endif // DYNAMIC_PRIMITIVES_BLOCK_H
