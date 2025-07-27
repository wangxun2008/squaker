#include "../include/module.h"
#include "../include/identifier.h"
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h> // for getpid
#include <ctime>
#include <chrono>

namespace squ {

    IdentifierData Module(std::string module_name) {
        // 模块注册逻辑
        // 数学模块
        if (module_name == "math") {
            return Namespace("math",
                Function("sin", static_cast<double (*)(double)>(std::sin)),
                Function("cos", static_cast<double (*)(double)>(std::cos)),
                Function("tan", static_cast<double (*)(double)>(std::tan)),
                Function("asin", static_cast<double (*)(double)>(std::asin)),
                Function("acos", static_cast<double (*)(double)>(std::acos)),
                Function("atan", static_cast<double (*)(double)>(std::atan)),
                Function("sqrt", static_cast<double (*)(double)>(std::sqrt)),
                Function("pow", static_cast<double (*)(double, double)>(std::pow)),
                Function("log", static_cast<double (*)(double)>(std::log)),
                Function("exp", static_cast<double (*)(double)>(std::exp)),
                Function("abs", static_cast<double (*)(double)>(std::abs)),
                Function("ceil", static_cast<double (*)(double)>(std::ceil)),
                Function("floor", static_cast<double (*)(double)>(std::floor)),
                Function("round", static_cast<double (*)(double)>(std::round)),
                Function("fmod", static_cast<double (*)(double, double)>(std::fmod)),
                Function("hypot", static_cast<double (*)(double, double)>(std::hypot)),
                Function("max", static_cast<double (*)(double, double)>(std::fmax)),
                Function("min", static_cast<double (*)(double, double)>(std::fmin)),
                Function("atan2", static_cast<double (*)(double, double)>(std::atan2)),
                Function("ceil", static_cast<double (*)(double)>(std::ceil)),
                Function("floor", static_cast<double (*)(double)>(std::floor)),
                Function("cosh", static_cast<double (*)(double)>(std::cosh)),
                Function("sinh", static_cast<double (*)(double)>(std::sinh)),
                Function("tanh", static_cast<double (*)(double)>(std::tanh)),
                Constant("PI", 3.14159265358979323846),
                Constant("E", 2.71828182845904523536),
                Constant("LN2", 0.69314718055994530942),
                Constant("LN10", 2.30258509299404568402),
                Constant("LOG2E", 1.44269504088896340736),
                Constant("LOG10E", 0.43429448190325182765)
            );
        } 
        // 字符串模块
        else if (module_name == "string") {
            return Namespace("string",
                Function("length", [](const std::string &s) { return static_cast<long long>(s.length()); }),
                Function("concat", [](const std::string &a, const std::string &b) { return a + b; }),
                Function("substr", [](const std::string &s, long long start, long long end) {
                    return s.substr(static_cast<size_t>(start), static_cast<size_t>(end - start));
                }),
                Function("upper", [](const std::string &s) {
                    std::string result = s;
                    for (auto &c : result) c = static_cast<char>(std::toupper(c));
                    return result;
                }),
                Function("lower", [](const std::string &s) {
                    std::string result = s;
                    for (auto &c : result) c = static_cast<char>(std::tolower(c));
                    return result;
                }),
                Function("find", [](const std::string &s, const std::string &sub) {
                    size_t pos = s.find(sub);
                    return pos != std::string::npos ? static_cast<long long>(pos) : -1;
                }),
                Function("replace", [](const std::string &s, const std::string &old_sub, const std::string &new_sub) {
                    std::string result = s;
                    size_t pos = 0;
                    while ((pos = result.find(old_sub, pos)) != std::string::npos) {
                        result.replace(pos, old_sub.length(), new_sub);
                        pos += new_sub.length();
                    }
                    return result;
                }),
                Function("split", [](const std::string &s, const std::string &delimiter) {
                    std::vector<std::string> result;
                    size_t start = 0, end;
                    while ((end = s.find(delimiter, start)) != std::string::npos) {
                        result.push_back(s.substr(start, end - start));
                        start = end + delimiter.length();
                    }
                    result.push_back(s.substr(start));
                    return result;
                }),
                Function("join", [](std::vector<std::string> parts, const std::string &delimiter) {
                    std::string result;
                    for (size_t i = 0; i < parts.size(); ++i) {
                        result += parts[i];
                        if (i < parts.size() - 1) {
                            result += delimiter;
                        }
                    }
                    return result;
                }),
                Function("trim", [](const std::string &s) {
                    size_t first = s.find_first_not_of(" \t\n\r");
                    size_t last = s.find_last_not_of(" \t\n\r");
                    return (first == std::string::npos || last == std::string::npos) ? "" : s.substr(first, last - first + 1);
                }),
                Function("reverse", [](const std::string &s) {
                    return std::string(s.rbegin(), s.rend());
                })
            );
        }
        // 文件输入输出模块
        else if (module_name == "io") {
            return Namespace("io",
                Function("read_file", [](const std::string &filename) {
                    std::ifstream file(filename);
                    if (!file.is_open()) {
                        throw std::runtime_error("[squaker.io] Failed to open file: " + filename);
                    }
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    return buffer.str();
                }),
                Function("write_file", [](const std::string &filename, const std::string &content) {
                    std::ofstream file(filename);
                    if (!file.is_open()) {
                        throw std::runtime_error("[squaker.io] Failed to open file for writing: " + filename);
                    }
                    file << content;
                })
            );
        }
        // 操作系统工具模块
        else if (module_name == "os") {
            return Namespace("os",
                Function("system", [](const std::string &command) {
                    int result = std::system(command.c_str());
                    return result;
                }),
                Function("getenv", [](const std::string &name) {
                    const char *value = std::getenv(name.c_str());
                    return value ? std::string(value) : "";
                }),
                Function("exit", [](long long code) {
                    std::exit(static_cast<int>(code));
                }),
                Function("sleep", [](long long seconds) {
                    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(seconds)));
                }),
                Function("clock", []() {
                    return static_cast<long long>(std::clock() / CLOCKS_PER_SEC);
                }),
                Function("remove" , [](const std::string &filename) {
                    if (std::remove(filename.c_str()) != 0) {
                        throw std::runtime_error("[squaker.os] Failed to remove file: " + filename);
                    }
                }),
                Function("date", []() {
                    std::time_t now = std::time(nullptr);
                    char buffer[100];
                    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
                    return std::string(buffer);
                }),
                Function("time", []() {
                    std::time_t now = std::time(nullptr);
                    return static_cast<long long>(now);
                }),
                Function("getpid", []() {
                    return static_cast<long long>(getpid());
                }),
                Function("getcwd", []() {
                    char buffer[1024];
                    if (getcwd(buffer, sizeof(buffer)) == nullptr) {
                        throw std::runtime_error("[squaker.os] Failed to get current working directory");
                    }
                    return std::string(buffer);
                }),
                Function("rename", [](const std::string &old_name, const std::string &new_name) {
                    if (std::rename(old_name.c_str(), new_name.c_str()) != 0) {
                        throw std::runtime_error("[squaker.os] Failed to rename file: " + old_name + " to " + new_name);
                    }
                })
            );
        }
        else {
            throw std::runtime_error("[squaker.module] Unknown module: " + module_name);
        }

    }

} // namespace squ
