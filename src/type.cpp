#include "../include/type.h"
#include <sstream>

namespace squ {

    // 重载小于运算符，用于比较两个TableData对象的地址
    inline bool operator<(const TableData &a, const TableData &b) noexcept {
        // 地址序即可，保证稳定
        return &a < &b;
    }

    // 实现TableData的index成员函数
    ValueData &TableData::index(const ValueData &index) {
        return array_map[index];
    }

    // 实现TableData的index_at成员函数
    ValueData &TableData::index_at(const ValueData &index) {
        if (index.type != ValueType::String && index.type != ValueType::Integer) {
            throw std::runtime_error("[squaker.table] Index must be a string or integer");
        }
        if (array_map.find(index) == array_map.end()) {
            throw std::runtime_error("[squaker.table] Index out of range");
        }
        return array_map[index];
    }

    // 实现TableData的dot成员函数
    ValueData &TableData::dot(const std::string &name) {
        return dot_map[name];
    }

    // 实现TableData的dot_at成员函数
    ValueData &TableData::dot_at(const std::string &name) {
        auto it = dot_map.find(name);
        if (it == dot_map.end()) {
            throw std::runtime_error("[squaker.table] Key not found in dot map: " + name);
        }
        return it->second;
    }

    // 实现TableData的length成员函数
    size_t TableData::length() const {
        return array_map.size() + dot_map.size();
    }

    // 实现ValueData的string成员函数
    std::string ValueData::string() const {

        switch (type) {
        case ValueType::Nil:
            return "nil";
        case ValueType::Integer:
            return std::to_string(std::get<long long>(value));
        case ValueType::Real:
            return std::to_string(std::get<double>(value));
        case ValueType::Bool:
            return std::get<bool>(value) ? "true" : "false";
        case ValueType::Char:
            return "'" + std::string(1, std::get<char>(value)) + "'";
        case ValueType::String:
            return "\"" + std::get<std::string>(value) + "\"";
        case ValueType::Array: {
            std::string result = "[";
            const auto &arr = std::get<std::vector<ValueData>>(value);
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0)
                    result += ", ";
                result += arr[i].string();
            }
            return result + "]";
        }
        case ValueType::Table: {
            std::string result = "[";
            const auto &table = std::get<TableData>(value);
            for (const auto &pair : table.array_map) {
                if (result.size() > 1)
                    result += ", ";
                result += pair.first.string() + "=" + pair.second.string();
            }
            for (const auto &pair : table.dot_map) {
                if (result.size() > 1)
                    result += ", ";
                result += pair.first + ": " + pair.second.string();
            }
            return result + "]";
        }
        case ValueType::Function: {
            return "[function]";
        }
        default:
            return "[complex_value]";
        }
    }

    // 比较两个ValueData对象
    bool operator<(const squ::ValueData &a, const squ::ValueData &b) noexcept {
        if (a.type != b.type)
            return a.type < b.type;

        using Func = std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM &)>;
        return std::visit(
            [](const auto &x, const auto &y) -> bool {
                using T = std::decay_t<decltype(x)>;
                using U = std::decay_t<decltype(y)>;

                if constexpr (std::is_same_v<T, Func> && std::is_same_v<U, Func>) {
                    return &x < &y; // 函数：地址序
                } else if constexpr (std::is_same_v<T, U>) {
                    return x < y; // 其余基本类型直接比较
                } else {
                    return typeid(x).before(typeid(y)); // 不同类型按 type_info 顺序
                }
            },
            a.value, b.value);
    }

} // namespace squ

template <> struct std::less<std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM &)>> {
    bool operator()(const std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM &)> &a,
                    const std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM &)> &b) const noexcept {
        return &a < &b;
    }
};