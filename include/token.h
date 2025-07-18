#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace squ {

// Token 类型枚举
enum class TokenType {
    Number,     // 数字，解析为 double 类型
    Assignment, // 赋值类运算符
    Operator,   // 普通运算符
    Identifier, // 标识符
    String,     // 字符串字面量
    Char,       // 字符字面量
    Punctuation // 标点符号
};

// Token 结构体，表示一个词法单元
struct Token {
    TokenType type;    // Token 类型
    std::string value; // Token 字符串值
    double num_value;  // 数字类型的值（仅当 type 为 Number 时有效）
};

// 函数声明
std::string ParsePreprocess(const std::string &input);
char ParseEscape(char c);
std::vector<Token> ParseTokens(const std::string &input);

} // namespace squ