#pragma once
#include <vector>
#include <cstddef>
#include "type.h"

namespace squ {

    // 帧结构体，包含函数调用的相关信息
    struct Frame {
        size_t base;        // 该帧在 mem 里的起始下标
        size_t retAddr;     // 字节码返回地址（先留空）
    };

    // 虚拟机类，管理内存和调用栈
    class VM {
    public:
        std::vector<ValueData> mem;   // 一条胖数组
        std::vector<Frame>     callStack; // 调用栈

        // 进入函数
        void enter(size_t localsNeeded);

        // 离开函数
        void leave();

        // 读写局部变量
        ValueData& local(size_t slot);
    };

}