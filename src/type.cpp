#include "../include/type.h"
#include <sstream>

namespace squ {
    // 实现ValueData的to_string成员函数
    std::string ValueData::to_string() const {

        switch (type) {
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
                result += arr[i].to_string();
            }
            return result + "]";
        }
        case ValueType::Map: {
            std::string result = "{";
            const auto &map =
                std::get<std::unordered_map<std::string, ValueData>>(value);
            bool first = true;
            for (const auto &[key, val] : map) {
                if (!first)
                    result += ", ";
                result += "\"" + key + "\": " + val.to_string();
                first = false;
            }
            return result + "}";
        }
        default:
            return "[complex_value]";
        }
    }

} // namespace squ