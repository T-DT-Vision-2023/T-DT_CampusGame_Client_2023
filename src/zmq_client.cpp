#include "tools/network/zmq_client.h"

namespace tools {
namespace network {

ZMQClient::ZMQClient(const std::string &server_address)
    : context(1), send_socket(context, ZMQ_PUSH),
      recv_socket(context, ZMQ_PULL) {
  int highWaterMark = 1000;
  send_socket.set(zmq::sockopt::sndhwm, highWaterMark);
  recv_socket.set(zmq::sockopt::rcvhwm, highWaterMark);
}

ZMQClient::~ZMQClient() {
  send_socket.close();
  recv_socket.close();
}

void ZMQClient::connectSend(const std::string &server_address) {
  send_socket.connect(server_address);
}

void ZMQClient::connectRecv(const std::string &server_address) {
  recv_socket.connect(server_address);
}

void ZMQClient::send(zmq::message_t &message) {
  send_socket.send(message, zmq::send_flags::none);
}

void ZMQClient::send(zmq::message_t &header, zmq::message_t &message) {
  send_socket.send(header, zmq::send_flags::sndmore);
  send_socket.send(message, zmq::send_flags::none);
}

void ZMQClient::recv(zmq::message_t &message) {
  recv_socket.recv(message, zmq::recv_flags::none);
}

} // namespace network
} // namespace tools