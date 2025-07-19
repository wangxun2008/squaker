#include "../include/type.h"
#include "../include/operator.h"
#include <cmath>
#include <stdexcept>

namespace squ {

    ValueData applyBinary(const ValueData &lhs,
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
            throw std::runtime_error("[squaker.eval.opt:'+'] unsupported types for operator +");
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
            throw std::runtime_error("[squaker.eval.opt:'-'] unsupported types for operator -");
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
            throw std::runtime_error("[squaker.eval.opt:'*'] unsupported types for operator *");
        }

        //--------------------------------------------------
        // 除法 /
        //--------------------------------------------------
        if (op == "/") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.eval.opt:'/'] division by zero");
                return ValueData{ValueType::Real, false, l / r};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.eval.opt:'/'] division by zero");
                return ValueData{ValueType::Real, false, static_cast<double>(l) / r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.eval.opt:'/'] division by zero");
                return ValueData{ValueType::Real, false, l / static_cast<double>(r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.eval.opt:'/'] division by zero");
                return ValueData{ValueType::Real, false, static_cast<double>(l) / r};
            }
            throw std::runtime_error("[squaker.eval.opt:'/'] unsupported types for operator /");
        }

        //--------------------------------------------------
        // 取模 %
        //--------------------------------------------------
        if (op == "%") {
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Real) {
                double l = std::get<double>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.eval.opt:'%'] modulo by zero");
                return ValueData{ValueType::Real, false, std::fmod(l, r)};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Integer) {
                long long l = std::get<long long>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.eval.opt:'%'] modulo by zero");
                return ValueData{ValueType::Integer, false, l % r};
            }
            if (lhs.type == ValueType::Real && rhs.type == ValueType::Integer) {
                double l = std::get<double>(lhs.value);
                long long r = std::get<long long>(rhs.value);
                if (r == 0)
                    throw std::runtime_error("[squaker.eval.opt:'%'] modulo by zero");
                return ValueData{ValueType::Real, false, std::fmod(l, static_cast<double>(r))};
            }
            if (lhs.type == ValueType::Integer && rhs.type == ValueType::Real) {
                long long l = std::get<long long>(lhs.value);
                double r = std::get<double>(rhs.value);
                if (r == 0.0)
                    throw std::runtime_error("[squaker.eval.opt:'%'] modulo by zero");
                return ValueData{ValueType::Real, false, std::fmod(static_cast<double>(l), r)};
            }
            throw std::runtime_error("[squaker.eval.opt:'%'] unsupported types for operator %");
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
        // 其他运算符 …
        //--------------------------------------------------
        throw std::runtime_error("[squaker.eval.opt] unknown binary operator: " + op);
    }

} // namespace squ