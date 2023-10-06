// NetworkManager.cpp

#include "NetworkManager.h"
#include <iostream>
#include <thread>

namespace Net {

NetworkManager::NetworkManager(std::string server_ip, int student_id,
                               std::string team_name, int server_port,
                               int client_port,
                               void (*error_handle)(int, std::string))
    : server_ip(server_ip), student_id(student_id), team_name(team_name),
      server_port(server_port), client_port(client_port),
      error_handle(error_handle),
      zmq_client("tcp://" + server_ip + ":" + std::to_string(server_port)) {
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
  // 注册视觉程序的实现
  SendStruct send_message;
  send_message.yaw = 0.0; // 设置具体的注册信息
  send_message.pitch = 0.0;
  send_message.time_stamp = cnt_time;
  // 其他字段也可以设置

  std::string json_message = JSONSerializer::serialize(send_message);
  zmq_client.send(json_message);

  // 等待注册成功的响应
  std::string response = zmq_client.receive();
  if (response == "Registration successful") {
    on_register = true;
    return true;
  } else {
    if (error_handle) {
      error_handle(1, "Registration failed: " + response);
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

  std::string json_message = JSONSerializer::serialize(send_message);
  zmq_client.send(json_message);

  // 等待下线成功的响应
  std::string response = zmq_client.receive();
  if (response == "Offline successful") {
    Close();
    return true;
  } else {
    if (error_handle) {
      error_handle(5, "Offline failed: " + response);
    }
    return false;
  }
}

RecvStruct NetworkManager::getNewestRecvMessage() {
  std::lock_guard<std::mutex> lock(handler_mutex);
  return newest_recv_message;
}

void NetworkManager::recv_handler() {
  while (is_running) {
    std::string json_message = zmq_client.receive();
    // 将收到的 JSON 消息反序列化为 RecvStruct
    RecvStruct recv_message = JSONSerializer::deserialize(json_message);

    std::lock_guard<std::mutex> lock(handler_mutex);
    newest_recv_message = recv_message;
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

} // namespace Net
