#pragma once
#include <string>
#include "type.h"
#include "vm.h"
#include "parser.h"

namespace squ {

    // 初始化控制台
    void InitConsole();

    // 打印 squaker 的 Logo
    void PrintLOGO();

    // 测试用例
    void RunTests();

    // 测试求值
    void RunEvalTests();

    // 独立的交互式代码执行函数
    void InteractiveExecution();

    // 封装的脚本类
    class Script {
      public:
        explicit Script(const std::string &code) : code(code) {}

        // 解析并执行脚本
        ValueData execute();

      private:
        std::string code; // 脚本代码
        VM vm; // 虚拟机实例
        Parser parser; // 解析器实例
    };

} // namespace squ