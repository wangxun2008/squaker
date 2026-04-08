# Squaker 语言文档

## 概述

本目录包含 Squaker 脚本语言的完整文档。Squaker 是一个使用 C++ 实现的轻量级脚本语言，支持动态类型、函数式编程、面向对象特性和模块化编程。

## 文档结构

### 核心文档

1. **[语言指南](language_guide.md)** - 完整的语言参考，涵盖所有语法和特性
   - 基本语法和数据类型
   - 变量、运算符和控制流
   - 函数、数组和表
   - 模块系统和错误处理
   - 内置函数和示例代码

2. **[快速入门](quick_start.md)** - 快速上手指南
   - 安装和构建步骤
   - 第一个 Squaker 程序
   - 基本语法示例
   - 实用示例和调试技巧

3. **[API 参考](api_reference.md)** - 详细的 API 文档
   - C++ API 和内置函数
   - 模块 API 和数据类型
   - 错误代码和扩展 API
   - 调试和性能优化

### 补充资源

- **示例代码**：查看 `../examples/` 目录中的示例
- **测试用例**：查看 `../test/` 目录中的测试代码
- **源代码**：查看 `../include/` 和 `../src/` 目录了解实现细节

## 快速链接

### 语言特性

- [基本语法](language_guide.md#基本语法)
- [数据类型](language_guide.md#数据类型)
- [控制流](language_guide.md#控制流)
- [函数](language_guide.md#函数)
- [模块系统](language_guide.md#模块系统)

### 入门指南

- [安装和构建](quick_start.md#安装和构建)
- [第一个程序](quick_start.md#第一个-squaker-程序)
- [常见问题](quick_start.md#常见问题)

### API 参考

- [Script 类](api_reference.md#script-类)
- [内置函数](api_reference.md#内置函数)
- [math 模块](api_reference.md#math-模块)
- [错误处理](api_reference.md#错误代码)

## 语言特性概览

### 数据类型
- 基本类型：Nil, Integer, Real, Bool, Char, String
- 复合类型：Array, Table (映射), Function
- 动态类型系统

### 控制结构
- 条件语句：if-else if-else
- 循环：while, do-while, for
- 循环控制：break, continue
- 函数返回：return

### 函数编程
- 命名函数和匿名函数（Lambda）
- 高阶函数和闭包
- 递归支持

### 模块系统
- 内置模块：math, string, io, os, table
- 模块导入：`import module_name`
- 命名空间访问

### 错误处理
- 异常处理：try-catch
- 运行时错误检测
- 类型安全操作

## 使用示例

### 简单计算
```squaker
// 基本运算
x = 10
y = 20
sum = x + y
println("Sum: " + str(sum))

// 函数定义
function factorial(n) {
    if (n <= 1) return 1
    return n * factorial(n - 1)
}

println("5! = " + str(factorial(5)))
```

### 文件处理
```squaker
import io

// 读取文件
content = io.read_file("input.txt")
println("File size: " + str(string.length(content)))

// 写入文件
io.write_file("output.txt", "Processed: " + content)
```

### 数学计算
```squaker
import math

radius = 5.0
area = math.PI * radius ** 2
circumference = 2 * math.PI * radius

println("Radius: " + str(radius))
println("Area: " + str(area))
println("Circumference: " + str(circumference))
```

## 开发指南

### 构建项目
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 运行测试
```bash
./squaker_test
```

### 交互式解释器
```bash
./squaker
```

### 从 C++ 调用
```cpp
#include "squaker.h"

int main() {
    squ::Script script;
    ValueData result = script.execute("println('Hello from C++!')");
    return 0;
}
```

## 扩展开发

### 添加自定义函数
```cpp
ValueData custom_func(const std::vector<ValueData>& args) {
    // 实现自定义逻辑
    return ValueData{ValueType::Integer, false, 42};
}

// 注册函数
script.register_function("custom", custom_func);
```

### 添加新模块
在 `module.cpp` 中添加：
```cpp
if (module_name == "custom") {
    return Namespace("custom",
        Function("func1", custom_function1),
        Function("func2", custom_function2),
        Constant("VERSION", "1.0")
    );
}
```

## 最佳实践

### 代码风格
1. 使用有意义的变量名
2. 添加注释解释复杂逻辑
3. 保持一致的缩进
4. 使用函数封装重复代码

### 错误处理
1. 使用 try-catch 处理可能失败的操作
2. 验证输入参数
3. 提供清晰的错误消息

### 性能优化
1. 避免不必要的计算
2. 使用内置函数
3. 预计算常量值
4. 避免深度递归

## 故障排除

### 常见问题

1. **构建失败**：确保安装了 C++17 编译器和 CMake
2. **运行时错误**：检查错误消息和代码语法
3. **性能问题**：使用内置函数，避免不必要的计算
4. **内存问题**：确保正确管理脚本对象生命周期

### 调试技巧

1. 使用 `print` 和 `println` 输出调试信息
2. 在交互式解释器中测试代码片段
3. 查看错误消息中的行号和上下文
4. 使用 try-catch 捕获和处理异常

## 贡献指南

### 文档改进
1. 修正错误或过时的信息
2. 添加新的示例代码
3. 改进文档结构和可读性
4. 翻译为其他语言

### 代码贡献
1. 遵循现有的代码风格
2. 添加测试用例
3. 更新相关文档
4. 提交清晰的提交信息

### 问题报告
1. 描述问题的具体表现
2. 提供重现步骤
3. 包含相关代码片段
4. 说明期望的行为

## 许可证

Squaker 项目使用 MIT 许可证。详情请查看项目根目录中的 LICENSE 文件。

## 支持

如需帮助：
1. 查看本文档
2. 查看示例代码
3. 运行测试用例了解功能
4. 在项目仓库中提出问题

## 更新日志

### 文档版本 1.0
- 创建完整的语言指南
- 添加快速入门指南
- 提供详细的 API 参考
- 包含丰富的示例代码

### 未来计划
- 添加更多示例和教程
- 提供视频教程
- 创建交互式学习工具
- 翻译为多种语言

---

**Happy coding with Squaker!** 🎉