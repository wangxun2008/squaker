#pragma once

#include "type.h"
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <stdexcept>

namespace squ::internal {

    // 类型转换工具
    template <typename T> struct TypeConverter;
    template <> struct TypeConverter<long long> {
        static constexpr ValueType type = ValueType::Integer;
        static long long convert(const ValueData &v) {
            if (v.type != ValueType::Integer)
                throw std::runtime_error("[squaker.wrapper] Expected integer type");
            return std::get<long long>(v.value);
        }
    };
    template <> struct TypeConverter<int> {
        static constexpr ValueType type = ValueType::Integer;
        static long long convert(const ValueData &v) {
            return TypeConverter<long long>::convert(v);
        }
    };
    template <> struct TypeConverter<double> {
        static constexpr ValueType type = ValueType::Real;
        static double convert(const ValueData &v) {
            if (v.type == ValueType::Integer)
                return static_cast<double>(std::get<long long>(v.value));
            if (v.type != ValueType::Real)
                throw std::runtime_error("[squaker.wrapper] Expected real or integer type");
            return std::get<double>(v.value);
        }
    };
    template <> struct TypeConverter<float> {
        static constexpr ValueType type = ValueType::Real;
        static double convert(const ValueData &v) {
            return TypeConverter<double>::convert(v);
        }
    };
    template <> struct TypeConverter<bool> {
        static constexpr ValueType type = ValueType::Bool;
        static bool convert(const ValueData &v) {
            if (v.type != ValueType::Bool)
                throw std::runtime_error("[squaker.wrapper] Expected boolean type");
            return std::get<bool>(v.value);
        }
    };
    template <> struct TypeConverter<char> {
        static constexpr ValueType type = ValueType::Char;
        static char convert(const ValueData &v) {
            if (v.type != ValueType::Char)
                throw std::runtime_error("[squaker.wrapper] Expected char type");
            return std::get<char>(v.value);
        }
    };
    template <> struct TypeConverter<std::string> {
        static constexpr ValueType type = ValueType::String;
        static std::string convert(const ValueData &v) {
            if (v.type != ValueType::String)
                throw std::runtime_error("[squaker.wrapper] Expected string type");
            return std::get<std::string>(v.value);
        }
    };
    template <> struct TypeConverter<const std::string &> {
        static constexpr ValueType type = ValueType::String;
        static std::string convert(const ValueData &v) {
            return TypeConverter<std::string>::convert(v);
        }
    };

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
                return internal::make_value(result);
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

} // namespace squ
