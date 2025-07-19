#pragma once

#include "type.h"
#include "environment.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace squ {

    class ExprNode {
      public:
        virtual ~ExprNode() = default;
        virtual std::string string() const = 0;
        virtual ValueData evaluate(Environment& env) const = 0; // 添加求值接口
    };

    // 统一字面量节点
    class LiteralNode : public ExprNode {
        ValueData data;

      public:
        explicit LiteralNode(ValueData data) : data(std::move(data)) {}

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 标识符节点
    class IdentifierNode : public ExprNode {
        std::string name;

      public:
        explicit IdentifierNode(std::string id) : name(std::move(id)) {}
        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 二元操作节点
    class BinaryOpNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> left;
        std::unique_ptr<ExprNode> right;

      public:
        BinaryOpNode(std::string op, std::unique_ptr<ExprNode> l,
                     std::unique_ptr<ExprNode> r);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 一元操作节点（前缀）
    class UnaryOpNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> operand;

      public:
        UnaryOpNode(std::string op, std::unique_ptr<ExprNode> expr);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 后缀操作节点
    class PostfixOpNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> operand;

      public:
        PostfixOpNode(std::string op, std::unique_ptr<ExprNode> expr);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 赋值节点
    class AssignmentNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> left;
        std::unique_ptr<ExprNode> right;

      public:
        AssignmentNode(std::string op, std::unique_ptr<ExprNode> l,
                       std::unique_ptr<ExprNode> r);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // Lambda节点（函数定义）
    class LambdaNode : public ExprNode {
        std::vector<std::string> parameters;
        std::unique_ptr<ExprNode> body;

      public:
        LambdaNode(std::vector<std::string> params, std::unique_ptr<ExprNode> b);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 函数应用节点（函数调用）
    class ApplyNode : public ExprNode {
        std::unique_ptr<ExprNode> callee;
        std::vector<std::unique_ptr<ExprNode>> arguments;

      public:
        ApplyNode(std::unique_ptr<ExprNode> callee,
                  std::vector<std::unique_ptr<ExprNode>> args);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 条件节点（if-else if-else）
    class IfNode : public ExprNode {
        std::vector<std::pair<std::unique_ptr<ExprNode>,
                              std::unique_ptr<ExprNode>>>
            branches;                         // (条件, 结果) 对
        std::unique_ptr<ExprNode> elseBranch; // 可选的else分支
      public:
        IfNode(std::vector<
                   std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
                   br,
               std::unique_ptr<ExprNode> eb);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // For循环节点
    class ForNode : public ExprNode {
        std::unique_ptr<ExprNode> init;
        std::unique_ptr<ExprNode> condition;
        std::unique_ptr<ExprNode> update;
        std::unique_ptr<ExprNode> body;

      public:
        ForNode(std::unique_ptr<ExprNode> i, std::unique_ptr<ExprNode> c,
                std::unique_ptr<ExprNode> u, std::unique_ptr<ExprNode> b);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 块节点（用于多语句）
    class BlockNode : public ExprNode {
        std::vector<std::unique_ptr<ExprNode>> statements;

      public:
        explicit BlockNode(std::vector<std::unique_ptr<ExprNode>> stmts);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // While循环节点
    class WhileNode : public ExprNode {
        std::unique_ptr<ExprNode> condition;
        std::unique_ptr<ExprNode> body;

      public:
        WhileNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> b);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 模块导入节点
    class ImportNode : public ExprNode {
        std::string moduleName;

      public:
        explicit ImportNode(std::string name) : moduleName(std::move(name)) {}

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 循环控制节点
    class ControlFlowNode : public ExprNode {
        std::string type;

      public:
        explicit ControlFlowNode(std::string t) : type(std::move(t)) {}

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 返回值节点
    class ReturnNode : public ExprNode {
        std::unique_ptr<ExprNode> value;

      public:
        explicit ReturnNode(std::unique_ptr<ExprNode> val);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 成员访问节点
    class MemberAccessNode : public ExprNode {
        std::unique_ptr<ExprNode> object;
        std::string member;

      public:
        MemberAccessNode(std::unique_ptr<ExprNode> obj, std::string mem);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 索引访问节点
    class IndexNode : public ExprNode {
        std::unique_ptr<ExprNode> container;
        std::unique_ptr<ExprNode> index;

      public:
        IndexNode(std::unique_ptr<ExprNode> cont, std::unique_ptr<ExprNode> idx);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 原生函数调用节点
    class NativeCallNode : public ExprNode {
        std::string functionName;
        std::vector<std::unique_ptr<ExprNode>> arguments;

      public:
        NativeCallNode(std::string name,
                       std::vector<std::unique_ptr<ExprNode>> args);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 数组节点
    class ArrayNode : public ExprNode {
        std::vector<std::unique_ptr<ExprNode>> elements;

      public:
        explicit ArrayNode(std::vector<std::unique_ptr<ExprNode>> elems);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

    // 映射表节点
    class MapNode : public ExprNode {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
            entries;

      public:
        explicit MapNode(
            std::vector<
                std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
                entries);

        std::string string() const override;
        ValueData evaluate(Environment& env) const override;
    };

} // namespace squ