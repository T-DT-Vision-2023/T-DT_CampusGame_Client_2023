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
      error_handle(error_handle) {
  // 构造函数的实现
  // 初始化成员变量
  // 创建 ZeroMQ 客户端
  // 启动通信线程
  handler_thread = std::thread(&NetworkManager::recvHandler, this);
}

NetworkManager::~NetworkManager() {
  // 析构函数的实现
  Close();
}

bool NetworkManager::registerUser(double cnt_time, int timeout) {

  std::string header_str = "msg";
  zmq::message_t header(header_str.c_str(), header_str.size());

  nlohmann::json json_message;
  json_message["type"] = "register";
  json_message["info"] = team_name;
  json_message["id"] = student_id;
  json_message["time"] = cnt_time;
  json_message["pwd"] = "T-DT ALGORITHM 2023";
  json_message["version"] = 1.1;

  std::string json_str = json_message.dump();
  zmq::message_t message(json_str.c_str(), json_str.size());

  zmq_client.send(header, zmq::send_flags::sndmore);
  zmq_client.send(message, zmq::send_flags::none);

  int elapsed_time = 0;
  while (elapsed_time < timeout) {
    std::lock_guard<std::mutex> lock(handler_mutex);
    if (on_register) {
      on_register = 0;
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    elapsed_time += 10;
  }
  return false;
}

bool NetworkManager::offlineUser(int timeout) {
  std::string header_str = "msg";
  zmq::message_t header(header_str.c_str(), header_str.size());

  nlohmann::json json_message;
  json_message["type"] = "offline";

  std::string json_str = json_message.dump();
  zmq::message_t message(json_str.c_str(), json_str.size());

  zmq_client.send(header, zmq::send_flags::sndmore);
  zmq_client.send(message, zmq::send_flags::none);

  int elapsed_time = 0;
  while (elapsed_time < timeout) {
    std::lock_guard<std::mutex> lock(handler_mutex);
    if (recv_close) {
      recv_close = 0;
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    elapsed_time += 10;
  }
  return false;
}

RecvStruct NetworkManager::getLatestRecvMessage() {
  RecvStruct ret;
  handler_mutex.lock();
  ret = latest_recv_message;
  handler_mutex.unlock();
  return ret;
}

void NetworkManager::sendControlMessage(const SendStruct &send_message) {
  std::string header_str = "data";
  zmq::message_t header(header_str.c_str(), header_str.size());

  nlohmann::json json_message = send_message;

  std::string json_str = json_message.dump();
  zmq::message_t message(json_str.c_str(), json_str.size());

  zmq_client.send(header, zmq::send_flags::sndmore);
  zmq_client.send(message, zmq::send_flags::none);
}

void NetworkManager::sendPulse() {
  std::string message_str = "pulse";
  zmq::message_t message(message_str.c_str(), message_str.size());
  zmq_client.send(message, zmq::send_flags::none);
}

bool NetworkManager::getGameStatus() {
  bool ret;
  handler_mutex.lock();
  ret = on_game;
  handler_mutex.unlock();
  return ret;
}

void NetworkManager::recvHandler() {
  while (true) {
    try {
      zmq::message_t header;
      auto ok = zmq_client.recv(header, zmq::recv_flags::none);
      if (!ok)
        continue;
      std::string header_str(static_cast<char *>(header.data()), header.size());

      zmq::message_t message;
      ok = zmq_client.recv(message, zmq::recv_flags::none);
      if (!ok)
        continue;
      std::string json_message(static_cast<char *>(message.data()),
                               message.size());
      nlohmann::json parsed_message = nlohmann::json::parse(json_message);

      std::lock_guard<std::mutex> lock(handler_mutex);

      if (header_str == "msg") {
        if (parsed_message["type"] == "register success") {
          on_register = 1;
        } else if (parsed_message["msg"] == "offline success") {
          recv_close = 1;
        }
      } else if (header_str == "data") {
        latest_recv_message = parsed_message;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));

    } catch (const zmq::error_t &e) {
      if (error_handle) {
        error_handle(-1, e.what());
      }
    }
  }
}

void NetworkManager::Close() {
  if (!close) {
    offlineUser(100);
    //    zmq_client->Close();
    close = 1;
  } else {
    std::cout << "[Error] NetworkManager has been closed." << std::endl;
  }
}

} // namespace network
