#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <vector>
#include <chrono>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// 类型定义
typedef unsigned long long ULL;
typedef pair<ULL, ULL> PULLS;
typedef pair<int, int> PINTS;
typedef int Vtype;
typedef multimap<Vtype, PULLS, greater<Vtype>> MMCHS;

// 常量定义
const int board_size = 7 * 7;
const ULL allones = (1ull << board_size) - 1;
const unsigned int check = 0xffff;
extern unsigned int Nodes;

// 方向常量
extern const PINTS inner[];
extern const PINTS innerall[];
extern const PINTS all[];
const ULL DBC = 0x0218a392cd3d5dbfu;
extern int DB[];

extern PULLS Now;
extern ULL& Board;
extern ULL& Chess;
extern chrono::steady_clock::time_point start;
extern bool is_timeout;
extern ULL mask3x3[];

#endif // TYPES_H