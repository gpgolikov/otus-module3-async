#include "statement_factory.h"

#include "statement.h"

namespace griha {

StatementPtr StatementFactory::create(std::string line) const {
    return std::make_shared<SomeStatement>(std::move(line));
}

} // namespace griha