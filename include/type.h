#pragma once

#include <functional>
#include <string>
#include <map>
#include <variant>
#include <vector>

namespace squ {

    // 数据类型枚举
    enum class ValueType {
        Nil,      // 空值
        Integer,  // 整数
        Real,     // 实数
        Bool,     // 布尔
        Char,     // 字符
        String,   // 字符串
        Function, // 函数
        Array,    // 数组
        Map       // 表
    };

    // 值数据存储结构
    struct ValueData {
        ValueType type = ValueType::Nil; // 默认类型为Nil
        bool is_const = false;
        std::variant<long long,              // 整数
                     double,                 // 实数
                     bool,                   // 布尔
                     char,                   // 字符
                     std::string,            // 字符串
                     std::vector<ValueData>, // 数组
                     std::map<std::string, ValueData>,
                     std::function<ValueData(std::vector<ValueData>)>> // 函数
            value = 0.0;                                               // 映射（使用字符串作为键）

        // 成员函数声明
        std::string string() const;

        ~ValueData() = default;
    };

} // namespace squ