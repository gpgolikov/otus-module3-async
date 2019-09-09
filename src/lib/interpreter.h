#pragma once

#include <string>
#include <string_view>
#include <memory>

#include "forward.h"
#include "logger.h"
#include "reader.h"

namespace griha {

class Interpreter {

    struct Impl;

public:
    struct Context {
        Logger logger;
        size_t block_size;
        size_t nthreads;
    };

public:
    Interpreter(Context context, std::string name);
    ~Interpreter();

    Interpreter(Interpreter&&);
    Interpreter& operator= (Interpreter&&);
    
    Interpreter(const Interpreter&) = delete;
    Interpreter& operator= (const Interpreter&) = delete;

    void consume(std::string_view data);
    void stop_and_log_metrics() const;

private:
    std::unique_ptr<Impl> priv_;
};
using InterpreterPtr = std::shared_ptr<Interpreter>;

} // namespace griha