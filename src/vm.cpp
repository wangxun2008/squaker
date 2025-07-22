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
        printf("[squaker.vm.local] access local slot %zu\n", slot);
        if (callStack.empty())
            throw std::runtime_error("[squaker.vm.local] access local without frame");
        if (slot >= mem.size())
            throw std::runtime_error("[squaker.vm.local] local slot out of range: " + std::to_string(slot));
        // 返回局部变量的引用
        return mem[callStack.back().base + slot];
    }

    // 打印当前调用栈
    void VM::printStack() const {
        printf("[squaker.vm.stack] Current call stack:\n");
        for (const auto &frame : callStack) {
            printf("  Frame(base=%zu, retAddr=%zu)\n", frame.base, frame.retAddr);
        }
        printf("[squaker.vm.stack] Total frames: %zu\n", callStack.size());
        printf("[squaker.vm.stack] Memory size: %zu\n", mem.size());
        for (size_t i = 0; i < mem.size(); ++i) {
            printf("  mem[%zu] = %s\n", i, mem[i].string().c_str());
        }
    }

} // namespace squ