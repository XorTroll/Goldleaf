#ifndef HACTOOL_RSA_H
#define HACTOOL_RSA_H

#include "mbedtls/rsa.h"

int rsa2048_pss_verify(const void *data, size_t len, const unsigned char *signature, const unsigned char *modulus);
int rsa2048_pkcs1_verify(const void *data, size_t len, const unsigned char *signature, const unsigned char *modulus);
int rsa2048_oaep_decrypt_verify(void *out, size_t max_out_len, const unsigned char *signature, const unsigned char *modulus, const unsigned char *exponent, size_t exponent_len, const unsigned char *label_hash, size_t *out_len);

#endif
