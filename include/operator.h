#pragma once

#include "type.h"
#include <string>

namespace squ {

    // 应用二元操作
    // lhs: 左操作数, op: 操作符, rhs: 右操作数
    ValueData ApplyBinary(const ValueData &lhs,
                          const std::string &op,
                          const ValueData &rhs);

    // 应用一元操作
    // op: 操作符, operand: 操作数
    ValueData ApplyUnary(const std::string &op, const ValueData &operand);

    // 应用后缀操作
    // op: 操作符, operand: 操作数
    ValueData ApplyPostfix(const std::string &op, ValueData &operand);

} // namespace squ