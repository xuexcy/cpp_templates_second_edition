#include "server.h"

#include "thirdparty/message_queue/message_queue.h"

bool Server::Start() {
    bool res{true};
    res |= MESSAGE_QUEUE_SEND("server_name", "attach_by_initialization");
    return res;
}
