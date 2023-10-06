/*************************
 * @note 请不要直接使用本头文件内的资源
 * @note 请通过NetworkManager.h中的NetworkManager类来使用本头文件内的资源
 */
#ifndef ZMQ_CLIENT_H
#define ZMQ_CLIENT_H

#include <zmq.hpp>
#include <string>

namespace tools
{
namespace network
{

class ZMQClient
{
public:
  ZMQClient(const std::string& server_address);
  ~ZMQClient();

  void send(const std::string& message);
  std::string receive();

private:
  zmq::context_t context;
  zmq::socket_t socket;
};

}  // namespace network
}  // namespace tools

#endif
