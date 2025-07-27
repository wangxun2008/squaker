#pragma once

#include "type.h"
#include <algorithm> // 添加 algorithm 头文件
#include <cmath>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace squ::internal {

    // 类型转换工具
    template <typename T, typename = void> struct TypeConverter;

    // 整型全家桶
    template <typename T>
    struct TypeConverter<
        T, std::enable_if_t<std::is_integral_v<std::decay_t<T>> && !std::is_same_v<std::decay_t<T>, bool>>> {
        using Raw = std::decay_t<T>;
        static constexpr ValueType type = ValueType::Integer;
        static long long convert(const ValueData &v) {
            return internal::TypeConverter<long long>::convert(v);
        }
        static ValueData convert_to_value(Raw v) {
            return internal::TypeConverter<long long>::convert_to_value(static_cast<long long>(v));
        }
    };

    // 浮点全家桶
    template <typename T>
    struct TypeConverter<T, std::enable_if_t<std::is_floating_point_v<std::decay_t<T>>>> {
        using Raw = std::decay_t<T>;
        static constexpr ValueType type = ValueType::Real;
        static double convert(const ValueData& v) {
            return TypeConverter<double>::convert(v);
        }
        static ValueData convert_to_value(Raw v) {
            return TypeConverter<double>::convert_to_value(static_cast<double>(v));
        }
    };
    
    template <> struct TypeConverter<bool> {
        static constexpr ValueType type = ValueType::Bool;
        static bool convert(const ValueData &v) {
            if (v.type != ValueType::Bool)
                throw std::runtime_error("[squaker.wrapper] Expected boolean type");
            return std::get<bool>(v.value);
        }
        static ValueData convert_to_value(bool value) {
            return ValueData{ValueType::Bool, false, value};
        }
    };
    template <> struct TypeConverter<char> {
        static constexpr ValueType type = ValueType::Char;
        static char convert(const ValueData &v) {
            if (v.type != ValueType::Char)
                throw std::runtime_error("[squaker.wrapper] Expected char type");
            return std::get<char>(v.value);
        }
        static ValueData convert_to_value(char value) {
            return ValueData{ValueType::Char, false, value};
        }
    };
    template <> struct TypeConverter<std::string> {
        static constexpr ValueType type = ValueType::String;
        static std::string convert(const ValueData &v) {
            if (v.type != ValueType::String)
                throw std::runtime_error("[squaker.wrapper] Expected string type");
            return std::get<std::string>(v.value);
        }
        static ValueData convert_to_value(const std::string &value) {
            return ValueData{ValueType::String, false, value};
        }
    };
    template <> struct TypeConverter<const std::string &> {
        static constexpr ValueType type = ValueType::String;
        static std::string convert(const ValueData &v) {
            return TypeConverter<std::string>::convert(v);
        }
    };

    // 为 std::vector<T> 添加特化
    template <typename T> struct TypeConverter<std::vector<T>> {
        static constexpr ValueType type = ValueType::Table;

        static std::vector<T> convert(const ValueData &v) {
            if (v.type != ValueType::Table) {
                throw std::runtime_error("Expected table type");
            }

            const TableData &table = std::get<TableData>(v.value);
            std::vector<T> result;
            result.reserve(table.array_map.size());

            // 按索引顺序提取值
            for (const auto &[key, value] : table.array_map) {
                if (key.type != ValueType::Integer) {
                    throw std::runtime_error("Expected integer index in table array");
                }
                result.push_back(TypeConverter<T>::convert(value));
            }

            return result;
        }

        static ValueData convert_to_value(const std::vector<T> &vec) {
            TableData table;
            long long index = 0;

            for (const auto &item : vec) {
                table.index(ValueData{ValueType::Integer, false, index++}) = TypeConverter<T>::convert_to_value(item);
            }

            return ValueData{ValueType::Table, false, table};
        }
    };

    template <> struct TypeConverter<std::vector<ValueData>> {
        static constexpr ValueType type = ValueType::Array;
        static std::vector<ValueData> convert(const ValueData &v) {
            if (v.type != ValueType::Array)
                throw std::runtime_error("[squaker.wrapper] Expected array type");
            return std::get<std::vector<ValueData>>(v.value);
        }
        static ValueData convert_to_value(const std::vector<ValueData> &value) {
            return ValueData{ValueType::Array, false, value};
        }
    };

    template <> struct TypeConverter<TableData> {
        static constexpr ValueType type = ValueType::Table;
        static TableData convert(const ValueData &v) {
            if (v.type != ValueType::Table)
                throw std::runtime_error("[squaker.wrapper] Expected table type");
            return std::get<TableData>(v.value);
        }
        static ValueData convert_to_value(const TableData &value) {
            return ValueData{ValueType::Table, false, value};
        }
    };

    // 通用转换到 ValueData
    template <typename T> ValueData convert_to_value(T &&value) {
        using RawType = std::decay_t<T>;
        return TypeConverter<RawType>::convert_to_value(std::forward<T>(value));
    }

    // 返回值构造工具
    template <typename T> ValueData make_value(T &&value) {
        using RawType = std::decay_t<T>;
        return ValueData{TypeConverter<RawType>::type, false, std::forward<T>(value)};
    }

    // 函数特征提取
    template <typename T> struct function_traits;
    template <typename Ret, typename... Args> struct function_traits<Ret(Args...)> {
        using result_type = Ret;
        using args_tuple = std::tuple<Args...>;
        static constexpr size_t arity = sizeof...(Args);
        template <size_t I> using arg_type = typename std::tuple_element<I, args_tuple>::type;
    };
    template <typename Ret, typename... Args>
    struct function_traits<Ret (*)(Args...)> : function_traits<Ret(Args...)> {};
    template <typename C, typename Ret, typename... Args>
    struct function_traits<Ret (C::*)(Args...)> : function_traits<Ret(Args...)> {};
    template <typename C, typename Ret, typename... Args>
    struct function_traits<Ret (C::*)(Args...) const> : function_traits<Ret(Args...)> {};
    template <typename F> struct function_traits {
      private:
        using call_type = function_traits<decltype(&F::operator())>;

      public:
        using result_type = typename call_type::result_type;
        using args_tuple = typename call_type::args_tuple;
        static constexpr size_t arity = call_type::arity;
        template <size_t I> using arg_type = typename call_type::template arg_type<I>;
    };
    template <typename F> struct function_traits<F &> : function_traits<F> {};
    template <typename F> struct function_traits<F &&> : function_traits<F> {};

    // 统一的函数包装器
    template <typename Func> struct FunctionWrapper {
        static ValueData wrap(Func &&func) {
            using traits = function_traits<std::decay_t<Func>>;
            return ValueData{
                ValueType::Function, false,
                [func = std::forward<Func>(func)](std::vector<ValueData> &args, VM &) mutable -> ValueData {
                    if (args.size() != traits::arity) {
                        throw std::runtime_error("[squaker.wrapper] Incorrect number of arguments. Expected: " +
                                                 std::to_string(traits::arity));
                    }
                    return call_impl(func, args, std::make_index_sequence<traits::arity>{});
                }};
        }

      private:
        template <typename F, size_t... Is>
        static ValueData call_impl(F &func, std::vector<ValueData> &args, std::index_sequence<Is...>) {
            using traits = function_traits<std::decay_t<F>>;
            if constexpr (std::is_same_v<typename traits::result_type, void>) {
                func(TypeConverter<typename traits::template arg_type<Is>>::convert(args[Is])...);
                return ValueData{ValueType::Nil, false};
            } else {
                auto result = func(TypeConverter<typename traits::template arg_type<Is>>::convert(args[Is])...);
                return convert_to_value(result);
            }
        }
    };

} // namespace squ::internal

namespace squ {

    // 统一标识符
    struct IdentifierData {
        std::string name;
        ValueData value;
    };

    // 包装任何可调用对象
    template <typename Func> auto make_function(Func &&func) {
        return internal::FunctionWrapper<Func>::wrap(std::forward<Func>(func));
    }

    // 工具：把任意对象变成 ValueData
    template <typename T> ValueData to_value(T &&v) {
        return internal::make_value(std::forward<T>(v));
    }

    // 工厂函数：返回 IdentifierData
    template <typename F> inline IdentifierData Function(std::string_view name, F &&f) {
        return {std::string(name), make_function(std::forward<F>(f))};
    }

    template <typename T> inline IdentifierData Variable(std::string_view name, T &&v) {
        return {std::string(name), to_value(std::forward<T>(v))};
    }

    template <typename T> inline IdentifierData Constant(std::string_view name, T &&v) {
        auto val = to_value(std::forward<T>(v));
        val.is_const = true;
        return {std::string(name), std::move(val)};
    }

    template <typename... Items> inline IdentifierData Namespace(std::string_view name, Items &&...items) {
        TableData tbl;
        (void(tbl.dot_map.emplace(items.name, items.value)), ...);
        return {std::string(name), ValueData{ValueType::Table, false, tbl}};
    }

    // 用户接口函数
    template <typename Func> auto wrap_function(Func &&func) {
        return internal::FunctionWrapper<Func>::wrap(std::forward<Func>(func));
    }

} // namespace squ