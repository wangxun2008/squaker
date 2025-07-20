#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>
#include "type.h"
#include "Environment.h"

namespace squ { 

    // 返回父级作用域
    Environment* Environment::parent_environment() const {
        return parent;
    }

    // 创建子作用域：返回独占指针
    std::unique_ptr<Environment> Environment::new_child() {
        return std::make_unique<Environment>(this);
    }

    // 读取变量：链式查找
    ValueData& Environment::get(const std::string &name) {
        auto it = locals.find(name);
        if (it != locals.end())
            return it->second;
        if (parent)
            return parent->get(name);
        throw std::runtime_error("undefined: " + name);
    }

    // 写入变量：链式查找，查不到在本层新建
    ValueData& Environment::set(const std::string &name, ValueData v) {
        Environment *curr = this;
        while (curr) {
            auto it = curr->locals.find(name);
            if (it != curr->locals.end()) { // 找到就覆盖
                it->second = v;
                return it->second;
            }
            curr = curr->parent; // 向上继续找
        }
        // 整条链都没找到 → 在本层（最顶层调用者）新建
        locals[name] = v;
        return locals[name];
    }

    // 检查是否存在变量
    bool Environment::exists(const std::string &name) const {
        if (locals.find(name) != locals.end())
            return true;
        if (parent)
            return parent->exists(name);
        return false;
    }

} // namespace squ