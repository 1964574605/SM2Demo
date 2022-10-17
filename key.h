//
// Created by 19645 on 2022/10/16.
//

#ifndef SM2DEMO_KEY_H
#define SM2DEMO_KEY_H

#include "ellipse.h"
#include "basedata.h"

typedef struct {
    SM2_POINT public_key;
    uint8_t private_key[32];
} SM2_KEY;


int sm2_key_generate(SM2_KEY *key);

int sm2_key_generate(SM2_KEY *key) {
    cout << "-----key_generate begin-----" << endl;
    base x;
    base y;
    JACOBIAN_POINT _P, *P = &_P;

    if (!key) {
        cout << "sm2_key_generate failed" << endl;
        return -1;
    }
    memset(key, 0, sizeof(SM2_KEY));

    do {
        base_rand_range(x, N);
    } while (base_is_zero(x));
    base_print("随机数k生成:", x);

    base_to_bytes(x, key->private_key);
    jacobian_point_mul_generator(P, x);
    jacobian_point_get_xy(P, x, y);
    base_to_bytes(x, key->public_key.x);
    base_to_bytes(y, key->public_key.y);
    cout << "-----key_generate over-----" << endl;
    return 1;
}

#endif //SM2DEMO_KEY_H

