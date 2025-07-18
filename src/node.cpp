#include "../include/node.h"
#include "../include/type.h"
#include "../include/operator.h"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <variant>

namespace squ {

    // 统一字面量节点
    std::string LiteralNode::toString() const {
        return data.to_string();
    }

    ValueData LiteralNode::evaluate() const {
        return data;
    }

    // 标识符节点
    std::string IdentifierNode::toString() const {
        return name;
    }

    ValueData IdentifierNode::evaluate() const {
        // 这里需要从环境中查找变量值
        throw std::runtime_error("Identifier evaluation requires environment context: " + name);
    }

    // 二元操作节点
    BinaryOpNode::BinaryOpNode(std::string op, std::unique_ptr<ExprNode> l,
                               std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string BinaryOpNode::toString() const {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }

    ValueData BinaryOpNode::evaluate() const {
        ValueData leftVal = left->evaluate();
        ValueData rightVal = right->evaluate();
        return applyBinary(leftVal, op, rightVal);
    }

    // 一元操作节点（前缀）
    UnaryOpNode::UnaryOpNode(std::string op, std::unique_ptr<ExprNode> expr)
        : op(std::move(op)), operand(std::move(expr)) {}

    std::string UnaryOpNode::toString() const {
        return "(" + op + operand->toString() + ")";
    }

    ValueData UnaryOpNode::evaluate() const {
        ValueData operandVal = operand->evaluate();

        // 实现一元操作的求值逻辑
        throw std::runtime_error("Unary operation evaluation not implemented for op: " + op);
    }

    // 后缀操作节点
    PostfixOpNode::PostfixOpNode(std::string op, std::unique_ptr<ExprNode> expr)
        : op(std::move(op)), operand(std::move(expr)) {}

    std::string PostfixOpNode::toString() const {
        return "(" + operand->toString() + op + ")";
    }

    ValueData PostfixOpNode::evaluate() const {
        ValueData operandVal = operand->evaluate();

        // 实现后缀操作的求值逻辑
        throw std::runtime_error("Postfix operation evaluation not implemented for op: " + op);
    }

    // 赋值节点
    AssignmentNode::AssignmentNode(std::string op, std::unique_ptr<ExprNode> l,
                                   std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string AssignmentNode::toString() const {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }

    ValueData AssignmentNode::evaluate() const {
        // 这里需要处理赋值操作
        // 左侧通常是标识符，右侧是表达式
        throw std::runtime_error("Assignment evaluation requires environment context");
    }

    // Lambda节点（函数定义）
    LambdaNode::LambdaNode(std::vector<std::string> params,
                           std::unique_ptr<ExprNode> b)
        : parameters(std::move(params)), body(std::move(b)) {}

    std::string LambdaNode::toString() const {
        std::string params;
        for (size_t i = 0; i < parameters.size(); i++) {
            if (i > 0)
                params += ", ";
            params += parameters[i];
        }
        return "(lambda (" + params + ") -> " + body->toString() + ")";
    }

    ValueData LambdaNode::evaluate() const {
        // 返回一个函数值，需要在ValueData中表示函数
        throw std::runtime_error("Lambda evaluation not implemented");
    }

    // 函数应用节点（函数调用）
    ApplyNode::ApplyNode(std::unique_ptr<ExprNode> callee,
                         std::vector<std::unique_ptr<ExprNode>> args)
        : callee(std::move(callee)), arguments(std::move(args)) {}

    std::string ApplyNode::toString() const {
        std::string args;
        for (size_t i = 0; i < arguments.size(); i++) {
            if (i > 0)
                args += ", ";
            args += arguments[i]->toString();
        }
        return "(apply:" + callee->toString() + "(" + args + "))";
    }

    ValueData ApplyNode::evaluate() const {
        // 实现函数调用的求值逻辑
        throw std::runtime_error("Function application evaluation not implemented");
    }

    // 条件节点（if-else if-else）
    IfNode::IfNode(
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
            br,
        std::unique_ptr<ExprNode> eb)
        : branches(std::move(br)), elseBranch(std::move(eb)) {}

    std::string IfNode::toString() const {
        std::string result = "(if ";
        for (size_t i = 0; i < branches.size(); i++) {
            const auto &branch = branches[i];
            if (i > 0)
                result += " else if ";
            result +=
                "(" + branch.first->toString() + ") " + branch.second->toString();
        }
        if (elseBranch) {
            result += " else " + elseBranch->toString();
        }
        result += ")";
        return result;
    }

    ValueData IfNode::evaluate() const {
        // 实现条件表达式的求值逻辑
        throw std::runtime_error("Condition evaluation not implemented");
    }

    // For循环节点
    ForNode::ForNode(std::unique_ptr<ExprNode> i, std::unique_ptr<ExprNode> c,
                     std::unique_ptr<ExprNode> u, std::unique_ptr<ExprNode> b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)),
          body(std::move(b)) {}

    std::string ForNode::toString() const {
        return "(for (" + (init ? init->toString() : "/* no init */") + "; " +
               (condition ? condition->toString() : "/* no condition */") + "; " +
               (update ? update->toString() : "/* no update */") + ") " +
               body->toString() + ")";
    }

    ValueData ForNode::evaluate() const {
        // 实现For循环表达式的求值逻辑
        throw std::runtime_error("For evaluation not implemented");
    }

    // 块节点（用于多语句）
    BlockNode::BlockNode(std::vector<std::unique_ptr<ExprNode>> stmts)
        : statements(std::move(stmts)) {}

    std::string BlockNode::toString() const {
        std::string result = "{ ";
        for (const auto &stmt : statements) {
            result += stmt->toString();
            result += "; ";
        }
        result += "}";
        return result;
    }

    ValueData BlockNode::evaluate() const {
        // 返回最后一条语句计算值
        if (statements.empty()) {
            return ValueData(); // 返回空值
        }
        return statements.back()->evaluate();
    }

    // While循环节点
    WhileNode::WhileNode(std::unique_ptr<ExprNode> cond,
                         std::unique_ptr<ExprNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    std::string WhileNode::toString() const {
        return "(while (" + condition->toString() + ") " + body->toString() + ")";
    }

    ValueData WhileNode::evaluate() const {
        // 实现While循环表达式的求值逻辑
        throw std::runtime_error("While evaluation not implemented");
    }

    // 模块导入节点
    std::string ImportNode::toString() const {
        return "(import " + moduleName + ")";
    }

    ValueData ImportNode::evaluate() const {
        // 实现模块导入的求值逻辑
        throw std::runtime_error("Module import evaluation not implemented for: " +
                                 moduleName);
    }

    // 循环控制节点
    std::string ControlFlowNode::toString() const {
        return "(" + type + ")";
    }

    ValueData ControlFlowNode::evaluate() const {
        // 实现循环控制的求值逻辑
        throw std::runtime_error("Control flow evaluation not implemented for: " + type);
    }

    // 返回值节点
    ReturnNode::ReturnNode(std::unique_ptr<ExprNode> val) : value(std::move(val)) {}

    std::string ReturnNode::toString() const {
        return "(return " + (value ? value->toString() : "void") + ")";
    }

    ValueData ReturnNode::evaluate() const {
        // 实现返回语句的求值逻辑
        // 这里应该抛出一个异常，由解释器捕获并处理返回值
        throw std::runtime_error("Return evaluation should be handled by the interpreter");
    }

    // 成员访问节点
    MemberAccessNode::MemberAccessNode(std::unique_ptr<ExprNode> obj,
                                       std::string mem)
        : object(std::move(obj)), member(std::move(mem)) {}

    std::string MemberAccessNode::toString() const {
        return "(" + object->toString() + "." + member + ")";
    }

    ValueData MemberAccessNode::evaluate() const {
        // 实现成员访问的求值逻辑
        throw std::runtime_error("Member access evaluation not implemented for: " + member);
    }

    // 索引访问节点
    IndexNode::IndexNode(std::unique_ptr<ExprNode> cont,
                         std::unique_ptr<ExprNode> idx)
        : container(std::move(cont)), index(std::move(idx)) {}

    std::string IndexNode::toString() const {
        return "(" + container->toString() + "[" + index->toString() + "])";
    }

    ValueData IndexNode::evaluate() const {
        // 实现索引访问的求值逻辑
        throw std::runtime_error("Index access evaluation not implemented");
    }

    // 原生函数调用节点
    NativeCallNode::NativeCallNode(std::string name,
                                   std::vector<std::unique_ptr<ExprNode>> args)
        : functionName(std::move(name)), arguments(std::move(args)) {}

    std::string NativeCallNode::toString() const {
        std::string args;
        for (size_t i = 0; i < arguments.size(); i++) {
            if (i > 0)
                args += ", ";
            args += arguments[i]->toString();
        }
        return "(@" + functionName + "(" + args + "))";
    }

    ValueData NativeCallNode::evaluate() const {
        // 实现原生函数调用的求值逻辑
        throw std::runtime_error(
            "Native function call evaluation not implemented for: " + functionName);
    }

    // 数组节点
    ArrayNode::ArrayNode(std::vector<std::unique_ptr<ExprNode>> elems)
        : elements(std::move(elems)) {}

    std::string ArrayNode::toString() const {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); i++) {
            if (i > 0)
                result += ", ";
            result += elements[i]->toString();
        }
        return result + "]";
    }

    ValueData ArrayNode::evaluate() const {
        // 实现数组创建的求值逻辑
        throw std::runtime_error("Array creation evaluation not implemented");
    }

    // 映射表节点
    MapNode::MapNode(
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
            entries)
        : entries(std::move(entries)) {}

    std::string MapNode::toString() const {
        std::string result = "{";
        for (size_t i = 0; i < entries.size(); i++) {
            if (i > 0)
                result += ", ";
            result +=
                entries[i].first->toString() + ": " + entries[i].second->toString();
        }
        return result + "}";
    }

    ValueData MapNode::evaluate() const {
        // 实现映射表创建的求值逻辑
        throw std::runtime_error("Map creation evaluation not implemented");
    }

} // namespace squ