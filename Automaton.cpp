/**
 * @file Automaton.cpp
 * @brief 语法解析自动机实现
 * 
 * 该文件实现了 `Automaton` 类
 * 主要用于对应语法树自动机处理
 * 主要功能：
 *  - 根据输入处理状态转移
 *  - 编辑距离计算（用于模糊匹配输入）
 */

#include "Syntax.h"

using namespace Syntax;


/**
 * @brief 构造函数，初始化自动机并开始处理输入
 * 
 * @param FileName 语法文件名。
 * @param Buffer 存储自动机输出的消息。
 */
Automaton::Automaton(const std::string& FileName, std::vector<std::string>& Buffer) : Rules(FileName) {
    Buffer.clear();
    if (!Rules.CheckReady()) {
        std::cerr << "错误 : 构建语法树错误\n";
        Ready = false;
    } else {
        std::string NxtState = Rules.AskBegin();
        auto [ok, tmpState] = Rules.AskState(NxtState);
        if (ok) {
            Ready = true;
            do {
                auto [ok, tmpState] = Rules.AskState(NxtState);
                if (ok) {
                    NowState = tmpState;
                } else {
                    NowState = Rules.AskState(DEFAULT_SYMBOL).second;
                }
                // 将输出信息传递至 Buffer 中
                for (const auto& s : NowState.ActionTable[BEGIN_SYMBOL].OutMessage) {
                    Buffer.push_back(s);
                }
                NxtState = NowState.ActionTable[BEGIN_SYMBOL].NextState;
            } while (!NxtState.empty());
        } else {
            Ready = false;
            std::cerr << "错误 : 无法找到初始状态\n";
        }
    }
}

/**
 * @brief 检查自动机是否准备好
 * 
 * @return true 如果自动机已准备好，false 如果自动机未准备好。
 */
bool Automaton::CheckReady(void) const {
    return Ready;
}

/**
 * @brief 退出自动机
 * 
 * 将自动机状态设置为 false。
 */
void Automaton::Exit(void) {
    Ready = false;
}

/**
 * @brief 计算编辑距离（Levenshtein距离）。
 * 
 * 该算法用于计算两个字符串之间的最小编辑操距离
 * 
 * @param temp 待匹配的第一个字符。
 * @param check 待匹配的第二个字符。
 * 
 * @return 返回两个字符串的编辑距离。
 */
int Automaton::CalWordDis(const std::string& temp, const std::string& check) {
    int len1 = temp.size();
    int len2 = check.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int> (len2 + 1));
    
    // 初始化边界
    for (int i = 0; i <= len1; i++) {
        dp[i][0] = i;
    }
    for (int i = 0; i <= len2; i++) {
        dp[0][i] = i;
    }

    // 计算编辑距离
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (temp[i - 1] == check[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]}) + 1;
            }
        }
    }
    return dp[len1][len2];
}

/**
 * @brief 处理输入并执行状态转移
 * 
 * 该函数根据输入的字符串和当前状态，计算最匹配的状态并进行状态转移。
 * 
 * @param input 输入字符串。
 * @param Buffer 存储输出结果的容器。
 */
void Automaton::Input(const std::string& input, std::vector<std::string>& Buffer) {
    constexpr int dif = 0; // 允许的匹配误差
    int minDis = INT_MAX;
    Buffer.clear();

    std::string MatchWord = "DEFAULT";

    // 遍历当前状态的所有输入，计算与输入的编辑距离，寻找最佳匹配
    for (auto [temp, action] : NowState.ActionTable) {
        if (temp == BEGIN_SYMBOL || temp == DEFAULT_SYMBOL) {
            continue;
        }
        int dis = CalWordDis(temp, input);
        int expect = abs(int(temp.size() - input.size()));
        if (dis > expect + dif) {
            continue;
        }
        if (dis < minDis) {
            minDis = dis;
            MatchWord = temp;
        }
    }

    // 根据最匹配的输入执行状态转移
    for (const auto& s : NowState.ActionTable[MatchWord].OutMessage) {
        Buffer.push_back(s);
    }
    std::string NxtState = NowState.ActionTable[MatchWord].NextState;
    
    // 处理退出符号 EXIT
    if (NxtState == EXIT_SYMBOL) {
        Exit();
        return;
    }
    
    // 处理状态转移输出信息
    do {
        auto [ok, tmpState] = Rules.AskState(NxtState);
        if (ok) {
            NowState = tmpState;
        } else {
            NowState = Rules.AskState(DEFAULT_SYMBOL).second;
        }
        for (const auto& s : NowState.ActionTable[BEGIN_SYMBOL].OutMessage) {
            Buffer.push_back(s);
        }
        NxtState = NowState.ActionTable[BEGIN_SYMBOL].NextState;
    } while (!NxtState.empty());
}