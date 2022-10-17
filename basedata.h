#ifndef BASEDATA
#define BASEDATA

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "tool.h"
#include <iostream>

using namespace std;

typedef uint64_t base[8];
#define GETU32(p) \
    ((uint32_t)(p)[0] << 24 | \
     (uint32_t)(p)[1] << 16 | \
     (uint32_t)(p)[2] <<  8 | \
     (uint32_t)(p)[3])

#define PUTU32(p, V) \
    ((p)[0] = (uint8_t)((V) >> 24), \
     (p)[1] = (uint8_t)((V) >> 16), \
     (p)[2] = (uint8_t)((V) >>  8), \
     (p)[3] = (uint8_t)(V))

static int hexchar2int(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    else if ('a' <= c && c <= 'f') return c - 'a' + 10;
    else if ('A' <= c && c <= 'F') return c - 'A' + 10;
    else return -1;
}

#define base_init(r)        base_set_zero(r)
#define base_clean(r)        base_set_zero(r)
#define base_set_zero(r)    base_copy((r), BASE_ZERO)
#define base_set_one(r)    base_copy((r), BASE_ONE)
#define base_is_zero(a)    (base_cmp((a), BASE_ZERO) == 0)
#define base_is_one(a)    (base_cmp((a), BASE_ONE) == 0)


const base BASE_ZERO = {0, 0, 0, 0, 0, 0, 0, 0};
const base BASE_ONE = {1, 0, 0, 0, 0, 0, 0, 0};
static const base BASE_TWO = {2, 0, 0, 0, 0, 0, 0, 0};
static const base BASE_THREE = {3, 0, 0, 0, 0, 0, 0, 0};
static const base BASE_FIVE = {5, 0, 0, 0, 0, 0, 0, 0};
const base BASE_MAX = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

int hex2bin(const char *in, size_t inlen, uint8_t *out) {
    int c;
    if (inlen % 2) {
        return -1;
    }

    while (inlen) {
        if ((c = hexchar2int(*in++)) < 0) {
            return -1;
        }
        *out = (uint8_t) c << 4;
        if ((c = hexchar2int(*in++)) < 0) {
            return -1;
        }
        *out |= (uint8_t) c;
        inlen -= 2;
        out++;
    }
    return 1;
}

int hex_to_bytes(const char *in, size_t inlen, uint8_t *out, size_t *outlen) {
    *outlen = inlen / 2;
    return hex2bin(in, inlen, out);
}

void base_to_bytes(const base a, uint8_t out[32]) {
    int i;
    for (i = 7; i >= 0; i--) {
        PUTU32(out, (uint32_t) a[i]);
        out += sizeof(uint32_t);
    }
}

void base_from_bytes(base r, const uint8_t in[32]) {
    int i;
    for (i = 7; i >= 0; i--) {
        r[i] = GETU32(in);
        in += sizeof(uint32_t);
    }
}

int base_from_hex(base r, const char hex[64]) {
    uint8_t buf[32];
    size_t len;
    if (hex_to_bytes(hex, 64, buf, &len) < 0) {
        return -1;
    }
    base_from_bytes(r, buf);
    return 1;
}

void base_to_hex(const base a, char hex[64]) {
    int i;
    for (i = 7; i >= 0; i--) {
        (void) sprintf(hex + 8 * (7 - i), "%08x", (uint32_t) a[i]);
        //hex += 8;
    }
}

void base_print(const char *prefix, const base a) {
    char hex[65] = {0};
    base_to_hex(a, hex);
    printf("%s%s\n", prefix, hex);
}

void base_to_bits(const base a, char bits[256]) {
    int i, j;
    for (i = 7; i >= 0; i--) {
        uint32_t w = a[i];
        for (j = 0; j < 32; j++) {
            *bits++ = (w & 0x80000000) ? '1' : '0';
            w <<= 1;
        }
    }
    bits[256] = '\0';
}

int base_cmp(const base a, const base b) {
    int i;
    for (i = 7; i >= 0; i--) {
        if (a[i] > b[i])
            return 1;
        if (a[i] < b[i])
            return -1;
    }
    return 0;
}


void base_copy(base r, const base a) {
    memcpy(r, a, sizeof(base));
}

void base_set_word(base r, uint32_t a) {
    base_set_zero(r);
    r[0] = a;
}

int base_rand_range(base r, const base range) {
    string random_str;
    do {
        random_str = getRandom(64);
        if (base_from_hex(r, random_str.data()) == -1) { cout << "random生成失败" << endl; }
    } while (base_cmp(r, range) >= 0);
    return 1;
}

int base_rand(base r, const base range) {
    uint8_t buf[256];
    do {
        for (size_t i = 0; i < 256; i++) {
            buf[i] = rand();
        }
        base_from_bytes(r, buf);
    } while (base_cmp(r, range) >= 0);
    return 1;
}

void base_format(base r, int max) {
    for (int i = 0; i < max; i++) {
        r[i] &= 0xffffffff;
    }
}

// r = a + b
void base_add(base r, const base a, const base b) {
    int i;
    r[0] = a[0] + b[0];
    for (i = 1; i < 8; i++) {
        r[i] = a[i] + b[i] + (r[i - 1] >> 32);
    }
    // 只保留后32位
    for (i = 0; i < 7; i++) {
        r[i] &= 0xffffffff;
    }
}


// ret = a-b
void base_sub(base ret, const base a, const base b) {
    int i;
    base r;
    // 从高位借1来做减法
    r[0] = ((uint64_t) 1 << 32) + a[0] - b[0];
    for (i = 1; i < 7; i++) {
        /* 初始从低位减去了1，所以表示为0xffffffff;
        (r[i - 1] >> 32) 如果低位借走的1没有用完就还回去 */
        r[i] = 0xffffffff + a[i] - b[i] + (r[i - 1] >> 32);
        // 只保留后32位
        r[i - 1] &= 0xffffffff;
    }
    // 给低位借1，低位没用完则换回来
    r[i] = a[i] - b[i] + (r[i - 1] >> 32) - 1;
    r[i - 1] &= 0xffffffff;
    base_copy(ret, r);
}

int base_equ(const base a, const base b) {
    int i;
    for (i = 0; i < 8; i++) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int get_max_n(const uint64_t a) {
    for (int i = 1; i <= 32; i++) {
        if (a >> i == 0) {
            return i;
        }
    }
    return 0;
}

int base_get_max_n(const base a) {
    for (int max = 7; max >= 0; max--) {
        if (a[max] != 0) {
            return max;
        }
    }
    return -1;
}

void base_div2n(base r[2], const base a[2], int n) {
    int max = base_get_max_n(a[1]);
    if (max >= 0) {
        max += 8;
    } else {
        max = base_get_max_n(a[0]);
    }
    base result[2];
    base_copy(result[0], BASE_ZERO);
    base_copy(result[1], BASE_ZERO);
    for (size_t i = 0; i <= max; i++) {
        result[i / 8][i % 8] =
                ((a[i / 8][i % 8] & 0xffffffff) >> n) + ((a[(i + 1) / 8][(i + 1) % 8] << (32 - n)) & 0xffffffff);
    }
    result[1][7] = (a[1][7] >> n) & 0xffffffff;
    base_copy(r[0], result[0]);
    base_copy(r[1], result[1]);
}

void base_mul2n(base r[2], const base a[2], int n) {
    int max = base_get_max_n(a[1]);
    if (max >= 0) {
        max += 8;
    } else {
        max = base_get_max_n(a[0]);
    }
    base result[2];
    base_copy(result[0], BASE_ZERO);
    base_copy(result[1], BASE_ZERO);
    for (size_t i = max; i > 0; i--) {
        result[i / 8][i % 8] = ((a[i / 8][i % 8] << n) & 0xffffffff) + (a[(i - 1) / 8][(i - 1) % 8] >> (32 - n));
    }
    result[0][0] = (a[0][0] << n) & 0xffffffff;
    base_copy(r[0], result[0]);
    base_copy(r[1], result[1]);
}

void base_castn(base r[2], const base a, int n) {
    base_copy(r[0], BASE_ZERO);
    base_copy(r[1], BASE_ZERO);
    int max = base_get_max_n(a);
    for (size_t i = 0; i <= max; i++) {
        r[(i + n) / 8][(i + n) % 8] = a[i];
    }
}

int base2_cmp(const base a[2], const base b[2]) {
    if (base_cmp(a[1], b[1]) > 0) {
        return 1;
    } else if (base_cmp(a[1], b[1]) < 0) {
        return -1;
    } else {
        return base_cmp(a[0], b[0]);
    }

}

void base_div(base r[2], base restR, const base a[2], const base b) {
    base result[2];
    int maxA = base_get_max_n(a[1]);
    if (maxA >= 0) {
        maxA += 8;
    } else {
        maxA = base_get_max_n(a[0]);
    }
    int maxB = base_get_max_n(b);
    int x = maxA - maxB;
    base rest[2];
    base_copy(rest[0], a[0]);
    base_copy(rest[1], a[1]);
    base_format(rest[0], 8);
    base_format(rest[1], 7);
    base_copy(restR, rest[0]);
    int ai = get_max_n(rest[maxA / 8][maxA % 8]);
    int bi = get_max_n(b[maxB]);
    int move = ai - bi;
    base bCast[2];
    base bpro;
    base_copy(bpro, b);
    base_format(bpro, 8);
    if (x >= 0) {
        base_castn(bCast, b, x);
    }
    if (x > 0 || (x == 0 && (move > 0 || move == 0 && base2_cmp(rest, bCast) >= 0))) {

        if (move > 0) {
            base_mul2n(bCast, bCast, move);
        } else if (move < 0) {
            base_div2n(bCast, bCast, -move);
        }
        if (base2_cmp(rest, bCast) < 0) {
            base_div2n(bCast, bCast, 1);
            move -= 1;
        }
        if (base_cmp(rest[0], bCast[0]) < 0) {
            base_sub(rest[1], rest[1], BASE_ONE);
            base_sub(rest[1], rest[1], bCast[1]);
            base_sub(bCast[0], BASE_MAX, bCast[0]);
            base_add(rest[0], rest[0], BASE_ONE);
            base_add(rest[0], rest[0], bCast[0]);
        } else {
            base_sub(rest[1], rest[1], bCast[1]);
            base_sub(rest[0], rest[0], bCast[0]);
        }
        base recurseR[2];
        base_copy(restR, rest[0]);
        if (base_cmp(rest[1], BASE_ZERO) > 0 || base_cmp(rest[0], BASE_ZERO) > 0) {
            base_div(recurseR, restR, rest, b);
        } else {
            base_copy(recurseR[0], BASE_ZERO);
            base_copy(recurseR[1], BASE_ZERO);
        }
        base cache[2];
        base_castn(cache, BASE_ONE, x);
        if (move > 0) {
            base_mul2n(cache, cache, move);
        } else if (move < 0) {
            base_div2n(cache, cache, -move);
        }
        base_copy(result[0], cache[0]);
        base_copy(result[1], cache[1]);
        base maxSubRec;
        base_sub(maxSubRec, BASE_MAX, recurseR[0]);
        if (base_cmp(maxSubRec, result[0]) < 0) {
            base_sub(result[0], result[0], maxSubRec);
            base_sub(result[0], result[0], BASE_ONE);
            base_add(result[1], result[1], recurseR[1]);
            base_add(result[1], result[1], BASE_ONE);
        } else {
            base_add(result[0], result[0], recurseR[0]);
            base_add(result[1], result[1], recurseR[1]);
        }
    } else {
        base_copy(result[0], BASE_ZERO);
        base_copy(result[1], BASE_ZERO);
    }
    base_format(result[0], 8);
    base_format(result[0], 7);
    base_copy(r[0], result[0]);
    base_copy(r[1], result[1]);
}

void base_div(base r[2], const base a[2], const base b) {
    base rest;
    base_div(r, rest, a, b);
}

void base_div(base r, base rest, const base a, const base b) {
    base newParam[2];
    base result[2];
    base_copy(newParam[0], a);
    base_copy(newParam[1], BASE_ZERO);
    base_div(result, rest, newParam, b);
    base_copy(r, result[0]);
}

void base_div(base r, const base a, const base b) {
    base newParam[2];
    base result[2];
    base_copy(newParam[0], a);
    base_copy(newParam[1], BASE_ZERO);
    base_div(result, newParam, b);
    base_copy(r, result[0]);
}

// r =  a mod b
void base_mod(base r, const base a[2], const base b) {
    base rest;
    base result[2];
    base_div(result, rest, a, b);
    base_copy(r, rest);
}

void base_mod(base r, const base a, const base b) {
    base newParam[2];
    base_copy(newParam[0], a);
    base_copy(newParam[1], BASE_ZERO);
    base_mod(r, newParam, b);
}

void base_mul(base r[2], const base a, const base b) {
    base s[2];
    uint64_t w;
    int i, j;

    // s = a * b
    for (i = 0; i < 16; i++) {
        s[i / 8][i % 8] = 0;
    }
    for (i = 0; i < 8; i++) {
        w = 0;
        for (j = 0; j < 8; j++) {
            int order = i + j;
            w += s[order / 8][order % 8] + a[i] * b[j];
            s[order / 8][order % 8] = w & 0xffffffff;
            w >>= 32;
        }
        int order = i + 8;
        s[order / 8][order % 8] = w;
    }
    base_copy(r[0], s[0]);
    base_copy(r[1], s[1]);
}

void base_mul_modn(base r, const base a, const base b, const base n) {
    base result[2];
    base_mul(result, a, b);
    base_mod(r, result, n);
}

void base_sub_mod(base r, const base a, const base b, const base mod) {
    base newA;
    base newB;
    base_mod(newA, a, mod);
    base_mod(newB, b, mod);
    if (base_cmp(newA, newB) < 0) {
        base modSubB;
        base_sub(modSubB, mod, newB);
        base_add(r, modSubB, newA);
    } else {
        base_sub(r, newA, newB);
    }
}

void base_mul_inv(base result, const base a, const base mod) {
    base r[3];
    base_copy(r[0], a);
    base_copy(r[1], mod);
    base_mod(r[2], a, mod);
    base s[3];
    base_copy(s[0], BASE_ONE);
    base_copy(s[1], BASE_ZERO);
    base_copy(s[2], BASE_ZERO);
    int i = 2;
    while (!base_equ(r[i], BASE_ZERO)) {
        base q;
        base_div(q, r[M(i - 2, 3)], r[M(i - 1, 3)]);
        base q_r;
        base_mul_modn(q_r, r[M(i - 1, 3)], q, mod);
        base_sub_mod(r[i], r[M(i - 2, 3)], q_r, mod);
        base q_s;
        base_mul_modn(q_s, s[M(i - 1, 3)], q, mod);
        base_sub_mod(s[i], s[M(i - 2, 3)], q_s, mod);
        if (base_equ(r[i], BASE_ZERO)) {
            break;
        }
        i = M(i + 1, 3);
    }
    base_mod(result, s[M(i - 1, 3)], mod);
}

void base_add_modn(base r, const base a, const base b, const base n) {
    base_add(r, a, b);
    if (base_cmp(r, n) >= 0) {
        base_sub(r, r, n);
    }
}

#endif