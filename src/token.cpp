#include "../include/token.h"
#include <cctype>
#include <cstdlib>
#include <map>
#include <memory>
#include <variant>

namespace squ {

    // 预处理函数，移除注释并保留字符串和字符字面量中的内容
    std::string ParsePreprocess(const std::string &input) {
        std::string output; // 预处理后的输出字符串
        enum class State { Normal,
                           String,
                           Char,
                           LineComment,
                           BlockComment }; // 状态枚举
        State state = State::Normal;       // 初始状态为 Normal

        for (size_t i = 0; i < input.size(); ++i) {
            char c = input[i];
            switch (state) {
            case State::Normal:                         // 正常状态
                if (c == '/' && i + 1 < input.size()) { // 检测到 '/'，可能是注释的开始
                    if (input[i + 1] == '/') {          // 单行注释
                        state = State::LineComment;
                        ++i;                          // 跳过 '/'
                    } else if (input[i + 1] == '*') { // 块注释
                        state = State::BlockComment;
                        ++i; // 跳过 '/'
                    } else
                        output += c; // 不是注释，保留 '/'
                } else {
                    if (c == '"')
                        state = State::String; // 进入字符串状态
                    else if (c == '\'')
                        state = State::Char; // 进入字符状态
                    output += c;             // 保留当前字符
                }
                break;

            case State::LineComment: // 单行注释状态
                if (c == '\n') {     // 遇到换行符，结束单行注释
                    state = State::Normal;
                    output += c; // 保留换行符
                }
                break;

            case State::BlockComment:                                          // 块注释状态
                if (c == '*' && i + 1 < input.size() && input[i + 1] == '/') { // 检测到块注释结束
                    state = State::Normal;
                    ++i; // 跳过 '/'
                }
                break;

            case State::String:
            case State::Char:                                      // 字符及字符串状态
                output += c;                                       // 保留当前字符
                if (c == '\\' && i + 1 < input.size()) {           // 处理转义字符
                    output += input[++i];                          // 保留转义字符后的字符
                } else if ((state == State::String && c == '"') || // 检测到字符串结束
                           (state == State::Char && c == '\'')) {  // 检测到字符结束
                    state = State::Normal;                         // 返回正常状态
                }
                break;
            }
        }
        if (state == State::BlockComment)
            throw std::runtime_error("Unclosed block comment"); // 未闭合块注释
        return output;
    }

    // 解析转义字符
    char ParseEscape(char c) {
        static const std::unordered_map<char, char> escapes = {// 转义字符映射表
                                                               {'n', '\n'},
                                                               {'t', '\t'},
                                                               {'r', '\r'},
                                                               {'0', '\0'},
                                                               {'\'', '\''},
                                                               {'"', '"'},
                                                               {'\\', '\\'}};
        if (!escapes.count(c))
            throw std::runtime_error(std::string("Invalid escape sequence: \\") + c); // 无效转义序列
        return escapes.at(c);                                                         // 返回对应的转义字符
    }

    // 解析输入字符串为 Token 列表
    std::vector<Token> ParseTokens(const std::string &input) {
        std::vector<Token> tokens;                            // 存储解析出的 Token
        size_t i = 0;                                         // 当前解析位置
        const std::string processed = ParsePreprocess(input); // 预处理输入字符串
        // 运算符定义（按长度降序）
        static const std::vector<std::pair<std::string, TokenType>> OPERATORS = {
            {">>=", TokenType::Assignment}, {"<<=", TokenType::Assignment}, {"+=", TokenType::Assignment}, {"-=", TokenType::Assignment}, {"*=", TokenType::Assignment}, {"/=", TokenType::Assignment}, {"%=", TokenType::Assignment}, {"&=", TokenType::Assignment}, {"|=", TokenType::Assignment}, {"^=", TokenType::Assignment}, {"++", TokenType::Assignment}, {"--", TokenType::Assignment}, {"<=>", TokenType::Operator}, {"...", TokenType::Operator}, {"->*", TokenType::Operator}, {"->", TokenType::Operator}, {"==", TokenType::Operator}, {"!=", TokenType::Operator}, {"<=", TokenType::Operator}, {">=", TokenType::Operator}, {"&&", TokenType::Operator}, {"||", TokenType::Operator}, {"<<", TokenType::Operator}, {">>", TokenType::Operator}, {".*", TokenType::Operator}, {"::", TokenType::Operator}, {"=", TokenType::Assignment}, {"+", TokenType::Operator}, {"-", TokenType::Operator}, {"*", TokenType::Operator}, {"/", TokenType::Operator}, {"<", TokenType::Operator}, {">", TokenType::Operator}, {"&", TokenType::Operator}, {"|", TokenType::Operator}, {"^", TokenType::Operator}, {"%", TokenType::Operator}, {"!", TokenType::Operator}};
        // 解析数字的 lambda 函数
        auto parse_number = [&](size_t &idx) -> Token {
            const size_t start = idx;            // 记录数字起始位置
            bool has_dot = false, has_e = false; // 标记是否包含小数点和指数部分
            // 检测是否为十六进制数字
            if (idx + 1 < processed.size() && processed[idx] == '0' &&
                tolower(processed[idx + 1]) == 'x') {
                for (idx += 2; idx < processed.size() && isxdigit(processed[idx]); ++idx)
                    ; // 解析十六进制数字
            } else {
                for (; idx < processed.size();) {
                    const char c = processed[idx];
                    if (isdigit(c)) {
                        ++idx; // 解析整数部分
                        continue;
                    }

                    // 解析小数部分
                    if (c == '.') {
                        if (has_dot || has_e)
                            throw std::runtime_error("[squaker.tokens] Invalid decimal format"); // 无效小数格式
                        if (idx + 1 >= processed.size() || !isdigit(processed[idx + 1]))
                            throw std::runtime_error("[squaker.tokens] Invalid decimal format"); // 无效小数格式
                        has_dot = true;
                        ++idx;
                    }
                    // 解析指数部分
                    else if (tolower(c) == 'e') {
                        if (has_e)
                            throw std::runtime_error("[squaker.tokens] Multiple exponents"); // 多重指数
                        if ((++idx < processed.size()) && (processed[idx] == '+' || processed[idx] == '-'))
                            ++idx;
                        if (idx >= processed.size() || !isdigit(processed[idx]))
                            throw std::runtime_error("[squaker.tokens] Invalid exponent"); // 无效指数
                        has_e = true;
                    } else
                        break;
                }
            }
            // 获取数字字符串
            const std::string num_str = processed.substr(start, idx - start);
            char *end = nullptr; // strtol 的结束指针
            // 将字符串转换为 double 类型
            const double value = strtod(num_str.c_str(), &end);
            if (end != num_str.data() + num_str.size())
                throw std::runtime_error("[squaker.tokens] Invalid number: " + num_str); // 无效数字
            return {TokenType::Number, num_str, value};                                  // 返回 Token
        };
        // 主解析循环
        while (i < processed.size()) {
            if (isspace(processed[i])) {
                ++i; // 跳过空白字符
                continue;
            }
            // 处理字符串和字符字面量
            if (processed[i] == '"' || processed[i] == '\'') {
                const bool is_string = (processed[i] == '"');                        // 判断是字符串还是字符
                const size_t quote_pos = i;                                          // 记录引号起始位置
                Token token{is_string ? TokenType::String : TokenType::Char, "", 0}; // 创建 Token复制
                for (++i; i < processed.size() && processed[i] != processed[quote_pos];) {
                    if (processed[i] == '\\') {
                        token.value += ParseEscape(processed[++i]); // 解析转义字符
                        ++i;
                    } else {
                        token.value += processed[i++]; // 保留普通字符
                    }
                }
                if (i >= processed.size())
                    throw std::runtime_error("[squaker.tokens] Unclosed literal"); // 未闭合字面量
                tokens.push_back(token);                                           // 添加 Token 到列表
                ++i;                                                               // 跳过闭合引号
                continue;
            }
            // 解析数字
            if (isdigit(processed[i]) || (processed[i] == '.' && i + 1 < processed.size() && isdigit(processed[i + 1]))) {
                tokens.push_back(parse_number(i)); // 调用 parse_number 函数解析数字
                continue;
            }
            // 解析运算符
            bool found = false;
            for (const auto &[op, type] : OPERATORS) {
                if (processed.substr(i, op.size()) == op) {
                    tokens.push_back({type, op, 0}); // 添加 Token 到列表
                    i += op.size();                  // 跳过运算符
                    found = true;
                    break;
                }
            }
            if (found)
                continue;
            // 解析标识符
            if (isalpha(processed[i]) || processed[i] == '_' || processed[i] == '@') {
                size_t start = i++; // 记录标识符起始位置
                while (i < processed.size() && (isalnum(processed[i]) || processed[i] == '_'))
                    ++i;                                                                          // 解析标识符
                tokens.push_back({TokenType::Identifier, processed.substr(start, i - start), 0}); // 添加 Token 到列表
                continue;
            }
            // 解析标点符号
            if (ispunct(processed[i])) {
                tokens.push_back({TokenType::Punctuation, std::string(1, processed[i++]), 0}); // 添加 Token 到列表
                continue;
            }
            // 抛出未知字符异常
            throw std::runtime_error("[squaker.tokens] Unknown character: " + processed[i]);
        }

        return tokens; // 返回 Token 列表
    }

} // namespace squ