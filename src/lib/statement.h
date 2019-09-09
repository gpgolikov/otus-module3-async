 #pragma once

#include <string>

#include "forward.h"

namespace griha {

struct Executer;
struct Statement {
    virtual ~Statement() {};
    virtual void execute(Executer&) = 0;
};

class SomeStatement : public Statement {

public:
    explicit SomeStatement(std::string value) : value_(std::move(value)) {}

    void execute(Executer& ex_ctx) override;

    const std::string& value() const { return value_; }

private:
    std::string value_;
};

struct Executer {
    virtual void execute(const SomeStatement&) = 0;
};


} // namespace griha