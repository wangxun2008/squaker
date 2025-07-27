#include "../include/module.h"
#include "../include/identifier.h"
#include <cmath>
#include <stdexcept>

namespace squ {

    IdentifierData Module(std::string module_name) {
        // 模块注册逻辑
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
                Function("atan2", static_cast<double (*)(double, double)>(std::atan2))
            );
        } else {
            throw std::runtime_error("[squaker.module] Unknown module: " + module_name);
        }

    }

} // namespace squ
