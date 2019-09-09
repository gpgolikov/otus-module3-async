#include "reader.h"

#include <algorithm>
#include <string>
#include <string_view>

#include "reader_subscriber.h"
#include "statement_factory.h"

namespace griha {

namespace {

using namespace std::string_view_literals;

constexpr auto c_explicit_block_begin   = "{"sv;
constexpr auto c_explicit_block_end     = "}"sv;

bool is_block_begin(std::string_view line) {
    using namespace std;
    return line == c_explicit_block_begin;
}

bool is_block_end(std::string_view line) {
    using namespace std;
    return line == c_explicit_block_end;
}

} // unnamed namespace

struct ReaderState : std::enable_shared_from_this<ReaderState> {
    virtual ~ReaderState() {}
    virtual void process(std::string) = 0;
    virtual void on_eof() {}
};
using ReaderStatePtr = std::shared_ptr<ReaderState>;

struct ReaderImpl {

    inline ReaderImpl(size_t bsize)
        : state(nullptr)
        , block_size(bsize) {}

    ReaderStatePtr state;
    const size_t block_size;

    std::vector<ReaderSubscriberPtr> subscribers;

    StatementFactory statement_factory;
    StatementContainer statements;

    Reader::Metrics metrics;

    template <typename State> State& change_state(); 

    void parse(std::string line);

    void process(std::string line);
    void on_eof();

    void notify_block();
    void notify_unexpected_eof();
};

struct InitialState : ReaderState {
    inline explicit InitialState(ReaderImpl& r_impl)
        : reader_impl(r_impl) {}

    void process(std::string line) override;
    void on_eof() override;

    ReaderImpl& reader_impl;
    size_t count {};
};

struct BlockState : ReaderState {
    inline BlockState(ReaderImpl& r_impl) 
        : reader_impl(r_impl) {}

    void process(std::string line) override;
    void on_eof() override;

    ReaderImpl& reader_impl;
    size_t level { 1 };
};

struct ErrorState : ReaderState {
    inline ErrorState(ReaderImpl& r_impl)
        : reader_impl(r_impl) {}

    void process(std::string line) override;

    ReaderImpl& reader_impl;
    std::string error;
};

template <typename State>
State& ReaderImpl::change_state() {
    // for further optimization it looks pretty to create states pool
    state = std::make_shared<State>(*this);
    return dynamic_cast<State&>(*state);
} 

void ReaderImpl::parse(std::string line) {
    ++metrics.nstatements;
    statements.push_back(statement_factory.create(std::move(line)));
}

void ReaderImpl::process(std::string line) {
    ++metrics.nlines;
    auto save_state_ptr = state->shared_from_this(); // protect against unexpected deletion
    return state->process(std::move(line));
}

void ReaderImpl::on_eof() {
    state->on_eof();
}

void ReaderImpl::notify_block() {
    if (statements.empty())
        return; // empty block doesn't require notification

    ++metrics.nblocks;

    for (auto& subscriber : subscribers)
        subscriber->on_block(statements);
    statements.clear();
}

void ReaderImpl::notify_unexpected_eof() {
    if (statements.empty())
        return; // empty block doesn't require notification

    for (auto& subscriber : subscribers)
        subscriber->on_unexpected_eof(statements);
}

void InitialState::process(std::string line) {
    using namespace std;

    if (is_block_end(line)) {
        reader_impl.change_state<ErrorState>().error = "unexpected end of block"s;
    } else if (is_block_begin(line)) {
        // in initial state start of explicit block triggers end of block
        reader_impl.notify_block();
        reader_impl.change_state<BlockState>();
    } else {
        reader_impl.parse(std::move(line));
        if (++count == reader_impl.block_size) {
            // fixed block size has been reached
            reader_impl.notify_block();
            count = 0;
        }
    }
}

void InitialState::on_eof() {
    reader_impl.notify_block();
}

void BlockState::process(std::string line) {
    using namespace std;

    if (is_block_begin(line)) {
        // nested explicit blocks are ignored but correction of syntax is required
        ++level;
    } else if (is_block_end(line)) {
        if (--level == 0) {
            // explicit block has been ended
            // block has statements - notify about end of block
            reader_impl.notify_block();
            reader_impl.change_state<InitialState>();
        }
    } else {
        reader_impl.parse(std::move(line));
    }
}

void BlockState::on_eof() {
    reader_impl.notify_unexpected_eof();
}

void ErrorState::process([[maybe_unused]] std::string line) {
    // do nothing
}

Reader::Reader(size_t block_size) 
    : priv_(std::make_unique<ReaderImpl>(block_size)) {
    priv_->change_state<InitialState>();
}

Reader::~Reader() = default;
Reader::Reader(Reader&&) = default;
Reader& Reader::operator= (Reader&&) = default;

void Reader::subscribe(ReaderSubscriberPtr subscriber) {
    auto& subscribers = priv_->subscribers;
    auto it = std::find(subscribers.begin(), subscribers.end(), subscriber);
    if (it == subscribers.end())
        subscribers.push_back(std::move(subscriber));
}

void Reader::consume(std::string line) {
    priv_->process(std::move(line));
}

auto Reader::get_metrics() const -> const Metrics& {
    return priv_->metrics;
}

void Reader::on_eof() {
    priv_->on_eof();
}

} // namespace griha