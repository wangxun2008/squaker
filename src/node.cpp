#include "../include/node.h"
#include "../include/type.h"
#include "../include/operator.h"
#include "../include/environment.h"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <variant>

namespace squ {

    // 统一字面量节点
    std::string LiteralNode::string() const {
        return data.string();
    }

    ValueData LiteralNode::evaluate(Environment& env) const {
        return data;
    }

    ValueData &LiteralNode::evaluate_lvalue(Environment& env) const {
        // 字面量节点通常不支持左值求值
        throw std::runtime_error("[squaker.literal] Literal nodes cannot be evaluated as lvalues");
    }

    // 标识符节点
    std::string IdentifierNode::string() const {
        return "v" + std::to_string(index);
    }

    ValueData IdentifierNode::evaluate(Environment& env) const {
        if (!env.exists(name)) {
            throw std::runtime_error("[squaker.identifier] Undefined identifier: " + name);
        }
        return env.get(name);
    }

    ValueData &IdentifierNode::evaluate_lvalue(Environment& env) const {
        // 如果不存在变量，则自动创建一个未初始化的变量
        if (!env.exists(name)) {
            env.set(name, ValueData{ValueType::Nil});
        }
        return env.get(name);
    }

    // 二元操作节点
    BinaryOpNode::BinaryOpNode(std::string op, std::unique_ptr<ExprNode> l,
                               std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string BinaryOpNode::string() const {
        return "(" + left->string() + " " + op + " " + right->string() + ")";
    }

    ValueData BinaryOpNode::evaluate(Environment& env) const {
        ValueData leftVal = left->evaluate(env);
        ValueData rightVal = right->evaluate(env);
        return ApplyBinary(leftVal, op, rightVal);
    }

    ValueData &BinaryOpNode::evaluate_lvalue(Environment& env) const {
        // 二元操作通常不支持左值求值
        throw std::runtime_error("[squaker.binary] Binary operations cannot be evaluated as lvalues");
    }

    // 一元操作节点（前缀）
    UnaryOpNode::UnaryOpNode(std::string op, std::unique_ptr<ExprNode> expr)
        : op(std::move(op)), operand(std::move(expr)) {}

    std::string UnaryOpNode::string() const {
        return "(" + op + operand->string() + ")";
    }

    ValueData UnaryOpNode::evaluate(Environment& env) const {
        ValueData operandVal = operand->evaluate(env);
        return ApplyUnary(op, operandVal);
    }

    ValueData &UnaryOpNode::evaluate_lvalue(Environment& env) const {
        // 一元操作通常不支持左值求值
        throw std::runtime_error("[squaker.unary] Unary operations cannot be evaluated as lvalues");
    }

    // 后缀操作节点
    PostfixOpNode::PostfixOpNode(std::string op, std::unique_ptr<ExprNode> expr)
        : op(std::move(op)), operand(std::move(expr)) {}

    std::string PostfixOpNode::string() const {
        return "(" + operand->string() + op + ")";
    }

    ValueData PostfixOpNode::evaluate(Environment& env) const {
        ValueData operandVal = operand->evaluate(env);
        ValueData& operandRef = operand->evaluate_lvalue(env);
        if (op == "++") {
            if (operandVal.type == ValueType::Integer) {
                long long &val = std::get<long long>(operandRef.value);
                val++;
                operandRef = ValueData{ValueType::Integer, false, val};
                return ValueData{ValueType::Integer, false, val};
            } else if (operandVal.type == ValueType::Real) {
                double &val = std::get<double>(operandRef.value);
                val++;
                operandRef = ValueData{ValueType::Real, false, val};
                return ValueData{ValueType::Real, false, val};
            }
            throw std::runtime_error("[squaker.postfix:'++'] unsupported type for postfix increment");
        } else if (op == "--") {
            if (operandVal.type == ValueType::Integer) {
                long long &val = std::get<long long>(operandRef.value);
                val--;
                operandRef = ValueData{ValueType::Integer, false, val};
                return ValueData{ValueType::Integer, false, val};
            } else if (operandVal.type == ValueType::Real) {
                double &val = std::get<double>(operandRef.value);
                val--;
                operandRef = ValueData{ValueType::Real, false, val};
                return ValueData{ValueType::Real, false, val};
            }
            throw std::runtime_error("[squaker.postfix:'--'] unsupported type for postfix decrement");
        }
        throw std::runtime_error("[squaker.postfix] unknown postfix operator: " + op);
    }

    ValueData &PostfixOpNode::evaluate_lvalue(Environment& env) const {
        // 后缀操作通常不支持左值求值
        throw std::runtime_error("[squaker.postfix] Postfix operations cannot be evaluated as lvalues");
    }

    // 赋值节点
    AssignmentNode::AssignmentNode(std::string op, std::unique_ptr<ExprNode> l,
                                   std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string AssignmentNode::string() const {
        return "(" + left->string() + " " + op + " " + right->string() + ")";
    }

    ValueData AssignmentNode::evaluate(Environment& env) const {
        // 计算左值和右值
        ValueData &leftVal = left->evaluate_lvalue(env);
        ValueData rightVal = right->evaluate(env);

        // 简单赋值
        leftVal = rightVal;
        return leftVal; // 返回赋值后的左值
    }

    ValueData &AssignmentNode::evaluate_lvalue(Environment& env) const {
        throw std::runtime_error("[squaker.assignment] Assignment nodes cannot be evaluated as lvalues");
    }

    // 复合赋值节点（如 +=, -= 等）
    CompoundAssignmentNode::CompoundAssignmentNode(std::string op,
                                                   std::unique_ptr<ExprNode> l,
                                                   std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string CompoundAssignmentNode::string() const {
        return "(" + left->string() + " " + op + " " + right->string() + ")";
    }

    ValueData CompoundAssignmentNode::evaluate(Environment& env) const {
        // 计算左值和右值
        ValueData leftVal = left->evaluate(env);
        ValueData rightVal = right->evaluate(env);
        ValueData &leftValRef = left->evaluate_lvalue(env);

        // 应用二元操作
        leftValRef = ApplyBinary(leftVal, op, rightVal);
        return leftValRef; // 返回赋值后的左值
    }

    ValueData &CompoundAssignmentNode::evaluate_lvalue(Environment& env) const {
        throw std::runtime_error("[squaker.compound_assignment] Compound assignment nodes cannot be evaluated as lvalues");
    }

    // Lambda节点（函数定义）
    LambdaNode::LambdaNode(std::vector<Parameter> params,
                           std::unique_ptr<ExprNode> b)
        : parameters(std::move(params)), body(std::move(b)) {}

    std::string LambdaNode::string() const {
        std::string params;
        for (size_t i = 0; i < parameters.size(); i++) {
            if (i > 0)
                params += ", ";
            params += "v" + std::to_string(parameters[i].slot);
        }
        return "(function (" + params + ") -> " + body->string() + ")";
    }

    ValueData LambdaNode::evaluate(Environment& env) const {
        // 返回一个lambda函数对象
        return ValueData{ValueType::Function, false,
        [this, &env](std::vector<ValueData> args) -> ValueData {
            // 创建一个新的子作用域
            std::unique_ptr<Environment> childEnv = env.new_child();

            // 检查参数数量是否匹配
            if (args.size() != parameters.size()) {
                throw std::runtime_error("[squaker.lambda] Argument count mismatch in lambda call");
            }

            // 设置参数到子作用域
            for (size_t i = 0; i < parameters.size(); i++) {
                childEnv->set(parameters[i].name, args[i]);
            }

            // 执行函数体
            return body->evaluate(*childEnv);
        }};
    }

    ValueData &LambdaNode::evaluate_lvalue(Environment& env) const {
        // Lambda节点通常不支持左值求值
        throw std::runtime_error("[squaker.lambda] Lambda nodes cannot be evaluated as lvalues");
    }

    // 函数应用节点（函数调用）
    ApplyNode::ApplyNode(std::unique_ptr<ExprNode> callee,
                         std::vector<std::unique_ptr<ExprNode>> args)
        : callee(std::move(callee)), arguments(std::move(args)) {}

    std::string ApplyNode::string() const {
        std::string args;
        for (size_t i = 0; i < arguments.size(); i++) {
            if (i > 0)
                args += ", ";
            args += arguments[i]->string();
        }
        return "(apply:" + callee->string() + "(" + args + "))";
    }

    ValueData ApplyNode::evaluate(Environment& env) const {
        // 计算被调用的函数
        ValueData calleeVal = callee->evaluate(env);
        if (calleeVal.type != ValueType::Function) {
            throw std::runtime_error("[squaker.apply] Attempted to call a non-function value");
        }

        // 准备参数
        std::vector<ValueData> argValues;
        for (const auto &arg : arguments) {
            argValues.push_back(arg->evaluate(env));
        }

        // 调用函数
        return std::get<std::function<ValueData(std::vector<ValueData>)>>(calleeVal.value)(argValues);
    }

    ValueData &ApplyNode::evaluate_lvalue(Environment& env) const {
        // 函数应用通常不支持左值求值
        throw std::runtime_error("[squaker.apply] Apply nodes cannot be evaluated as lvalues");
    }

    // 条件节点（if-else if-else）
    IfNode::IfNode(
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
            br,
        std::unique_ptr<ExprNode> eb)
        : branches(std::move(br)), elseBranch(std::move(eb)) {}

    std::string IfNode::string() const {
        std::string result = "(if ";
        for (size_t i = 0; i < branches.size(); i++) {
            const auto &branch = branches[i];
            if (i > 0)
                result += " else if ";
            result +=
                "(" + branch.first->string() + ") " + branch.second->string();
        }
        if (elseBranch) {
            result += " else " + elseBranch->string();
        }
        result += ")";
        return result;
    }

    ValueData IfNode::evaluate(Environment& env) const {
        // 进入作用域并执行条件分支
        std::unique_ptr<Environment> childEnv = env.new_child();
        for (const auto &branch : branches) {
            ValueData condValue = branch.first->evaluate(env);
            if (condValue.type == ValueType::Bool && std::get<bool>(condValue.value)) {
                return branch.second->evaluate(*childEnv); // 条件为真时执行对应分支
            } else if (condValue.type == ValueType::Integer && std::get<long long>(condValue.value) != 0) {
                return branch.second->evaluate(*childEnv); // 整数非零视为真
            } else if (condValue.type == ValueType::Real && std::get<double>(condValue.value) != 0.0) {
                return branch.second->evaluate(*childEnv); // 实数非零视为真
            }
        }
        // 如果没有条件匹配且有else分支，执行else分支
        if (elseBranch) {
            return elseBranch->evaluate(*childEnv);
        }
        // 如果没有匹配的分支，返回Nil
        return ValueData{ValueType::Nil};
    }

    ValueData &IfNode::evaluate_lvalue(Environment& env) const {
        // 条件节点通常不支持左值求值
        throw std::runtime_error("[squaker.if] If nodes cannot be evaluated as lvalues");
    }

    // For循环节点
    ForNode::ForNode(std::unique_ptr<ExprNode> i, std::unique_ptr<ExprNode> c,
                     std::unique_ptr<ExprNode> u, std::unique_ptr<ExprNode> b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)),
          body(std::move(b)) {}

    std::string ForNode::string() const {
        return "(for (" + (init ? init->string() : "/* no init */") + "; " +
               (condition ? condition->string() : "/* no condition */") + "; " +
               (update ? update->string() : "/* no update */") + ") " +
               body->string() + ")";
    }

    ValueData ForNode::evaluate(Environment& env) const {
        // 进入作用域并执行初始化
        std::unique_ptr<Environment> childEnv = env.new_child();
        if (init) init->evaluate(*childEnv);
        ValueData result = ValueData{ValueType::Nil}; // 初始化结果为Nil
        while (true) {
            // 检查循环条件
            if (condition) {
                ValueData condValue = condition->evaluate(*childEnv);
                if (condValue.type == ValueType::Bool && !std::get<bool>(condValue.value)) {
                    break; // 条件为false时退出循环
                } else if (condValue.type == ValueType::Integer && std::get<long long>(condValue.value) == 0) {
                    break; // 如果条件是整数0，视为false
                } else if (condValue.type == ValueType::Real && std::get<double>(condValue.value) == 0.0) {
                    break; // 如果条件是实数0.0，视为false
                }
            }
            // 执行循环体
            result = body->evaluate(*childEnv);
            // 更新
            if (update) update->evaluate(*childEnv);
        }
        return result; // 返回最后一次循环体的结果
    }

    ValueData &ForNode::evaluate_lvalue(Environment& env) const {
        // For循环节点通常不支持左值求值
        throw std::runtime_error("[squaker.for] For nodes cannot be evaluated as lvalues");
    }

    // 块节点（用于多语句）
    BlockNode::BlockNode(std::vector<std::unique_ptr<ExprNode>> stmts)
        : statements(std::move(stmts)) {}

    std::string BlockNode::string() const {
        std::string result = "{ ";
        for (const auto &stmt : statements) {
            result += stmt->string();
            result += "; ";
        }
        result += "}";
        return result;
    }

    ValueData BlockNode::evaluate(Environment& env) const {
        // 进入作用域并执行每个语句
        std::unique_ptr<Environment> childEnv = env.new_child();
        if (statements.empty()) {
            return ValueData{ValueType::Nil}; // 如果没有语句，返回Nil
        }
        ValueData result = ValueData{ValueType::Nil}; // 初始化结果为Nil
        for (const auto &stmt : statements) {
            result = stmt->evaluate(*childEnv);
        }
        return result; // 返回最后一条语句的结果
    }

    ValueData &BlockNode::evaluate_lvalue(Environment& env) const {
        // 块节点通常不支持左值求值
        throw std::runtime_error("[squaker.block] Block nodes cannot be evaluated as lvalues");
    }

    // While循环节点
    WhileNode::WhileNode(std::unique_ptr<ExprNode> cond,
                         std::unique_ptr<ExprNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    std::string WhileNode::string() const {
        return "(while (" + condition->string() + ") " + body->string() + ")";
    }

    ValueData WhileNode::evaluate(Environment& env) const {
        // 进入作用域并执行循环
        std::unique_ptr<Environment> childEnv = env.new_child();
        ValueData result = ValueData{ValueType::Nil}; // 初始化结果为Nil
        while (true) {
            // 计算条件
            ValueData condValue = condition->evaluate(*childEnv);
            if (condValue.type == ValueType::Bool && !std::get<bool>(condValue.value)) {
                break; // 条件为false时退出循环
            } else if (condValue.type == ValueType::Integer && std::get<long long>(condValue.value) == 0) {
                break; // 如果条件是整数0，视为false
            } else if (condValue.type == ValueType::Real && std::get<double>(condValue.value) == 0.0) {
                break; // 如果条件是实数0.0，视为false
            }
            // 执行循环体
            result = body->evaluate(*childEnv);
        }
        return result; // 返回最后一次循环体的结果
    }

    ValueData &WhileNode::evaluate_lvalue(Environment& env) const {
        // While循环节点通常不支持左值求值
        throw std::runtime_error("[squaker.while] While nodes cannot be evaluated as lvalues");
    }

    // 模块导入节点
    std::string ImportNode::string() const {
        return "(import " + moduleName + ")";
    }

    ValueData ImportNode::evaluate(Environment& env) const {
        // 实现模块导入的求值逻辑
        throw std::runtime_error("Module import evaluation not implemented for: " +
                                 moduleName);
    }

    ValueData &ImportNode::evaluate_lvalue(Environment& env) const {
        // 模块导入通常不支持左值求值
        throw std::runtime_error("[squaker.import] Import nodes cannot be evaluated as lvalues");
    }

    // 循环控制节点
    std::string ControlFlowNode::string() const {
        return "(" + control_type + ")";
    }

    ValueData ControlFlowNode::evaluate(Environment& env) const {
        // 实现控制流的求值逻辑
        if (control_type == "break") {
            throw std::runtime_error("Break statement evaluation not implemented");
        } else if (control_type == "continue") {
            throw std::runtime_error("Continue statement evaluation not implemented");
        } else {
            throw std::runtime_error("Unknown control flow type: " + control_type);
        }
    }

    ValueData &ControlFlowNode::evaluate_lvalue(Environment& env) const {
        // 控制流节点通常不支持左值求值
        throw std::runtime_error("[squaker.control] Control flow nodes cannot be evaluated as lvalues");
    }

    // 返回值节点
    ReturnNode::ReturnNode(std::unique_ptr<ExprNode> val) : value(std::move(val)) {}

    std::string ReturnNode::string() const {
        return "(return " + (value ? value->string() : "void") + ")";
    }

    ValueData ReturnNode::evaluate(Environment& env) const {
        // 实现返回语句的求值逻辑
        // 这里应该抛出一个异常，由解释器捕获并处理返回值
        throw std::runtime_error("Return evaluation should be handled by the interpreter");
    }

    ValueData &ReturnNode::evaluate_lvalue(Environment& env) const {
        // 返回值节点通常不支持左值求值
        throw std::runtime_error("[squaker.return] Return nodes cannot be evaluated as lvalues");
    }

    // 成员访问节点
    MemberAccessNode::MemberAccessNode(std::unique_ptr<ExprNode> obj,
                                       std::string mem)
        : object(std::move(obj)), member(std::move(mem)) {}

    std::string MemberAccessNode::string() const {
        return "(" + object->string() + "." + member + ")";
    }

    ValueData MemberAccessNode::evaluate(Environment& env) const {
        // 计算对象的值
        ValueData objValue = object->evaluate(env);

        // 检查对象类型
        if (objValue.type != ValueType::Map) {
            throw std::runtime_error("[squaker.member] Member access on non-map type: " + objValue.string());
        }

        // 获取映射中的成员
        auto &map = std::get<std::unordered_map<std::string, ValueData>>(objValue.value);
        auto it = map.find(member);
        if (it == map.end()) {
            throw std::runtime_error("[squaker.member] Member not found: " + member);
        }

        return it->second; // 返回成员值
    }

    ValueData &MemberAccessNode::evaluate_lvalue(Environment& env) const {
        // 计算对象的值
        ValueData &objValue = object->evaluate_lvalue(env);

        // 检查对象类型
        if (objValue.type != ValueType::Map) {
            throw std::runtime_error("[squaker.member] Member access on non-map type: " + objValue.string());
        }

        // 获取映射中的成员
        auto &map = std::get<std::unordered_map<std::string, ValueData>>(objValue.value);
        auto it = map.find(member);
        if (it == map.end()) {
            throw std::runtime_error("[squaker.member] Member not found: " + member);
        }

        return it->second; // 返回成员值的引用
    }

    // 索引访问节点
    IndexNode::IndexNode(std::unique_ptr<ExprNode> cont,
                         std::unique_ptr<ExprNode> idx)
        : container(std::move(cont)), index(std::move(idx)) {}

    std::string IndexNode::string() const {
        return "(" + container->string() + "[" + index->string() + "])";
    }

    ValueData IndexNode::evaluate(Environment& env) const {
        // 计算容器和索引
        ValueData containerValue = container->evaluate(env);
        ValueData indexValue = index->evaluate(env);

        // 检查容器类型
        if (containerValue.type != ValueType::Array && containerValue.type != ValueType::Map) {
            throw std::runtime_error("[squaker.index] Indexing on non-array/map type: " + containerValue.string());
        }

        // 处理数组索引
        if (containerValue.type == ValueType::Array) {
            if (indexValue.type != ValueType::Integer) {
                throw std::runtime_error("[squaker.index] Array index must be an integer: " + indexValue.string());
            }
            auto &array = std::get<std::vector<ValueData>>(containerValue.value);
            long long idx = std::get<long long>(indexValue.value);
            if (idx < 0 || idx >= static_cast<long long>(array.size())) {
                throw std::out_of_range("[squaker.index] Array index out of bounds");
            }
            return array[idx]; // 返回数组元素
        }

        // 处理映射索引
        if (containerValue.type == ValueType::Map) {
            if (indexValue.type != ValueType::String) {
                throw std::runtime_error("[squaker.index] Map index must be a string: " + indexValue.string());
            }
            auto &map = std::get<std::unordered_map<std::string, ValueData>>(containerValue.value);
            auto it = map.find(std::get<std::string>(indexValue.value));
            if (it == map.end()) {
                throw std::runtime_error("[squaker.index] Key not found in map: " + std::get<std::string>(indexValue.value));
            }
            return it->second; // 返回映射值
        }

        throw std::runtime_error("[squaker.index] Unsupported container type for indexing");
    }

    ValueData &IndexNode::evaluate_lvalue(Environment& env) const {
        // 计算容器和索引
        ValueData &containerValue = container->evaluate_lvalue(env);
        ValueData indexValue = index->evaluate(env);

        // 检查容器类型
        if (containerValue.type != ValueType::Array && containerValue.type != ValueType::Map) {
            throw std::runtime_error("[squaker.index] Indexing on non-array/map type: " + containerValue.string());
        }

        // 处理数组索引
        if (containerValue.type == ValueType::Array) {
            if (indexValue.type != ValueType::Integer) {
                throw std::runtime_error("[squaker.index] Array index must be an integer: " + indexValue.string());
            }
            auto &array = std::get<std::vector<ValueData>>(containerValue.value);
            long long idx = std::get<long long>(indexValue.value);
            if (idx < 0 || idx >= static_cast<long long>(array.size())) {
                throw std::out_of_range("[squaker.index] Array index out of bounds");
            }
            return array[idx]; // 返回数组元素的引用
        }

        // 处理映射索引
        if (containerValue.type == ValueType::Map) {
            if (indexValue.type != ValueType::String) {
                throw std::runtime_error("[squaker.index] Map index must be a string: " + indexValue.string());
            }
            auto &map = std::get<std::unordered_map<std::string, ValueData>>(containerValue.value);
            auto it = map.find(std::get<std::string>(indexValue.value));
            if (it == map.end()) {
                throw std::runtime_error("[squaker.index] Key not found in map: " + std::get<std::string>(indexValue.value));
            }
            return it->second; // 返回映射值的引用
        }

        throw std::runtime_error("[squaker.index] Unsupported container type for indexing");
    }

    // 原生函数调用节点
    NativeCallNode::NativeCallNode(std::string name,
                                   std::vector<std::unique_ptr<ExprNode>> args)
        : functionName(std::move(name)), arguments(std::move(args)) {}

    std::string NativeCallNode::string() const {
        std::string args;
        for (size_t i = 0; i < arguments.size(); i++) {
            if (i > 0)
                args += ", ";
            args += arguments[i]->string();
        }
        return "(@" + functionName + "(" + args + "))";
    }

    ValueData NativeCallNode::evaluate(Environment& env) const {
        // 实现原生函数调用的求值逻辑
        throw std::runtime_error(
            "Native function call evaluation not implemented for: " + functionName);
    }

    ValueData &NativeCallNode::evaluate_lvalue(Environment& env) const {
        // 原生函数调用通常不支持左值求值
        throw std::runtime_error("[squaker.native] Native call nodes cannot be evaluated as lvalues");
    }

    // 数组节点
    ArrayNode::ArrayNode(std::vector<std::unique_ptr<ExprNode>> elems)
        : elements(std::move(elems)) {}

    std::string ArrayNode::string() const {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); i++) {
            if (i > 0)
                result += ", ";
            result += elements[i]->string();
        }
        return result + "]";
    }

    ValueData ArrayNode::evaluate(Environment& env) const {
        // 实现数组创建的求值逻辑
        ValueData arrayValue;
        arrayValue.type = ValueType::Array;
        std::vector<ValueData> arrayElements;

        for (const auto &elem : elements) {
            arrayElements.push_back(elem->evaluate(env));
        }

        arrayValue.value = std::move(arrayElements);
        return arrayValue; // 返回创建的数组
    }

    ValueData &ArrayNode::evaluate_lvalue(Environment& env) const {
        // 数组节点通常不支持左值求值
        throw std::runtime_error("[squaker.array] Array nodes cannot be evaluated as lvalues");
    }

    // 映射表节点
    MapNode::MapNode(
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>>
            entries)
        : entries(std::move(entries)) {}

    std::string MapNode::string() const {
        std::string result = "{";
        for (size_t i = 0; i < entries.size(); i++) {
            if (i > 0)
                result += ", ";
            result +=
                entries[i].first->string() + ": " + entries[i].second->string();
        }
        return result + "}";
    }

    ValueData MapNode::evaluate(Environment& env) const {
        // 实现映射表创建的求值逻辑
        ValueData mapValue;
        mapValue.type = ValueType::Map;
        std::unordered_map<std::string, ValueData> mapEntries;

        for (const auto &entry : entries) {
            ValueData key = entry.first->evaluate(env);
            if (key.type != ValueType::String) {
                throw std::runtime_error("[squaker.map] Map keys must be strings: " + key.string());
            }
            ValueData value = entry.second->evaluate(env);
            mapEntries[std::get<std::string>(key.value)] = value;
        }

        mapValue.value = std::move(mapEntries);
        return mapValue; // 返回创建的映射表
    }

    ValueData &MapNode::evaluate_lvalue(Environment& env) const {
        // 映射表节点通常不支持左值求值
        throw std::runtime_error("[squaker.map] Map nodes cannot be evaluated as lvalues");
    }

} // namespace squ