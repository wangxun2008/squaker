#include "../include/parser.h"
#include "../include/scope.h"
#include "../include/identifier.h"
#include "../include/module.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace squ {

    // RAII风格的作用域栈管理类
    class ScopeStackGuard {
        Parser *parser;

      public:
        explicit ScopeStackGuard(Parser *p) : parser(p) {
            parser->scopeStack.emplace(std::move(parser->curScope));
            parser->curScope = std::make_unique<Scope>();
        }
        ~ScopeStackGuard() {
            auto oldScope = std::move(parser->scopeStack.top());
            parser->scopeStack.pop();
            parser->curScope = std::move(oldScope);
        }
        ScopeStackGuard(const ScopeStackGuard &) = delete;
        ScopeStackGuard &operator=(const ScopeStackGuard &) = delete;
    };

    Parser::Parser() : curScope(std::make_unique<Scope>()) {}

    Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), curScope(std::make_unique<Scope>()) {}

    void Parser::reset(std::vector<Token> newTokens) {
        tokens = std::move(newTokens);
        current = 0;
    }

    size_t Parser::register_identifiers(std::string identifier) {
        size_t slot = curScope->size();
        if (curScope->find(identifier) != Scope::npos) {
            throw std::runtime_error("[squaker.parser] Identifier already declared: " + identifier);
        }
        curScope->add(identifier);
        return slot;
    }

    std::unique_ptr<ExprNode> Parser::parse() {

        std::vector<std::unique_ptr<ExprNode>> statements;

        while (current < tokens.size()) {
            // 解析块内的表达式
            statements.push_back(parse_expression());

            // 可选分号分隔符，也可没有
            match(TokenType::Punctuation, ";");
        }

        // 检查是否还有未消耗的token
        if (current < tokens.size()) {
            std::string unexpected;
            for (size_t i = current; i < tokens.size(); i++) {
                unexpected += tokens[i].value + " ";
                if (unexpected.length() > 20) { // 限制长度
                    unexpected += "...";
                    break;
                }
            }
            throw std::runtime_error("[squaker.parser] Unexpected tokens after expression: " + unexpected);
        }

        return std::make_unique<BlockNode>(std::move(statements));
    }

    // 辅助函数：检查当前token是否匹配
    bool Parser::match(TokenType type, const std::string &value) {
        if (current >= tokens.size())
            return false;
        const Token &token = tokens[current];
        if (token.type == type && (value.empty() || token.value == value)) {
            current++;
            return true;
        }
        return false;
    }

    // 辅助函数：检测token是否匹配，但不消耗
    bool Parser::peek(std::size_t ahead, TokenType type, const std::string &value) {
        if (current + ahead >= tokens.size())
            return false;
        const Token &token = tokens[current + ahead];
        if (token.type == type && (value.empty() || token.value == value)) {
            return true;
        }
        return false;
    }

    // 辅助函数：获取前一个token
    Token Parser::previous() const {
        if (current == 0) {
            throw std::runtime_error("[squaker.parser] No previous token available");
        }
        return tokens[current - 1];
    }

    // 表达式入口
    std::unique_ptr<ExprNode> Parser::parse_expression() {
        if (peek(0, TokenType::Punctuation, ";")) {
            // 如果当前token是分号，直接返回空表达式
            return std::make_unique<LiteralNode>(ValueData()); // 返回一个空的LiteralNode
        }
        return parse_assignment();
    }

    // 赋值表达式
    std::unique_ptr<ExprNode> Parser::parse_assignment() {
        auto left = parse_logical_or();

        if (match(TokenType::Assignment)) {
            Token op = previous();
            auto right = parse_assignment();
            if (op.value == "=") {
                // 简单赋值
                return std::make_unique<AssignmentNode>(op.value, std::move(left), std::move(right));
            } else {
                // 复合赋值
                return std::make_unique<CompoundAssignmentNode>(op.value, std::move(left), std::move(right));
            }
        }

        return left;
    }

    // 逻辑或
    std::unique_ptr<ExprNode> Parser::parse_logical_or() {
        auto left = parse_logical_and();

        while (match(TokenType::Operator, "||")) {
            Token op = previous();
            auto right = parse_logical_and();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 逻辑与
    std::unique_ptr<ExprNode> Parser::parse_logical_and() {
        auto left = parse_equality();

        while (match(TokenType::Operator, "&&")) {
            Token op = previous();
            auto right = parse_equality();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 相等性
    std::unique_ptr<ExprNode> Parser::parse_equality() {
        auto left = parse_relational();

        while (match(TokenType::Operator, "==") || match(TokenType::Operator, "!=")) {
            Token op = previous();
            auto right = parse_relational();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 关系
    std::unique_ptr<ExprNode> Parser::parse_relational() {
        auto left = parse_concatenation();

        while (match(TokenType::Operator, "<") || match(TokenType::Operator, ">") || match(TokenType::Operator, "<=") ||
               match(TokenType::Operator, ">=")) {
            Token op = previous();
            auto right = parse_shift();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 字符串连接
    std::unique_ptr<ExprNode> Parser::parse_concatenation() {
        auto left = parse_shift();

        while (match(TokenType::Operator, "..")) {
            Token op = previous();
            auto right = parse_shift();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 移位
    std::unique_ptr<ExprNode> Parser::parse_shift() {
        auto left = parse_additive();

        while (match(TokenType::Operator, "<<") || match(TokenType::Operator, ">>")) {
            Token op = previous();
            auto right = parse_additive();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 加减
    std::unique_ptr<ExprNode> Parser::parse_additive() {
        auto left = parse_multiplicative();

        while (match(TokenType::Operator, "+") || match(TokenType::Operator, "-")) {
            Token op = previous();
            auto right = parse_multiplicative();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 乘除模
    std::unique_ptr<ExprNode> Parser::parse_multiplicative() {
        auto left = parse_unary();

        while (match(TokenType::Operator, "*") || match(TokenType::Operator, "/") || match(TokenType::Operator, "%")) {
            Token op = previous();
            auto right = parse_unary();
            left = std::make_unique<BinaryOpNode>(op.value, std::move(left), std::move(right));
        }

        return left;
    }

    // 一元操作
    std::unique_ptr<ExprNode> Parser::parse_unary() {
        if (match(TokenType::Operator)) {
            Token op = previous();
            if (op.value == "+" || op.value == "-" || op.value == "!" || op.value == "~" || op.value == "++" ||
                op.value == "--" || op.value == "&" || op.value == "*") {
                auto operand = parse_unary();
                return std::make_unique<UnaryOpNode>(op.value, std::move(operand));
            } else {
                // 不是单目前缀，回退
                current--;
            }
        }
        return parse_postfix();
    }

    // 后缀操作（增加成员访问和索引访问）
    std::unique_ptr<ExprNode> Parser::parse_postfix() {
        auto expr = parse_primary();

        // 处理成员访问、索引访问、函数调用等
        while (true) {
            // 成员访问 a.b
            if (match(TokenType::Punctuation, ".")) {
                if (match(TokenType::Identifier)) {
                    expr = std::make_unique<MemberAccessNode>(std::move(expr), previous().value);
                } else {
                    std::string context;
                    if (current < tokens.size()) {
                        context = " at token '" + tokens[current].value + "'";
                    }
                    throw std::runtime_error("[squaker.parser.member] Expected identifier after '.'" + context);
                }
            }
            // 索引访问 a[i]
            else if (match(TokenType::Punctuation, "[")) {
                auto index = parse_expression();
                if (!match(TokenType::Punctuation, "]")) {
                    std::string context;
                    if (current < tokens.size()) {
                        context = " at token '" + tokens[current].value + "'";
                    }
                    throw std::runtime_error("[squaker.parser.index] Expected ']' after index expression" + context);
                }
                expr = std::make_unique<IndexNode>(std::move(expr), std::move(index));
            }
            // 函数调用
            else if (match(TokenType::Punctuation, "(")) {
                expr = parse_function_call(std::move(expr));
            }
            // 后缀自增/减
            else if (match(TokenType::Assignment, "++") || match(TokenType::Assignment, "--")) {
                Token op = previous();
                expr = std::make_unique<PostfixOpNode>(op.value, std::move(expr));
            } else {
                break;
            }
        }

        return expr;
    }

    // 解析函数调用
    std::unique_ptr<ExprNode> Parser::parse_function_call(std::unique_ptr<ExprNode> callee) {
        std::vector<std::unique_ptr<ExprNode>> arguments;

        // 检查是否有参数
        if (!match(TokenType::Punctuation, ")")) {
            do {
                arguments.push_back(parse_expression());
            } while (match(TokenType::Punctuation, ","));

            // 期望右括号
            if (!match(TokenType::Punctuation, ")")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.call] Expected ')' after argument list" + context);
            }
        }

        return std::make_unique<ApplyNode>(std::move(callee), std::move(arguments));
    }

    // 解析块表达式（多语句）
    std::unique_ptr<ExprNode> Parser::parse_block() {
        std::vector<std::unique_ptr<ExprNode>> statements;

        // 检查空块 {}
        if (match(TokenType::Punctuation, "}")) {
            return std::make_unique<BlockNode>(std::move(statements));
        }

        // 进入块作用域
        ScopeGuard scopeGuard(curScope.get());

        do {
            // 解析块内的表达式
            statements.push_back(parse_expression());

            // 检查块结束或语句分隔符
            if (match(TokenType::Punctuation, "}")) {
                break; // 块正常结束
            }

            // 可选分号分隔符，也可以没有
            match(TokenType::Punctuation, ";");

            // 分号后可能直接是块结束
            if (match(TokenType::Punctuation, "}")) {
                break;
            }
        } while (true);

        return std::make_unique<BlockNode>(std::move(statements));
    }

    // 解析while循环表达式
    std::unique_ptr<ExprNode> Parser::parse_while_expression() {
        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            throw std::runtime_error("[squaker.parser.while] Expected '(' after 'while'");
        }

        // 进入作用域
        ScopeGuard scopeGuard(curScope.get());

        // 解析条件表达式
        auto condition = parse_expression();

        // 期望右括号
        if (!match(TokenType::Punctuation, ")")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.while] Expected ')' after condition" + context);
        }

        // 解析循环体（支持块表达式或单行表达式）
        auto body = parse_expression();

        return std::make_unique<WhileNode>(std::move(condition), std::move(body));
    }

    // 解析do-while循环表达式
    std::unique_ptr<ExprNode> Parser::parse_do_while_expression() {
        // 进入作用域
        ScopeGuard scopeGuard(curScope.get());

        // 解析循环体（支持块表达式或单行表达式）
        auto body = parse_expression();

        // 期望"while"
        if (!match(TokenType::Identifier, "while")) {
            throw std::runtime_error("[squaker.parser.do] Expected 'while' after do-while body");
        }

        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            throw std::runtime_error("[squaker.parser.do] Expected '(' after 'while'");
        }

        // 解析条件表达式
        auto condition = parse_expression();

        // 期望右括号
        if (!match(TokenType::Punctuation, ")")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.do] Expected ')' after condition" + context);
        }

        // 可选分号分隔符，也可没有
        match(TokenType::Punctuation, ";");

        return std::make_unique<DoWhileNode>(std::move(body), std::move(condition));
    }

    // 解析for循环表达式
    std::unique_ptr<ExprNode> Parser::parse_for_expression() {
        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            throw std::runtime_error("[squaker.parser.for] Expected '(' after 'for'");
        }

        // 进入作用域
        ScopeGuard scopeGuard(curScope.get());

        // 解析初始化表达式（可选）
        std::unique_ptr<ExprNode> init;
        if (!match(TokenType::Punctuation, ";")) {
            init = parse_expression();
            if (!match(TokenType::Punctuation, ";")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.for] Expected ';' after init expression" + context);
            }
        }

        // 解析条件表达式（可选）
        std::unique_ptr<ExprNode> condition;
        if (!match(TokenType::Punctuation, ";")) {
            condition = parse_expression();
            if (!match(TokenType::Punctuation, ";")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.for] Expected ';' after condition expression" + context);
            }
        }

        // 解析更新表达式（可选）
        std::unique_ptr<ExprNode> update;
        if (!match(TokenType::Punctuation, ")")) {
            update = parse_expression();
            if (!match(TokenType::Punctuation, ")")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.for] Expected ')' after update expression" + context);
            }
        }

        // 解析循环体（支持块表达式或单行表达式）
        auto body = parse_expression();

        return std::make_unique<ForNode>(std::move(init), std::move(condition), std::move(update), std::move(body));
    }

    // 解析条件表达式
    std::unique_ptr<ExprNode> Parser::parse_if_expression() {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> branches;

        // 进入作用域
        ScopeGuard scopeGuard(curScope.get());

        // 解析初始if分支
        branches.push_back(parse_if_branch());

        // 解析可能的else if分支
        while (true) {
            // 检查 else 或 else if
            if (match(TokenType::Identifier, "else")) {
                if (match(TokenType::Identifier, "if")) {
                    // 检查else if
                    branches.push_back(parse_if_branch());
                } else {
                    // 解析else分支
                    auto elseBody = parse_primary();
                    return std::make_unique<IfNode>(std::move(branches), std::move(elseBody));
                }
            } else {
                // 没有更多分支
                break;
            }
        }

        // 没有else分支
        return std::make_unique<IfNode>(std::move(branches), nullptr);
    }

    // 解析单个条件分支（if或else if）
    std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>> Parser::parse_if_branch() {
        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            throw std::runtime_error("[squaker.parser.if] Expected '(' after 'if' or 'else if'");
        }

        // 解析条件表达式
        auto condition = parse_expression();

        // 期望右括号
        if (!match(TokenType::Punctuation, ")")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.if] Expected ')' after condition" + context);
        }

        // 解析结果表达式
        auto body = parse_expression();

        // 可选分号分隔符，也可没有
        match(TokenType::Punctuation, ";");

        return {std::move(condition), std::move(body)};
    }

    // 解析switch表达式
    std::unique_ptr<ExprNode> Parser::parse_switch_expression() {
        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            throw std::runtime_error("[squaker.parser.switch] Expected '(' after 'switch'");
        }

        // 进入作用域
        ScopeGuard scopeGuard(curScope.get());

        // 解析条件表达式
        auto condition = parse_expression();

        // 期望右括号
        if (!match(TokenType::Punctuation, ")")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.switch] Expected ')' after condition" + context);
        }

        // 期望左大括号开始case分支
        if (!match(TokenType::Punctuation, "{")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.switch] Expected '{' after switch condition" + context);
        }

        // 解析case分支
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> cases;
        while (match(TokenType::Identifier, "case")) {
            // 解析case条件
            auto caseCondition = parse_expression();

            // 期望冒号
            if (!match(TokenType::Punctuation, ":")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.switch] Expected ':' after case condition" + context);
            }

            // 解析case结果
            auto caseBody = parse_expression();
            cases.emplace_back(std::move(caseCondition), std::move(caseBody));

            // 可选分号分隔符，也可没有
            match(TokenType::Punctuation, ";");
        }

        // 可选的default分支
        std::unique_ptr<ExprNode> defaultBody;
        if (match(TokenType::Identifier, "default")) {
            if (!match(TokenType::Punctuation, ":")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.switch] Expected ':' after 'default'" + context);
            }
            defaultBody = parse_expression();
        }

        // 期望右大括号结束case分支
        if (!match(TokenType::Punctuation, "}")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.switch] Expected '}' after switch cases" + context);
        }

        return std::make_unique<SwitchNode>(std::move(condition), std::move(cases), std::move(defaultBody));
    }

    // 解析Lambda表达式
    std::unique_ptr<ExprNode> Parser::parse_lambda_expression() {
        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            throw std::runtime_error("[squaker.parser.lambda] Expected '(' after 'lambda'");
        }

        // 进入函数作用域
        ScopeStackGuard scopeGuard(this);

        // 解析参数列表
        std::vector<std::string> parameters;
        if (!match(TokenType::Punctuation, ")")) {
            do {
                if (match(TokenType::Identifier)) {
                    parameters.push_back(previous().value);
                } else {
                    std::string context;
                    if (current < tokens.size()) {
                        context = " at token '" + tokens[current].value + "'";
                    }
                    throw std::runtime_error("[squaker.parser.lambda] Expected identifier in parameter list" + context);
                }
            } while (match(TokenType::Punctuation, ","));

            // 期望右括号
            if (!match(TokenType::Punctuation, ")")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.lambda] Expected ')' after parameter list" + context);
            }
        }

        // 参数解析为slot
        std::vector<Parameter> slot_parameters;
        for (const auto &name : parameters) {
            size_t slot = curScope->add(name);
            slot_parameters.emplace_back(name, slot);
        }

        // 解析函数体
        auto body = parse_expression();

        return std::make_unique<LambdaNode>(std::move(slot_parameters), std::move(body));
    }

    // 解析函数定义
    std::unique_ptr<ExprNode> Parser::parse_function_definition() {
        // 如果当前token是括号，则认为是Lambda表达式
        if (peek(0, TokenType::Punctuation, "(")) {
            return parse_lambda_expression();
        }

        // 解析函数名，暂时只能是标识符，后续可以扩展为支持其他类型
        auto functionName = parse_primary();
        std::unique_ptr<ExprNode> lambda;

        // 进入函数作用域
        {
            ScopeStackGuard scopeGuard(this);

            // 期望左括号
            if (!match(TokenType::Punctuation, "(")) {
                throw std::runtime_error("[squaker.parser.function] Expected '(' after function name");
            }

            // 解析参数列表
            std::vector<std::string> parameters;
            if (!match(TokenType::Punctuation, ")")) {
                do {
                    if (match(TokenType::Identifier)) {
                        parameters.push_back(previous().value);
                    } else {
                        std::string context;
                        if (current < tokens.size()) {
                            context = " at token '" + tokens[current].value + "'";
                        }
                        throw std::runtime_error("[squaker.parser.function] Expected identifier in parameter list" +
                                                 context);
                    }
                } while (match(TokenType::Punctuation, ","));

                // 期望右括号
                if (!match(TokenType::Punctuation, ")")) {
                    std::string context;
                    if (current < tokens.size()) {
                        context = " at token '" + tokens[current].value + "'";
                    }
                    throw std::runtime_error("[squaker.parser.function] Expected ')' after parameter list" + context);
                }
            }

            // 参数解析为slot
            std::vector<Parameter> slot_parameters;
            for (const auto &name : parameters) {
                size_t slot = curScope->add(name);
                slot_parameters.emplace_back(name, slot);
            }

            // 解析函数体
            auto body = parse_expression();

            // 创建函数赋值表达式: functionName = lambda(parameters) -> body
            lambda = std::make_unique<LambdaNode>(slot_parameters, std::move(body));
        }

        // 在当前作用域中添加函数
        return std::make_unique<AssignmentNode>("=", std::move(functionName), std::move(lambda));
    }

    // 解析导入语句
    std::unique_ptr<ExprNode> Parser::parse_import_statement() {
        // 期望模块名（标识符或字符串）
        if (match(TokenType::Identifier)) {
            std::string moduleName = previous().value;
            auto module = Module(moduleName);
            // 在当前作用域中注册模块
            if (curScope->find(moduleName) != Scope::npos) {
                throw std::runtime_error("[squaker.parser.import] Module already imported: " + moduleName);
            }
            size_t slot = curScope->add(moduleName);
            // 返回导入节点
            return std::make_unique<AssignmentNode>(
                "=", std::make_unique<IdentifierNode>(moduleName, slot),
                std::make_unique<LiteralNode>(module.value)
            );
        } else if (match(TokenType::String)) {
            std::string moduleName = previous().value;
            auto module = Module(moduleName);
            // 在当前作用域中注册模块
            if (curScope->find(moduleName) != Scope::npos) {
                throw std::runtime_error("[squaker.parser.import] Module already imported: " + moduleName);
            }
            size_t slot = curScope->add(moduleName);
            // 返回导入节点
            return std::make_unique<AssignmentNode>(
                "=", std::make_unique<IdentifierNode>(moduleName, slot),
                std::make_unique<LiteralNode>(module.value)
            );
        } else {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.import] Expected module name" + context);
        }
    }

    // 解析return语句
    std::unique_ptr<ExprNode> Parser::parse_return_statement() {
        // 检查是否有返回值
        if (current < tokens.size() && tokens[current].type != TokenType::Punctuation) {
            auto value = parse_expression();
            return std::make_unique<ReturnNode>(std::move(value));
        }
        return std::make_unique<ReturnNode>(nullptr); // 无返回值
    }

    // 解析原生函数调用
    std::unique_ptr<ExprNode> Parser::parse_native_call(const std::string &functionName) {
        // 期望左括号
        if (!match(TokenType::Punctuation, "(")) {
            std::string context;
            if (current < tokens.size()) {
                context = " at token '" + tokens[current].value + "'";
            }
            throw std::runtime_error("[squaker.parser.native] Expected '(' after '@" + functionName + "'" + context);
        }

        std::vector<std::unique_ptr<ExprNode>> arguments;

        // 检查是否有参数
        if (!match(TokenType::Punctuation, ")")) {
            do {
                arguments.push_back(parse_expression());
            } while (match(TokenType::Punctuation, ","));

            // 期望右括号
            if (!match(TokenType::Punctuation, ")")) {
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.native] Expected ')' after argument list" + context);
            }
        }

        return std::make_unique<NativeCallNode>(functionName, std::move(arguments));
    }

    // 解析常量字面量
    std::unique_ptr<ExprNode> Parser::parse_constant() {
        auto value = parse_expression();
        return std::make_unique<ConstantNode>(std::move(value));
    }

    // 解析数组字面量
    std::unique_ptr<ExprNode> Parser::parse_array() {
        std::vector<std::unique_ptr<ExprNode>> elements;

        // 检查空数组 []
        if (match(TokenType::Punctuation, "]")) {
            return std::make_unique<ArrayNode>(std::move(elements));
        }

        do {
            // 解析数组元素表达式
            elements.push_back(parse_expression());
        } while (match(TokenType::Punctuation, ","));

        // 期望右方括号
        if (!match(TokenType::Punctuation, "]")) {
            std::string context = current < tokens.size() ? " at token '" + tokens[current].value + "'" : "";
            throw std::runtime_error("[squaker.parser.array] Expected ']' after array elements" + context);
        }

        return std::make_unique<ArrayNode>(std::move(elements));
    }

    // 解析表字面量
    std::unique_ptr<ExprNode> Parser::parse_table() {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> entries;
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> members;
        std::vector<std::unique_ptr<ExprNode>> elements;

        // 检查空表[]
        if (match(TokenType::Punctuation, "]")) {
            return std::make_unique<TableNode>(std::move(entries), std::move(members), std::move(elements));
        }

        do {
            std::unique_ptr<ExprNode> key;

            // 解析键表达式（必须是数组或标识符）
            ValueType type = ValueType::Nil;
            if (match(TokenType::Punctuation, "[")) {
                key = parse_array();
                type = ValueType::Array;
            } else if (match(TokenType::Identifier)) {
                Token token = previous();
                ValueData data;
                data.type = ValueType::String;
                data.value = token.value;
                key = std::make_unique<LiteralNode>(data);
                type = ValueType::String;
            } else {
                key = parse_expression();
                type = ValueType::Nil; // 默认为Nil类型
            }

            // 如果有等号分隔符，则解析为键值对
            if (match(TokenType::Assignment, "=")) {
                // 键必须是数组或标识符
                if (type == ValueType::Nil) {
                    std::string context = current < tokens.size() ? " at token '" + tokens[current].value + "'" : "";
                    throw std::runtime_error("[squaker.parser.table] Expected array or identifier as table key" +
                                             context);
                }

                // 解析值表达式
                auto value = parse_expression();

                if (type == ValueType::Array) {
                    entries.emplace_back(std::move(key), std::move(value));
                } else {
                    members.emplace_back(std::move(key), std::move(value));
                }
            }
            // 否则解析为没有键的纯数组
            else {
                elements.emplace_back(std::move(key));
            }

        } while (match(TokenType::Punctuation, ","));

        // 期望右方括号
        if (!match(TokenType::Punctuation, "]")) {
            std::string context = current < tokens.size() ? " at token '" + tokens[current].value + "'" : "";
            throw std::runtime_error("[squaker.parser.table] Expected ']' after table entries" + context);
        }

        return std::make_unique<TableNode>(std::move(entries), std::move(members), std::move(elements));
    }

    // 基本表达式
    std::unique_ptr<ExprNode> Parser::parse_primary() {

        // 检查数组字面量（方括号）
        if (match(TokenType::Punctuation, "[")) {
            return parse_table();
        }

        // 检查块表达式
        if (match(TokenType::Punctuation, "{")) {
            return parse_block();
        }

        if (match(TokenType::Identifier)) {
            Token token = previous();

            // 检查布尔字面量
            if (token.value == "true" || token.value == "false") {
                ValueData data;
                data.type = ValueType::Bool;
                data.value = (token.value == "true");
                return std::make_unique<LiteralNode>(data);
            }
            // 检查while关键字
            else if (token.value == "while") {
                return parse_while_expression();
            }
            // 检查do关键字
            else if (token.value == "do") {
                return parse_do_while_expression();
            }
            // 检查for关键字
            else if (token.value == "for") {
                return parse_for_expression();
            }
            // 检查if关键字
            else if (token.value == "if") {
                return parse_if_expression();
            }
            // 检查switch关键字
            else if (token.value == "switch") {
                return parse_switch_expression();
            }
            // 检查function关键字
            else if (token.value == "function") {
                return parse_function_definition();
            }
            // 检查import关键字
            else if (token.value == "import") {
                return parse_import_statement();
            }
            // 检查break关键字
            else if (token.value == "break") {
                return std::make_unique<ControlFlowNode>("break");
            }
            // 检查continue关键字
            else if (token.value == "continue") {
                return std::make_unique<ControlFlowNode>("continue");
            }
            // 检查return关键字
            else if (token.value == "return") {
                return parse_return_statement();
            }
            // 检查原生函数调用（以@开头）
            else if (!token.value.empty() && token.value[0] == '@') {
                return parse_native_call(token.value.substr(1));
            }
            // 检查constant关键字
            else if (token.value == "const") {
                return parse_constant();
            }
            // 否则是标识符
            else {
                // 检查当前作用域中是否有该标识符
                size_t index = curScope->find(token.value);
                if (index == Scope::npos) {
                    return std::make_unique<IdentifierNode>(token.value, curScope->add(token.value));
                }
                return std::make_unique<IdentifierNode>(token.value, index);
            }
        }

        if (match(TokenType::Real)) {
            Token token = previous();
            ValueData data;
            data.type = ValueType::Real;
            data.value = token.num_real;
            return std::make_unique<LiteralNode>(data);
        }

        if (match(TokenType::Integer)) {
            Token token = previous();
            ValueData data;
            data.type = ValueType::Integer;
            data.value = token.num_integer;
            return std::make_unique<LiteralNode>(data);
        }

        if (match(TokenType::String)) {
            Token token = previous();
            ValueData data;
            data.type = ValueType::String;
            data.value = token.value;
            return std::make_unique<LiteralNode>(data);
        }

        if (match(TokenType::Char)) {
            Token token = previous();
            if (token.value.empty()) {
                throw std::runtime_error("[squaker.parser.primary] Empty char literal");
            }
            ValueData data;
            data.type = ValueType::Char;
            data.value = token.value[0];
            return std::make_unique<LiteralNode>(data);
        }

        if (match(TokenType::Punctuation, "(")) {
            auto expr = parse_expression();
            if (!match(TokenType::Punctuation, ")")) {
                // 获取当前位置信息
                std::string context;
                if (current < tokens.size()) {
                    context = " at token '" + tokens[current].value + "'";
                }
                throw std::runtime_error("[squaker.parser.primary] Expected ')' after expression" + context);
            }
            return expr;
        }

        // 获取当前token值用于错误信息
        std::string tokenValue = "end of input";
        if (current < tokens.size()) {
            tokenValue = "token '" + tokens[current].value + "'";
        }

        throw std::runtime_error("[squaker.parser.primary] Unexpected " + tokenValue);
    }

} // namespace squ