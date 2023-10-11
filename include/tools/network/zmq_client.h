/*************************
* @note 请不要直接使用本头文件内的资源
* @note 请通过NetworkManager.h中的NetworkManager类来使用本头文件内的资源
*/
#ifndef ZMQ_CLIENT_H
#define ZMQ_CLIENT_H

#include <string>
#include <zmq.hpp>

namespace tools {
namespace network {

class ZMQClient {
public:
 ZMQClient(const std::string &server_address);
 ~ZMQClient();

 void connectSend(const std::string &server_address);
 void connectRecv(const std::string &server_address);
 void send(zmq::message_t &message);
 void send(zmq::message_t &header, zmq::message_t &message);
 void recv(zmq::message_t &message);

private:
 zmq::context_t context;
 zmq::socket_t send_socket;
 zmq::socket_t recv_socket;
};

} // namespace network
} // namespace tools

#endif