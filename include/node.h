#pragma once

#include "type.h"
#include "vm.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace squ {

    // 节点类型枚举
    enum class NodeType {
        Literal,        // 字面量
        Identifier,     // 标识符
        BinaryOp,       // 二元操作
        UnaryOp,        // 一元操作
        PostfixOp,      // 后缀操作
        Assignment,     // 赋值
        CompoundAssign, // 复合赋值
        Lambda,         // Lambda 函数
        Apply,          // 函数应用
        If,             // 条件语句
        For,            // 循环语句
        Block,          // 代码块
        While,          // 循环语句
        Import,         // 导入语句
        ControlFlow,    // 控制流语句
        Return,         // 返回语句
        MemberAccess,   // 成员访问
        Index,          // 索引访问
        NativeCall,     // 原生函数调用
        Array,          // 数组
        Map,            // 映射表
        Table           // 表
    };

    class ExprNode {
      public:
        virtual ~ExprNode() = default;
        virtual std::string string() const = 0;
        // 节点类型接口
        virtual NodeType type() const = 0;
        // 右值求值接口
        virtual ValueData evaluate(VM &vm) const = 0;
        // 左值求值接口
        virtual ValueData &evaluate_lvalue(VM &vm) const = 0;
        // 克隆接口，用于深拷贝
        virtual std::unique_ptr<ExprNode> clone() const = 0;
    };

    // 统一字面量节点
    class LiteralNode : public ExprNode {
        ValueData data;

      public:
        explicit LiteralNode(ValueData data) : data(std::move(data)) {}

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Literal;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 标识符节点
    class IdentifierNode : public ExprNode {
        std::string name;
        size_t index;

      public:
        explicit IdentifierNode(std::string id, size_t idx) : name(std::move(id)), index(idx) {}
        std::string string() const override;
        NodeType type() const override {
            return NodeType::Identifier;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 二元操作节点
    class BinaryOpNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> left;
        std::unique_ptr<ExprNode> right;

      public:
        BinaryOpNode(std::string op, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::BinaryOp;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 一元操作节点（前缀）
    class UnaryOpNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> operand;

      public:
        UnaryOpNode(std::string op, std::unique_ptr<ExprNode> expr);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::UnaryOp;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 后缀操作节点
    class PostfixOpNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> operand;

      public:
        PostfixOpNode(std::string op, std::unique_ptr<ExprNode> expr);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::PostfixOp;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 赋值节点
    class AssignmentNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> left;
        std::unique_ptr<ExprNode> right;

      public:
        AssignmentNode(std::string op, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Assignment;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 复合赋值节点（如 +=, -= 等）
    class CompoundAssignmentNode : public ExprNode {
        std::string op;
        std::unique_ptr<ExprNode> left;
        std::unique_ptr<ExprNode> right;

      public:
        CompoundAssignmentNode(std::string op, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::CompoundAssign;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // Lambda节点（函数定义）
    struct Parameter {
        std::string name;
        size_t slot;
        Parameter() = default;
        Parameter(std::string n, size_t s) : name(std::move(n)), slot(s) {}
    };
    class LambdaNode : public ExprNode {
        std::vector<Parameter> parameters;
        std::shared_ptr<ExprNode> body;
        size_t maxSlot = 0; // 局部变量总数

      public:
        LambdaNode(std::vector<Parameter> params, std::unique_ptr<ExprNode> b);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Lambda;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 函数应用节点（函数调用）
    class ApplyNode : public ExprNode {
        std::unique_ptr<ExprNode> callee;
        std::vector<std::unique_ptr<ExprNode>> arguments;

      public:
        ApplyNode(std::unique_ptr<ExprNode> callee, std::vector<std::unique_ptr<ExprNode>> args);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Apply;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 条件节点（if-else if-else）
    class IfNode : public ExprNode {
        std::vector<std::pair<std::unique_ptr<ExprNode>,
                              std::unique_ptr<ExprNode>>> branches; // (条件, 结果) 对
        std::unique_ptr<ExprNode> elseBranch;                       // 可选的else分支
      public:
        IfNode(std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> br,
               std::unique_ptr<ExprNode> eb);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::If;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // For循环节点
    class ForNode : public ExprNode {
        std::unique_ptr<ExprNode> init;
        std::unique_ptr<ExprNode> condition;
        std::unique_ptr<ExprNode> update;
        std::unique_ptr<ExprNode> body;

      public:
        ForNode(std::unique_ptr<ExprNode> i, std::unique_ptr<ExprNode> c, std::unique_ptr<ExprNode> u,
                std::unique_ptr<ExprNode> b);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::For;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 块节点（用于多语句）
    class BlockNode : public ExprNode {
        std::vector<std::unique_ptr<ExprNode>> statements;

      public:
        explicit BlockNode(std::vector<std::unique_ptr<ExprNode>> stmts);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Block;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // While循环节点
    class WhileNode : public ExprNode {
        std::unique_ptr<ExprNode> condition;
        std::unique_ptr<ExprNode> body;

      public:
        WhileNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> b);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::While;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 模块导入节点
    class ImportNode : public ExprNode {
        std::string moduleName;

      public:
        explicit ImportNode(std::string name) : moduleName(std::move(name)) {}

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Import;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 循环控制节点
    class ControlFlowNode : public ExprNode {
        std::string control_type;

      public:
        explicit ControlFlowNode(std::string t) : control_type(std::move(t)) {}

        std::string string() const override;
        NodeType type() const override {
            return NodeType::ControlFlow;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 返回值节点
    class ReturnNode : public ExprNode {
        std::unique_ptr<ExprNode> value;

      public:
        explicit ReturnNode(std::unique_ptr<ExprNode> val);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Return;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 成员访问节点
    class MemberAccessNode : public ExprNode {
        std::unique_ptr<ExprNode> object;
        std::string member;

      public:
        MemberAccessNode(std::unique_ptr<ExprNode> obj, std::string mem);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::MemberAccess;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 索引访问节点
    class IndexNode : public ExprNode {
        std::unique_ptr<ExprNode> container;
        std::unique_ptr<ExprNode> index;

      public:
        IndexNode(std::unique_ptr<ExprNode> cont, std::unique_ptr<ExprNode> idx);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Index;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 原生函数调用节点
    class NativeCallNode : public ExprNode {
        std::string functionName;
        std::vector<std::unique_ptr<ExprNode>> arguments;

      public:
        NativeCallNode(std::string name, std::vector<std::unique_ptr<ExprNode>> args);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::NativeCall;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 数组节点
    class ArrayNode : public ExprNode {
      public:
        std::vector<std::unique_ptr<ExprNode>> elements;
        explicit ArrayNode(std::vector<std::unique_ptr<ExprNode>> elems);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Array;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 映射表节点
    class MapNode : public ExprNode {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> entries;

      public:
        explicit MapNode(std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> entries);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Map;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

    // 表节点
    class TableNode : public ExprNode {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> entries;
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> members;
        std::vector<std::unique_ptr<ExprNode>> elements;

      public:
        explicit TableNode(std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> entries,
                           std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> members,
                           std::vector<std::unique_ptr<ExprNode>> elements);

        std::string string() const override;
        NodeType type() const override {
            return NodeType::Table;
        }
        ValueData evaluate(VM &vm) const override;
        ValueData &evaluate_lvalue(VM &vm) const override;
        std::unique_ptr<ExprNode> clone() const override;
    };

} // namespace squ