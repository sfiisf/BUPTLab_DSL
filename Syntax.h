/**
 * @file Syntax.h
 * @brief 语法解析器头文件，定义语法解析器和自动机相关类
 * 
 * 该文件声明了 `SyntaxParser` 类和 `Automaton` 类，它们分别用于构建语法树以及根据该语法树进行状态转移处理。
 * 主要功能：
 *  - `SyntaxParser`：从语法文件中读取并解析语法规则，构建状态树
 *  - `Automaton`：基于已构建的语法树执行状态转移，处理输入并生成输出
 * 
 * 关键功能：
 * - 语法规则解析
 * - 状态转移
 * - 编辑距离计算（用于模糊匹配输入）
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

namespace Syntax {

    // 特殊符号定义，用于语法解析的特殊符号
    const std::string BEGIN_SYMBOL = "BEGIN"; // 起始符号
    const std::string EXIT_SYMBOL = "EXIT"; // 退出符号
    const std::string DEFAULT_SYMBOL = "DEFAULT"; // 默认符号

    // 定义保留字及其对应的编号，便于语法解析器进行语法分析
    const std::unordered_map<std::string, int> RemainWord {
        {"STATE", 0}, {"OUT", 1}, {"NEXT", 2}, {"CASE", 3}, {"DEFAULT", 4}, {"BEGIN", 5}, {"EXIT", 6}
    };

    // 定义该状态下的动作信息
    class Action {
        public:
            std::vector<std::string> OutMessage; // 输出信息
            std::string NextState; // 下一状态
    };

    // State 类表示一个状态，包含一张动作表
    class State {
        public:
            std::unordered_map<std::string, Action> ActionTable; // 对应输入 映射 对应动作
    };

    // 解析语法文件，并构建语法树
    class SyntaxParser {
        private:
            std::string SyntaxFileName; // 语法文件名
            std::ifstream SyntaxFile; // 语法文件文件流
            std::string BeginState; // 起始状态信息
            std::unordered_map<std::string, State> StateTable; // 状态表， 用状态名字 映射 状态
            bool Begin; // 是否已经处理起始状态
            bool IsSyntaxTreeReady; // 判断语法树是否构建完成

            void BuildSyntaxTree(void); // 构建语法树

            // 处理各个关键字，进行语法树构建
            bool DealSTATE(const std::vector<std::string>&, std::string&, std::string&);
            bool DealOUT(const std::vector<std::string>&, std::string&, std::string&);
            bool DealNEXT(const std::vector<std::string>&, std::string&, std::string&);
            bool DealCASE(const std::vector<std::string>&, std::string&, std::string&);
            bool DealDEFAULT(const std::vector<std::string>&, std::string&, std::string&);
        
        public:
            // 构造函数：根据文件名初始化语法解析器
            SyntaxParser(const std::string&);

            // 检查语法树构建状体
            bool CheckReady(void);

            // 展示语法树信息
            void ShowSyntaxTree(void);

            // 获取起始状态
            std::string AskBegin(void) const;

            // 根据 状态名 查找 状态
            std::pair<bool, State> AskState(const std::string&);
    };

    // 表示自动机，根据语法树进行状态转移
    class Automaton {
        private:
            SyntaxParser Rules; // 语法解析器对象
            State NowState; // 当前状态
            bool Ready; // 自动机准备状态
            int CalWordDis(const std::string&, const std::string&); // 计算两字符编辑距离

        public:
            // 构造函数：根据语法文件初始化有限自动机
            Automaton(const std::string&, std::vector<std::string>&);
            
            // 检查自动机是否准备好
            bool CheckReady(void) const;

            // 退出自动机
            void Exit(void);

            // 输入并根据当前状态进行状态转移
            void Input(const std::string&, std::vector<std::string>&);
    };
};