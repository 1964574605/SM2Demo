//
// Created by 19645 on 2022/10/16.
//

#ifndef SM2DEMO_CIPHERTEXT_H
#define SM2DEMO_CIPHERTEXT_H

#include "sm3.h"

#define SM2_MAX_PLAINTEXT_SIZE    255 // re-compute SM2_MAX_CIPHERTEXT_SIZE when modify


typedef struct {
    SM2_POINT point;
    uint8_t hash[32];
    uint8_t ciphertext_size;
    uint8_t ciphertext[SM2_MAX_PLAINTEXT_SIZE];
} SM2_CIPHERTEXT;

int sm2_do_encrypt_ex(const SM2_KEY *key, int flags, const uint8_t *in, size_t inlen, SM2_CIPHERTEXT *out);

int sm2_do_encrypt(const SM2_KEY *key, const uint8_t *in, size_t inlen, SM2_CIPHERTEXT *out);

int sm2_do_decrypt(const SM2_KEY *key, const SM2_CIPHERTEXT *in, uint8_t *out, size_t *outlen);

int asn1_length_to_der(size_t len, uint8_t **out, size_t *outlen) {
    if (len < 128) {
        if (out && *out) {
            *(*out)++ = (uint8_t) len;
        }
        (*outlen)++;

    } else {
        uint8_t buf[4];
        int i;

        PUTU32(buf, (uint32_t) len);
        if (len < 256) i = 1;
        else if (len < 65536) i = 2;
        else if (len < (1 << 24)) i = 3;
        else i = 4;

        if (out && *out) {
            *(*out)++ = 0x80 + i;
            memcpy(*out, buf + 4 - i, i);
            (*out) += i;
        }
        (*outlen) += 1 + i;
    }
    return 1;
}

int asn1_integer_to_der_ex(int tag, const uint8_t *a, size_t alen, uint8_t **out, size_t *outlen) {
    if (!outlen) {
        return -1;
    }

    if (!a) {
        return 0;
    }
    if (alen <= 0 || alen > INT_MAX) {
        return -1;
    }

    if (out && *out)
        *(*out)++ = tag;
    (*outlen)++;

    while (*a == 0 && alen > 1) {
        a++;
        alen--;
    }

    if (a[0] & 0x80) {
        asn1_length_to_der(alen + 1, out, outlen);
        if (out && *out) {
            *(*out)++ = 0x00;
            memcpy(*out, a, alen);
            (*out) += alen;
        }
        (*outlen) += 1 + alen;
    } else {
        asn1_length_to_der(alen, out, outlen);
        if (out && *out) {
            memcpy(*out, a, alen);
            (*out) += alen;
        }
        (*outlen) += alen;
    }

    return 1;
}

int sm2_kdf(const uint8_t *in, size_t inlen, size_t outlen, uint8_t *out) {
    SM3_CTX ctx;
    uint8_t counter_be[4];
    uint8_t dgst[SM3_DIGEST_SIZE];
    uint32_t counter = 1;
    size_t len;

    while (outlen) {
        PUTU32(counter_be, counter);
        counter++;

        sm3_init(&ctx);
        sm3_update(&ctx, in, inlen);
        sm3_update(&ctx, counter_be, sizeof(counter_be));
        sm3_finish(&ctx, dgst);

        len = outlen < SM3_DIGEST_SIZE ? outlen : SM3_DIGEST_SIZE;
        memcpy(out, dgst, len);
        out += len;
        outlen -= len;
    }

    memset(&ctx, 0, sizeof(SM3_CTX));
    memset(dgst, 0, sizeof(dgst));
    return 1;
}

int sm2_do_encrypt_ex(const SM2_KEY *key, int fixed_outlen, const uint8_t *in, size_t inlen, SM2_CIPHERTEXT *out) {
    cout << "-----encrypt begin-----" << endl;
    base k;
    JACOBIAN_POINT _P, *P = &_P;
    SM3_CTX sm3_ctx;
    uint8_t buf[64];
    int i;

    retry:
    // rand k in [1, n - 1]
    base_rand_range(k, N);
    if (base_is_zero(k)) goto retry;

    // C1 = k * G = (x1, y1)
    jacobian_point_mul_generator(P, k);

    cout << "C1 = k * G = (x1, y1), print(C1):" << endl;
    jacobian_print_xy(P);

    jacobian_point_to_bytes(P, (uint8_t *) &out->point);

    if (fixed_outlen) {
        size_t xlen = 0, ylen = 0;
        asn1_integer_to_der_ex(2, out->point.x, 32, NULL, &xlen);
        if (xlen != 34) goto retry;
        asn1_integer_to_der_ex(2, out->point.y, 32, NULL, &ylen);
        if (ylen != 34) goto retry;
    }

    // Q = k * P = (x2, y2)
    jacobian_point_from_bytes(P, (uint8_t *) &key->public_key);

    jacobian_point_mul(P, k, P);

    cout << "Q = k * P = (x2, y2), print(Q):" << endl;
    jacobian_print_xy(P);

    jacobian_point_to_bytes(P, buf);


    // t = KDF(x2 || y2, klen)
    sm2_kdf(buf, sizeof(buf), inlen, out->ciphertext);



    // C2 = M xor t
    for (i = 0; i < inlen; i++) {
        out->ciphertext[i] ^= in[i];
    }
    out->ciphertext_size = (uint32_t) inlen;

    // C3 = Hash(x2 || m || y2)
    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, buf, 32);
    sm3_update(&sm3_ctx, in, inlen);
    sm3_update(&sm3_ctx, buf + 32, 32);
    sm3_finish(&sm3_ctx, out->hash);

    cout << "-----encrypt over-----" << endl;
    return 1;
}

int sm2_do_encrypt(const SM2_KEY *key, const uint8_t *in, size_t inlen, SM2_CIPHERTEXT *out) {
    return sm2_do_encrypt_ex(key, 0, in, inlen, out);
}

int sm2_do_decrypt(const SM2_KEY *key, const SM2_CIPHERTEXT *in, uint8_t *out, size_t *outlen) {
    cout << "-----decrypt begin-----" << endl;
    uint32_t inlen;
    base d;
    JACOBIAN_POINT _P, *P = &_P;
    SM3_CTX sm3_ctx;
    uint8_t buf[64];
    uint8_t hash[32];
    int i;

    // FIXME: check SM2_CIPHERTEXT format

    // check C1
    jacobian_point_from_bytes(P, (uint8_t *) &in->point);
    //point_print(stdout, P, 0, 2);

    /*
    if (!sm2_jacobian_point_is_on_curve(P)) {
        fprintf(stderr, "%s %d: invalid ciphertext\n", __FILE__, __LINE__);
        return -1;
    }
    */

    // d * C1 = (x2, y2)
    base_from_bytes(d, key->private_key);
    jacobian_point_mul(P, d, P);
    //#define sm2_bn_clean(r) memset((r),0,sizeof(SM2_BN))

    memset((d), 0, sizeof(base));
    jacobian_point_to_bytes(P, buf);

    // t = KDF(x2 || y2, klen)
    if ((inlen = in->ciphertext_size) <= 0) {
        fprintf(stderr, "%s %d: invalid ciphertext\n", __FILE__, __LINE__);
        return -1;
    }

    sm2_kdf(buf, sizeof(buf), inlen, out);



    // M = C2 xor t
    for (i = 0; i < inlen; i++) {
        out[i] ^= in->ciphertext[i];
    }
    *outlen = inlen;

    // u = Hash(x2 || M || y2)
    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, buf, 32);
    sm3_update(&sm3_ctx, out, inlen);
    sm3_update(&sm3_ctx, buf + 32, 32);
    sm3_finish(&sm3_ctx, hash);

    // check if u == C3
    if (memcmp(in->hash, hash, sizeof(hash)) != 0) {
        fprintf(stderr, "%s %d: invalid ciphertext\n", __FILE__, __LINE__);
        return -1;
    }
    cout << "-----decrypt over-----" << endl;
    return 1;
}


#endif //SM2DEMO_CIPHERTEXT_H
