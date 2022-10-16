//
// Created by 19645 on 2022/10/13.
//

#ifndef SM2DEMO_COORDINATE_H
#define SM2DEMO_COORDINATE_H


#include "basedata.h"


struct coordinate {
    //coordinate:坐标
    base x;
    base y;

    coordinate() {
        base_copy(x, BASE_ZERO);
        base_copy(y, BASE_ZERO);
    }

    coordinate(base _x, base _y) {
        base_copy(x, _x);
        base_copy(y, _y);
    }

    coordinate(coordinate &c) {
        /**
         * 拷贝构造函数
         */
        base_copy(x, c.x);
        base_copy(y, c.y);
    }

    coordinate &operator=(const coordinate &);//重载运算符=，大数之间进行赋值操作

    coordinate selfDouble(base a, base p);

    coordinate add(coordinate point, base a, base p);

    coordinate mulNum(base n, base a, base p);

    bool equalTo(const coordinate &point) const;

    void print();
};

coordinate &coordinate::operator=(const coordinate &n) {
    base_copy(this->x, n.x);
    base_copy(this->y, n.y);
    return *this;
}

coordinate coordinate::selfDouble(base a, base p) {
    /**
     * 把this[Q]做Q=Q+Q
     * a: y^2=x^3+ax+b
     * p: 大质数
     */
    base zero;
    base_copy(zero, BASE_ZERO);
    if (this->equalTo(coordinate(zero, zero))) {
        return *this;//如果是和(0,0)相加的话，直接返回自己
    }

    base k;//斜率
    // P=Q 则 k=(3 * x_1^2 + a) / (2 * y_1)  (mod p)
    base molecular;//分子
    base_mul_modn(molecular, x, x, p);
    base_mul_modn(molecular, BASE_THREE, molecular, p);
    base_print("---a:", a);
    base_print("----mole:", molecular);
    base_add_modn(molecular, molecular, a, p);
    base_print("----mole:", molecular);

    base denominator, denominator_inv;//分母
    base_mul_modn(denominator, BASE_TWO, y, p);
    base_mul_inv(denominator_inv, denominator, p);//获得分母的乘法逆元

    base_mul_modn(k, molecular, denominator_inv, p);

    base temp;
    base x_mul2;
    base_mul_modn(temp, k, k, p);//k^2
    base_add_modn(x_mul2, x, x, p);//2x

    base_print("---x:", x);
    coordinate new_co;
    base_sub_mod(new_co.x, temp, x_mul2, p);//k^2-2x
    base x1_x3;//x1-x3
    base_sub_mod(x1_x3, x, new_co.x, p);//x1-x3
    base_mul_modn(new_co.y, k, x1_x3, p);//k(x1-x3)
    base_sub_mod(new_co.y, new_co.y, y, p);//k(x1-x3)-y1


    return new_co;
}

coordinate coordinate::add(coordinate point, base a, base p) {
    /**
     * 椭圆曲线的点加法：R=P+Q，这里R和P都是self，Q是point
     */
    base zero;
    base_copy(zero, BASE_ZERO);
    if (point.equalTo(coordinate(zero, zero))) {
        return *this;//如果是和(0,0)相加的话，直接返回自己
    }
    bool isEqual = this->equalTo(point);
    coordinate ret;
    if (isEqual) {
        ret = selfDouble(a, p);
    } else {
        base molecular, denominator, denominator_inv;
        base_sub_mod(molecular, point.y, this->y, p);
        base_sub_mod(denominator, point.x, this->x, p);
        base k;
        base_mul_inv(denominator_inv, denominator, p);
        base_mul_modn(k, molecular, denominator_inv, p);


        base temp, x3, y3;
        base_mul_modn(temp, k, k, p);
        base_sub_mod(temp, temp, this->x, p);
        base_sub_mod(x3, temp, point.x, p);

        base_sub_mod(temp, this->x, x3, p);
        base_mul_modn(temp, k, temp, p);
        base_sub_mod(y3, temp, this->y, p);

        base_copy(ret.x, x3);
        base_copy(ret.y, y3);
    }
    return ret;
}


coordinate coordinate::mulNum(base n, base a, base p) {
    char base_bits[257];
    base_to_bits(n, base_bits);
    base_bits[256] = '\0';

    coordinate Q;//写一个Q来做次方的存储,不传参，初始化为(0,0)
    for (int i = 0; i < 256; ++i) {
        Q = Q.selfDouble(a, p);//做一个Q=Q+Q
        if (base_bits[i] == '1') {
            Q = Q.add((*this), a, p);
        }
    }
    return Q;
}

bool coordinate::equalTo(const coordinate &point) const {
    int x_compare, y_compare;
    x_compare = base_cmp(x, point.x);
    y_compare = base_cmp(y, point.y);
    return bool(x_compare == 0 && y_compare == 0);
}

void coordinate::print() {
    base_print("x:", x);
    base_print("y:", y);
}

#endif //SM2DEMO_COORDINATE_H