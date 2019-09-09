#pragma once

#include <memory>
#include <vector>

namespace griha {

struct Statement;
using StatementPtr = std::shared_ptr<Statement>;
using StatementContainer = std::vector<StatementPtr>;

class Reader;

struct ReaderSubscriber;
using ReaderSubscriberPtr = std::shared_ptr<ReaderSubscriber>;

} // namespace griha