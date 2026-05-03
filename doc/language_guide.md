# Squaker 语言指南

## 概述

Squaker 是一个轻量级脚本语言，使用 C++ 实现。它支持动态类型、函数式编程特性等。本指南将详细介绍 Squaker 语言的语法、特性和使用方法。

## 目录

1. [基本语法](## 基本语法)
2. [数据类型](## 数据类型)
3. [变量和常量](## 变量和常量)
4. [运算符](## 运算符)
5. [控制流](## 控制流)
6. [函数](## 函数)
7. [数组和表](## 数组和表)
8. [模块系统](## 模块系统)
9. [错误处理](## 错误处理)
10. [内置函数](## 内置函数)

## 基本语法

### 注释

Squaker 支持两种注释方式：

```squaker
// 单行注释

/*
  多行注释
  可以跨越多行
*/
```

### 语句分隔符

语句可以以分号结尾，但不是必需的：

```squaker
a = 1
b = 2;  // 分号可选
c = a + b
```

### 代码块

使用花括号 `{}` 创建代码块：

```squaker
{
    a = 1
    b = 2
    c = a + b
}
```

## 数据类型

Squaker 支持以下数据类型：

| 类型         | 描述   | 示例                       |
| ---------- | ---- | ------------------------ |
| `Nil`      | 空值   | `nil`                    |
| `Integer`  | 整数   | `42`, `-10`, `0xFF`      |
| `Real`     | 浮点数  | `3.14`, `-2.5`, `1.0e-3` |
| `Bool`     | 布尔值  | `true`, `false`          |
| `Char`     | 字符   | `'a'`, `'\n'`, `'\t'`    |
| `String`   | 字符串  | `"hello"`, `"world\n"`   |
| `Function` | 函数   | `function(x) x * x`      |
| `Array`    | 数组   | `[1, 2, 3]`              |
| `Table`    | 表/映射 | `{"key": "value"}`       |

### 类型转换

Squaker 提供内置类型转换函数：

```squaker
a = int("42")      // 字符串转整数: 42
b = real("3.14")   // 字符串转浮点数: 3.14
c = bool(1)        // 整数转布尔: true
d = char(65)       // 整数转字符: 'A'
e = str(123)       // 整数转字符串: "123"
```

## 变量和常量

### 变量声明

变量使用赋值运算符声明：

```squaker
x = 10              // 整数变量
y = 3.14            // 浮点数变量
name = "Squaker"    // 字符串变量
is_valid = true     // 布尔变量
```

### 常量

使用 `const` 关键字声明常量：

```squaker
const PI = 3.14159
const MAX_SIZE = 100
```

### 变量作用域

Squaker 支持块级作用域：

```squaker
x = 1               // 全局变量

{
    y = 2           // 块内变量
    x = 3           // 修改全局变量
}

// y 在这里不可访问
```

## 运算符

### 算术运算符

| 运算符 | 描述  | 示例      |
| --- | --- | ------- |
| `+` | 加法  | `a + b` |
| `-` | 减法  | `a - b` |
| `*` | 乘法  | `a * b` |
| `/` | 除法  | `a / b` |
| `%` | 取模  | `a % b` |

### 比较运算符

| 运算符  | 描述   | 示例       |
| ---- | ---- | -------- |
| `==` | 等于   | `a == b` |
| `!=` | 不等于  | `a != b` |
| `<`  | 小于   | `a < b`  |
| `>`  | 大于   | `a > b`  |
| `<=` | 小于等于 | `a <= b` |
| `>=` | 大于等于 | `a >= b` |

### 逻辑运算符

| 运算符  | 描述  | 示例       |
| ---- | --- | -------- |
| `&&` | 逻辑与 | `a && b` |
| `\|  | `   | 逻辑或      |
| `!`  | 逻辑非 | `!a`     |

### 赋值运算符

| 运算符  | 描述    | 示例       |
| ---- | ----- | -------- |
| `=`  | 赋值    | `a = 10` |
| `+=` | 加后赋值  | `a += 5` |
| `-=` | 减后赋值  | `a -= 3` |
| `*=` | 乘后赋值  | `a *= 2` |
| `/=` | 除后赋值  | `a /= 2` |
| `%=` | 取模后赋值 | `a %= 3` |
| `++` | 自增    | `a++`    |
| `--` | 自减    | `a--`    |

### 位运算符

| 运算符  | 描述   | 示例       |
| ---- | ---- | -------- |
| `&`  | 按位与  | `a & b`  |
| `\|` | 按位或  | `a \| b` |
| `^`  | 按位异或 | `a ^ b`  |
| `~`  | 按位取反 | `~a`     |
| `<<` | 左移   | `a << 2` |
| `>>` | 右移   | `a >> 1` |

### 运算符优先级

从高到低：

1. `()` 括号
2. `++` `--` 后缀自增/自减
3. `!` `~` `+` `-` 一元运算符
4. `**` 幂运算
5. `*` `/` `%` 乘除取模
6. `+` `-` 加减
7. `<<` `>>` 位移
8. `<` `<=` `>` `>=` 比较
9. `==` `!=` 相等性
10. `&` 按位与
11. `^` 按位异或
12. `|` 按位或
13. `&&` 逻辑与
14. `||` 逻辑或
15. `=` `+=` `-=` `*=` `/=` `%=` 赋值

## 控制流

### 条件语句

#### if-else

```squaker
if (condition) {
    // 条件为真时执行
} else if (another_condition) {
    // 另一个条件为真时执行
} else {
    // 所有条件都为假时执行
}
```

示例：

```squaker
x = 10
if (x > 0) {
    println("x is positive")
} else if (x < 0) {
    println("x is negative")
} else {
    println("x is zero")
}
```

### 循环语句

#### while 循环

```squaker
while (condition) {
    // 循环体
}
```

示例：

```squaker
i = 0
while (i < 5) {
    println("i =", str(i))
    i = i + 1
}
```

#### do-while 循环

```squaker
do {
    // 循环体
} while (condition)
```

示例：

```squaker
i = 0
do {
    println("i =", str(i))
    i = i + 1
} while (i < 5)
```

#### for 循环

```squaker
for (initialization; condition; update) {
    // 循环体
}
```

示例：

```squaker
for (i = 0; i < 10; i = i + 1) {
    println("i =", str(i))
}
```

### 循环控制

#### break

跳出当前循环：

```squaker
for (i = 0; i < 10; i = i + 1) {
    if (i == 5) {
        break
    }
    println(i)
}
```

#### continue

跳过当前循环迭代：

```squaker
for (i = 0; i < 10; i = i + 1) {
    if (i % 2 == 0) {
        continue
    }
    println(i)
}
```

## 函数

### 函数定义

#### 命名函数

```squaker
function function_name(parameter1, parameter2, ...) {
    // 函数体
    return value
}
```

示例：

```squaker
function add(a, b) {
    return a + b
}

result = add(3, 4)  // result = 7
```

#### 匿名函数（Lambda）

```squaker
function(parameters) expression
```

或

```squaker
function(parameters) {
    // 函数体
    return value
}
```

示例：

```squaker
square = function(x) x * x
result = square(5)  // result = 25

add = function(a, b) {
    return a + b
}
```

### 函数调用

```squaker
function_name(arg1, arg2, ...)
```

### 高阶函数

函数可以作为参数传递和返回：

```squaker
function apply_twice(f, x) {
    return f(f(x))
}

function increment(x) {
    return x + 1
}

result = apply_twice(increment, 5)  // result = 7
```

### 闭包（尚未实现）

函数可以捕获外部变量：

```squaker
function make_counter() {
    count = 0
    return function() {
        count = count + 1
        return count
    }
}

counter = make_counter()
println(counter())  // 输出: 1
println(counter())  // 输出: 2
println(counter())  // 输出: 3
```

## 数组和表

### 数组

#### 创建数组

```squaker
arr = [1, 2, 3, 4, 5]
mixed = [1, "hello", true, 3.14]
nested = [[1, 2], [3, 4], [5, 6]]
```

#### 访问数组元素

```squaker
arr = [10, 20, 30, 40, 50]
first = arr[0]      // 10
second = arr[1]     // 20
last = arr[4]       // 50
```

#### 修改数组元素

```squaker
arr = [1, 2, 3]
arr[0] = 10         // arr 变为 [10, 2, 3]
arr[2] = 30         // arr 变为 [10, 2, 30]
```

#### ！数组长度

```squaker
arr = [1, 2, 3, 4, 5]
length = arr.length()  // 5
```

### 表（映射）

#### 创建表

```squaker
person = [
    ["name"] = "Alice",
    ["age"] = 30,
    ["city"] = "New York"
]

config = [
    ["debug"] = true,
    ["port"] = 8080,
    ["host"] = "localhost"
]
```

#### 访问表元素

```squaker
person = [["name"] = "Alice", age = 30]
name = person["name"]      // "Alice"
age = person.age           // 30
```

#### 修改表元素

```squaker
person = [["name"] = "Alice"]
person["age"] = 30
person.city = "New York"
```

#### 表操作

```squaker
table = [["a"] = 1, ["b"] = 2, ["c"] = 3}

// 获取所有键
keys = table.keys()

// 获取所有值
values = table.values()

// 获取大小
size = table.size()

// 删除键
table.remove("b")
```

## 模块系统

### 导入模块

```squaker
import math
import string
import io
import os
import table
```

### 使用模块

```squaker
import math

result = math.sin(math.PI / 2)  // 1.0
radius = 5.0
area = math.PI * radius * radius
```

### 可用模块

#### math 模块

数学函数和常量：

- `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- `sqrt`, `pow`, `log`, `exp`, `abs`
- `ceil`, `floor`, `round`, `fmod`
- `PI`, `E`, `LN2`, `LN10`, `LOG2E`, `LOG10E`

#### string 模块

字符串操作：

- `length(str)` - 字符串长度
- `concat(a, b)` - 字符串连接
- `substr(str, start, end)` - 子字符串
- `split(str, delimiter)` - 分割字符串
- `join(parts, delimiter)` - 连接字符串数组
- `trim(str)` - 去除空白字符
- `reverse(str)` - 反转字符串

#### io 模块

文件输入输出：

- `read_file(filename)` - 读取文件内容
- `write_file(filename, content)` - 写入文件

#### os 模块

操作系统功能：

- `system(command)` - 执行系统命令
- `getenv(name)` - 获取环境变量
- `exit(code)` - 退出程序
- `sleep(seconds)` - 休眠指定秒数
- `clock()` - 获取CPU时间
- `remove(filename)` - 删除文件
- `date()` - 获取当前日期时间
- `time()` - 获取时间戳
- `getpid()` - 获取进程ID
- `rename(old_name, new_name)` - 重命名文件

#### table 模块

表操作：

- `remove(table, key)` - 删除键
- `keys(table)` - 获取所有键
- `values(table)` - 获取所有值
- `size(table)` - 获取表大小

## 内置函数

### 输入输出

#### print

打印内容，不换行：

```squaker
print("Hello, ")
print("World!")  // 输出: Hello, World!
```

#### println

打印内容并换行：

```squaker
println("Hello, World!")  // 输出: Hello, World! 并换行
```

#### input

读取用户输入：

```squaker
name = input("Enter your name: ")
println("Hello, "..name.."!")
```

### 类型转换

- `int(value)` - 转换为整数
- `real(value)` - 转换为浮点数
- `bool(value)` - 转换为布尔值
- `char(value)` - 转换为字符
- `str(value)` - 转换为字符串
