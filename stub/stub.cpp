#include "stubSyntax.h"

using namespace Syntax;

int main() {
    std::vector<std::string> buffer;

    // 创建 SyntaxParser 测试桩
    SyntaxParserStub parserStub("syntax1.txt");
    std::cout << "SyntaxParser CheckReady: " << parserStub.CheckReady() << "\n";
    parserStub.ShowSyntaxTree();
    std::cout << "Begin State: " << parserStub.AskBegin() << "\n";

    auto [found, state] = parserStub.AskState("StartState");
    if (found) {
        std::cout << "Found State with Input: TestInput\n";
        for (const auto& msg : state.ActionTable["TestInput"].OutMessage) {
            std::cout << "\tOutput Message: " << msg << "\n";
        }
    }

    // 创建 Automaton 测试桩
    AutomatonStub automatonStub("syntax1.txt", buffer);
    std::cout << "Automaton CheckReady: " << automatonStub.CheckReady() << "\n";
    
    automatonStub.Input("TestInput", buffer);
    for (const auto& msg : buffer) {
        std::cout << msg << "\n";
    }

    automatonStub.Exit();
    return 0;
}