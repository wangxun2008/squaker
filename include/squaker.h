#pragma once
#include "parser.h"
#include "type.h"
#include "vm.h"
#include "identifier.h"
#include <string>

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

    // 测试脚本执行
    void RunScriptTests();

    // 封装的脚本类
    class Script {
      public:
        // 构造函数
        explicit Script();

        // 添加代码
        void append(const std::string &append_code);

        // 注册标识符
        void register_identifier(const IdentifierData &identifier);

        // 解析并执行脚本
        ValueData execute();

      private:
        std::vector<std::string> code; // 代码
        size_t current_index = 0;      // 当前代码索引
        VM vm;                         // 虚拟机实例
        Parser parser;                 // 解析器实例
    };

} // namespace squ