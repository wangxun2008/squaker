#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include "../include/scope.h"

namespace squ { 

    // 创建子作用域：返回独占指针
    std::unique_ptr<Scope> Scope::new_child() {
        return std::make_unique<Scope>(this);
    }

    // 查：找到返回 slot，否则 npos
    size_t Scope::find(const std::string& name) const {
        for (const Scope* s = this; s; s = s->parent_) {
            auto it = vars_.find(name);
            if (it != vars_.end()) return it->second;
        }
        return npos;
    }

    // 加：只在当前层插入，返回新 slot
    size_t Scope::add(const std::string& name) {
        size_t slot = vars_.size();   // 连续编号
        vars_.emplace(name, slot);
        return slot;
    }

} // namespace squ