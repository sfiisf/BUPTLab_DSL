/**
 * @file DSL.cpp
 * @brief 实现客服机器人功能
 * 
 *  读入自定义语法信息文本，据此对输入做出相应反应
 */

#include <windows.h>
#include <ctime>
#include <iomanip>
#include "Syntax.h"
using namespace Syntax;

// 控制台颜色代码
constexpr WORD COLOR_SERVER = 0x01;    // 蓝色
constexpr WORD COLOR_CLIENT = 0x04;   // 红色
constexpr WORD COLOR_DEFAULT = 0x07; // 默认颜色

/**
 * @brief 打印当前时间并设置输出颜色
 * @param color 控制台文本颜色代码
 * 
 * 该函数在输出当前时间戳之前，设置控制台文本颜色，输出时间格式为 [HH:MM:SS]。
 * 在打印结束后，恢复默认控制台颜色。
 */
void PrintDialogTime(WORD color) {
    HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
    SetConsoleTextAttribute(outHandle, color); // 设置文本颜色

    // 获取当前时间
    time_t now = time(0);
    auto it = localtime(&now);

    // 形式化输出当前时间[HH:MM:SS]
    std::cout << '[' << std::setw(2) << std::setfill('0') << it->tm_hour << ':'
              << std::setw(2) << std::setfill('0') << it->tm_min << ':'
              << std::setw(2) << std::setfill('0') << it->tm_sec << "]";

    SetConsoleTextAttribute(outHandle, COLOR_DEFAULT); // 恢复默认颜色
}


/**
 * @brief 主函数：根据命令行参数启动不同的调试模式
 * @param argc 参数数量
 * @param argv 参数数组
 * @return 程序执行状态
 * 
 * 程序支持两种模式：
 *  1. 交互式命令行调试：用户输入语法文件名，手动输入命令与自动机交互。
 *  2. 文件输入输出调试：通过指定语法文件、输入文件和输出文件，实现自动机的批量处理。
 */
int main(int argc, char* argv[]) {
    // 仅接受语法文件 进入交互式调试
    if (argc == 2) {
        std::string FileName(argv[1]); // 获取语法文件名
        std::vector<std::string> Buffer; // 输出缓冲区
        Automaton DSL(FileName, Buffer); // 构建自动机

        // 进入交互测试
        while (DSL.CheckReady()) {
            HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            for (const auto& s : Buffer) {
                PrintDialogTime(COLOR_SERVER); // 打印当前时间
                std::cout << s << '\n';
            }

            // 读入信息并根子输入进行状态转移
            PrintDialogTime(COLOR_CLIENT);
            std::string s;
            std::cin >> s;
            DSL.Input(s, Buffer);
        }

        // 将缓冲区信息全部输出
        if (!Buffer.empty()) {
            for (const auto& s : Buffer) {
                PrintDialogTime(COLOR_SERVER);
                std::cout << s << '\n';
            }
        }
    } else if (argc == 4) {
        // 接受语法文件 输入文件 输出文件，进入文件输入输出调试

        std::string SyntaxFileName(argv[1]); // 语法文件名
        std::string InputFileName(argv[2]); // 输入文件名
        std::string OutputFileName(argv[3]); // 输出文件名

        // 打开输入输出文件
        std::ifstream InputFile(InputFileName);
        std::ofstream OutputFile(OutputFileName);

        // 输入输出文件打开错误处理
        if(!InputFile.is_open()) {
            std::cerr << "错误：无法打开输入文件 \"" << InputFileName << "\"。请检查文件路径和权限。\n";
            std::cerr << "正确用法：\n";
            std::cerr << "  程序 <语法文件> <输入文件> <输出文件>\n";
            exit(1);
        }
        if(!OutputFile.is_open()) {
            std::cerr << "错误：无法打开输出文件 \"" << OutputFileName << "\"。请检查文件路径和权限。\n";
            std::cerr << "正确用法：\n";
            std::cerr << "  程序 <语法文件> <输入文件> <输出文件>\n";
            exit(1);
        }

        std::vector<std::string> Buffer; // 输出缓冲区
        Automaton DSL(SyntaxFileName, Buffer); // 构建自动机

        // 从输入文件逐行读取输入，进行状态转移
        while (DSL.CheckReady()) {
            for (const auto& s : Buffer) {
                // 将自动机的输出信息写入文件
                OutputFile << s << '\n';
            }

            // 从输入文件读取输入 并根据输入进行状态转移
            std::string s;
            InputFile >> s;
            DSL.Input(s, Buffer);
        }

        // 将缓冲区消息全部清空
        if (!Buffer.empty()) {
            for (const auto& s : Buffer) {
                OutputFile << s << '\n';
            }
        }
    } else {
        // 处理错误用法 输出使用方法帮助信息

        std::cerr << "错误：命令行参数错误。\n";
        std::cerr << "正确用法：\n";
        std::cerr << "\t命令行调试：\n";
        std::cerr << "\t\t程序 <文件名>\n";
        std::cerr << "\t重定向输入输出文件：\n";
        std::cerr << "\t\t程序 <语法文件> <输入文件> <输出文件>\n";
        exit(1);
    }
    return 0;
}