#ifndef TOOL
#define TOOL

#include<cstdlib>
#include<ctime>
#include <iostream>
#include <sstream>
#include <algorithm>
//#include "sm3.h"
//
//#define PUTU32(p,V) \
//	((p)[0] = (uint8_t)((V) >> 24), \
//	 (p)[1] = (uint8_t)((V) >> 16), \
//	 (p)[2] = (uint8_t)((V) >>  8), \
//	 (p)[3] = (uint8_t)(V))

using namespace std;

int M(int x, int mod) {
    x %= mod;
    if (x < 0) {
        x = (x - ((int) (x / mod) - 1) * mod) % mod;
    }
    return x;
}

string plaintextToHex(string str, string separator = "") {
    /**
     * 明文转化为16进制串
     */
    const string hex = "0123456789ABCDEF";
    stringstream ss;

    for (unsigned int i = 0; i < str.size(); ++i)
        ss << hex[(unsigned char) str[i] >> 4] << hex[(unsigned char) str[i] & 0xf] << separator;

    return ss.str();
}

string getRandom(int length) {
    //主要想采用每位都随机生成的方式生成这么大的一个随机数，否则普通的方法没法实现
    //引入洗牌，对alpha和ret都洗一次，增加随机性
    string ret;
    string alpha = "0123456789ABCDEF";
    srand((unsigned) time(NULL));//种子
    random_shuffle(alpha.begin(), alpha.end());//洗牌，增加随机性
    for (int i = 0; i < length; ++i) {
        ret.push_back(alpha[rand() % 16]);
    }
    random_shuffle(ret.begin(), ret.end());//洗牌，增加随机性
    return ret;
}

#endif