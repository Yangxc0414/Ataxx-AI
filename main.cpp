#include "game_logic.h"
#include <ctime>
#include <windows.h>

int main() {
    // 设置控制台代码页为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    srand(static_cast<unsigned int>(time(0))); // 为随机数生成器播种
    ios::sync_with_stdio(false);
    cin.tie(0);

    // 初始化mask3x3数组
    for (int k = 0; k < board_size; ++k) {
        PINTS p = { k / 7, k % 7 };
        mask3x3[k] = 0;
        for (int i_idx = 0; i_idx < 9; i_idx++) {
            auto i = innerall[i_idx];
            PINTS tmp = p + i;
            if (in(tmp)) {
                mask3x3[k] |= (1ull << abs(tmp));
            }
        }
    }

    clearScreen();
    cout << "欢迎来到Ataxx游戏！" << endl;
    cout << "选择你的棋子颜色：" << endl;
    cout << "1. 黑棋 (●)" << endl;
    cout << "2. 白棋 (○)" << endl;
    cout << "请选择 (1 或 2): ";

    int choice;
    cin >> choice;

    bool playerIsBlack = (choice == 1);

    if (!playerIsBlack) {
        // 如果玩家选择白棋，AI先走第一步
        PULLS ai_move = getAIMove();
        Now = { ai_move.first, ai_move.second };
        clearScreen();
        cout << "AI移动了" << endl;
        displayBoard();
        cout << "按回车继续...";
        cin.ignore();
        cin.get();
    }

    while (true) {
        clearScreen();
        displayBoard();

        // 检查玩家是否有合法移动
        if (!hasValidMoves(Now, true)) {
            cout << "你没有合法移动，游戏继续！" << endl;
            cout << "按回车继续...";
            cin.ignore();
            cin.get();
        }
        else {
            // 玩家移动
            PINTS from, to;
            bool valid_move = false;

            while (!valid_move) {
                clearScreen();
                displayBoard();
                cout << "轮到你移动了！" << endl;
                if (!getPlayerMove(from, to)) {
                    cout << "无法获取有效的移动，请重试！" << endl;
                    cout << "按回车继续...";
                    cin.ignore();
                    cin.get();
                    continue;
                }

                // 尝试执行移动
                PULLS new_state = TrySet(from, to, Now);
                if (new_state.first != Now.first || new_state.second != Now.second) {
                    Now = new_state;
                    valid_move = true;
                    clearScreen();
                    cout << "你移动了 (" << from.first << ", " << from.second << ") 到 (" << to.first << ", " << to.second << ")" << endl;
                    displayBoard();
                    cout << "按回车继续...";
                    cin.ignore();
                    cin.get();
                }
                else {
                    cout << "无效的移动 (" << from.first << ", " << from.second << ") 到 (" << to.first << ", " << to.second << ")，请重试！" << endl;
                    cout << "按回车继续...";
                    cin.ignore();
                    cin.get();
                }
            }
        }

        // 检查游戏是否结束
        if (count(Board & Chess) > count(~Board & allones) && !going(Now) || !going2(Now)) {
            clearScreen();
            displayBoard();
            int player_count = count(Board & Chess);
            int ai_count = count(~Board & allones);
            cout << "游戏结束！" << endl;
            cout << "玩家棋子数: " << player_count << endl;
            cout << "AI棋子数: " << ai_count << endl;
            if (player_count > ai_count) {
                cout << "恭喜你获胜！" << endl;
            }
            else if (ai_count > player_count) {
                cout << "AI获胜！" << endl;
            }
            else {
                cout << "平局！" << endl;
            }
            cout << "按回车退出...";
            cin.ignore();
            cin.get();
            break;
        }

        // AI移动
        if (hasValidMoves({ Now.first, ~Now.second }, false)) {  // 检查AI是否有合法移动
            clearScreen();
            cout << "AI正在思考..." << endl;
            displayBoard();
            PULLS ai_state = getAIMove();
            Now = { ai_state.first, ai_state.second };
            clearScreen();
            cout << "AI移动了" << endl;
            displayBoard();
            cout << "按回车继续...";
            cin.ignore();
            cin.get();
        }
        else {
            clearScreen();
            displayBoard();
            cout << "AI没有合法移动，游戏继续！" << endl;
            cout << "按回车继续...";
            cin.ignore();
            cin.get();
        }

        // 检查游戏是否结束
        if (count(Board & Chess) > count(~Board & allones) && !going(Now) || !going2(Now)) {
            clearScreen();
            displayBoard();
            int player_count = count(Board & Chess);
            int ai_count = count(~Board & allones);
            cout << "游戏结束！" << endl;
            cout << "玩家棋子数: " << player_count << endl;
            cout << "AI棋子数: " << ai_count << endl;
            if (player_count > ai_count) {
                cout << "恭喜你获胜！" << endl;
            }
            else if (ai_count > player_count) {
                cout << "AI获胜！" << endl;
            }
            else {
                cout << "平局！" << endl;
            }
            cout << "按回车退出...";
            cin.ignore();
            cin.get();
            break;
        }
    }

    return 0;
}
