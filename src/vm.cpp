#include "../include/vm.h"

namespace squ {

    // 进入函数
    void VM::enter(size_t localsNeeded) {
        callStack.emplace_back(mem.size(), 0);
        mem.resize(mem.size() + localsNeeded);
    }

    // 离开函数
    void VM::leave() {
        callStack.pop_back();
    }

    // 读写局部变量
    ValueData& VM::local(size_t slot) {
        return mem[callStack.back().base + slot];
    }

}