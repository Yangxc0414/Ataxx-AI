#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "utility.h"

// 游戏状态相关函数
inline bool going(const PULLS Now) {
    auto& Board = Now.first;
    auto& Chess = Now.second;
    ULL BoardT = ~Board & allones;
    while (BoardT) {
        ULL i = getLowestBit(BoardT);
        BoardT &= ~i;
        auto k = DB[DBC * i >> 58];
        auto _i = trnsfrm(k);
        for (int idx = 0; idx < 24; idx++) {
            auto j = all[idx];
            if (in(_i + j) && (Board & Chess & (1ull << abs(_i + j)))) return true;
        }
    }
    return false;
}

inline bool going2(const PULLS Now) {
    auto& Board = Now.first;
    auto& Chess = Now.second;
    ULL BoardT = Board & Chess;
    while (BoardT) {
        ULL i = getLowestBit(BoardT);
        BoardT &= ~i;
        auto k = DB[DBC * i >> 58];
        auto _i = trnsfrm(k);
        for (int idx = 0; idx < 24; idx++) {
            auto j = all[idx];
            if (in(_i + j) && (~Board & (1ull << abs(_i + j)))) return true;
        }
    }
    return false;
}

inline void Set(const PINTS _p, const PINTS p) {
    if (far(_p, p)) Board &= ~(1ull << abs(_p));
    Board |= 1ull << abs(p);
    ULL temp = (Chess >> abs(_p)) & 1ull;
    int idx = abs(p);
    ULL mask = mask3x3[idx];
    if (temp) {
        Chess |= mask;
    }
    else {
        Chess &= ~mask;
    }
}

// 移动验证和执行
bool isValidMove(const PINTS& from, const PINTS& to, const PULLS& state);
PULLS TrySet(const PINTS _p, const PINTS p, PULLS Now);

// AI相关函数
Vtype value(PULLS Now, const unsigned depth, Vtype alpha, const Vtype beta);

// 界面和交互函数
void clearScreen();
void displayBoard();
vector<PINTS> getValidPieces();
vector<PINTS> getValidMovesForPiece(const PINTS& piece);
bool getPlayerMove(PINTS& from, PINTS& to);
bool hasValidMoves(PULLS current_state, bool player_turn);
PULLS getAIMove();

#endif // GAME_LOGIC_H