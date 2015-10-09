#include "util.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
void hmac_init(HMAC_CTX *ctx, const char* key, size_t len)
{
    HMAC_CTX_init(ctx);
    HMAC_Init_ex(ctx, key, len, EVP_sha256(), NULL);
}

void hmac_update(HMAC_CTX *ctx, const char* bytes, size_t len)
{
    HMAC_Update(ctx, (unsigned char*)bytes, len);
}

void hmac_digest(HMAC_CTX *ctx, char* digest)
{
    unsigned int len;
    HMAC_Final(ctx, (unsigned char*)digest, &len);
}

void hmac_cleanup(HMAC_CTX *ctx)
{
    HMAC_CTX_cleanup(ctx);
}
#pragma GCC diagnostic pop
