#include "../include/squaker.h"
#include "../include/node.h"
#include "../include/parser.h"
#include "../include/token.h"
#include "../include/type.h"
#include <chrono>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

namespace squ {

    const std::string RESET = "\033[0m";
    const std::string YELLOW = "\033[33m";
    const std::string CYAN = "\033[38;2;121;146;227m";
    const std::string RED = "\033[38;2;161;115;131m";
    const std::string GRAY = "\033[90m";

    void InitConsole() {
        system("chcp 65001");
        system("cls");
    }

    void PrintLOGO() {
        std::vector<std::string> lines = {"██╗      ███████╗ ██████╗ ██╗   ██╗ █████╗ ██╗  ██╗███████╗██████╗",
                                          "╚██╗     ██╔════╝██╔═══██╗██║   ██║██╔══██╗██║ ██╔╝██╔════╝██╔══██╗",
                                          " ╚██╗    ███████╗██║   ██║██║   ██║███████║█████╔╝ █████╗  ██████╔╝",
                                          " ██╔╝    ╚════██║██║▄▄ ██║██║   ██║██╔══██║██╔═██╗ ██╔══╝  ██╔══██╗",
                                          "██╔╝     ███████║╚██████╔╝╚██████╔╝██║  ██║██║  ██╗███████╗██║  ██║",
                                          "╚═╝      ╚══════╝ ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝"};

        int total_width = int(lines[0].size());
        const int r1 = 121, g1 = 146, b1 = 227;
        const int r2 = 136, g2 = 127, b2 = 188;
        const int r3 = 161, g3 = 115, b3 = 131;
        const int gray = 50; // 调整灰色亮度

        // 定义阴影字符集合（UTF-8编码）
        const std::string shadow_chars = "╚═╝╗╔║ ";

        for (const auto &line : lines) {
            int byte_pos = 0;
            for (size_t char_pos = 0; char_pos < line.size();) {
                int char_len = 1;
                if ((line[char_pos] & 0xF0) == 0xF0)
                    char_len = 4;
                else if ((line[char_pos] & 0xE0) == 0xE0)
                    char_len = 3;
                else if ((line[char_pos] & 0xC0) == 0xC0)
                    char_len = 2;

                std::string character = line.substr(char_pos, char_len);
                int r, g, b;

                // 检查当前字符是否是阴影字符
                bool is_shadow = false;
                for (size_t i = 0; i < shadow_chars.size();) {
                    int cmp_len = 1;
                    if ((shadow_chars[i] & 0xF0) == 0xF0)
                        cmp_len = 4;
                    else if ((shadow_chars[i] & 0xE0) == 0xE0)
                        cmp_len = 3;
                    else if ((shadow_chars[i] & 0xC0) == 0xC0)
                        cmp_len = 2;

                    if (character == shadow_chars.substr(i, cmp_len)) {
                        is_shadow = true;
                        break;
                    }
                    i += cmp_len;
                }

                if (is_shadow) {
                    r = g = b = gray;
                } else {
                    float t = static_cast<float>(byte_pos) / (total_width - 1);
                    if (t < 0.5f) {
                        float phase = t * 2.0f;
                        r = r1 + (r2 - r1) * phase;
                        g = g1 + (g2 - g1) * phase;
                        b = b1 + (b2 - b1) * phase;
                    } else {
                        float phase = (t - 0.5f) * 2.0f;
                        r = r2 + (r3 - r2) * phase;
                        g = g2 + (g3 - g2) * phase;
                        b = b2 + (b3 - b2) * phase;
                    }
                }

                r = std::max(0, std::min(255, r));
                g = std::max(0, std::min(255, g));
                b = std::max(0, std::min(255, b));

                printf("\033[38;2;%d;%d;%dm%s", r, g, b, character.c_str());
                char_pos += char_len;
                byte_pos += char_len;
            }
            printf("\033[0m\n");
        }
    }

    void RunTests() {

        // 测试用例
        std::vector<std::string> test_cases = {
            "a = 3 + 4 * (2 - 1) && true",          // 缺少左括号
            "a = 3 + 4 balabala * (2 - 1) && true", // 无效标识符
            "a = 3 + 4)",                           // 多余右括号
            "a = 'x' + \"hello\"",                  // 字符和字符串
            "a = while (!x) 1+1",
            "for (i = 0; i < 10; i++) { i; }",                                            // for循环测试用例
            "for (; i < 10; i++) { i;i+=1 }",                                             // 无初始化表达式
            "for (i = 0; ; i++) { i+=3 }",                                                // 无条件表达式
            "for (i = 0; i < 10; ) { 1-=5;!i }",                                          // 无更新表达式
            "for(i=0; i<10; i++) { for(j=0; j<10; j++) {} }",                             // 嵌套表达式
            "*-i",                                                                        // 访问指针
            "if (x > 0)  y = 1 else if (x < 0) y = -1 else if (x < 0) y = -1 else y = 0", // if-else if-else 测试用例
            "function (x, y) {x + y}",                                                    // Lambda 函数定义
            "(function (x) x * x)(5)",                                                    // Lambda 函数调用
            "add = function (a, b) a + b",                                                // 函数赋值和调用
            "(a+b)()((function (x) x * x), 2)",                                           // 函数赋值和调用
            "function add(x, y) x + y",                                                   // 函数定义测试用例
            "import math",                                                                // 模块导入测试用例
            "import \"math\"",                                        // 模块导入测试用例（字符串形式）
            "for (i = 0; i < 10; i++) { if (i == 5) break; }",        // break 测试用例
            "for (i = 0; i < 10; i++) { if (i % 2 == 0) continue; }", // continue 测试用例
            "return 10",                                              // 返回值测试用例
            "a.b",                                                    // 成员访问测试用例
            "a[2]",                                                   // 索引访问测试用例
            "@print(\"Hello, World!\")",                              // 原生函数调用测试用例
            "a = [1, 2, 3 + 4, 5 * 6]",                               // 基本数组
            "b = [[1,2], [3,4], [5,6]]",                              // 嵌套数组
            "c = [1 + 2, 3 * 4, func(5)]",                            // 包含表达式的数组
            "d = {\"key1\": 42, \"key2\": 3.14}",                     // 基本映射
            "f = {\"arr\": [1,2,3], \"map\": {\"1\":2, \"2\":3}}",    // 嵌套结构
            "g = {\"key\": func(42), \"value\": 3.14 * 2}",           // 包含表达式的映射
            "a = 1; b = 2; function f(x, y) {x + y}; c = 4",          // 变量编号测试
            "a = 1; b = 2; {c = 3; d = 4}; c = 5; e = 6",             // 块表达式测试
        };

        for (const auto &input : test_cases) {
            std::cout << "Parsing: " << input << std::endl;
            try {
                auto tokens = ParseTokens(input);
                squ::Parser parser(tokens);
                auto expr = parser.parse();
                std::cout << "AST: " << expr->string() << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "  Error: " << e.what() << std::endl;
            }
            std::cout << "------------------------" << std::endl;
        }
    }

    // 写一个函数测试求值功能
    void RunEvalTests() {

        // 随机生成四则运算字符串列表：
        int num_tests = 1000000;
        std::vector<std::string> test_cases;
        for (int i = 0; i < num_tests; ++i) {
            std::string expression;          // 示例表达式
            int num_terms = rand() % 10 + 5; // 随机生成5到10个操作数
            for (int j = 0; j < num_terms; ++j) {
                if (j > 0) {
                    int op = rand() % 4; // 随机选择一个运算符
                    if (op == 0) {
                        expression += " * "; // 使用乘法作为运算符
                    } else if (op == 1) {
                        expression += " / "; // 使用除法作为运算符
                    } else if (op == 2) {
                        expression += " - "; // 使用减法作为运算符
                    } else {
                        expression += " + "; // 使用加法作为运算符
                    }
                }
                expression += std::to_string(rand() % 1000 + 1); // 随机生成数字
            }
            test_cases.push_back(expression);
        }

        // 先解析成tokens数组
        auto parse_start = std::chrono::high_resolution_clock::now();
        std::cout << "Parsing tokens..." << std::endl;
        std::vector<std::vector<squ::Token>> tokens;
        for (auto input : test_cases) {
            try {
                auto parsed_tokens = ParseTokens(input);
                tokens.push_back(parsed_tokens);
            } catch (const std::exception &e) {
                std::cerr << "Error parsing input: " << input << " - " << e.what() << std::endl;
            }
        }
        auto parse_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> parse_elapsed = parse_end - parse_start;
        std::cout << "Parsing completed in " << parse_elapsed.count() << " seconds." << std::endl;

        // 计算用时
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto &token_list : tokens) {
            try {
                squ::VM vm;                     // 创建一个新的环境
                squ::Parser parser(token_list); // 创建解析器并传入 VM
                auto expr = parser.parse();
                auto result = expr->evaluate(vm); // 调用求值接口
            } catch (const std::exception &e) {
                std::cerr << "Error evaluating expression: " << e.what() << std::endl;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Evaluation completed in " << elapsed.count() << " seconds." << std::endl;

        // 与C++速度做对比
        auto cpp_start = std::chrono::high_resolution_clock::now();
        std::cout << "Running C++ evaluation..." << std::endl;
        for (int i = 0; i < num_tests; ++i) {
            volatile int r = rand();
        }
        auto cpp_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> cpp_elapsed = cpp_end - cpp_start;
        std::cout << "C++ evaluation completed in " << cpp_elapsed.count() << " seconds." << std::endl;
    }

    // 检查括号匹配
    bool isBalanced(const std::vector<Token> &tokens) {
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

    // 独立的交互式代码执行函数
    void InteractiveExecution() {
        std::string input_buffer;
        squ::VM vm;         // 创建一个新的环境
        squ::Parser parser; // 创建一个空解析器
        vm.enter(10);       // 预留足够的局部变量空间

        for (int i = 0;; ++i) {
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
                    // 计算用时
                    std::cout << CYAN;
                    auto start = std::chrono::high_resolution_clock::now();
                    auto tokens = ParseTokens(input_buffer);
                    std::cout << PrintTokens(tokens) << std::endl;
                    parser.reset(std::move(tokens)); // 重置解析器
                    auto expr = parser.parse();      // 解析表达式
                    std::cout << "AST: " << expr->string() << std::endl;
                    auto result = expr->evaluate(vm); // 调用求值接口
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

    void RunScriptTests() {
        std::string input_buffer;
        Script script;

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

    // 脚本类的实现
    Script::Script() : current_index(0) {
        vm.enter(1024); // 预留足够的局部变量空间
    }

    void Script::append(const std::string &append_code) {
        code.emplace_back(append_code);
    }

    ValueData Script::execute() {
        // 初始化
        auto result = ValueData{ValueType::Nil, false, 0.0};

        // 逐行执行
        for (; current_index < code.size(); ++current_index) {
            const auto &code = this->code[current_index];
            // 解析tokens
            auto tokens = ParseTokens(code);

            // 解释器实例化
            parser.reset(std::move(tokens)); // 重置解析器
            auto expr = parser.parse();      // 解析表达式

            // 执行表达式
            result = expr->evaluate(vm); // 调用求值接口
        }

        // 返回结果
        return result;

        // 返回一个空的 ValueData
        return ValueData{ValueType::Nil, false, 0.0};
    }

} // namespace squ