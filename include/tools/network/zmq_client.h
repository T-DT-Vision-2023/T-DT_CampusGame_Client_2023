#ifndef ZMQ_CLIENT_H
#define ZMQ_CLIENT_H

#include <zmq.hpp>
#include <string>

namespace tools {
namespace network {

class ZMQClient {
public:
    ZMQClient(const std::string& server_address);
    ~ZMQClient();

    void send(const std::string& message);
    std::string receive();

private:
    zmq::context_t context;
    zmq::socket_t socket;
};

} // namespace network
} // namespace tools

#endif
