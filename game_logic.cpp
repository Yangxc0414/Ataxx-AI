#include "game_logic.h"

// 验证移动的合法性
bool isValidMove(const PINTS& from, const PINTS& to, const PULLS& state) {
    auto& current_Board = state.first;
    auto& current_Chess = state.second;

    if (!in(from) || !in(to)) return false;
    int from_pos = abs(from);
    int to_pos = abs(to);

    // 检查起始位置是否有玩家棋子
    if (!(current_Board & (1ull << from_pos)) || !(current_Chess & (1ull << from_pos))) {
        return false;
    }

    // 检查目标位置是否为空
    if (current_Board & (1ull << to_pos)) {
        return false;
    }

    return true;
}

// 尝试执行移动
PULLS TrySet(const PINTS _p, const PINTS p, PULLS Now) {
    auto& Board = Now.first;
    auto& Chess = Now.second;

    if (!isValidMove(_p, p, Now)) {
        return Now; // 返回原状态表示无效移动
    }

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
    return Now;
}

// 评估函数
Vtype value(PULLS Now, const unsigned depth, Vtype alpha, const Vtype beta) {
    auto& Board = Now.first;
    auto& Chess = Now.second;
    ULL old_chess = Chess;
    Chess = ~Chess;
    if (count(Board & Chess) > count(~Board & allones) && !going(Now) || !going2(Now)) {
        Chess = old_chess; // 恢复
        return (count(Board & Chess) > 24) ? numeric_limits<Vtype>::max() : -numeric_limits<Vtype>::max();
    }
    if (depth == 0) {
        ++Nodes;
        int player_score = 0;
        int opponent_score = 0;
        for (int k = 0; k < board_size; ++k) {
            if (Board & (1ull << k)) {
                int row = k / 7;
                int col = k % 7;
                int pos_value = 0;
                if ((row == 0 || row == 6) && (col == 0 || col == 6)) {
                    pos_value = 2;
                }
                else if (row == 0 || row == 6 || col == 0 || col == 6) {
                    pos_value = 1;
                }
                if (Chess & (1ull << k)) {
                    player_score += 1 + pos_value;
                }
                else {
                    opponent_score += 1 + pos_value;
                }
            }
        }
        Chess = old_chess; // 恢复
        return player_score - opponent_score;
    }
    if (is_timeout) {
        Chess = old_chess; // 恢复
        return alpha;
    }
    Vtype res = -numeric_limits<Vtype>::max();
    Vtype alpha_local = alpha;
    ULL BoardT_clone = Board & Chess;
    while (BoardT_clone && alpha_local < beta && !is_timeout) {
        ULL i_bit = getLowestBit(BoardT_clone);
        BoardT_clone &= ~i_bit;
        int idx = DB[DBC * i_bit >> 58];
        PINTS _i = trnsfrm(idx);
        for (int j_idx = 0; j_idx < 8; j_idx++) {
            auto j = inner[j_idx];
            PINTS p = _i + j;
            PULLS temp_state = TrySet(_i, p, Now);
            if (temp_state.first != Now.first || temp_state.second != Now.second) { // 有效移动
                Vtype Value = -value(temp_state, depth - 1, -beta, -alpha_local);
                if (!(Nodes & check)) timeout();
                if (is_timeout) break;
                res = maximum(Value, res);
                alpha_local = maximum(Value, alpha_local);
                if (alpha_local >= beta) {
                    Chess = old_chess; // 恢复
                    return res;
                }
            }
        }
    }
    if (!is_timeout && alpha_local < beta) {
        ULL BoardT_jump = Board & Chess;
        while (BoardT_jump && alpha_local < beta && !is_timeout) {
            ULL i_bit = getLowestBit(BoardT_jump);
            BoardT_jump &= ~i_bit;
            int idx = DB[DBC * i_bit >> 58];
            PINTS _i = trnsfrm(idx);
            for (int k = 8; k < 24; ++k) {
                auto j = all[k];
                PINTS p = _i + j;
                PULLS temp_state = TrySet(_i, p, Now);
                if (temp_state.first != Now.first || temp_state.second != Now.second) { // 有效移动
                    Vtype Value = -value(temp_state, depth - 1, -beta, -alpha_local);
                    if (!(Nodes & check)) timeout();
                    if (is_timeout) break;
                    res = maximum(Value, res);
                    alpha_local = maximum(Value, alpha_local);
                    if (alpha_local >= beta) {
                        Chess = old_chess; // 恢复
                        return res;
                    }
                }
            }
        }
    }
    Chess = old_chess; // 恢复
    return res;
}

// 清屏函数
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 显示棋盘
void displayBoard() {
    cout << "  0 1 2 3 4 5 6" << endl;
    for (int i = 0; i < 7; i++) {
        cout << i << " ";
        for (int j = 0; j < 7; j++) {
            int pos = i * 7 + j;
            if (Board & (1ull << pos)) {
                if (Chess & (1ull << pos)) {
                    cout << "● ";  // 玩家棋子
                }
                else {
                    cout << "○ ";  // AI棋子
                }
            }
            else {
                cout << ". ";  // 空位
            }
        }
        cout << endl;
    }
}

// 获取所有有效棋子
vector<PINTS> getValidPieces() {
    vector<PINTS> pieces;
    ULL BoardT = Board & Chess;  // 找到玩家棋子

    while (BoardT) {
        ULL i_bit = getLowestBit(BoardT);
        BoardT &= ~i_bit;
        int idx = DB[DBC * i_bit >> 58];
        PINTS piece = trnsfrm(idx);

        // 检查该棋子是否有任何有效移动
        bool hasMove = false;

        // 普通移动
        for (int j_idx = 0; j_idx < 8; j_idx++) {
            auto j = inner[j_idx];
            PINTS p = piece + j;
            if (in(p) && !(Board & (1ull << abs(p)))) {
                pieces.push_back(piece);
                hasMove = true;
                break;
            }
        }

        if (!hasMove) {
            // 跳跃移动
            for (int k = 8; k < 24; ++k) {
                auto j = all[k];
                PINTS p = piece + j;
                if (in(p) && !(Board & (1ull << abs(p)))) {
                    pieces.push_back(piece);
                    hasMove = true;
                    break;
                }
            }
        }
    }

    return pieces;
}

// 获取指定棋子的目标移动位置
vector<PINTS> getValidMovesForPiece(const PINTS& piece) {
    vector<PINTS> moves;

    // 普通移动
    for (int j_idx = 0; j_idx < 8; j_idx++) {
        auto j = inner[j_idx];
        PINTS p = piece + j;
        if (in(p) && !(Board & (1ull << abs(p)))) {
            moves.push_back(p);
        }
    }

    // 跳跃移动
    for (int k = 8; k < 24; ++k) {
        auto j = all[k];
        PINTS p = piece + j;
        if (in(p) && !(Board & (1ull << abs(p)))) {
            moves.push_back(p);
        }
    }

    return moves;
}

// 获取玩家移动
bool getPlayerMove(PINTS& from, PINTS& to) {
    // 获取所有可移动的棋子列表
    vector<PINTS> validPieces = getValidPieces();

    if (validPieces.empty()) {
        cout << "没有可移动的棋子！" << endl;
        return false;
    }

    cout << "可移动的棋子位置：" << endl;
    for (size_t i = 0; i < validPieces.size(); i++) {
        cout << i + 1 << ". (" << validPieces[i].first << ", " << validPieces[i].second << ")" << endl;
    }

    int piece_choice;
    while (true) {
        cout << "请选择棋子编号 (1-" << validPieces.size() << "): ";
        cin >> piece_choice;

        if (cin.fail() || piece_choice < 1 || piece_choice > static_cast<int>(validPieces.size())) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入无效，请重新输入！" << endl;
        } else {
            break;
        }
    }

    from = validPieces[piece_choice - 1];

    // 获取该棋子的目标移动位置
    vector<PINTS> validMoves = getValidMovesForPiece(from);

    if (validMoves.empty()) {
        cout << "棋子 (" << from.first << ", " << from.second << ") 没有可移动的位置！" << endl;
        return false;
    }

    cout << "棋子 (" << from.first << ", " << from.second << ") 可以移动到的位置：" << endl;
    for (size_t i = 0; i < validMoves.size(); i++) {
        cout << i + 1 << ". (" << validMoves[i].first << ", " << validMoves[i].second << ")" << endl;
    }

    int move_choice;
    while (true) {
        cin >> move_choice;

        if (cin.fail() || move_choice < 1 || move_choice > static_cast<int>(validMoves.size())) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "输入无效，请重新输入！" << endl;
        } else {
            break;
        }
    }

    to = validMoves[move_choice - 1];

    return true;
}

// 检查是否有有效移动
bool hasValidMoves(PULLS current_state, bool player_turn) {
    auto& current_Board = current_state.first;
    auto& current_Chess = current_state.second;

    // 根据是玩家还是AI移动来选择棋子颜色
    ULL player_pieces = player_turn ? (current_Board & current_Chess) : (current_Board & ~current_Chess);

    ULL BoardT = player_pieces;
    while (BoardT) {
        ULL i_bit = getLowestBit(BoardT);
        BoardT &= ~i_bit;
        int idx = DB[DBC * i_bit >> 58];
        PINTS _i = trnsfrm(idx);

        // 普通移动
        for (int j_idx = 0; j_idx < 8; j_idx++) {
            auto j = inner[j_idx];
            PINTS p = _i + j;
            if (in(p) && !(current_Board & (1ull << abs(p)))) {
                return true;
            }
        }

        // 跳跃移动
        for (int k = 8; k < 24; ++k) {
            auto j = all[k];
            PINTS p = _i + j;
            if (in(p) && !(current_Board & (1ull << abs(p)))) {
                return true;
            }
        }
    }
    return false;
}

// AI移动
PULLS getAIMove() {
    MMCHS Values;
    ULL BoardT = Board & ~Chess;  // AI的棋子

    while (BoardT) {
        ULL i_bit = getLowestBit(BoardT);
        BoardT &= ~i_bit;
        int idx = DB[DBC * i_bit >> 58];
        PINTS _i = trnsfrm(idx);

        // 普通移动
        for (int j_idx = 0; j_idx < 8; j_idx++) {
            auto j = inner[j_idx];
            PINTS p = _i + j;
            PULLS temp_state = TrySet(_i, p, { Board, ~Chess });
            if (temp_state.first != Board || temp_state.second != ~Chess) { // 有效移动
                Values.insert(make_pair(0, temp_state));
            }
        }

        // 跳跃移动
        for (int k = 8; k < 24; ++k) {
            auto j = all[k];
            PINTS p = _i + j;
            PULLS temp_state = TrySet(_i, p, { Board, ~Chess });
            if (temp_state.first != Board || temp_state.second != ~Chess) { // 有效移动
                Values.insert(make_pair(0, temp_state));
            }
        }
    }

    if (Values.empty()) {
        return { Board, Chess };  // 没有有效移动，返回
    }

    int depth = 0;
    MMCHS OldValues;
    start = chrono::steady_clock::now();
    is_timeout = false;

    while (!is_timeout && !Values.empty()) {
        Vtype alpha = -numeric_limits<Vtype>::max();
        Vtype beta = numeric_limits<Vtype>::max();
        OldValues.swap(Values);
        Values.clear();
        if (!OldValues.empty() && OldValues.begin()->first == numeric_limits<Vtype>::max()) break;
        for (auto j : OldValues) {
            auto temp = -value(j.second, depth, -beta, -alpha);
            Values.insert(make_pair(temp, j.second));
            if (is_timeout) break;
            alpha = maximum(temp, alpha);
        }
        depth++;
    }

    if (is_timeout && OldValues.empty()) {
        // 如果超时且OldValues为空，随机选择一个有效移动
        vector<PULLS> possible_moves;
        ULL BoardT2 = Board & ~Chess;  // AI的棋子
        while (BoardT2) {
            ULL i_bit = getLowestBit(BoardT2);
            BoardT2 &= ~i_bit;
            int idx = DB[DBC * i_bit >> 58];
            PINTS _i = trnsfrm(idx);

            // 普通移动
            for (int j_idx = 0; j_idx < 8; j_idx++) {
                auto j = inner[j_idx];
                PINTS p = _i + j;
                PULLS temp_state = TrySet(_i, p, { Board, ~Chess });
                if (temp_state.first != Board || temp_state.second != ~Chess) { // 有效移动
                    possible_moves.push_back(temp_state);
                }
            }

            // 跳跃移动
            for (int k = 8; k < 24; ++k) {
                auto j = all[k];
                PINTS p = _i + j;
                PULLS temp_state = TrySet(_i, p, { Board, ~Chess });
                if (temp_state.first != Board || temp_state.second != ~Chess) { // 有效移动
                    possible_moves.push_back(temp_state);
                }
            }
        }
        if (!possible_moves.empty()) {
            // 随机选择一个移动
            int random_idx = rand() % possible_moves.size();
            return { possible_moves[random_idx].first, ~possible_moves[random_idx].second };
        }
        else {
            return { Board, Chess };  // 没有有效移动
        }
    }
    else {
        // 选择OldValues中的最佳移动
        auto best = OldValues.begin();
        return { best->second.first, ~best->second.second };  // 转换回玩家视角
    }
}