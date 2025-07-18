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
            throw std::runtime_error("[squaker.eval.opt:'%'] unsupported types for operator %");
        }

        //--------------------------------------------------
        // 其他运算符 …
        //--------------------------------------------------
        throw std::runtime_error("[squaker.eval.opt] unknown binary operator: " + op);
    }

} // namespace squ