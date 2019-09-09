#include "logger.h"

#include <mutex>

namespace griha {

struct Logger::Impl {

    std::ostream& output;
    std::mutex guard;

    explicit Impl(std::ostream& os)
        : output(os) {}
};

Logger::Logger(std::ostream& output) 
    : priv_(std::make_shared<Impl>(output)) {}

void Logger::log(std::string_view message) const {
    std::lock_guard l { priv_->guard };
    priv_->output << message << std::endl;
}

} // namespace griha