#pragma once

#include <string>
#include <memory>

#include "forward.h"

namespace griha {

class Reader {
public:
    struct Metrics {
        size_t nlines;
        size_t nstatements;
        size_t nblocks;
    };

public:
    Reader(size_t block_size);
    ~Reader();

    Reader(Reader&&);
    Reader& operator= (Reader&&);

    // delete copy operations to simplify Reader class
    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    void subscribe(ReaderSubscriberPtr subscriber);

    void consume(std::string line);
    const Metrics& get_metrics() const;

    void on_eof();

private:
    std::unique_ptr<struct ReaderImpl> priv_;
};

} // namespace griha