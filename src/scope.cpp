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

    // 进入块级作用域
    void Scope::enter() {
        blockStack.emplace_back();
    }

    // 离开块级作用域
    void Scope::leave() {
        if (blockStack.empty())
            throw std::runtime_error("[squaker.scope.leave] leave without enter");
        blockStack.pop_back();
    }

    // 查：找到返回 slot，否则 npos
    size_t Scope::find(const std::string& name) const {
        for (auto it = blockStack.rbegin(); it != blockStack.rend(); ++it) {
            auto& block = *it;
            auto it_var = block.find(name);
            if (it_var != block.end()) return it_var->second;
        }
        return npos;
    }

    // 加：只在当前层插入，返回新 slot
    size_t Scope::add(const std::string& name) {
        size_t slot = vars_.size();
        blockStack.back()[name] = slot;
        vars_.push_back(name);  // 保留变量名，方便调试
        return slot;
    }

    // 获取当前作用域的变量数量
    size_t Scope::size() const {
        return vars_.size();
    }

} // namespace squ