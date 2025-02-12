/**
 * @file stubSyntax.h
 * @brief 语法解析器测试桩头文件，定义语法解析器和自动机相关类
 * 
 * 该文件定义了 `SyntaxParser` 类和 `Automaton` 类，它们分别用于构建语法树以及根据该语法树进行状态转移处理。
 * 以及 `SyntaxParser` 的测试桩子类  `SyntaxParserStub` 和 `Automaton` 的测试桩子类 `AutomatonStub` ，分别用于对 `SyntaxParser` 与 `Automaton` 类定义的函数进行测试
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
        {"STATE", 0}, {"OUT", 1}, {"NEXT", 2}, {"CASE", 3}, {"DEFAULT", 4}
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

    // SyntaxParser类测试桩
    class SyntaxParserStub : public SyntaxParser {
        public:
            SyntaxParserStub(const std::string& FileName) : SyntaxParser(FileName) {}
            
            bool CheckReady(void) {
                return true; // 固定返回语法树已就绪
            }

            void ShowSyntaxTree(void) {
                std::cout << "Syntax Tree Stub Output\n"; // 模拟输出
            }

            std::string AskBegin(void) const {
                return "StartState"; // 固定返回一个起始状态
            }

            std::pair<bool, State> AskState(const std::string& ask) {
                if (ask == "StartState") { // 模拟查询对应状态
                    State testState;
                    Action testAction;
                    testAction.OutMessage.push_back("Output Message");
                    testAction.NextState = "NextState";
                    testState.ActionTable["TestInput"] = testAction;
                    return {true, testState};
                }
                return {false, State()};// 模拟查询失败
            }
    };

    // Automaton类测试桩
    class AutomatonStub : public Automaton {
    public:
        AutomatonStub(const std::string& FileName, std::vector<std::string>& Buffer) : Automaton(FileName, Buffer) {
            Buffer.push_back("Automaton Initialized");
        }

        bool CheckReady(void) const {
            return true; // 固定返回自动机已就绪
        }

        void Input(const std::string& input, std::vector<std::string>& Buffer) {
            Buffer.push_back("Processed Input: " + input); // 模拟输入处理结果
        }

        void Exit(void) {
            std::cout << "Automaton Exited\n"; // 模拟退出自动机
        }
    };
};