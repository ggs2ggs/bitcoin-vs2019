// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KEY_H
#define BITCOIN_KEY_H

#include "pubkey.h"
#include "serialize.h"
#include "support/allocators/secure.h"
#include "uint256.h"

#include <stdexcept>
#include <vector>

class base58string;

/**
 * secp256k1:
 * const unsigned int PRIVATE_KEY_SIZE = 279;
 * const unsigned int PUBLIC_KEY_SIZE  = 65;
 * const unsigned int SIGNATURE_SIZE   = 72;
 *
 * see www.keylength.com
 * script supports up to 75 for single byte push
 */

/**
 * secure_allocator is defined in allocators.h
 * CPrivKey is a serialized private key, with all parameters included (279 bytes)
 */
typedef std::vector<unsigned char, secure_allocator<unsigned char> > CPrivKey;

// util
class CKeyUtil {
public:
    //! Check whether the 32-byte array pointed to be vch is valid keydata.
    bool static Check(const unsigned char* vch);
};

/** An encapsulated private key. */
class CKey
{
private:
    //! Whether this private key is valid. We check for correctness when modifying the key
    //! data, so fValid should always correspond to the actual state.
    bool m_fValid;

    //! Whether the public key corresponding to this private key is (to be) compressed.
    bool m_fCompressed;

    //! The actual byte data
    std::vector<unsigned char, secure_allocator<unsigned char> > m_keydata;

public:
    //! Construct an invalid private key.
    CKey() : m_fValid(false), m_fCompressed(false)
    {
        // Important: vch must be 32 bytes in length to not break serialization
        m_keydata.resize(32);
    }

    static CKey FromBase58string(const base58string& str);

    //! Destructor (again necessary because of memlocking).
    ~CKey()
    {
    }

    friend bool operator==(const CKey& a, const CKey& b)
    {
        return a.m_fCompressed == b.m_fCompressed &&
            a.size() == b.size() &&
            memcmp(a.m_keydata.data(), b.m_keydata.data(), a.size()) == 0;
    }

    //! Initialize using begin and end iterators to byte data.
    void SetBinary(const unsigned char* pbegin, const unsigned char* pend, bool fCompressedIn)
    {
        if (size_t(pend - pbegin) != m_keydata.size()) {
            m_fValid = false;
        } else if (CKeyUtil::Check(&pbegin[0])) {
            memcpy(m_keydata.data(), (unsigned char*)&pbegin[0], m_keydata.size());
            m_fValid = true;
            m_fCompressed = fCompressedIn;
        } else {
            m_fValid = false;
        }
    }

    //! Simple read-only vector-like interface.
    unsigned int size() const { return (m_fValid ? m_keydata.size() : 0); }
    const unsigned char* begin() const { return m_keydata.data(); }
    const unsigned char* end() const { return m_keydata.data() + size(); }

    //! Check whether this private key is valid.
    bool IsValid() const { return m_fValid; }

    //! Check whether the public key corresponding to this private key is (to be) compressed.
    bool IsCompressed() const { return m_fCompressed; }

    //! Initialize from a CPrivKey (serialized OpenSSL private key data).
    bool SetPrivKey(const CPrivKey& vchPrivKey, bool fCompressed);

    //! Generate a new private key using a cryptographic PRNG.
    void MakeNewKey(bool fCompressed);

    /**
     * Convert the private key to a CPrivKey (serialized OpenSSL private key data).
     * This is expensive.
     */
    CPrivKey GetPrivKey() const;

    base58string GetBase58stringWithNetworkSecretKeyPrefix() const;

    /**
     * Compute the public key from a private key.
     * This is expensive.
     */
    CPubKey GetPubKey() const;

    /**
     * Create a DER-serialized signature.
     * The test_case parameter tweaks the deterministic nonce.
     */
    bool Sign(const uint256& hash, std::vector<unsigned char>& vchSig, uint32_t test_case = 0) const;

    /**
     * Create a compact signature (65 bytes), which allows reconstructing the used public key.
     * The format is one header byte, followed by two times 32 bytes for the serialized r and s values.
     * The header byte: 0x1B = first key with even y, 0x1C = first key with odd y,
     *                  0x1D = second key with even y, 0x1E = second key with odd y,
     *                  add 0x04 for compressed keys.
     */
    bool SignCompact(const uint256& hash, std::vector<unsigned char>& vchSig) const;

    //! Derive BIP32 child key.
    bool Derive(CKey& keyChild, ChainCode &ccChild, unsigned int nChild, const ChainCode& cc) const;

    /**
     * Verify thoroughly whether a private key and a public key match.
     * This is done using a different mechanism than just regenerating it.
     */
    bool VerifyPubKey(const CPubKey& vchPubKey) const;

    //! Load private key and check that public key matches.
    bool Load(CPrivKey& privkey, CPubKey& vchPubKey, bool fSkipCheck);
};

/** Initialize the elliptic curve support. May not be called twice without calling ECC_Stop first. */
void ECC_Start(void);

/** Deinitialize the elliptic curve support. No-op if ECC_Start wasn't called first. */
void ECC_Stop(void);

/** Check that required EC support is available at runtime. */
bool ECC_InitSanityCheck(void);

#endif // BITCOIN_KEY_H
