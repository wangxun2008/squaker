#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
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
        Table     // 表
    };

    // 前向声明
    class VM;
    struct ValueData;

    // 表数据存储结构
    struct TableData {
        using ArrayMap = std::map<ValueData, ValueData>;
        ArrayMap array_map;

        using DotMap = std::map<std::string, ValueData>;
        DotMap dot_map;

        ValueData &index_at(const ValueData &index);

        ValueData &index(const ValueData &index);

        ValueData &dot_at(const std::string &name);

        ValueData &dot(const std::string &name);

        size_t length() const;
    };

    // 值数据存储结构
    struct ValueData {
        ValueType type = ValueType::Nil; // 默认类型为Nil
        bool is_const = false;
        std::variant<long long,                                              // 整数
                     double,                                                 // 实数
                     bool,                                                   // 布尔
                     char,                                                   // 字符
                     std::string,                                            // 字符串
                     std::vector<ValueData>,                                 // 数组
                     TableData,                                              // 表
                     std::function<ValueData(std::vector<ValueData>&, VM &)>> // 函数
            value = 0.0;                                                     // 映射（使用字符串作为键）

        // 成员函数声明
        std::string string() const;

        ~ValueData() = default;
    };

} // namespace squ