#include "tools/network/zmq_client.h"

namespace tools {
namespace network {

ZMQClient::ZMQClient(const std::string &server_address)
    : context(1), socket(context, ZMQ_REQ) {
  socket.connect(server_address);
}

ZMQClient::~ZMQClient() { socket.close(); }

void ZMQClient::send(const std::string &message) {
  zmq::message_t zmq_message(message.size());
  memcpy(zmq_message.data(), message.data(), message.size());
  socket.send(zmq_message, zmq::send_flags::none);
}

std::string ZMQClient::receive() {
  zmq::message_t zmq_message;
  socket.recv(zmq_message, zmq::recv_flags::none);
  return std::string(static_cast<char *>(zmq_message.data()),
                     zmq_message.size());
}

} // namespace network
} // namespace tools
