//
// Created by 19645 on 2022/10/16.
//

#ifndef SM2DEMO_ELLIPSE_H
#define SM2DEMO_ELLIPSE_H

#include "basedata.h"

typedef struct {
    base X;
    base Y;
    base Z;
} JACOBIAN_POINT;

const base P = {0xffffffff, 0xffffffff, 0x00000000, 0xffffffff,
                0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
};

const base N = {
        0x39d54123, 0x53bbf409, 0x21c6052b, 0x7203df6b,
        0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
};


const base A = {0, 0, 0, 0, 0, 0, 0, 0};

const base B = {
        0x4d940e93, 0xddbcbd41, 0x15ab8f92, 0xf39789f5,
        0xcf6509a7, 0x4d5a9e4b, 0x9d9f5e34, 0x28e9fa9e,
};//椭圆曲线的参数B

base Gx = {//基点G
        0x334c74c7, 0x715a4589, 0xf2660be1, 0x8fe30bbf,
        0x6a39c994, 0x5f990446, 0x1f198119, 0x32c4ae2c,
};
base Gy = {//基点G
        0x2139f0a0, 0x02df32e5, 0xc62a4740, 0xd0a9877c,
        0x6b692153, 0x59bdcee3, 0xf4f6779c, 0xbc3736a2,
};
base Gz = {1, 0, 0, 0, 0, 0, 0, 0};

const JACOBIAN_POINT SM2_G = {
        {
                0x334c74c7, 0x715a4589, 0xf2660be1, 0x8fe30bbf,
                                                       0x6a39c994, 0x5f990446, 0x1f198119, 0x32c4ae2c,
        },
        {
                0x2139f0a0, 0x02df32e5, 0xc62a4740, 0xd0a9877c,
                                                       0x6b692153, 0x59bdcee3, 0xf4f6779c, 0xbc3736a2,
        },
        {
                1,          0,          0,          0, 0,          0,          0,          0,
        },
};

void jacobian_point_init(JACOBIAN_POINT *R);

void jacobian_point_set_xy(JACOBIAN_POINT *R, const base x, const base y); // 应该返回错误
void jacobian_point_get_xy(const JACOBIAN_POINT *P, base x, base y);

void jacobian_point_neg(JACOBIAN_POINT *R, const JACOBIAN_POINT *P);

void jacobian_point_dbl(JACOBIAN_POINT *R, const JACOBIAN_POINT *P);

void jacobian_point_add(JACOBIAN_POINT *R, const JACOBIAN_POINT *P, const JACOBIAN_POINT *Q);

void jacobian_point_sub(JACOBIAN_POINT *R, const JACOBIAN_POINT *P, const JACOBIAN_POINT *Q);

void jacobian_point_mul(JACOBIAN_POINT *R, const base k, const JACOBIAN_POINT *P);

void jacobian_point_to_bytes(const JACOBIAN_POINT *P, uint8_t out[64]);

void jacobian_point_from_bytes(JACOBIAN_POINT *P, const uint8_t in[64]);

void jacobian_point_mul_generator(JACOBIAN_POINT *R, const base k);

void jacobian_point_mul_sum(JACOBIAN_POINT *R, const base t, const JACOBIAN_POINT *P,
                            const base s); // 应该返回错误
void jacobian_point_from_hex(JACOBIAN_POINT *P, const char hex[64 * 2]); // 应该返回错误

int jacobian_point_is_at_infinity(const JACOBIAN_POINT *P);

int jacobian_point_is_on_curve(const JACOBIAN_POINT *P);

int jacobian_point_equ_hex(const JACOBIAN_POINT *P, const char hex[128]);


#define jacobian_point_set_infinity(R) jacobian_point_init(R)
#define jacobian_point_copy(R, P) memcpy((R), (P), sizeof(JACOBIAN_POINT))
#define jacobian_point_mul_generator(R, K) jacobian_point_mul(R, K, &SM2_G)

void fp_mul(base r, const base a, const base b) {
    base_mul_modn(r, a, b, P);
}

void fp_add(base r, const base a, const base b) {
    base_add_modn(r, a, b, P);
}

void fp_sub(base r, const base a, const base b) {
    base_sub_mod(r, a, b, P);
}

void fp_sqr(base r, const base a) {
    fp_mul(r, a, a);
}

void fp_inv(base r, const base a) {
    base_mul_inv(r, a, P);
}

void fp_dbl(base r, const base a) {
    fp_mul(r, a, BASE_TWO);
}

void fp_tri(base r, const base a) {
    fp_mul(r, a, BASE_THREE);
}

void fp_neg(base r, const base a) {
    fp_sub(r, BASE_ZERO, a);
}

void fp_div2(base r, const base a) {
    int i;
    base_copy(r, a);
    if (r[0] & 0x01) {
        base_add(r, r, P);
    }
    for (i = 0; i < 7; i++) {
        r[i] = (r[i] >> 1) | ((r[i + 1] & 0x01) << 31);
    }
    r[i] >>= 1;
}

void jacobian_point_init(JACOBIAN_POINT *R) {
    memset(R, 0, sizeof(JACOBIAN_POINT));
    R->X[0] = 1;
    R->Y[0] = 1;
}

int jacobian_point_is_at_infinity(const JACOBIAN_POINT *P) {
    return base_is_zero(P->Z);
}

void jacobian_point_set_xy(JACOBIAN_POINT *R, const base x, const base y) {
    base_copy(R->X, x);
    base_copy(R->Y, y);
    base_set_one(R->Z);
}

void jacobian_point_get_xy(const JACOBIAN_POINT *P, base x, base y) {
    base z_inv;
//    base_print("pz:", P->Z);
    if (base_is_one(P->Z)) {
        base_copy(x, P->X);
        base_copy(y, P->Y);
    } else {
        fp_inv(z_inv, P->Z);
        if (y)
            fp_mul(y, P->Y, z_inv);
        fp_sqr(z_inv, z_inv);
        fp_mul(x, P->X, z_inv);
        if (y)
            fp_mul(y, y, z_inv);
    }
}

void jacobian_print_xy(const JACOBIAN_POINT *P) {
    base x, y;
    jacobian_point_get_xy(P, x, y);
    base_print("x:", x);
    base_print("y:", y);
}

int jacobian_point_is_on_curve(const JACOBIAN_POINT *P) {
    base t0;
    base t1;
    base t2;

    if (base_is_one(P->Z)) {
        fp_sqr(t0, P->Y);
        fp_add(t0, t0, P->X);
        fp_add(t0, t0, P->X);
        fp_add(t0, t0, P->X);
        fp_sqr(t1, P->X);
        fp_mul(t1, t1, P->X);
        fp_add(t1, t1, B);
    } else {
        fp_sqr(t0, P->Y);
        fp_sqr(t1, P->Z);
        fp_sqr(t2, t1);
        fp_mul(t1, t1, t2);
        fp_mul(t1, t1, B);
        fp_mul(t2, t2, P->X);
        fp_add(t0, t0, t2);
        fp_add(t0, t0, t2);
        fp_add(t0, t0, t2);
        fp_sqr(t2, P->X);
        fp_mul(t2, t2, P->X);
        fp_add(t1, t1, t2);
    }

    if (base_cmp(t0, t1) != 0) {
        return -1;
    }
    return 1;
}

void jacobian_point_neg(JACOBIAN_POINT *R, const JACOBIAN_POINT *P) {
    base_copy(R->X, P->X);
    fp_neg(R->Y, P->Y);
    base_copy(R->Z, P->Z);
}

void jacobian_point_dbl(JACOBIAN_POINT *R, const JACOBIAN_POINT *P) {
    const uint64_t *X1 = P->X;
    const uint64_t *Y1 = P->Y;
    const uint64_t *Z1 = P->Z;
    base T1;
    base T2;
    base T3;
    base X3;
    base Y3;
    base Z3;
    //printf("X1 = "); print_bn(X1);
    //printf("Y1 = "); print_bn(Y1);
    //printf("Z1 = "); print_bn(Z1);

    if (jacobian_point_is_at_infinity(P)) {
        jacobian_point_copy(R, P);
        return;
    }
    ///现在是要doubleP,所以没有x2、y2
    ///k=(3x1+a)/2y1
    ///x3=k^2-2x1 modp
    ///y3=-y1 modp

    fp_sqr(T1, Z1);        //printf("T1 = Z1^2    = "); print_bn(T1);
    fp_sub(T2, X1, T1);    //printf("T2 = X1 - T1 = "); print_bn(T2);
    fp_add(T1, X1, T1);    //printf("T1 = X1 + T1 = "); print_bn(T1);
    fp_mul(T2, T2, T1);    //printf("T2 = T2 * T1 = "); print_bn(T2);
    fp_tri(T2, T2);        //printf("T2 =  3 * T2 = "); print_bn(T2);
    fp_dbl(Y3, Y1);        //printf("Y3 =  2 * Y1 = "); print_bn(Y3);
    fp_mul(Z3, Y3, Z1);    //printf("Z3 = Y3 * Z1 = "); print_bn(Z3);
    fp_sqr(Y3, Y3);        //printf("Y3 = Y3^2    = "); print_bn(Y3);
    fp_mul(T3, Y3, X1);    //printf("T3 = Y3 * X1 = "); print_bn(T3);
    fp_sqr(Y3, Y3);        //printf("Y3 = Y3^2    = "); print_bn(Y3);
    fp_div2(Y3, Y3);    //printf("Y3 = Y3/2    = "); print_bn(Y3);
    fp_sqr(X3, T2);        //printf("X3 = T2^2    = "); print_bn(X3);
    fp_dbl(T1, T3);        //printf("T1 =  2 * T1 = "); print_bn(T1);
    fp_sub(X3, X3, T1);    //printf("X3 = X3 - T1 = "); print_bn(X3);
    fp_sub(T1, T3, X3);    //printf("T1 = T3 - X3 = "); print_bn(T1);
    fp_mul(T1, T1, T2);    //printf("T1 = T1 * T2 = "); print_bn(T1);
    fp_sub(Y3, T1, Y3);    //printf("Y3 = T1 - Y3 = "); print_bn(Y3);

    base_copy(R->X, X3);
    base_copy(R->Y, Y3);
    base_copy(R->Z, Z3);

    //printf("X3 = "); print_bn(R->X);
    //printf("Y3 = "); print_bn(R->Y);
    //printf("Z3 = "); print_bn(R->Z);

}

void jacobian_point_add(JACOBIAN_POINT *R, const JACOBIAN_POINT *P, const JACOBIAN_POINT *Q) {
    const uint64_t *X1 = P->X;
    const uint64_t *Y1 = P->Y;
    const uint64_t *Z1 = P->Z;
    const uint64_t *x2 = Q->X;
    const uint64_t *y2 = Q->Y;
    base T1;
    base T2;
    base T3;
    base T4;
    base X3;
    base Y3;
    base Z3;

    if (jacobian_point_is_at_infinity(Q)) {
        jacobian_point_copy(R, P);
        return;
    }

    if (jacobian_point_is_at_infinity(P)) {
        jacobian_point_copy(R, Q);
        return;
    }

    assert(base_is_one(Q->Z));

    fp_sqr(T1, Z1);
    fp_mul(T2, T1, Z1);
    fp_mul(T1, T1, x2);
    fp_mul(T2, T2, y2);
    fp_sub(T1, T1, X1);
    fp_sub(T2, T2, Y1);
    if (base_is_zero(T1)) {
        if (base_is_zero(T2)) {
            JACOBIAN_POINT _Q, *Q = &_Q;
            jacobian_point_set_xy(Q, x2, y2);

            jacobian_point_dbl(R, Q);
            return;
        } else {
            jacobian_point_set_infinity(R);
            return;
        }
    }
    fp_mul(Z3, Z1, T1);
    fp_sqr(T3, T1);
    fp_mul(T4, T3, T1);
    fp_mul(T3, T3, X1);
    fp_dbl(T1, T3);
    fp_sqr(X3, T2);
    fp_sub(X3, X3, T1);
    fp_sub(X3, X3, T4);
    fp_sub(T3, T3, X3);
    fp_mul(T3, T3, T2);
    fp_mul(T4, T4, Y1);
    fp_sub(Y3, T3, T4);

    base_copy(R->X, X3);
    base_copy(R->Y, Y3);
    base_copy(R->Z, Z3);
}

void jacobian_point_sub(JACOBIAN_POINT *R, const JACOBIAN_POINT *P, const JACOBIAN_POINT *Q) {
    JACOBIAN_POINT _T, *T = &_T;
    jacobian_point_neg(T, Q);
    jacobian_point_add(R, P, T);
}

void jacobian_point_mul(JACOBIAN_POINT *R, const base k, const JACOBIAN_POINT *P) {
    char bits[257] = {0};
    JACOBIAN_POINT _Q, *Q = &_Q;
    JACOBIAN_POINT _T, *T = &_T;
    int i;

    /// FIXME: point_add need affine, so we can not use point_add
    if (!base_is_one(P->Z)) {
        cout << "1" << endl;
        base x;
        base y;
        jacobian_point_get_xy(P, x, y);
        jacobian_point_set_xy(T, x, y);
        P = T;
    }

    jacobian_point_set_infinity(Q);
    base_to_bits(k, bits);
    for (i = 0; i < 256; i++) {
        jacobian_point_dbl(Q, Q);
        if (bits[i] == '1') {
            jacobian_point_add(Q, Q, P);
        }
    }
    jacobian_point_copy(R, Q);
}

void jacobian_point_to_bytes(const JACOBIAN_POINT *P, uint8_t out[64]) {
    base x;
    base y;
    jacobian_point_get_xy(P, x, y);
    base_to_bytes(x, out);
    base_to_bytes(y, out + 32);
}

void jacobian_point_from_bytes(JACOBIAN_POINT *P, const uint8_t in[64]) {
    base_from_bytes(P->X, in);
    base_from_bytes(P->Y, in + 32);
    base_set_word(P->Z, 1);
    /* should we check if jacobian_point_is_on_curve */
}

//void jacobian_point_mul_generator(JACOBIAN_POINT *R, const base k) {
//    jacobian_point_mul(R, k, &SM2_G);
//}

/* R = t * P + s * G */
void jacobian_point_mul_sum(JACOBIAN_POINT *R, const base t, const JACOBIAN_POINT *P, const base s) {
    JACOBIAN_POINT _sG, *sG = &_sG;
    base x;
    base y;

    /* T = s * G */
    jacobian_point_mul_generator(sG, s);

    // R = t * P
    jacobian_point_mul(R, t, P);
    jacobian_point_get_xy(R, x, y);
    jacobian_point_set_xy(R, x, y);

    // R = R + T
    jacobian_point_add(R, sG, R);
}

void jacobian_point_from_hex(JACOBIAN_POINT *P, const char hex[64 * 2]) {
    base_from_hex(P->X, hex);
    base_from_hex(P->Y, hex + 64);
    base_set_one(P->Z);
}

int jacobian_point_equ_hex(const JACOBIAN_POINT *P, const char hex[128]) {
    base x;
    base y;
    JACOBIAN_POINT _T, *T = &_T;

    jacobian_point_get_xy(P, x, y);

//    cout << "test_equ_value: " << endl;
//    base_print("x:", x);
//    base_print("y:", y);


    jacobian_point_from_hex(T, hex);

    return (base_cmp(x, T->X) == 0) && (base_cmp(y, T->Y) == 0);
}

////-------以上是jacobian的坐标，下面是只有(x,y)的坐标-------

typedef struct {
    uint8_t x[32];
    uint8_t y[32];
} SM2_POINT;

#endif //SM2DEMO_ELLIPSE_H

