#pragma once

#include <memory>
#include <iostream>
#include <string_view>

namespace griha {

class Logger {

    struct Impl;

public:
    explicit Logger(std::ostream& output = std::cout);

    void log(std::string_view message) const;

private:
    std::shared_ptr<Impl> priv_;
};

} // namespace griha