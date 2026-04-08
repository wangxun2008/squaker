# Squaker 快速入门

## 安装和构建

### 前提条件

- C++17 或更高版本
- CMake 3.10 或更高版本
- 支持 C++ 的编译器（GCC, Clang, MSVC）

### 构建步骤

1. 确保路径不包含中文（CMake不支持中文路径）：

2. 创建构建目录：
   
   ```bash
   cd squaker
   mkdir build
   cd build
   ```

3. 配置和构建：
   
   ```bash
   cmake ..
   cmake --build .
   ```

4. 运行：
   
   ```bash
   ./squaker.exe
   ```

## 第一个 Squaker 程序

### 交互式解释器

Squaker 提供了一个交互式解释器：

```bash
./squaker
```

在交互式环境中，你可以直接输入代码：

```
>>> println("Hello, Squaker!")
Hello, Squaker!
>>> 1 + 2 * 3
7
>>> x = 10
>>> x * 2
20
```

### 运行脚本文件

创建一个 `.squ` 文件：

```squaker
// hello.squ
println("Hello, World!")

// 计算
a = 10
b = 20
sum = a + b
println("Sum: " + str(sum))

// 循环
for (i = 1; i <= 5; i = i + 1) {
    println("Count: " + str(i))
}
```

使用 C++ 程序运行脚本：

```cpp
#include "squaker.h"

int main() {
    squ::Script script;

    // 从文件读取
    std::string code = squ::ReadFile("hello.squ");
    auto result = script.execute(code);

    // 或者直接执行代码
    result = script.execute("println('Hello from C++!')");

    return 0;
}
```

## 基本语法示例

### 变量和数据类型

```squaker
// types
integer_var = 42
float_var = 3.14159
bool_var = true
char_var = 'A'
string_var = "Hello, Squaker!"
nil_var = nil

// table
numbers = [1, 2, 3, 4, 5]
mixed = [1, "two", 3.0, true]
person = [
    ["name"] = "Alice",
    ["age"] = 30,
    ["city"] = "New York"
]
```

### 控制流

```squaker
// if-else
x = 10
if (x > 0) {
    println("x is positive")
} else if (x < 0) {
    println("x is negative")
} else {
    println("x is zero")
}

// for
for (i = 0; i < 5; i = i + 1) {
    println("i =", str(i))
}

// while
count = 0
while (count < 3) {
    println("Count:", str(count))
    count = count + 1
}

// do-while
n = 0
do {
    println("n =", str(n))
    n = n + 1
} while (n < 3)
```

### 函数

```squaker
function greet(name) {
    return "Hello, "..name.."!"
}

message = greet("Alice")
println(message)

square = function(x) x * x
println(square(5)) 

function apply_operation(f, x, y) {
    return f(x, y)
}

add = function(a, b) a + b
result = apply_operation(add, 3, 4)
```

## 模块使用

### 导入模块

```squaker
import math

radius = 5.0
area = math.PI * radius * radius

import string

text = "  Hello, World!  "
trimmed = string.trim(text)
println("Trimmed: '"..trimmed.."'")

import io

io.write_file("output.txt", "This is a test file.")
content = io.read_file("output.txt")
println("File content:", content)
```

## 示例: 计算器

```squaker
{
    println("Simple Calculator")
    println("1. Add")
    println("2. Subtract")
    println("3. Multiply")
    println("4. Divide")
    choice = input("Enter choice (1-4): ")
    a = real(input("Enter first number: "))
    b = real(input("Enter second number: "))
    result = 0.0
    operation = "";

    if (choice == "1") {
        result = a + b
        operation = "+"
    } else if (choice == "2") {
        result = a - b
        operation = "-"
    } else if (choice == "3") {
        result = a * b
        operation = "*"
    } else if (choice == "4") {
        if (b == 0) {
            println("Error: Division by zero!")
            return
        }
        result = a / b
        operation = "/"
    } else {
        println("Invalid choice!")
        return
    }
    println(str(a), operation, str(b), "=", str(result))
} 
```
