#include "../include/vm.h"
#include <limits>
#include <stdexcept>

namespace squ {

    // 进入函数
    void VM::enter(size_t localsNeeded) {
        size_t base = mem.size();
        if (localsNeeded > std::numeric_limits<size_t>::max() - base) {
            throw std::runtime_error("[squaker.vm.enter] stack overflow");
        }
        mem.resize(base + localsNeeded);
        callStack.emplace_back(base, 0);
    }

    // 离开函数
    void VM::leave() {
        if (callStack.empty())
            throw std::runtime_error("[suqaker.vm.leave] leave without enter");
        size_t base = callStack.back().base;
        if (base > mem.size())
            throw std::runtime_error("[squaker.vm.leave] stack corruption");
        mem.resize(base);
        callStack.pop_back();
    }

    // 读写局部变量
    ValueData &VM::local(size_t slot) {
        if (callStack.empty())
            throw std::runtime_error("[squaker.vm.local] access local without frame");
        return mem[callStack.back().base + slot];
    }

} // namespace squ