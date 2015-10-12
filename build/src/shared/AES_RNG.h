#include <cryptopp/cryptlib.h>
#include <cryptopp/config.h>
#include <cryptopp/misc.h>
#include <cryptopp/secblock.h>
#include <cryptopp/smartptr.h>
#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/sha.h>
#include <cryptopp/smartptr.h>

class AES_RNG : public CryptoPP::RandomNumberGenerator, public CryptoPP::NotCopyable
{
public:
    explicit AES_RNG(const byte *seed = NULL, size_t length = 0);

    bool CanIncorporateEntropy() const;

    void IncorporateEntropy(const byte *input, size_t length);
    
    void GenerateIntoBufferedTransformation(CryptoPP::BufferedTransformation &target,
					    const std::string &channel, 
					    CryptoPP::lword size);
    
protected:
    // Sets up to use the cipher. Its a helper to allow a throw
    //   in the contructor during initialization.  Returns true
    //   if the cipher was keyed, and false if it was not.
    bool EntropyHelper(const byte* input, size_t length, bool ctor = true);
    
private:
    CryptoPP::FixedSizeSecBlock<byte, 32> m_key;
    CryptoPP::FixedSizeSecBlock<byte, 16> m_seed;    
    CryptoPP::member_ptr<CryptoPP::BlockCipher> m_pCipher;
    bool m_keyed;
};
