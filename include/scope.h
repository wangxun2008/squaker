#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace squ {

    class Scope {
      public:
        static constexpr size_t npos = static_cast<size_t>(-1);

        explicit Scope(Scope *p = nullptr) : parent_(p) {blockStack.emplace_back();}

        // 获取父作用域指针
        Scope *parent() const {
            return parent_;
        }

        // 创建一个新的子作用域
        std::unique_ptr<Scope> new_child();

        // 进入块级作用域
        void enter();

        // 离开块级作用域
        void leave();

        // 查：找到返回 slot，否则 npos
        size_t find(const std::string &name) const;

        // 加：只在当前层插入，返回新 slot
        size_t add(const std::string &name);

        // 获取当前作用域的变量数量
        size_t size() const;

      private:
        std::vector<std::string> vars_;  // 保留变量名，方便调试
        std::vector<std::unordered_map<std::string, size_t>> blockStack; // 栈管理每个块的变量
        Scope *parent_ = nullptr;
    };

    // RAII风格的作用域管理类
    class ScopeGuard {
        Scope* scope;
    public:
        explicit ScopeGuard(Scope* s) : scope(s) { s->enter(); }
        ~ScopeGuard() { scope->leave(); }
        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
    };

} // namespace squ