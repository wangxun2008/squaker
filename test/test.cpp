#include "squaker.h"

#include <chrono>
#include <cmath>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

double sqrt(double x) {
    return std::sqrt(x);
}

#include <iostream>
const std::string RESET = "\033[0m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[38;2;121;146;227m";
const std::string RED = "\033[38;2;161;115;131m";
const std::string GRAY = "\033[90m";

// 检查括号匹配
bool isBalanced(const std::vector<squ::Token> &tokens) {
    using namespace squ;
    std::stack<char> brackets;

    for (const auto &token : tokens) {
        if (token.type == TokenType::Punctuation) {
            const char c = token.value[0];
            if (c == '(' || c == '{' || c == '[') {
                brackets.push(c);
            } else if (c == ')') {
                if (brackets.empty() || brackets.top() != '(')
                    return false;
                brackets.pop();
            } else if (c == '}') {
                if (brackets.empty() || brackets.top() != '{')
                    return false;
                brackets.pop();
            } else if (c == ']') {
                if (brackets.empty() || brackets.top() != '[')
                    return false;
                brackets.pop();
            }
        }
    }
    return brackets.empty();
}

// 检查代码块完整性
bool isCompleteBlock(const std::string &code) {
    using namespace squ;
    try {
        auto tokens = ParseTokens(code);

        // 1. 检查括号匹配
        if (!isBalanced(tokens))
            return false;

        // 2. 检查语句结尾（最后token是否为分号或块结束符）
        if (!tokens.empty()) {
            const auto &last = tokens.back();
            if (last.type == TokenType::Punctuation) {
                if (last.value == ";" || last.value == "}" || last.value == "{")
                    return true;
            }
        }

        // 3. 检查赋值运算符是否完整
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::Assignment && tokens[i].value != "++" && tokens[i].value != "--") {
                // 赋值运算符后必须有表达式
                if (i == tokens.size() - 1)
                    return false;
            }
        }

        return true;
    } catch (const std::exception &ex) {
        throw std::runtime_error(ex.what());
    }
}

int main() {
    std::string input_buffer;
    squ::Script script;

    auto math = squ::Namespace(
        "math", squ::Function("sin", [](double x) { return std::sin(x); }),
        squ::Function("cos", [](double x) { return std::cos(x); }),
        squ::Function("tan", [](double x) { return std::tan(x); }),
        squ::Namespace("constants", 
            squ::Constant("pi", 3.14159265358979323846), 
            squ::Constant("e", 2.71828182845904523536)
        )
    );

    script.register_identifier(math); // 修改为直接接受 IdentifierData

    while (true) {
        std::string line;

        // 显示智能提示符
        if (input_buffer.empty()) {
            std::cout << YELLOW << ">>> " << RESET; // 首行提示符
        } else {
            std::cout << YELLOW << "... " << RESET; // 多行提示符
        }

        std::getline(std::cin, line);

        // 添加换行符保证块识别正确
        if (!input_buffer.empty() && input_buffer.back() != '\n') {
            input_buffer += ' ';
        }
        input_buffer += line;

        // 自动检测代码完整性
        try {
            if (isCompleteBlock(input_buffer)) {
                script.append(input_buffer);
                std::cout << CYAN;
                auto start = std::chrono::high_resolution_clock::now();
                auto result = script.execute();
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                std::cout << GRAY << "(return: " << CYAN << result.string() << GRAY << ", time: " << RED
                          << elapsed.count() * 1000 << "ms" << GRAY << ")" << std::endl;
                input_buffer.clear();
            }
        } catch (const std::exception &ex) {
            // 完整代码执行出错时清空缓冲区
            std::cout << RED << ex.what() << RESET << std::endl;
            input_buffer.clear();
        }
    }
}