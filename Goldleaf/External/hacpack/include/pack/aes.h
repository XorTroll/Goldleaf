#ifndef HACPACK_AES_H
#define HACPACK_AES_H

#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h"

/* Enumerations. */
typedef enum {
    AES_MODE_ECB = MBEDTLS_CIPHER_AES_128_ECB,
    AES_MODE_CTR = MBEDTLS_CIPHER_AES_128_CTR,
    AES_MODE_XTS = MBEDTLS_CIPHER_AES_128_XTS
} aes_mode_t;

typedef enum {
    AES_DECRYPT = MBEDTLS_DECRYPT,
    AES_ENCRYPT = MBEDTLS_ENCRYPT,
} aes_operation_t;

/* Define structs. */
typedef struct {
    mbedtls_cipher_context_t cipher_enc;
    mbedtls_cipher_context_t cipher_dec;
} aes_ctx_t;

/* Function prototypes. */
aes_ctx_t *new_aes_ctx(const void *key, unsigned int key_size, aes_mode_t mode);
void free_aes_ctx(aes_ctx_t *ctx);

void aes_setiv(aes_ctx_t *ctx, const void *iv, size_t l);

void aes_encrypt(aes_ctx_t *ctx, void *dst, const void *src, size_t l);
void aes_decrypt(aes_ctx_t *ctx, void *dst, const void *src, size_t l);

void aes_calculate_cmac(void *dst, void *src, size_t size, const void *key);

void aes_xts_encrypt(aes_ctx_t *ctx, void *dst, const void *src, size_t l, size_t sector, size_t sector_size);
void aes_xts_decrypt(aes_ctx_t *ctx, void *dst, const void *src, size_t l, size_t sector, size_t sector_size);

#endif
