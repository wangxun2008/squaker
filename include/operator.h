#pragma once

#include "type.h"
#include <string>

namespace squ {

    ValueData applyBinary(const ValueData &lhs,
                          const std::string &op,
                          const ValueData &rhs);

} // namespace squ