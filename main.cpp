#include <iostream>
#include "basedata.h"
#include "tool.h"
#include "coordinate.h"

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
    uint8_t plaintext[] = "Hello World!";
    cout << sizeof(plaintext) << endl;

    uint8_t buf[64];

//    sm2_kdf(buf, sizeof(buf), sizeof(plaintext), plaintext);

    cout << "------------------------test2Ending----------------------" << endl;
}

int main() {
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


int main1() {
    test2();
    return 0;
}