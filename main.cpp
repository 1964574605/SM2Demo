#include <iostream>
#include "basedata.h"
#include "tool.h"
#include "coordinate.h"
#include "ellipse.h"
#include "key.h"
#include "ciphertext.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;


void test1(coordinate co, base a, base p) {
    cout << "------------------------test1Begin----------------------" << endl;
    base zero;
    base_copy(zero, BASE_ZERO);
    coordinate Q(zero, zero);
    co.print();
    co = co.add(Q, a, p);
    co.print();
    cout << "------------------------test1Ending----------------------" << endl;
}

void test3() {
    cout << "------------------------test3Begin----------------------" << endl;
    cout << "测一下selfDouble" << endl;
    base a, p;
    coordinate Q;
    base BASE_23 = {23, 0, 0, 0, 0, 0, 0, 0};
    base BASE_17 = {17, 0, 0, 0, 0, 0, 0, 0};
    base BASE_10 = {10, 0, 0, 0, 0, 0, 0, 0};
    //p=23,a=1,b=1
    //(0,1)
    base_copy(p, BASE_23);
    base_copy(a, BASE_ONE);
    base_print("p:", p);
    base_print("a:", a);
    base_copy(Q.x, BASE_17);
    base_copy(Q.y, BASE_10);
    cout << "打印Q:" << endl;
    Q.print();
    coordinate doub;
    doub = Q.selfDouble(a, p);
    cout << "Q.selfDouble:" << endl;
    doub.print();
    cout << "------------------------test3Ending----------------------" << endl;
}

void test2() {
    cout << "------------------------test2Begin----------------------" << endl;
    base P = {
            0xffffffff, 0xffffffff, 0x00000000, 0xffffffff,
            0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
    };//大素数
    base A = {0, 0, 0, 0, 0, 0, 0, 0};
    base B = {
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
    coordinate G(Gx, Gy);
    base N = {
            0x39d54123, 0x53bbf409, 0x21c6052b, 0x7203df6b,
            0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
    };
    G.print();
    coordinate doubleTest;
    doubleTest = G.selfDouble(A, P);
    doubleTest.print();


    cout << "------------------------test2Ending----------------------" << endl;
}

void test4() {
    cout << "------------------------test4Begin----------------------" << endl;
    base P = {
            0xffffffff, 0xffffffff, 0x00000000, 0xffffffff,
            0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
    };//大素数
    base A = {0, 0, 0, 0, 0, 0, 0, 0};
    base B = {
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
    coordinate G(Gx, Gy);

    base _Gy;
    base_sub_mod(_Gy, BASE_ZERO, Gy, P);
    coordinate G_neg(Gx, _Gy);
    cout << "neg:" << endl;
    G_neg.print();

    base N = {
            0x39d54123, 0x53bbf409, 0x21c6052b, 0x7203df6b,
            0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
    };
    G.print();
    coordinate doubleTest;
    doubleTest = G.selfDouble(A, P);
    doubleTest.print();
    doubleTest = doubleTest.add(G_neg, A, P);
    doubleTest.print();


    cout << "------------------------test4Ending----------------------" << endl;
}

#define hex_G \
    "32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7" \
    "bc3736a2f4f6779c59bdcee36b692153d0a9877cc62a474002df32e52139f0a0"
#define hex_2G \
    "56cefd60d7c87c000d58ef57fa73ba4d9c0dfa08c08a7331495c2e1da3f2bd52" \
    "31b7e7e6cc8189f668535ce0f8eaf1bd6de84c182f6c8e716f780d3a970a23c3"
#define hex_3G \
    "a97f7cd4b3c993b4be2daa8cdb41e24ca13f6bd945302244e26918f1d0509ebf" \
    "530b5dd88c688ef5ccc5cec08a72150f7c400ee5cd045292aaacdd037458f6e6"
#define hex_negG \
    "32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7" \
    "43c8c95c0b098863a642311c9496deac2f56788239d5b8c0fd20cd1adec60f5f"
#define hex_10G \
    "d3f94862519621c121666061f65c3e32b2d0d065cd219e3284a04814db522756" \
    "4b9030cf676f6a742ebd57d146dca428f6b743f64d1482d147d46fb2bab82a14"
#define hex_bG \
    "528470bc74a6ebc663c06fc4cfa1b630d1e9d4a80c0a127b47f73c324c46c0ba" \
    "832cf9c5a15b997e60962b4cf6e2c9cee488faaec98d20599d323d4cabfc1bf4"

#define hex_P \
    "504cfe2fae749d645e99fbb5b25995cc6fed70196007b039bdc44706bdabc0d9" \
    "b80a8018eda5f55ddc4b870d7784b7b84e53af02f575ab53ed8a99a3bbe2abc2"
#define hex_2P \
    "a53d20e89312b5243f66aec12ef6471f5911941d86302d5d8337cb70937d65ae" \
    "96953c46815e4259363256ddd6c77fcc33787aeafc6a57beec5833f476dd69e0"

#define hex_tP \
    "02deff2c5b3656ca3f7c7ca9d710ca1d69860c75a9c7ec284b96b8adc50b2936" \
    "b74bcba937e9267fce4ccc069a6681f5b04dcedd9e2794c6a25ddc7856df7145"


int test_jacobian_point() {
    JACOBIAN_POINT _P, *P = &_P;
    JACOBIAN_POINT _G, *G = &_G;
    base k;
    int i = 1, ok;

    uint8_t buf[64];

    printf("jacobian_point_test\n");


    jacobian_point_copy(G, &SM2_G);
    ok = jacobian_point_equ_hex(G, hex_G);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//1
    if (!ok) return -1;

    ok = jacobian_point_is_on_curve(G);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//2
    if (!ok) return -1;

    jacobian_point_dbl(P, G);
    ok = jacobian_point_equ_hex(P, hex_2G);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//3
    if (!ok) return -1;

    jacobian_point_add(P, P, G);
    ok = jacobian_point_equ_hex(P, hex_3G);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//4
    if (!ok) return -1;

    jacobian_point_sub(P, P, G);
    ok = jacobian_point_equ_hex(P, hex_2G);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//5
    if (!ok) return -1;

    jacobian_point_neg(P, G);
    ok = jacobian_point_equ_hex(P, hex_negG);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//6
    if (!ok) return -1;

    base_set_word(k, 10);
//    神奇，这个换行删掉居然会报个小错，说_G越界
//    cout << "" << endl;

    jacobian_point_mul(P, k, &SM2_G);

    ok = jacobian_point_equ_hex(P, hex_10G);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//7
    if (!ok) return -1;

    jacobian_point_mul_generator(P, B);
    ok = jacobian_point_equ_hex(P, hex_bG);
    printf("sm2 point test %d %s\n", i++, ok ? "ok" : "failed");//8
    if (!ok) return -1;

    jacobian_point_to_bytes(P, buf);
    jacobian_point_from_hex(P, hex_P);

    return 1;
}

int test_sm2_do_encrypt() {
    SM2_KEY sm2_key;
    uint8_t plaintext[] = "Hello World!";
    SM2_CIPHERTEXT ciphertext;

    uint8_t plainbuf[SM2_MAX_PLAINTEXT_SIZE] = {0};
    size_t plainlen = 0;
    int r = 0;

    if (sm2_key_generate(&sm2_key) != 1) {
        return -1;
    } else { cout << "key generate success" << endl; }


    if (sm2_do_encrypt(&sm2_key, plaintext, sizeof(plaintext), &ciphertext) != 1
        || sm2_do_decrypt(&sm2_key, &ciphertext, plainbuf, &plainlen) != 1) {
        return -1;
    } else { cout << "encrypt and decrypt success" << endl; }

    if (plainlen != sizeof(plaintext)
        || memcmp(plainbuf, plaintext, sizeof(plaintext)) != 0) {
        return -1;
    } else { cout << "plaintext get success" << endl; }

    printf("%s() ok\n", __FUNCTION__);
    return 1;
}

int main1() {
    /** 以下参数都使用国密局《SM2椭圆曲线公钥密码算法第4部分:公钥加密算法》中示例1：Fp-256的数据，以便对比 **/
    //p:大质数,范围被限定在2^256内
    char p[] = "8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3";
    base base_p;
    if (base_from_hex(base_p, p) == -1) { cout << "p转化失败" << endl; }

    //a:椭圆曲线的系数 [y^2=x^3+ax+b]
    char a[] = "787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498";
    base base_a;
    if (base_from_hex(base_a, a) == -1) { cout << "a转化失败" << endl; }

    //b:椭圆曲线的系数
    char b[] = "63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A";
    base base_b;
    if (base_from_hex(base_b, b) == -1) { cout << "b转化失败" << endl; }

    //n:阶
    char n[] = "8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7";
    base base_n;
    if (base_from_hex(base_n, n) == -1) { cout << "n转化失败" << endl; }

    //xG:基点坐标G的x值
    char xG[] = "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D";
    base base_xG;
    if (base_from_hex(base_xG, xG) == -1) { cout << "xG转化失败" << endl; }

    //yG:基点坐标G的y值
    char yG[] = "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2";
    base base_yG;
    if (base_from_hex(base_yG, yG) == -1) { cout << "yG转化失败" << endl; }

    //G:基点
    coordinate G(base_xG, base_yG);

    char xB[] = "435B39CCA8F3B508C1488AFC67BE491A0F7BA07E581A0E4849A5CF70628A7E0A";
    base base_xB;
    if (base_from_hex(base_xB, xB) == -1) { cout << "xB转化失败" << endl; }

    char yB[] = "75DDBA78F15FEECB4C7895E2C1CDF5FE01DEBB2CDBADF45399CCF77BBA076A42";
    base base_yB;
    if (base_from_hex(base_yB, yB) == -1) { cout << "yB转化失败" << endl; }

    //PB:椭圆曲线公钥
    coordinate PB(base_xB, base_yB);

    //plaintext:明文
    string plaintext = "encryption standard";//明文
    string hex_plaintext = plaintextToHex(plaintext);//转化为16进制串
    int klen = 152; //消息M的比特长度klen

    ///明文不用变成大数，因为不参与运算
//    if (hex_plaintext.length() > 64) {
//        cout << "明文长度超过64!" << endl;
//        return 0;
//    } else if (hex_plaintext.length() < 64) {
//        //前置补0直到64位
//        int zero_num = 64 - hex_plaintext.length();
//        string zero_str(zero_num, '0');
//        hex_plaintext = zero_str + hex_plaintext;
//    }
//    base base_plaintext;
//    if (base_from_hex(base_plaintext, hex_plaintext.data()) == -1) { cout << "plaintext转化失败" << endl; }
    /** --------------------加密部分-------------------- **/
    //A1: 用随机数发生器产生随机数k 属于 [1,n-1]
    base base_k;
    do {
        //string k = getRandom(klen);
        //为了测试算法正确性，这里用国密局文档里的k。实际使用的时候，把上一行注释打开，下一行注释掉
        string k = "4C62EEFD6ECFC2B95B92FD6C3D9575148AFA17425546D49018E5388D49DD7B4F";
        if (base_from_hex(base_k, k.data()) == -1) { cout << "k转化失败" << endl; }
        //当 base_k >= base_n 或者 base_k < 1 的时候，再来一次，直到落在[1,n-1]之间
    } while (base_cmp(base_k, base_n) != -1 || base_cmp(base_k, BASE_ONE) == -1);

    //A2: 计算椭圆曲线点C1=[k]G=(x1,y1)
    coordinate c1;
    c1 = G.mulNum(base_k, base_a, base_p);
    cout << "打印C1:\n";
    c1.print();

    //A3: 计算椭圆曲线点S=[h]P_B,这一步主要是生成S，用来判断S是否为0, h=N/n, N是有限域的阶(没给),n是基点G的阶(给了)
    //这里h没给，要算的话很难算；N是#E(F_2^m),由Hasse定理知:  2^m+1-2^(1+m/2) <= #E(F_2^m) <= 2^m+1+2^(1+m/2)
    //强哥说这里S不可能是无穷远点，这个判断没意义；国密局的代码里也把这步跳掉了，我也不去算它了，不影响加解密结果

    //A4: 计算[k]P_B
    coordinate c2;
    c2 = PB.mulNum(base_k, base_a, base_p);
    cout << "打印C2:\n";
    c2.print();

    //A5:计算t=KDF(x2||y2,klen),若t为全0比特串，则返回A1;


    //A6:计算C2=M 异或 t

    //A7:计算计算C3=Hash(x2||M||y2)

    //A8:输出密文C=C1||C2||C3


    return 0;
}

int main() {
//    test_jacobian_point();
    test_sm2_do_encrypt();
    return 0;
}