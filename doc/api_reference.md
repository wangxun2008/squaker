# Squaker API 参考

## 目录

1. [C++ API](#c-api)
2. [内置函数](#内置函数)
3. [模块API](#模块api)
4. [数据类型](#数据类型)
5. [错误代码](#错误代码)

## C++ API

### Script 类

`Script` 类是 Squaker 的主要接口，用于执行脚本代码。

#### 构造函数

```cpp
#include "squaker.h"

// 创建脚本对象
squ::Script script;
```

#### 方法

##### execute

执行 Squaker 代码并返回结果。

```cpp
// 执行字符串代码
ValueData result = script.execute("1 + 2 * 3");

// 执行文件
std::string code = squ::ReadFile("script.squ");
ValueData result = script.execute(code);
```

##### 返回值

`execute` 方法返回 `ValueData` 对象，包含执行结果。可以使用以下方法获取值：

```cpp
ValueData result = script.execute("42");

// 获取字符串表示
std::string str = result.string();

// 检查类型
if (result.type == ValueType::Integer) {
    long long value = std::get<long long>(result.value);
}

// 转换为特定类型
long long int_value = result.as_integer();
double real_value = result.as_real();
bool bool_value = result.as_bool();
std::string string_value = result.as_string();
```

### 工具函数

#### ReadFile

读取文件内容。

```cpp
std::string content = squ::ReadFile("filename.txt");
```

#### ParseTokens

将代码字符串解析为令牌序列（用于调试）。

```cpp
std::vector<squ::Token> tokens = squ::ParseTokens("1 + 2 * 3");
```

#### PrintTokens

将令牌序列转换为可读字符串（用于调试）。

```cpp
std::vector<squ::Token> tokens = squ::ParseTokens("1 + 2");
std::string debug_str = squ::PrintTokens(tokens);
```

## 内置函数

### 输入输出函数

#### print

打印内容到标准输出，不换行。

**语法：**

```squaker
print(value1, value2, ...)
```

**参数：**

- `value`：要打印的值（任意类型）

**示例：**

```squaker
print("Hello, ")
print("World!")  // 输出: Hello, World!
```

#### println

打印内容到标准输出并换行。

**语法：**

```squaker
println(value1, value2, ...)
```

**参数：**

- `value`：要打印的值（任意类型）

**示例：**

```squaker
println("Hello, World!")  // 输出: Hello, World! 并换行
println(42)               // 输出: 42 并换行
```

#### input

从标准输入读取一行。

**语法：**

```squaker
input(prompt)
```

**参数：**

- `prompt`（可选）：提示字符串

**返回值：** 用户输入的字符串

**示例：**

```squaker
name = input("Enter your name: ")
println("Hello, "..name.."!")
```

### 类型转换函数

#### int

转换为整数。

**语法：**

```squaker
int(value)
```

**参数：**

- `value`：要转换的值（字符串、实数、布尔值）

**返回值：** 整数

**示例：**

```squaker
int("42")      // 42
int(3.14)      // 3
int(true)      // 1
int(false)     // 0
```

#### real

转换为实数（浮点数）。

**语法：**

```squaker
real(value)
```

**参数：**

- `value`：要转换的值（字符串、整数、布尔值）

**返回值：** 实数

**示例：**

```squaker
real("3.14")   // 3.14
real(42)       // 42.0
real(true)     // 1.0
```

#### bool

转换为布尔值。

**语法：**

```squaker
bool(value)
```

**参数：**

- `value`：要转换的值（字符串、整数、实数）

**返回值：** 布尔值

**示例：**

```squaker
bool(1)        // true
bool(0)        // false
bool(3.14)     // true
bool("true")   // true
bool("")       // false
```

#### char

转换为字符。

**语法：**

```squaker
char(value)
```

**参数：**

- `value`：要转换的值（整数、字符串）

**返回值：** 字符

**示例：**

```squaker
char(65)       // 'A'
char("B")      // 'B'
```

#### str

转换为字符串。

**语法：**

```squaker
str(value)
```

**参数：**

- `value`：要转换的值（任意类型）

**返回值：** 字符串表示

**示例：**

```squaker
str(42)        // "42"
str(3.14)      // "3.14"
str(true)      // "true"
str([1, 2, 3]) // "[1, 2, 3]"
```

## 模块API

### math 模块

数学函数和常量。

#### 常量

| 常量       | 值                      | 描述         |
| -------- | ---------------------- | ---------- |
| `PI`     | 3.14159265358979323846 | 圆周率        |
| `E`      | 2.71828182845904523536 | 自然对数的底     |
| `LN2`    | 0.69314718055994530942 | 2的自然对数     |
| `LN10`   | 2.30258509299404568402 | 10的自然对数    |
| `LOG2E`  | 1.44269504088896340736 | e的以2为底的对数  |
| `LOG10E` | 0.43429448190325182765 | e的以10为底的对数 |

#### 函数

##### 三角函数

| 函数            | 描述        | 示例                            |
| ------------- | --------- | ----------------------------- |
| `sin(x)`      | 正弦        | `math.sin(math.PI/2)` → `1.0` |
| `cos(x)`      | 余弦        | `math.cos(0)` → `1.0`         |
| `tan(x)`      | 正切        | `math.tan(math.PI/4)` → `1.0` |
| `asin(x)`     | 反正弦       | `math.asin(1)` → `1.5708`     |
| `acos(x)`     | 反余弦       | `math.acos(1)` → `0.0`        |
| `atan(x)`     | 反正切       | `math.atan(1)` → `0.7854`     |
| `atan2(y, x)` | 反正切（两个参数） | `math.atan2(1, 1)` → `0.7854` |

##### 双曲函数

| 函数        | 描述   | 示例                     |
| --------- | ---- | ---------------------- |
| `sinh(x)` | 双曲正弦 | `math.sinh(0)` → `0.0` |
| `cosh(x)` | 双曲余弦 | `math.cosh(0)` → `1.0` |
| `tanh(x)` | 双曲正切 | `math.tanh(0)` → `0.0` |

##### 指数和对数函数

| 函数          | 描述    | 示例                         |
| ----------- | ----- | -------------------------- |
| `exp(x)`    | e的x次幂 | `math.exp(1)` → `2.71828`  |
| `log(x)`    | 自然对数  | `math.log(math.E)` → `1.0` |
| `pow(x, y)` | x的y次幂 | `math.pow(2, 3)` → `8.0`   |
| `sqrt(x)`   | 平方根   | `math.sqrt(4)` → `2.0`     |

##### 取整函数

| 函数           | 描述    | 示例                            |
| ------------ | ----- | ----------------------------- |
| `ceil(x)`    | 向上取整  | `math.ceil(3.14)` → `4.0`     |
| `floor(x)`   | 向下取整  | `math.floor(3.14)` → `3.0`    |
| `round(x)`   | 四舍五入  | `math.round(3.14)` → `3.0`    |
| `fmod(x, y)` | 浮点数取模 | `math.fmod(5.5, 2.0)` → `1.5` |

##### 其他函数

| 函数            | 描述     | 示例                         |
| ------------- | ------ | -------------------------- |
| `abs(x)`      | 绝对值    | `math.abs(-3.14)` → `3.14` |
| `hypot(x, y)` | 欧几里得距离 | `math.hypot(3, 4)` → `5.0` |
| `max(x, y)`   | 最大值    | `math.max(3, 5)` → `5.0`   |
| `min(x, y)`   | 最小值    | `math.min(3, 5)` → `3.0`   |

### string 模块

字符串操作函数。

#### 函数

| 函数        | 描述      | 语法                               | 示例                                                 |
| --------- | ------- | -------------------------------- | -------------------------------------------------- |
| `length`  | 字符串长度   | `string.length(str)`             | `string.length("hello")` → `5`                     |
| `concat`  | 字符串连接   | `string.concat(a, b)`            | `string.concat("hello", "world")` → `"helloworld"` |
| `substr`  | 子字符串    | `string.substr(str, start, end)` | `string.substr("hello", 1, 4)` → `"ell"`           |
| `split`   | 分割字符串   | `string.split(str, delimiter)`   | `string.split("a,b,c", ",")` → `["a", "b", "c"]`   |
| `join`    | 连接字符串数组 | `string.join(parts, delimiter)`  | `string.join(["a", "b", "c"], ",")` → `"a,b,c"`    |
| `trim`    | 去除空白字符  | `string.trim(str)`               | `string.trim("  hello  ")` → `"hello"`             |
| `reverse` | 反转字符串   | `string.reverse(str)`            | `string.reverse("hello")` → `"olleh"`              |

### io 模块

文件输入输出函数。

#### 函数

| 函数           | 描述   | 语法                                 | 示例                                     |
| ------------ | ---- | ---------------------------------- | -------------------------------------- |
| `read_file`  | 读取文件 | `io.read_file(filename)`           | `content = io.read_file("test.txt")`   |
| `write_file` | 写入文件 | `io.write_file(filename, content)` | `io.write_file("output.txt", "Hello")` |

### os 模块

操作系统功能。

#### 函数

| 函数       | 描述       | 语法                    | 示例                                    |
| -------- | -------- | --------------------- | ------------------------------------- |
| `system` | 执行系统命令   | `os.system(command)`  | `os.system("ls -la")`                 |
| `getenv` | 获取环境变量   | `os.getenv(name)`     | `os.getenv("PATH")`                   |
| `exit`   | 退出程序     | `os.exit(code)`       | `os.exit(0)`                          |
| `sleep`  | 休眠       | `os.sleep(seconds)`   | `os.sleep(1)`                         |
| `clock`  | 获取CPU时间  | `os.clock()`          | `os.clock()`                          |
| `remove` | 删除文件     | `os.remove(filename)` | `os.remove("temp.txt")`               |
| `date`   | 获取当前日期时间 | `os.date()`           | `os.date()` → `"2024-01-01 12:00:00"` |
| `time`   | 获取时间戳    | `os.time()`           | `os.time()` → `1704067200`            |
| `getpid` | 获取进程ID   | `os.getpid()`         | `os.getpid()`                         |
| `rename` | 重命名文件    | `os.rename(old, new)` | `os.rename("old.txt", "new.txt")`     |

### table 模块

表（映射）操作函数。

#### 函数

| 函数       | 描述    | 语法                         | 示例                                          |
| -------- | ----- | -------------------------- | ------------------------------------------- |
| `remove` | 删除键   | `table.remove(table, key)` | `table.remove(map, "key")`                  |
| `keys`   | 获取所有键 | `table.keys(table)`        | `table.keys({"a":1, "b":2})` → `["a", "b"]` |
| `values` | 获取所有值 | `table.values(table)`      | `table.values({"a":1, "b":2})` → `[1, 2]`   |
| `size`   | 获取表大小 | `table.size(table)`        | `table.size({"a":1, "b":2})` → `2`          |

## 数据类型

### ValueType 枚举

| 值          | 描述  | C++ 类型                                             |
| ---------- | --- | -------------------------------------------------- |
| `Nil`      | 空值  | `std::monostate`                                   |
| `Integer`  | 整数  | `long long`                                        |
| `Real`     | 实数  | `double`                                           |
| `Bool`     | 布尔值 | `bool`                                             |
| `Char`     | 字符  | `char`                                             |
| `String`   | 字符串 | `std::string`                                      |
| `Function` | 函数  | `std::function<ValueData(std::vector<ValueData>)>` |
| `Array`    | 数组  | `std::vector<ValueData>`                           |
| `Table`    | 表   | `TableData`（包含 `dot_map` 和 `index_map`）            |

### ValueData 结构

`ValueData` 是 Squaker 中所有值的容器。

#### 成员

- `type`: `ValueType` - 值的类型
- `constant`: `bool` - 是否为常量
- `value`: `std::variant<...>` - 实际值

#### 方法

##### string

获取值的字符串表示。

```cpp
std::string str = value.string();
```

##### as_integer

转换为整数。

```cpp
long long int_value = value.as_integer();
```

##### as_real

转换为实数。

```cpp
double real_value = value.as_real();
```

##### as_bool

转换为布尔值。

```cpp
bool bool_value = value.as_bool();
```

##### as_string

转换为字符串。

```cpp
std::string str_value = value.as_string();
```

### TableData 结构

表数据结构，支持点访问和索引访问。

#### 成员

- `dot_map`: `std::unordered_map<std::string, ValueData>` - 点访问映射
- `index_map`: `std::unordered_map<ValueData, ValueData>` - 索引访问映射

#### 方法

##### index_at

通过索引获取值。

```cpp
ValueData value = table.index_at(index);
```

## 错误代码

### 语法错误

| 错误消息                                                  | 描述         | 可能原因           |
| ----------------------------------------------------- | ---------- | -------------- |
| `[squaker.parser] Unexpected tokens after expression` | 表达式后有多余的令牌 | 语法错误           |
| `[squaker.parser] Expected '('`                       | 期望左括号      | 函数调用或控制流语句缺少括号 |
| `[squaker.parser] Expected ')'`                       | 期望右括号      | 括号不匹配          |
| `[squaker.parser] Expected identifier`                | 期望标识符      | 变量名或函数名错误      |
| `[squaker.parser] Expected ';'`                       | 期望分号       | 语句分隔符缺失        |

### 运行时错误

| 错误消息                                                | 描述        | 可能原因        |
| --------------------------------------------------- | --------- | ----------- |
| `[squaker.operator:'/'] division by zero`           | 除零错误      | 除数为0        |
| `[squaker.index] Array index out of bounds`         | 数组越界      | 访问不存在的数组索引  |
| `[squaker.index] Array index must be an integer`    | 数组索引必须是整数 | 使用非整数索引访问数组 |
| `[squaker.operator] unsupported types for operator` | 运算符不支持的类型 | 类型不匹配的操作    |
| `[squaker.module] Unknown module`                   | 未知模块      | 导入不存在的模块    |
| `[squaker.parser.import] Module already imported`   | 模块已导入     | 重复导入模块      |

### 类型错误

| 错误消息                         | 描述     | 可能原因      |
| ---------------------------- | ------ | --------- |
| `Type mismatch`              | 类型不匹配  | 操作数类型不兼容  |
| `Cannot convert type`        | 无法转换类型 | 类型转换失败    |
| `Invalid type for operation` | 操作类型无效 | 不支持该类型的操作 |

### 作用域错误

| 错误消息                        | 描述      | 可能原因     |
| --------------------------- | ------- | -------- |
| `Undefined variable`        | 未定义变量   | 使用未声明的变量 |
| `Variable already defined`  | 变量已定义   | 重复定义变量   |
| `Cannot assign to constant` | 不能赋值给常量 | 尝试修改常量   |

## 扩展API

### 添加自定义函数

要在 C++ 中添加自定义函数：

```cpp
#include "squaker.h"
#include "identifier.h"

// 定义函数
ValueData my_function(const std::vector<ValueData>& args) {
    // 处理参数
    if (args.size() != 1) {
        throw std::runtime_error("Expected 1 argument");
    }

    // 获取参数
    long long value = args[0].as_integer();

    // 计算结果
    return ValueData{ValueType::Integer, false, value * 2};
}

// 注册函数
squ::Script script;
script.register_function("double", my_function);

// 在 Squaker 中使用
script.execute("println(double(21))");  // 输出: 42
```

### 添加自定义模块

要在 C++ 中添加自定义模块：

```cpp
#include "module.h"

// 在 module.cpp 中添加
IdentifierData Module(std::string module_name) {
    if (module_name == "mymodule") {
        return Namespace("mymodule",
            Function("func1", [](const std::string& s) {
                return "Processed: " + s;
            }),
            Function("func2", [](long long a, long long b) {
                return a * b;
            }),
            Constant("VERSION", "1.0.0")
        );
    }
    // ... 其他模块
}
```

### 自定义类型

要添加自定义类型，需要：

1. 在 `type.h` 中添加新的 `ValueType` 枚举值
2. 在 `ValueData` 的 variant 中添加对应的类型
3. 实现相应的操作和转换函数

## 调试API

### 令牌调试

```cpp
// 查看令牌序列
std::vector<squ::Token> tokens = squ::ParseTokens("1 + 2 * 3");
std::string debug = squ::PrintTokens(tokens);
std::cout << "Tokens: " << debug << std::endl;
```
