#include <iostream>
#include <string>
#include <vector>
#include "../include/token.h"
#include "../include/type.h"
#include "../include/node.h"
#include "../include/parser.h"

namespace squ {

    void InitConsole() {
        system("chcp 65001");
        system("cls");
    }

    void PrintLOGO() {
        std::vector<std::string> lines = {
            "██╗      ███████╗ ██████╗ ██╗   ██╗ █████╗ ██╗  ██╗███████╗██████╗",
            "╚██╗     ██╔════╝██╔═══██╗██║   ██║██╔══██╗██║ ██╔╝██╔════╝██╔══██╗",
            " ╚██╗    ███████╗██║   ██║██║   ██║███████║█████╔╝ █████╗  ██████╔╝",
            " ██╔╝    ╚════██║██║▄▄ ██║██║   ██║██╔══██║██╔═██╗ ██╔══╝  ██╔══██╗",
            "██╔╝     ███████║╚██████╔╝╚██████╔╝██║  ██║██║  ██╗███████╗██║  ██║",
            "╚═╝      ╚══════╝ ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝"};

        int total_width = lines[0].size();
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
			"a = 3 + 4 * (2 - 1) && true",  // 缺少左括号
			"a = 3 + 4 balabala * (2 - 1) && true",  // 无效标识符
			"a = 3 + 4)",  // 多余右括号
			"a = 'x' + \"hello\"",  // 字符和字符串
			"a = while (!x) 1+1",
			"for (i = 0; i < 10; i++) { i; }", // for循环测试用例
			"for (; i < 10; i++) { i;i+=1 }", // 无初始化表达式
			"for (i = 0; ; i++) { i+=3 }", // 无条件表达式
			"for (i = 0; i < 10; ) { 1-=5;!i }", // 无更新表达式
			"for(i=0; i<10; i++) { for(j=0; j<10; j++) {} }", // 嵌套表达式
			"*-i", // 访问指针
			"if (x > 0)  y = 1 else if (x < 0) y = -1 else if (x < 0) y = -1 else y = 0", // if-else if-else 测试用例
			"lambda (x, y) -> {x + y}", // Lambda 函数定义
			"(lambda (x) -> x * x)(5)", // Lambda 函数调用
			"add = lambda (a, b) -> a + b", // 函数赋值和调用
			"(a+b)()((lambda (x) -> x * x), 2)", // 函数赋值和调用
			"function add(x, y) { x + y; }", // 函数定义测试用例
			"import math", // 模块导入测试用例
			"import \"math\"", // 模块导入测试用例（字符串形式）
			"for (i = 0; i < 10; i++) { if (i == 5) break; }", // break 测试用例
			"for (i = 0; i < 10; i++) { if (i % 2 == 0) continue; }", // continue 测试用例
			"return 10", // 返回值测试用例
			"a.b", // 成员访问测试用例
			"a[2]", // 索引访问测试用例
			"@print(\"Hello, World!\")", // 原生函数调用测试用例
			"a = [1, 2, 3 + 4, 5 * 6]",  // 基本数组
			"b = [[1,2], [3,4], [5,6]]",  // 嵌套数组
			"c = [1 + 2, 3 * 4, func(5)]",  // 包含表达式的数组
			"d = {\"key1\": 42, \"key2\": 3.14}",  // 基本映射
			"f = {\"arr\": [1,2,3], \"map\": {\"1\":2, \"2\":3}}",  // 嵌套结构
			"g = {\"key\": func(42), \"value\": 3.14 * 2}"  // 包含表达式的映射
		};

		for (const auto& input : test_cases) {
			std::cout << "Parsing: " << input << std::endl;
			try {
				auto tokens = ParseTokens(input);
				squ::Parser parser(tokens);
				auto expr = parser.parse();
				std::cout << "AST: " << expr->toString() << std::endl;
			} catch (const std::exception& e) {
				std::cerr << "  Error: " << e.what() << std::endl;
			}
			std::cout << "------------------------" << std::endl;
		}
    }

    // 写一个函数测试求值功能
    void RunEvalTests() {
        std::vector<std::string> test_cases = {
            "3 + 4 * (2 - 1)",
            "a + 5",
            "b * 2",
            "c / 3",
            "d - 1",
            "e % 2",
            "f == 0",
            "g && true",
            "h || false"
        };

        for (const auto& input : test_cases) {
            std::cout << "Evaluating: " << input << std::endl;
            try {
                auto tokens = ParseTokens(input);
                squ::Parser parser(tokens);
                auto expr = parser.parse();
                auto result = expr->evaluate();
                std::cout << "Result: " << result.to_string() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "  Error: " << e.what() << std::endl;
            }
            std::cout << "------------------------" << std::endl;
        }
    }

} // namespace squ