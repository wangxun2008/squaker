#pragma once
#include "type.h"
#include <cstddef>
#include <vector>

namespace squ {

    // 帧结构体，包含函数调用的相关信息
    struct Frame {
        size_t base;    // 该帧在 mem 里的起始下标
        size_t retAddr; // 字节码返回地址（先留空）
        Frame() = default;
        Frame(size_t b, size_t r) : base(b), retAddr(r) {}
    };

    // 虚拟机类，管理内存和调用栈
    class VM {
      public:
        std::vector<ValueData> mem;   // 一条胖数组
        std::vector<Frame> callStack; // 调用栈

        // 进入函数
        void enter(size_t localsNeeded);

        // 离开函数
        void leave();

        // 读写局部变量
        ValueData &local(size_t slot);

        // 打印当前调用栈
        void printStack() const;
    };

    // RAII风格的虚拟机保护类，用于自动管理函数调用的进入和离开
    class VMGuard {
        VM& vm;
    public:
        explicit VMGuard(VM& v, size_t locals) : vm(v) { vm.enter(locals); }
        ~VMGuard() { vm.leave(); }
        VMGuard(const VMGuard&) = delete;
        VMGuard& operator=(const VMGuard&) = delete;
    };

} // namespace squ