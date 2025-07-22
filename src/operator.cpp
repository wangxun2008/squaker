#include "../include/type.h"
#include "../include/operator.h"
#include <cmath>
#include <stdexcept>

namespace squ {

    // 应用二元操作
    ValueData ApplyBinary(const ValueData &lhs,
                                 const std::string &op,
                                 const ValueData &rhs) {

        //--------------------------------------------------
        // 加法 +
        //--------------------------------------------------
        if (op == "+") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                return ValueData{ValueType::Real, false, l + r};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l + r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Real, false, l + static_cast<double>(r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                return ValueData{ValueType::Real, false, static_cast<double>(l) + r};
            }
            /* 预留：String、Array、Char … */
            throw std::runtime_error("[squaker.operator:'+'] unsupported types for operator +");
        }

        //--------------------------------------------------
        // 减法 -
        //--------------------------------------------------
        if (op == "-") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                return ValueData{ValueType::Real, false, l - r};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l - r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Real, false, l - static_cast<double>(r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                return ValueData{ValueType::Real, false, static_cast<double>(l) - r};
            }
            throw std::runtime_error("[squaker.operator:'-'] unsupported types for operator -");
        }

        //--------------------------------------------------
        // 乘法 *
        //--------------------------------------------------
        if (op == "*") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                return ValueData{ValueType::Real, false, l * r};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l * r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Real, false, l * static_cast<double>(r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                return ValueData{ValueType::Real, false, static_cast<double>(l) * r};
            }
            throw std::runtime_error("[squaker.operator:'*'] unsupported types for operator *");
        }

        //--------------------------------------------------
        // 除法 /
        //--------------------------------------------------
        if (op == "/") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.operator:'/'] division by zero");
                return ValueData{ValueType::Real, false, l / r};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.operator:'/'] division by zero");
                return ValueData{ValueType::Real, false, static_cast<double>(l) / r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.operator:'/'] division by zero");
                return ValueData{ValueType::Real, false, l / static_cast<double>(r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.operator:'/'] division by zero");
                return ValueData{ValueType::Real, false, static_cast<double>(l) / r};
            }
            throw std::runtime_error("[squaker.operator:'/'] unsupported types for operator /");
        }

        //--------------------------------------------------
        // 取模 %
        //--------------------------------------------------
        if (op == "%") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.operator:'%'] modulo by zero");
                return ValueData{ValueType::Real, false, std::fmod(l, r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.operator:'%'] modulo by zero");
                return ValueData{ValueType::Integer, false, l % r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.operator:'%'] modulo by zero");
                return ValueData{ValueType::Real, false, std::fmod(l, static_cast<double>(r))};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.operator:'%'] modulo by zero");
                return ValueData{ValueType::Real, false, std::fmod(static_cast<double>(l), r)};
            }
            throw std::runtime_error("[squaker.operator:'%'] unsupported types for operator %");
        }

        
        //--------------------------------------------------
        // 字符串拼接..
        //--------------------------------------------------
        if (op == "..") {
            std::string l, r;
            if (lhs.type == ValueType::String) l = std::get<std::string>(lhs.value);
            else if (lhs.type == ValueType::Char) l = std::to_string(std::get<char>(lhs.value));
            else l = lhs.string();
            if (rhs.type == ValueType::String) r = std::get<std::string>(rhs.value);
            else if (rhs.type == ValueType::Char) r = std::to_string(std::get<char>(rhs.value));
            else r = rhs.string();
            return ValueData{ValueType::String, false, l + r};
        }

        //--------------------------------------------------
        // 比较操作符
        //--------------------------------------------------
        if (op == "==") {
            if (lhs.type == rhs.type) {
                if (lhs.type == ValueType::Real) {
                    return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) == std::get<double>(rhs.value)};
                }
                if (lhs.type == ValueType::Integer) {
                    return ValueData{ValueType::Bool, false, std::get<long long>(lhs.value) == std::get<long long>(rhs.value)};
                }
                if (lhs.type == ValueType::String) {
                    return ValueData{ValueType::Bool, false, std::get<std::string>(lhs.value) == std::get<std::string>(rhs.value)};
                }
                if (lhs.type == ValueType::Char) {
                    return ValueData{ValueType::Bool, false, std::get<char>(lhs.value) == std::get<char>(rhs.value)};
                }
                // 其他类型的比较
                return ValueData{ValueType::Bool, false, lhs.string() == rhs.string()};
            }
            return ValueData{ValueType::Bool, false, false}; // 不同类型直接返回false
        }

        if (op == "!=") {
            if (lhs.type == rhs.type) {
                if (lhs.type == ValueType::Real) {
                    return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) != std::get<double>(rhs.value)};
                }
                if (lhs.type == ValueType::Integer) {
                    return ValueData{ValueType::Bool, false, std::get<long long>(lhs.value) != std::get<long long>(rhs.value)};
                }
                if (lhs.type == ValueType::String) {
                    return ValueData{ValueType::Bool, false, std::get<std::string>(lhs.value) != std::get<std::string>(rhs.value)};
                }
                if (lhs.type == ValueType::Char) {
                    return ValueData{ValueType::Bool, false, std::get<char>(lhs.value) != std::get<char>(rhs.value)};
                }
                // 其他类型的比较
                return ValueData{ValueType::Bool, false, lhs.string() != rhs.string()};
            }
            return ValueData{ValueType::Bool, false, true}; // 不同类型直接返回true
        }

        if (op == "<") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) < std::get<double>(rhs.value)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<long long>(lhs.value) < std::get<long long>(rhs.value)};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) < static_cast<double>(std::get<long long>(rhs.value))};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, static_cast<double>(std::get<long long>(lhs.value)) < std::get<double>(rhs.value)};
            }
            throw std::runtime_error("[squaker.operator:'<'] unsupported types for operator <");
        }

        if (op == "<=") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) <= std::get<double>(rhs.value)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<long long>(lhs.value) <= std::get<long long>(rhs.value)};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) <= static_cast<double>(std::get<long long>(rhs.value))};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, static_cast<double>(std::get<long long>(lhs.value)) <= std::get<double>(rhs.value)};
            }
            throw std::runtime_error("[squaker.operator:'<='] unsupported types for operator <=");
        }

        if (op == ">") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) > std::get<double>(rhs.value)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<long long>(lhs.value) > std::get<long long>(rhs.value)};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) > static_cast<double>(std::get<long long>(rhs.value))};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, static_cast<double>(std::get<long long>(lhs.value)) > std::get<double>(rhs.value)};
            }
            throw std::runtime_error("[squaker.operator:'>'] unsupported types for operator >");
        }

        if (op == ">=") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) >= std::get<double>(rhs.value)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<long long>(lhs.value) >= std::get<long long>(rhs.value)};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                return ValueData{ValueType::Bool, false, std::get<double>(lhs.value) >= static_cast<double>(std::get<long long>(rhs.value))};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                return ValueData{ValueType::Bool, false, static_cast<double>(std::get<long long>(lhs.value)) >= std::get<double>(rhs.value)};
            }
            throw std::runtime_error("[squaker.operator:'>='] unsupported types for operator >=");
        }

        //--------------------------------------------------
        // 位操作符
        //--------------------------------------------------
        if (op == "&") {
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l & r};
            }
            throw std::runtime_error("[squaker.operator:'&'] unsupported types for bitwise AND");
        }

        if (op == "|") {
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l | r};
            }
            throw std::runtime_error("[squaker.operator:'|'] unsupported types for bitwise OR");
        }

        if (op == "^") {
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l ^ r};
            }
            throw std::runtime_error("[squaker.operator:'^'] unsupported types for bitwise XOR");
        }

        if (op == "<<") {
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l << r};
            }
            throw std::runtime_error("[squaker.operator:'<<'] unsupported types for left shift");
        }

        if (op == ">>") {
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                return ValueData{ValueType::Integer, false, l >> r};
            }
            throw std::runtime_error("[squaker.operator:'>>'] unsupported types for right shift");
        }

        //--------------------------------------------------
        // 其他运算符 …
        //--------------------------------------------------
        throw std::runtime_error("[squaker.operator] unknown binary operator: " + op);
    }

    // 应用一元操作
    ValueData ApplyUnary(const std::string &op, const ValueData &operand) {
        //--------------------------------------------------
        // 一元加 +
        //--------------------------------------------------
        if (op == "+") {
            if (operand.type == ValueType::Real) {
                double val = std::get<double>(operand.value);
                return ValueData{ValueType::Real, false, +val};
            }
            if (operand.type == ValueType::Integer) {
                long long val = std::get<long long>(operand.value);
                return ValueData{ValueType::Integer, false, +val};
            }
            throw std::runtime_error("[squaker.operator:'+'] unsupported type for unary +");
        }

        //--------------------------------------------------
        // 一元减 -
        //--------------------------------------------------
        if (op == "-") {
            if (operand.type == ValueType::Real) {
                double val = std::get<double>(operand.value);
                return ValueData{ValueType::Real, false, -val};
            }
            if (operand.type == ValueType::Integer) {
                long long val = std::get<long long>(operand.value);
                return ValueData{ValueType::Integer, false, -val};
            }
            throw std::runtime_error("[squaker.operator:'-'] unsupported type for unary -");
        }

        //--------------------------------------------------
        // 逻辑非 !
        //--------------------------------------------------
        if (op == "!") {
            if (operand.type == ValueType::Bool) {
                bool val = std::get<bool>(operand.value);
                return ValueData{ValueType::Bool, false, !val};
            }
            throw std::runtime_error("[squaker.operator:'!'] unsupported type for logical NOT");
        }

        //--------------------------------------------------
        // 其他一元操作 …
        //--------------------------------------------------
        throw std::runtime_error("[squaker.operator] unknown unary operator: " + op);
    }

} // namespace squ