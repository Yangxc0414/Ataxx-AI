#include "utility.h"

// 实现 __builtin_popcountll
unsigned int popcountll(ULL x) {
    unsigned int count = 0;
    x &= allones;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

// 获取最低位的1
ULL getLowestBit(ULL x) {
    return x & ((~x) + 1);  // 这是获取最低位1的另一种写法
}

// 超时检测
bool timeout() {
    if (chrono::duration_cast<chrono::duration<double>>(chrono::steady_clock::now() - start).count() >= 0.9) {
        is_timeout = true;
        return true;
    }
    return false;
}



// 显式实例化常用类型的模板函数
template int maximum<int>(const int a, const int b);
template ULL maximum<ULL>(const ULL a, const ULL b);