#pragma once

#include <string>

#include "forward.h"

namespace griha {

struct StatementFactory {
    StatementPtr create(std::string) const;
};

} // namespace griha