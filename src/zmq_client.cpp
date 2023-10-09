#include "tools/network/zmq_client.h"

namespace tools {
namespace network {

ZMQClient::ZMQClient(const std::string &server_address)
    : context(1), socket(context, ZMQ_REQ) {}

ZMQClient::~ZMQClient() { socket.close(); }

void ZMQClient::connect(const std::string &server_address) {
  socket.connect(server_address);
}

void ZMQClient::send(zmq::message_t &message) {
  socket.send(message, zmq::send_flags::none);
}

void ZMQClient::send(zmq::message_t &header, zmq::message_t &message) {
  socket.send(header, zmq::send_flags::sndmore);
  socket.send(message, zmq::send_flags::none);
}

void ZMQClient::recv(zmq::message_t &message) {
  socket.recv(message, zmq::recv_flags::none);
}

} // namespace network
} // namespace tools
