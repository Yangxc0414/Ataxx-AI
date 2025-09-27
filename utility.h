#ifndef UTILITY_H
#define UTILITY_H

#include "types.h"

// 位操作函数
unsigned int popcountll(ULL x);
ULL getLowestBit(ULL x);

extern bool timeout();

// 运算符重载
constexpr PINTS operator+(const PINTS a, const PINTS b) {
    return { a.first + b.first, a.second + b.second };
}

constexpr int abs(const PINTS p) {
    return p.first * 7 + p.second;
}

constexpr PINTS trnsfrm(const int i) {
    return { i / 7, i % 7 };
}

// 辅助函数
inline bool far(const PINTS a, const PINTS b) {
    return (a.first - b.first) > 1 || (a.first - b.first) < -1 || (a.second - b.second) > 1 || (a.second - b.second) < -1;
}

template <class T>
inline T maximum(const T a, const T b) {
    return (a > b) ? a : b;
}

inline bool in(const PINTS p) {
    return p.first >= 0 && p.first < 7 && p.second >= 0 && p.second < 7;
}

inline unsigned count(ULL x) {
    return popcountll(x);
}

#endif // UTILITY_H