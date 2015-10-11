#include <cryptopp/cryptlib.h>
using CryptoPP::BufferedTransformation;
using CryptoPP::BlockCipher;

#include <cryptopp/config.h>
using CryptoPP::lword;

#include <cryptopp/misc.h>
using CryptoPP::NotCopyable;

#include <cryptopp/secblock.h>
using CryptoPP::AlignedSecByteBlock;
using CryptoPP::FixedSizeSecBlock;

#include <cryptopp/smartptr.h>
using CryptoPP::member_ptr;

#include <cryptopp/osrng.h>
using CryptoPP::OS_GenerateRandomBlock;
using CryptoPP::RandomNumberGenerator;

#include <cryptopp/aes.h>
using CryptoPP::AES;

#include <cryptopp/sha.h>
using CryptoPP::SHA512;

class AES_RNG : public RandomNumberGenerator, public NotCopyable
{
public:
    explicit AES_RNG(const byte *seed = NULL, size_t length = 0)
    : m_pCipher(new AES::Encryption), m_keyed(EntropyHelper(seed, length))
    {
    }
    
    bool CanIncorporateEntropy() const
    {
        return true;
    }
    
    void IncorporateEntropy(const byte *input, size_t length)
    {
        m_keyed = EntropyHelper(input, length, false);
    }
    
    void GenerateIntoBufferedTransformation(BufferedTransformation &target, const std::string &channel, lword size)
    {
        if (!m_keyed) {
            m_pCipher->SetKey(m_key, m_key.size());
            m_keyed = true;
        }
        
        while (size > 0)
        {
            m_pCipher->ProcessBlock(m_seed);
            size_t len = std::min((size_t)16, (size_t)size);
            target.ChannelPut(channel, m_seed, len);
            size -= len;
        }
    }
    
protected:
    // Sets up to use the cipher. Its a helper to allow a throw
    //   in the contructor during initialization.  Returns true
    //   if the cipher was keyed, and false if it was not.
    bool EntropyHelper(const byte* input, size_t length, bool ctor = true)
    {
        if(ctor)
        {
            memset(m_key, 0x00, m_key.size());
            memset(m_seed, 0x00, m_seed.size());
        }
        
        // 32-byte key, 16-byte seed
        AlignedSecByteBlock seed(32 + 16);
        SHA512 hash;
        
        if(input && length)
        {
            // Use the user supplied seed.
            hash.Update(input, length);
        }
        else
        {
            // No seed or size. Use the OS to gather entropy.
            OS_GenerateRandomBlock(false, seed, seed.size());
            hash.Update(seed, seed.size());
        }
        
        hash.Update(m_key.data(), m_key.size());
        hash.TruncatedFinal(seed.data(), seed.size());
        
        memcpy(m_key.data(), seed.data() + 0, 32);
        memcpy(m_seed.data(), seed.data() + 32, 16);
        
        // Return false. This allows the constructor to complete
        //   initialization before the pointer m_pCipher is used.
        return false;
    }
    
private:
    FixedSizeSecBlock<byte, 32> m_key;
    FixedSizeSecBlock<byte, 16> m_seed;    
    member_ptr<BlockCipher> m_pCipher;
    bool m_keyed;
};
