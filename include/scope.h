#pragma once
#include <unordered_map>
#include <string>
#include <memory>

namespace squ {

class Scope {
public:
    static constexpr size_t npos = static_cast<size_t>(-1);

    explicit Scope(Scope* p = nullptr) : parent_(p) {}

    Scope* parent() const { return parent_; }

    std::unique_ptr<Scope> new_child();

    // 查：找到返回 slot，否则 npos
    size_t find(const std::string& name) const;

    // 加：只在当前层插入，返回新 slot
    size_t add(const std::string& name);

private:
    std::unordered_map<std::string, size_t> vars_;
    Scope* parent_ = nullptr;
};

} // namespace squ