#include "async.h"

#include <mutex>
#include <future>
#include <map>

#include "interpreter.h"
#include "logger.h"

using namespace griha;

namespace async {

constexpr auto nthreads_per_connection = 2u;

struct ConnectionsHandler {
    Logger logger;
    uintptr_t next_id {};
    std::map<uintptr_t, InterpreterPtr> connections;
    std::mutex guard;
};
ConnectionsHandler g_conn_handler;

handle_t connect(std::size_t bulk) {
    Interpreter::Context context { g_conn_handler.logger, bulk, nthreads_per_connection };

    std::lock_guard l { g_conn_handler.guard };
    uintptr_t id = g_conn_handler.next_id++;
    g_conn_handler.connections.emplace(id, std::make_shared<Interpreter>(context, std::to_string(id)));
    return reinterpret_cast<handle_t>(id);
}

void receive(handle_t handle, const char *data, std::size_t size) {
    auto id = reinterpret_cast<uintptr_t>(handle);

    InterpreterPtr intrp;
    {
        std::lock_guard l { g_conn_handler.guard };
        if (g_conn_handler.connections.count(id) == 0)
            return;
        intrp = g_conn_handler.connections[id];
    }

    intrp->consume(std::string_view { data, size });
}

void disconnect(handle_t handle) {
    auto id = reinterpret_cast<uintptr_t>(handle);
    
    InterpreterPtr intrp;
    {
        std::lock_guard l { g_conn_handler.guard };
        if (g_conn_handler.connections.count(id) == 0)
            return;
        intrp = g_conn_handler.connections[id];
        g_conn_handler.connections.erase(id);
    }

    intrp->stop_and_log_metrics();
}

}
