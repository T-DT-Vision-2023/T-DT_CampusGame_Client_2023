#include "NetworkManager.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

namespace network {

NetworkManager::NetworkManager(std::string server_ip, int student_id,
                               std::string team_name, int server_port,
                               int client_port,
                               void (*error_handle)(int, std::string))
    : server_ip(server_ip), student_id(student_id), team_name(team_name),
      server_port(server_port), client_port(client_port),
      error_handle(error_handle), context(1), zmq_client(context, ZMQ_REQ) {
  // 构造函数的实现
  // 初始化成员变量
  // 创建 ZeroMQ 客户端
  // 启动通信线程
  handler_thread = std::thread(&NetworkManager::recv_handler, this);
}

NetworkManager::~NetworkManager() {
  // 析构函数的实现
  Close();
}

bool NetworkManager::registerUser(double cnt_time, int timeout) {
  SendStruct send_message;
  send_message.yaw = 0.0;
  send_message.pitch = 0.0;
  send_message.time_stamp = cnt_time;

  nlohmann::json json_message = send_message;

  std::string json_str = json_message.dump();

  // 发送JSON字符串
  zmq::message_t message(json_str.c_str(), json_str.size());
  zmq_client.send(message);

  // 等待注册成功的响应
  zmq::message_t response;
  zmq_client.recv(&response);
  std::string response_str(static_cast<char *>(response.data()),
                           response.size());

  if (response_str == "Registration successful") {
    on_register = true;
    return true;
  } else {
    if (error_handle) {
      error_handle(1, "Registration failed: " + response_str);
    }
    return false;
  }
}

bool NetworkManager::offlineUser(int timeout) {
  // 下线视觉程序的实现
  if (!on_register) {
    if (error_handle) {
      error_handle(5, "User not registered");
    }
    return false;
  }

  SendStruct send_message;
  // 设置下线信息

  std::string json_message = nlohmann::json(send_message).dump();
  zmq::message_t message(json_message.c_str(), json_message.size());
  zmq_client.send(message);

  // 等待下线成功的响应
  zmq::message_t response;
  zmq_client.recv(&response);
  std::string response_str(static_cast<char *>(response.data()),
                           response.size());

  if (response_str == "Offline successful") {
    Close();
    return true;
  } else {
    if (error_handle) {
      error_handle(5, "Offline failed: " + response_str);
    }
    return false;
  }
}

RecvStruct NetworkManager::getLatestRecvMessage() {
  std::lock_guard<std::mutex> lock(handler_mutex);
  return latest_recv_message;
}

void NetworkManager::recv_handler() {
  while (is_running) {
    zmq::message_t response;
    zmq_client.recv(&response);
    std::string json_message(static_cast<char *>(response.data()),
                             response.size());

    auto temp_struct = nlohmann::json::parse(json_message);

    std::lock_guard<std::mutex> lock(handler_mutex);
    latest_recv_message = temp_struct;
  }
}

void NetworkManager::Close() {
  if (is_running) {
    is_running = false;
    if (handler_thread.joinable()) {
      handler_thread.join();
    }
  }

  if (on_register) {
    // 下线用户
    offlineUser();
  }
}

} // namespace network
