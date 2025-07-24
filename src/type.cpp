#include "../include/type.h"
#include <sstream>

namespace squ {

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
        case ValueType::Map: {
            std::string result = "{";
            const auto &map = std::get<std::map<std::string, ValueData>>(value);
            bool first = true;
            for (const auto &[key, val] : map) {
                if (!first)
                    result += ", ";
                result += "\"" + key + "\": " + val.string();
                first = false;
            }
            return result + "}";
        }
        default:
            return "[complex_value]";
        }
    }

	bool operator<(const squ::ValueData& a, const squ::ValueData& b) noexcept {
		if (a.type != b.type) return a.type < b.type;
		
		using Func = std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM&)>;
		return std::visit([](const auto& x, const auto& y) -> bool {
			using T = std::decay_t<decltype(x)>;
			using U = std::decay_t<decltype(y)>;
			
			if constexpr (std::is_same_v<T, Func> && std::is_same_v<U, Func>) {
				return &x < &y;                     // 函数：地址序
			} else if constexpr (std::is_same_v<T, U>) {
				return x < y;                       // 其余基本类型直接比较
			} else {
				return typeid(x).before(typeid(y)); // 不同类型按 type_info 顺序
			}
		}, a.value, b.value);
	}

} // namespace squ

template<>
struct std::less<std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM&)>> {
	bool operator()(const std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM&)>& a,
		const std::function<squ::ValueData(std::vector<squ::ValueData>, squ::VM&)>& b) const noexcept {
			return &a < &b;
		}
};