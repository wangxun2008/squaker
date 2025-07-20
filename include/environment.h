#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>
#include "type.h"

namespace squ { 

    // 作用域环境类：用于变量查找和存储
    class Environment {
      private:
        using Map = std::unordered_map<std::string, ValueData>;
        Map locals;
        Environment *parent = nullptr;

      public:
        explicit Environment(Environment *p = nullptr) : parent(p) {}

        // 返回父级作用域
        Environment *parent_environment() const;

        // 创建子作用域：返回独占指针
        std::unique_ptr<Environment> new_child();

        // 读取变量：链式查找
        ValueData &get(const std::string &name);

        // 写入变量：链式查找，查不到在本层新建
        ValueData &set(const std::string &name, ValueData v);

        // 检查是否存在变量
        bool exists(const std::string &name) const;
    };
    
} // namespace squ