/**
 * @file Syntax.cpp
 * @brief 语法解析器实现，解析并构建状态转移树
 * 
 * 该文件实现了 `SyntaxParser`
 * 主要用于构建语法树
 * 主要功能：
 *  - 构建语法树
 */

#include "Syntax.h"

using namespace Syntax;

/**
 * @brief SyntaxParser 构造函数，负责初始化语法解析器。
 * 
 * 该构造函数尝试打开指定的文件，并根据文件内容构建语法树。
 * 如果文件打开失败，则设置 `IsSyntaxTreeReady` 为 `false`。
 * 
 * @param FileName 文件名，用于打开语法文件。
 */
SyntaxParser::SyntaxParser(const std::string& FileName) : SyntaxFileName(FileName) {
    SyntaxFile.open(SyntaxFileName);
    if (SyntaxFile.fail()) {
        std::cerr << "错误：文件" << FileName << "打开失败\n";
        IsSyntaxTreeReady = false;
    } else {
        BuildSyntaxTree();
    }
    SyntaxFile.close();
}

/**
 * @brief 构建语法树
 * 
 * 该函数读取语法文件中的每一行，将每行的关键字进行解析，构建相应的状态转移树。
 * 解析每一行并调用不同的处理函数（例如 `DealSTATE`、`DealOUT` 等），
 * 若发现错误则打印错误信息并停止构建。
 */
void SyntaxParser::BuildSyntaxTree(void) {
    IsSyntaxTreeReady = true; // 初始化语法树状态
    Begin = false; // 是否找到开始状态标志
    int LineNum = 1;
    std::string Line;
    std::string NowState;
    std::string NowChoice;
    // 逐行处理语法文件
    while (getline(SyntaxFile, Line)) {
        std::istringstream input(Line);
        std::vector<std::string> TokenBag;
        std::string token;

        // 将每行进行分词处理
        while (input >> token) {
            if (!token.empty()) {
                TokenBag.push_back(token);
            }
        }

        // 跳过空行
        if (TokenBag.empty()) {
            LineNum++;
            continue;
        }

        // 关键字查找
        auto it = RemainWord.find(TokenBag.front());
        if (it == RemainWord.end()) {
            IsSyntaxTreeReady = false;
            std::cerr << "错误关键字: " << TokenBag.front() << " in Line: " << LineNum << '\n';
            return;
        }

        // 关键字处理
        int WordNum = it -> second;
        switch(WordNum) {
            case 0: IsSyntaxTreeReady &= DealSTATE(TokenBag, NowState, NowChoice); break;
            case 1: IsSyntaxTreeReady &= DealOUT(TokenBag, NowState, NowChoice); break;
            case 2: IsSyntaxTreeReady &= DealNEXT(TokenBag, NowState, NowChoice); break;
            case 3: IsSyntaxTreeReady &= DealCASE(TokenBag, NowState, NowChoice); break;
            case 4: IsSyntaxTreeReady &= DealDEFAULT(TokenBag, NowState, NowChoice); break;
        }

        // 分析解析结果，并给出错误信息
        if (!IsSyntaxTreeReady) {
            std::cerr << "错误用法: \"";
            for (auto s : TokenBag) {
                std::cerr << s << ' ';
            }
            std::cerr << "\" in Line: " << LineNum << '\n';
            return;
        }
        LineNum++;
    }
}

/**
 * @brief 处理 STATE 关键字
 * 
 * 该函数用于处理解析到的 "STATE" 关键字，并将其设置为当前状态。
 * 
 * @param TokenBag 解析出的词袋，包含关键字和参数。
 * @param NowState 当前状态的引用，更新为当前解析的状态。
 * @param NowChoice 当前选择的引用，更新为初始符号。
 * @return true 如果处理成功，false 如果出现错误。
 */
bool SyntaxParser::DealSTATE(const std::vector<std::string>& TokenBag, std::string& NowState, std::string& NowChoice) {
    // 对 TokenBag 内容进行规范判断
    if (TokenBag.size() != 2 || RemainWord.find(TokenBag[1]) != RemainWord.end() || StateTable.find(TokenBag[1]) != StateTable.end()) {
        return false;
    }

    NowState = TokenBag[1];
    if (!Begin) {
        BeginState = NowState;
        Begin = true;
    }

    NowChoice = BEGIN_SYMBOL; // 起始化符号信息
    return true;
}

/**
 * @brief 处理 OUT 关键字
 * 
 * 该函数用于处理 "OUT" 关键字，将其后的消息保存到当前状态的 ActionTable 中。
 * 
 * @param TokenBag 解析出的词袋，包含关键字和输出信息。
 * @param NowState 当前状态的引用。
 * @param NowChoice 当前选择的引用。
 * @return true 如果处理成功，false 如果出现错误。
 */
bool SyntaxParser::DealOUT(const std::vector<std::string>& TokenBag, std::string& NowState, std::string& NowChoice) {
    // 对 TokenBag 内容进行规范判断
    if (NowChoice.empty() || NowState.empty() || TokenBag.size() <= 1 || TokenBag[1].front() != '\"') {
        return false;
    }

    // 拼接并判断输出信息是否规范
    std::string tmp = TokenBag[1];
    for (int i = 2; i < TokenBag.size(); i++) {
        tmp += ' ' + TokenBag[i];
    }
    if (tmp.front() != '\"' || tmp.back() != '\"') {
        return false;
    }

    StateTable[NowState].ActionTable[NowChoice].OutMessage.push_back(tmp.substr(1, tmp.size() - 2));
    return true;
}

/**
 * @brief 处理 NEXT 关键字
 * 
 * 该函数用于处理 "NEXT" 关键字，更新当前状态的下一状态。
 * 
 * @param TokenBag 解析出的词袋，包含关键字和目标状态。
 * @param NowState 当前状态的引用。
 * @param NowChoice 当前选择的引用。
 * @return true 如果处理成功，false 如果出现错误。
 */
bool SyntaxParser::DealNEXT(const std::vector<std::string>& TokenBag, std::string& NowState, std::string& NowChoice) {
    // 对 TokenBag 内容进行规范判断
    if (NowState.empty() || TokenBag.size() != 2 || (TokenBag[1] != EXIT_SYMBOL && RemainWord.find(TokenBag[1]) != RemainWord.end())) {
        return false;
    }

    StateTable[NowState].ActionTable[NowChoice].NextState = TokenBag[1];
    return true;
}

/**
 * @brief 处理 CASE 关键字
 * 
 * 该函数用于处理 "CASE" 关键字，将其后的选择值提取出来并更新当前选择。
 * 
 * @param TokenBag 解析出的词袋，包含关键字和选择值。
 * @param NowState 当前状态的引用。
 * @param NowChoice 当前选择的引用。
 * @return true 如果处理成功，false 如果出现错误。
 */
bool SyntaxParser::DealCASE(const std::vector<std::string>& TokenBag, std::string& NowState, std::string& NowChoice) {
    // 对 TokenBag 内容进行规范判断
    if (NowState.empty() || TokenBag.size() != 2 || TokenBag[1].size() <= 2 || TokenBag[1].front() != '\"' || TokenBag[1].back() != '\"' || RemainWord.find(TokenBag[1]) != RemainWord.end()) {
        return false;
    }

    NowChoice = TokenBag[1].substr(1, TokenBag[1].size() - 2);
    return true;
}

/**
 * @brief 处理 DEFAULT 关键字
 * 
 * 该函数用于处理 "DEFAULT" 关键字，将其设置为默认选择。
 * 
 * @param TokenBag 解析出的词袋，应该只有一个元素 "DEFAULT"。
 * @param NowState 当前状态的引用。
 * @param NowChoice 当前选择的引用。
 * @return true 如果处理成功，false 如果出现错误。
 */
bool SyntaxParser::DealDEFAULT(const std::vector<std::string>& TokenBag, std::string& NowState, std::string& NowChoice) {
    if (NowState.empty() || TokenBag.size() != 1) {
        return false;
    }
    NowChoice = "DEFAULT";
    return true;
}

/**
 * @brief 检查语法树是否已准备好
 * 
 * @return true 如果语法树已准备好，false 如果语法树构建失败。
 */
bool SyntaxParser::CheckReady(void) {
    return IsSyntaxTreeReady;
}

/**
 * @brief 输出语法树
 * 
 * 该函数用于输出语法树的状态信息，包括每个状态的输入、输出和下一状态。
 */
void SyntaxParser::ShowSyntaxTree(void) {
    std::cout << "开始状态: " << BeginState << '\n';
    for (const auto& [StateName, St] : StateTable) {
        std::cout << "--------------------------\n";
        std::cout << "状态: " << StateName << '\n';
        for (const auto& [input, act] : St.ActionTable) {
            std::cout << "\t输入:" << input << '\n';
            for (const auto& output : act.OutMessage) {
                std::cout << "\t\t输出: " << output << '\n';
            }
            std::cout << "\t\t下一状态: " << act.NextState << '\n';
        }
        std::cout << "--------------------------\n";
    }
}

/**
 * @brief 获取起始状态
 * 
 * @return 返回起始状态。
 */
std::string SyntaxParser::AskBegin() const {
    return BeginState;
}

/**
 * @brief 根据状态名称查询状态
 * 
 * @param ask 要查询的状态名称。
 * @return 返回一个包含查询结果的 pair<bool, State>，first 中 true 表示找到该状态，false 表示未找到； second 为查找结果。
 */
std::pair<bool, State> SyntaxParser::AskState(const std::string& ask) {
    if (StateTable.find(ask) == StateTable.end()) {
        return {false, State({})};
    } else {
        return {true, StateTable[ask]};
    }
}