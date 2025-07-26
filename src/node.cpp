#include "../include/node.h"
#include "../include/operator.h"
#include "../include/type.h"
#include "../include/vm.h"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <variant>

namespace squ {

    // 控制流异常类
    class BreakException : public std::exception {
      public:
        BreakException() : std::exception() {}
    };

    class ContinueException : public std::exception {
      public:
        ContinueException() : std::exception() {}
    };

    class ReturnException : public std::exception {
      public:
        ValueData value; // 返回值
        explicit ReturnException(ValueData val) : value(std::move(val)) {}
    };

    // 统一字面量节点
    std::string LiteralNode::string() const {
        return data.string();
    }

    ValueData LiteralNode::evaluate(VM &vm) const {
        return data;
    }

    ValueData &LiteralNode::evaluate_lvalue(VM &vm) const {
        // 字面量节点通常不支持左值求值
        throw std::runtime_error("[squaker.literal] Literal nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> LiteralNode::clone() const {
        return std::make_unique<LiteralNode>(data);
    }

    // 标识符节点
    std::string IdentifierNode::string() const {
        return "v" + std::to_string(index);
    }

    ValueData IdentifierNode::evaluate(VM &vm) const {
        ValueData &data = vm.local(index);
        if (data.type == ValueType::Nil) {
            throw std::runtime_error("[squaker.identifier] Undefined identifier: " + name);
        }
        return data;
    }

    ValueData &IdentifierNode::evaluate_lvalue(VM &vm) const {
        return vm.local(index);
    }

    std::unique_ptr<ExprNode> IdentifierNode::clone() const {
        return std::make_unique<IdentifierNode>(name, index);
    }

    // 常量字面量节点
    ConstantNode::ConstantNode(std::unique_ptr<ExprNode> expr) : expr(std::move(expr)) {}

    std::string ConstantNode::string() const {
        return "const " + expr->string();
    }

    ValueData ConstantNode::evaluate(VM &vm) const {
        ValueData data = expr->evaluate(vm);
        data.is_const = true;
        return data;
    }

    ValueData &ConstantNode::evaluate_lvalue(VM &vm) const {
        // 常量节点通常不支持左值求值
        throw std::runtime_error("[squaker.constant] Constant nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ConstantNode::clone() const {
        return std::make_unique<ConstantNode>(expr->clone());
    }

    // 二元操作节点
    BinaryOpNode::BinaryOpNode(std::string op, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string BinaryOpNode::string() const {
        return "(" + left->string() + " " + op + " " + right->string() + ")";
    }

    ValueData BinaryOpNode::evaluate(VM &vm) const {
        // 计算左值和右值
        ValueData leftVal = left->evaluate(vm);
        ValueData rightVal = right->evaluate(vm);

        // 应用二元操作
        return ApplyBinary(leftVal, op, rightVal);
    }

    ValueData &BinaryOpNode::evaluate_lvalue(VM &vm) const {
        // 二元操作通常不支持左值求值
        throw std::runtime_error("[squaker.binary] Binary operations cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> BinaryOpNode::clone() const {
        return std::make_unique<BinaryOpNode>(op, left->clone(), right->clone());
    }

    // 一元操作节点（前缀）
    UnaryOpNode::UnaryOpNode(std::string op, std::unique_ptr<ExprNode> expr)
        : op(std::move(op)), operand(std::move(expr)) {}

    std::string UnaryOpNode::string() const {
        return "(" + op + operand->string() + ")";
    }

    ValueData UnaryOpNode::evaluate(VM &vm) const {
        ValueData operandVal = operand->evaluate(vm);
        ValueData &operandRef = operand->evaluate_lvalue(vm);

        // 应用一元操作
        return ApplyUnary(op, operandVal);
    }

    ValueData &UnaryOpNode::evaluate_lvalue(VM &vm) const {
        // 一元操作通常不支持左值求值
        throw std::runtime_error("[squaker.unary] Unary operations cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> UnaryOpNode::clone() const {
        return std::make_unique<UnaryOpNode>(op, operand->clone());
    }

    // 后缀操作节点
    PostfixOpNode::PostfixOpNode(std::string op, std::unique_ptr<ExprNode> expr)
        : op(std::move(op)), operand(std::move(expr)) {}

    std::string PostfixOpNode::string() const {
        return "(" + operand->string() + op + ")";
    }

    ValueData PostfixOpNode::evaluate(VM &vm) const {
        ValueData operandVal = operand->evaluate(vm);
        if (operandVal.is_const == true) {
            throw std::runtime_error("[squaker.postfix] Cannot apply postfix operator to const");
        }
        ValueData &operandRef = operand->evaluate_lvalue(vm);
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

    ValueData &PostfixOpNode::evaluate_lvalue(VM &vm) const {
        // 后缀操作通常不支持左值求值
        throw std::runtime_error("[squaker.postfix] Postfix operations cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> PostfixOpNode::clone() const {
        return std::make_unique<PostfixOpNode>(op, operand->clone());
    }

    // 赋值节点
    AssignmentNode::AssignmentNode(std::string op, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string AssignmentNode::string() const {
        return "(" + left->string() + " " + op + " " + right->string() + ")";
    }

    ValueData AssignmentNode::evaluate(VM &vm) const {
        // 计算左值和右值
        ValueData &leftValRef = left->evaluate_lvalue(vm);
        if (leftValRef.is_const == true) {
            throw std::runtime_error("[squaker.assignment] Cannot assign to const variable");
        }
        ValueData rightVal = right->evaluate(vm);

        // 应用二元操作
        leftValRef = rightVal; // 简单赋值
        leftValRef.is_const = false; // 确保左值不是常量
        return leftValRef;     // 返回赋值后的左值
    }

    ValueData &AssignmentNode::evaluate_lvalue(VM &vm) const {
        throw std::runtime_error("[squaker.assignment] Assignment nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> AssignmentNode::clone() const {
        return std::make_unique<AssignmentNode>(op, left->clone(), right->clone());
    }

    // 复合赋值节点（如 +=, -= 等）
    CompoundAssignmentNode::CompoundAssignmentNode(std::string op, std::unique_ptr<ExprNode> l,
                                                   std::unique_ptr<ExprNode> r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    std::string CompoundAssignmentNode::string() const {
        return "(" + left->string() + " " + op + " " + right->string() + ")";
    }

    ValueData CompoundAssignmentNode::evaluate(VM &vm) const {
        // 计算左值和右值
        ValueData leftVal = left->evaluate(vm);
        if (leftVal.is_const == true) {
            throw std::runtime_error("[squaker.assignment] Cannot assign to const variable");
        }
        ValueData &leftValRef = left->evaluate_lvalue(vm);
        ValueData rightVal = right->evaluate(vm);

        // 应用二元操作
        leftValRef = ApplyBinary(leftVal, op, rightVal);
        leftValRef.is_const = false; // 确保左值不是常量
        return leftValRef; // 返回赋值后的左值
    }

    ValueData &CompoundAssignmentNode::evaluate_lvalue(VM &vm) const {
        throw std::runtime_error(
            "[squaker.compound_assignment] Compound assignment nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> CompoundAssignmentNode::clone() const {
        return std::make_unique<CompoundAssignmentNode>(op, left->clone(), right->clone());
    }

    // Lambda节点（函数定义）
    LambdaNode::LambdaNode(std::vector<Parameter> params, std::unique_ptr<ExprNode> b)
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

    ValueData LambdaNode::evaluate(VM &vm) const {
        // 返回一个lambda函数对象
        return ValueData{ValueType::Function, false,
                         [body = body, parameters = parameters, maxSlot = maxSlot](std::vector<ValueData> args,
                                                                                   VM &vm) -> ValueData {
                             VMGuard guard(vm, parameters.size());

                             // 检查参数数量是否匹配
                             // printf("[squaker.lambda] Evaluating lambda with %zu parameters\n", parameters.size());
                             if (args.size() != parameters.size()) {
                                 throw std::runtime_error(
                                     "[squaker.lambda] Argument count mismatch in lambda call (expected " +
                                     std::to_string(parameters.size()) + ", got " + std::to_string(args.size()) + ")");
                             }

                             // 设置参数到虚拟机的局部变量
                             // printf("[squaker.lambda] Setting parameters in VM\n");
                             for (size_t i = 0; i < parameters.size(); i++) {
                                 vm.local(parameters[i].slot) = args[i];
                             }

                             // 执行函数体
                             // printf("[squaker.lambda] Executing lambda body\n");
                             try {
                                 ValueData result = body->evaluate(vm);
                                 return result; // 返回函数体的结果
                                 // printf("[squaker.lambda] Lambda execution completed, returning value\n");
                             } catch (const ReturnException &e) {
                                 // 捕获返回异常，直接返回结果
                                 // printf("[squaker.lambda] Lambda returned value: %s\n", e.value.string().c_str());
                                 return e.value;
                             }

                             // 如果没有返回异常，默认返回Nil
                             return ValueData{ValueType::Nil, false, 0.0};
                         }};
    }

    ValueData &LambdaNode::evaluate_lvalue(VM &vm) const {
        // Lambda节点通常不支持左值求值
        throw std::runtime_error("[squaker.lambda] Lambda nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> LambdaNode::clone() const {
        return std::make_unique<LambdaNode>(parameters, body->clone());
    }

    // 函数应用节点（函数调用）
    ApplyNode::ApplyNode(std::unique_ptr<ExprNode> callee, std::vector<std::unique_ptr<ExprNode>> args)
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

    ValueData ApplyNode::evaluate(VM &vm) const {
        // printf("[squaker.apply] Evaluating function application: %s\n", callee->string().c_str());
        // 计算被调用的函数
        ValueData calleeVal = callee->evaluate(vm);
        // printf("[squaker.apply] Function value type: %d\n", static_cast<int>(calleeVal.type));
        if (calleeVal.type != ValueType::Function) {
            throw std::runtime_error("[squaker.apply] Attempted to call a non-function value");
        }

        // 准备参数
        // printf("[squaker.apply] Preparing arguments for function call\n");
        std::vector<ValueData> argValues;
        for (const auto &arg : arguments) {
            argValues.push_back(arg->evaluate(vm));
        }

        // 调用函数
        // printf("[squaker.apply] Calling function with %zu arguments\n", argValues.size());
        return std::get<std::function<ValueData(std::vector<ValueData>&, VM &)>>(calleeVal.value)(argValues, vm);
    }

    ValueData &ApplyNode::evaluate_lvalue(VM &vm) const {
        // 函数应用通常不支持左值求值
        throw std::runtime_error("[squaker.apply] Apply nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ApplyNode::clone() const {
        std::vector<std::unique_ptr<ExprNode>> clonedArgs;
        for (const auto &arg : arguments) {
            clonedArgs.push_back(arg->clone());
        }
        return std::make_unique<ApplyNode>(callee->clone(), std::move(clonedArgs));
    }

    // 条件节点（if-else if-else）
    IfNode::IfNode(std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> br,
                   std::unique_ptr<ExprNode> eb)
        : branches(std::move(br)), elseBranch(std::move(eb)) {}

    std::string IfNode::string() const {
        std::string result = "(if ";
        for (size_t i = 0; i < branches.size(); i++) {
            const auto &branch = branches[i];
            if (i > 0)
                result += " else if ";
            result += "(" + branch.first->string() + ") " + branch.second->string();
        }
        if (elseBranch) {
            result += " else " + elseBranch->string();
        }
        result += ")";
        return result;
    }

    ValueData IfNode::evaluate(VM &vm) const {
        // 执行条件分支
        for (const auto &branch : branches) {
            ValueData condValue = branch.first->evaluate(vm);
            if (condValue.type == ValueType::Bool && std::get<bool>(condValue.value)) {
                return branch.second->evaluate(vm); // 条件为真时执行对应分支
            } else if (condValue.type == ValueType::Integer && std::get<long long>(condValue.value) != 0) {
                return branch.second->evaluate(vm); // 整数非零视为真
            } else if (condValue.type == ValueType::Real && std::get<double>(condValue.value) != 0.0) {
                return branch.second->evaluate(vm); // 实数非零视为真
            }
        }
        // 如果没有条件匹配且有else分支，执行else分支
        if (elseBranch) {
            return elseBranch->evaluate(vm);
        }
        // 如果没有匹配的分支，返回Nil
        return ValueData{ValueType::Nil};
    }

    ValueData &IfNode::evaluate_lvalue(VM &vm) const {
        // 条件节点通常不支持左值求值
        throw std::runtime_error("[squaker.if] If nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> IfNode::clone() const {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> clonedBranches;
        for (const auto &branch : branches) {
            clonedBranches.emplace_back(branch.first->clone(), branch.second->clone());
        }
        return std::make_unique<IfNode>(std::move(clonedBranches), elseBranch ? elseBranch->clone() : nullptr);
    }

    // For循环节点
    ForNode::ForNode(std::unique_ptr<ExprNode> i, std::unique_ptr<ExprNode> c, std::unique_ptr<ExprNode> u,
                     std::unique_ptr<ExprNode> b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {}

    std::string ForNode::string() const {
        return "(for (" + (init ? init->string() : "/* no init */") + "; " +
               (condition ? condition->string() : "/* no condition */") + "; " +
               (update ? update->string() : "/* no update */") + ") " + body->string() + ")";
    }

    ValueData ForNode::evaluate(VM &vm) const {
        // 执行初始化
        if (init)
            init->evaluate(vm);
        ValueData result = ValueData{ValueType::Nil}; // 初始化结果为Nil
        while (true) {
            // 检查循环条件
            if (condition) {
                ValueData condValue = condition->evaluate(vm);
                if (condValue.type == ValueType::Bool && !std::get<bool>(condValue.value)) {
                    break; // 条件为false时退出循环
                } else if (condValue.type == ValueType::Integer && std::get<long long>(condValue.value) == 0) {
                    break; // 如果条件是整数0，视为false
                } else if (condValue.type == ValueType::Real && std::get<double>(condValue.value) == 0.0) {
                    break; // 如果条件是实数0.0，视为false
                }
            }
            try {
                // 执行循环体
                result = body->evaluate(vm);
            } catch (const BreakException &) {
                break; // 捕获break异常，退出循环
            } catch (const ContinueException &) {
                continue; // 捕获continue异常，跳过当前循环迭代
            } catch (const ReturnException &e) {
                throw e; // 直接抛出返回异常
            }
            // 更新
            if (update)
                update->evaluate(vm);
        }
        return result; // 返回最后一次循环体的结果
    }

    ValueData &ForNode::evaluate_lvalue(VM &vm) const {
        // For循环节点通常不支持左值求值
        throw std::runtime_error("[squaker.for] For nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ForNode::clone() const {
        return std::make_unique<ForNode>(init ? init->clone() : nullptr, condition ? condition->clone() : nullptr,
                                         update ? update->clone() : nullptr, body->clone());
    }

    // 块节点（用于多语句）
    BlockNode::BlockNode(std::vector<std::unique_ptr<ExprNode>> stmts) : statements(std::move(stmts)) {}

    std::string BlockNode::string() const {
        std::string result = "{ ";
        for (const auto &stmt : statements) {
            result += stmt->string();
            result += "; ";
        }
        result += "}";
        return result;
    }

    ValueData BlockNode::evaluate(VM &vm) const {
        if (statements.empty()) {
            return ValueData{ValueType::Nil}; // 如果没有语句，返回Nil
        }
        ValueData result = ValueData{ValueType::Nil}; // 初始化结果为Nil
        for (const auto &stmt : statements) {
            result = stmt->evaluate(vm);
        }
        return result; // 返回最后一条语句的结果
    }

    ValueData &BlockNode::evaluate_lvalue(VM &vm) const {
        // 块节点通常不支持左值求值
        throw std::runtime_error("[squaker.block] Block nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> BlockNode::clone() const {
        std::vector<std::unique_ptr<ExprNode>> clonedStatements;
        for (const auto &stmt : statements) {
            clonedStatements.push_back(stmt->clone());
        }
        return std::make_unique<BlockNode>(std::move(clonedStatements));
    }

    // While循环节点
    WhileNode::WhileNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    std::string WhileNode::string() const {
        return "(while (" + condition->string() + ") " + body->string() + ")";
    }

    ValueData WhileNode::evaluate(VM &vm) const {
        // 进入作用域并执行循环
        ValueData result = ValueData{ValueType::Nil}; // 初始化结果为Nil
        while (true) {
            // 计算条件
            ValueData condValue = condition->evaluate(vm);
            if (condValue.type == ValueType::Bool && !std::get<bool>(condValue.value)) {
                break; // 条件为false时退出循环
            } else if (condValue.type == ValueType::Integer && std::get<long long>(condValue.value) == 0) {
                break; // 如果条件是整数0，视为false
            } else if (condValue.type == ValueType::Real && std::get<double>(condValue.value) == 0.0) {
                break; // 如果条件是实数0.0，视为false
            }
            try {
                // 执行循环体
                result = body->evaluate(vm);
            } catch (const BreakException &) {
                break; // 捕获break异常，退出循环
            } catch (const ContinueException &) {
                continue; // 捕获continue异常，跳过当前循环迭代
            } catch (const ReturnException &e) {
                throw e; // 直接抛出返回异常
            }
        }
        return result; // 返回最后一次循环体的结果
    }

    ValueData &WhileNode::evaluate_lvalue(VM &vm) const {
        // While循环节点通常不支持左值求值
        throw std::runtime_error("[squaker.while] While nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> WhileNode::clone() const {
        return std::make_unique<WhileNode>(condition->clone(), body->clone());
    }

    // 模块导入节点
    std::string ImportNode::string() const {
        return "(import " + moduleName + ")";
    }

    ValueData ImportNode::evaluate(VM &vm) const {
        // 实现模块导入的求值逻辑
        throw std::runtime_error("Module import evaluation not implemented for: " + moduleName);
    }

    ValueData &ImportNode::evaluate_lvalue(VM &vm) const {
        // 模块导入通常不支持左值求值
        throw std::runtime_error("[squaker.import] Import nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ImportNode::clone() const {
        return std::make_unique<ImportNode>(moduleName);
    }

    // 循环控制节点
    std::string ControlFlowNode::string() const {
        return "(" + control_type + ")";
    }

    ValueData ControlFlowNode::evaluate(VM &vm) const {
        // 实现控制流的求值逻辑
        if (control_type == "break") {
            throw BreakException(); // 抛出break异常
        } else if (control_type == "continue") {
            throw ContinueException(); // 抛出continue异常
        } else {
            throw std::runtime_error("Unknown control flow type: " + control_type);
        }
    }

    ValueData &ControlFlowNode::evaluate_lvalue(VM &vm) const {
        // 控制流节点通常不支持左值求值
        throw std::runtime_error("[squaker.control] Control flow nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ControlFlowNode::clone() const {
        return std::make_unique<ControlFlowNode>(control_type);
    }

    // 返回值节点
    ReturnNode::ReturnNode(std::unique_ptr<ExprNode> val) : value(std::move(val)) {}

    std::string ReturnNode::string() const {
        return "(return " + (value ? value->string() : "void") + ")";
    }

    ValueData ReturnNode::evaluate(VM &vm) const {
        if (!value) {
            throw ReturnException(ValueData{ValueType::Nil}); // 返回Nil
        }
        ValueData returnValue = value->evaluate(vm);
        throw ReturnException(std::move(returnValue)); // 抛出返回异常
    }

    ValueData &ReturnNode::evaluate_lvalue(VM &vm) const {
        // 返回值节点通常不支持左值求值
        throw std::runtime_error("[squaker.return] Return nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ReturnNode::clone() const {
        return std::make_unique<ReturnNode>(value ? value->clone() : nullptr);
    }

    // 成员访问节点
    MemberAccessNode::MemberAccessNode(std::unique_ptr<ExprNode> obj, std::string mem)
        : object(std::move(obj)), member(std::move(mem)) {}

    std::string MemberAccessNode::string() const {
        return "(" + object->string() + "." + member + ")";
    }

    ValueData MemberAccessNode::evaluate(VM &vm) const {
        // 计算对象的值
        ValueData objValue = object->evaluate(vm);

        // 检查对象类型
        if (objValue.type != ValueType::Table) {
            throw std::runtime_error("[squaker.member] Member access on non-table type: " + objValue.string());
        }

        // 获取映射中的成员
        auto &map = std::get<TableData>(objValue.value);
        return map.dot_at(member); // 返回成员值
    }

    ValueData &MemberAccessNode::evaluate_lvalue(VM &vm) const {
        // 计算对象的值
        ValueData &objValue = object->evaluate_lvalue(vm);

        // 检查对象类型
        if (objValue.type != ValueType::Table) {
            throw std::runtime_error("[squaker.member] Member access on non-map type: " + objValue.string());
        }

        // 获取映射中的成员
        auto &map = std::get<TableData>(objValue.value);
        return map.dot(member); // 返回成员值
    }

    std::unique_ptr<ExprNode> MemberAccessNode::clone() const {
        return std::make_unique<MemberAccessNode>(object->clone(), member);
    }

    // 索引访问节点
    IndexNode::IndexNode(std::unique_ptr<ExprNode> cont, std::unique_ptr<ExprNode> idx)
        : container(std::move(cont)), index(std::move(idx)) {}

    std::string IndexNode::string() const {
        return "(" + container->string() + "[" + index->string() + "])";
    }

    ValueData IndexNode::evaluate(VM &vm) const {
        // 计算容器和索引
        ValueData containerValue = container->evaluate(vm);
        ValueData indexValue = index->evaluate(vm);

        // 检查容器类型
        if (containerValue.type != ValueType::Array && containerValue.type != ValueType::Table) {
            throw std::runtime_error("[squaker.index] Indexing on non-table type: " + containerValue.string());
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

        // 处理表索引
        if (containerValue.type == ValueType::Table) {
            auto &table = std::get<TableData>(containerValue.value);
            return table.index_at(indexValue); // 返回表值
        }

        throw std::runtime_error("[squaker.index] Unsupported container type for indexing");
    }

    ValueData &IndexNode::evaluate_lvalue(VM &vm) const {
        // 计算容器和索引
        ValueData &containerValue = container->evaluate_lvalue(vm);
        ValueData indexValue = index->evaluate(vm);

        // 检查容器类型
        if (containerValue.type != ValueType::Array && containerValue.type != ValueType::Table) {
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

        // 处理表索引
        if (containerValue.type == ValueType::Table) {
            auto &table = std::get<TableData>(containerValue.value);
            return table.index(indexValue); // 返回表值
        }

        throw std::runtime_error("[squaker.index] Unsupported container type for indexing");
    }

    std::unique_ptr<ExprNode> IndexNode::clone() const {
        return std::make_unique<IndexNode>(container->clone(), index->clone());
    }

    // 原生函数调用节点
    NativeCallNode::NativeCallNode(std::string name, std::vector<std::unique_ptr<ExprNode>> args)
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

    ValueData NativeCallNode::evaluate(VM &vm) const {
        // 标准输出
        if (functionName == "print") {
            // 特殊处理print函数
            for (const auto &arg : arguments) {
                ValueData argValue = arg->evaluate(vm);
                std::cout << argValue.string() << " ";
            }
            std::cout << std::endl;
            return ValueData{ValueType::Nil}; // print函数返回Nil
        }
        // 打印调用栈
        if (functionName == "stack") {
            // 打印当前调用栈
            vm.printStack();
            return ValueData{ValueType::Nil}; // stack函数返回Nil
        }
        // 打印类型
        if (functionName == "type") {
            // 特殊处理type函数
            ValueData argValue = arguments[0]->evaluate(vm);
            switch (argValue.type) {
                case ValueType::Nil:
                    return ValueData{ValueType::String, false, "nil"};
                case ValueType::Bool:
                    return ValueData{ValueType::String, false, "bool"};
                case ValueType::Integer:
                    return ValueData{ValueType::String, false, "integer"};
                case ValueType::Real:
                    return ValueData{ValueType::String, false, "real"};
                case ValueType::String: 
                    return ValueData{ValueType::String, false, "string"};
                case ValueType::Array: 
                    return ValueData{ValueType::String, false, "array"};
                case ValueType::Table: 
                    return ValueData{ValueType::String, false, "table"};
                case ValueType::Function: {
                    return ValueData{ValueType::String, false, "function"};
                }
            }
            return ValueData{ValueType::Nil}; // type函数返回Nil
        }
        // 其他原生函数调用
        throw std::runtime_error("Native function call evaluation not implemented for: " + functionName);
    }

    ValueData &NativeCallNode::evaluate_lvalue(VM &vm) const {
        // 原生函数调用通常不支持左值求值
        throw std::runtime_error("[squaker.native] Native call nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> NativeCallNode::clone() const {
        std::vector<std::unique_ptr<ExprNode>> clonedArgs;
        for (const auto &arg : arguments) {
            clonedArgs.push_back(arg->clone());
        }
        return std::make_unique<NativeCallNode>(functionName, std::move(clonedArgs));
    }

    // 数组节点
    ArrayNode::ArrayNode(std::vector<std::unique_ptr<ExprNode>> elems) : elements(std::move(elems)) {}

    std::string ArrayNode::string() const {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); i++) {
            if (i > 0)
                result += ", ";
            result += elements[i]->string();
        }
        return result + "]";
    }

    ValueData ArrayNode::evaluate(VM &vm) const {
        // 实现数组创建的求值逻辑
        ValueData arrayValue;
        arrayValue.type = ValueType::Array;
        std::vector<ValueData> arrayElements;

        for (const auto &elem : elements) {
            arrayElements.push_back(elem->evaluate(vm));
        }

        arrayValue.value = std::move(arrayElements);
        return arrayValue; // 返回创建的数组
    }

    ValueData &ArrayNode::evaluate_lvalue(VM &vm) const {
        // 数组节点通常不支持左值求值
        throw std::runtime_error("[squaker.array] Array nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> ArrayNode::clone() const {
        std::vector<std::unique_ptr<ExprNode>> clonedElements;
        for (const auto &elem : elements) {
            clonedElements.push_back(elem->clone());
        }
        return std::make_unique<ArrayNode>(std::move(clonedElements));
    }

    // 表节点
    TableNode::TableNode(std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> entries,
                         std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> members,
                         std::vector<std::unique_ptr<ExprNode>> elements)
        : entries(std::move(entries)), members(std::move(members)), elements(std::move(elements)) {}

    std::string TableNode::string() const {
        std::string result = "[";
        // 1.打印数组
        for (size_t i = 0; i < elements.size(); i++) {
            if (i > 0)
                result += ", ";
            result += elements[i]->string();
        }
        // 2.打印映射表
        if (!entries.empty()) {
            if (!elements.empty())
                result += ", ";
        }
        for (size_t i = 0; i < entries.size(); i++) {
            if (i > 0)
                result += ", ";
            result += entries[i].first->string() + " = " + entries[i].second->string();
        }
        // 3.打印成员表
        if (!members.empty()) {
            if (!elements.empty() || !entries.empty())
                result += ", ";
        }
        for (size_t i = 0; i < members.size(); i++) {
            if (i > 0)
                result += ", ";
            result += members[i].first->string() + " = " + members[i].second->string();
        }
        return result + "]";
    }

    ValueData TableNode::evaluate(VM &vm) const {
        // 实现表创建的求值逻辑
        TableData table;
        int index = 0;

        // 1.处理数组部分
        for (const auto &elem : elements) {
            ValueData value = elem->evaluate(vm);
            table.index(ValueData{ValueType::Integer, false, index}) = value;
            index++;
        }

        // 2.处理映射表部分
        for (const auto &entry : entries) {
            if (entry.first->type() != NodeType::Array) {
                throw std::runtime_error("[squaker.table] Member keys must be identifiers: " + entry.first->string());
            }
            ValueData keys = entry.first->evaluate(vm);
            ValueData value = entry.second->evaluate(vm);
            if (keys.type != ValueType::Array) {
                throw std::runtime_error("[squaker.table] Member keys must be arrays: " + keys.string());
            }
            for (const auto &key : std::get<std::vector<ValueData>>(keys.value)) {
                table.index(key) = value;
            }
        }

        // 3.处理成员表部分
        for (const auto &entry : members) {
            if (entry.first->type() != NodeType::Literal) {
                throw std::runtime_error("[squaker.table] Member keys must be literals: " + entry.first->string());
            }
            ValueData key = entry.first->evaluate(vm);
            if (key.type != ValueType::String) {
                throw std::runtime_error("[squaker.table] Member keys must be literals: " + key.string());
            }
            ValueData value = entry.second->evaluate(vm);
            table.dot(std::get<std::string>(key.value)) = value;
        }

        return ValueData{ValueType::Table, false, std::move(table)};
    }

    ValueData &TableNode::evaluate_lvalue(VM &vm) const {
        // 表节点通常不支持左值求值
        throw std::runtime_error("[squaker.table] Table nodes cannot be evaluated as lvalues");
    }

    std::unique_ptr<ExprNode> TableNode::clone() const {
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> clonedEntries;
        std::vector<std::pair<std::unique_ptr<ExprNode>, std::unique_ptr<ExprNode>>> clonedMembers;
        std::vector<std::unique_ptr<ExprNode>> clonedElements;
        for (const auto &entry : entries) {
            clonedEntries.emplace_back(entry.first->clone(), entry.second->clone());
        }
        for (const auto &entry : members) {
            clonedMembers.emplace_back(entry.first->clone(), entry.second->clone());
        }
        for (const auto &entry : elements) {
            clonedElements.emplace_back(entry->clone());
        }
        return std::make_unique<TableNode>(std::move(clonedEntries), std::move(clonedMembers),
                                           std::move(clonedElements));
    }

} // namespace squ