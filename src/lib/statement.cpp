#include "statement.h"

namespace griha {

void SomeStatement::execute(Executer& ex_ctx) {
    ex_ctx.execute(*this);
}

} // namespace griha