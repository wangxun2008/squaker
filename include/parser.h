#pragma once

#include "node.h"
#include "scope.h"
#include "token.h"
#include "vm.h"
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace squ {

    class Parser {
      public:
        std::vector<Token> tokens;
        size_t current = 0;
        std::unique_ptr<Scope> curScope;
        std::stack<std::unique_ptr<Scope>> scopeStack;

        // 默认构造函数
        Parser();

        // 构造函数：接受初始tokens
        explicit Parser(std::vector<Token> tokens);

        // 重置解析器状态
        void reset(std::vector<Token> newTokens);

        // 解析入口函数
        std::unique_ptr<ExprNode> parse();

      private:
        // 辅助函数：检查当前token是否匹配
        bool match(TokenType type, const std::string &value = "");

        // 辅助函数：检测token是否匹配，但不消耗
        bool peek(std::size_t ahead, TokenType type, const std::string &value = "");

        // 辅助函数：获取前一个token
        Token previous() const;

        // 表达式入口
        std::unique_ptr<ExprNode> parse_expression();

        // 赋值表达式
        std::unique_ptr<ExprNode> parse_assignment();

        // 逻辑或
        std::unique_ptr<ExprNode> parse_logical_or();

        // 逻辑与
        std::unique_ptr<ExprNode> parse_logical_and();

        // 相等性
        std::unique_ptr<ExprNode> parse_equality();

        // 关系
        std::unique_ptr<ExprNode> parse_relational();

        // 字符串连接
        std::unique_ptr<ExprNode> parse_concatenation();

        // 移位
        std::unique_ptr<ExprNode> parse_shift();

        // 加减
        std::unique_ptr<ExprNode> parse_additive();

        // 乘除模
        std::unique_ptr<ExprNode> parse_multiplicative();

        // 一元操作
        std::unique_ptr<ExprNode> parse_unary();

        // 后缀操作（增加成员访问和索引访问）
        std::unique_ptr<ExprNode> parse_postfix();

        // 解析函数调用
        std::unique_ptr<ExprNode> parse_function_call(std::unique_ptr<ExprNode> callee);

        // 解析块表达式（多语句）
        std::unique_ptr<ExprNode> parse_block();

        // 解析while循环表达式
        std::unique_ptr<ExprNode> parse_while_expression();

        // 解析for循环表达式
        std::unique_ptr<ExprNode> parse_for_expression();

        // 解析条件表达式
        std::unique_ptr<ExprNode> parse_if_expression();

        // 解析单个条件分支（if或else if）
        std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>> parse_if_branch();

        // 解析Lambda表达式
        std::unique_ptr<ExprNode> parse_lambda_expression();

        // 解析函数定义
        std::unique_ptr<ExprNode> parse_function_definition();

        // 解析导入语句
        std::unique_ptr<ExprNode> parse_import_statement();

        // 解析return语句
        std::unique_ptr<ExprNode> parse_return_statement();

        // 解析原生函数调用
        std::unique_ptr<ExprNode> parse_native_call(const std::string &functionName);

        // 解析常量字面量
        std::unique_ptr<ExprNode> parse_constant();

        // 解析数组字面量
        std::unique_ptr<ExprNode> parse_array();

        // 解析表字面量
        std::unique_ptr<ExprNode> parse_table();

        // 基本表达式
        std::unique_ptr<ExprNode> parse_primary();
    };

} // namespace squ