#include "NetworkManager.h"
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <utility>

namespace network {

NetworkManager::NetworkManager(const std::string &server_ip, int student_id,
                               std::string team_name, int server_port,
                               int client_port)
    : student_id(student_id), team_name(std::move(team_name)),
      server_address("tcp://" + server_ip + ":" + std::to_string(server_port)),
      zmq_client(server_address) {}

NetworkManager::~NetworkManager() { Close(); }

bool NetworkManager::registerUser(double cnt_time, int timeout) {
  int elapsed_time = 0;

  while (elapsed_time < timeout) {
    try {
      zmq_client.connectSend(server_address);

      std::string header_str = "msg";
      zmq::message_t header(header_str.c_str(), header_str.size());

      nlohmann::json json_message;
      json_message["type"] = "register";
      json_message["info"] = team_name;
      json_message["id"] = student_id;
      json_message["time"] = cnt_time;
      json_message["pwd"] = "T-DT ALGORITHM 2024";
      json_message["version"] = 1.2;

      std::string json_str = json_message.dump();
      zmq::message_t message(json_str.c_str(), json_str.size());

      zmq_client.send(header, message);

      std::lock_guard<std::mutex> lock(handler_mutex);
      if (on_register) {
        startHeartbeatTimer();
        handler_thread = std::thread(&NetworkManager::recvHandler, this);
        return true;
      }

      break;
    } catch (const zmq::error_t &e) {
      std::cout << "ZMQ Error: " << e.what() << ", retrying..." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    elapsed_time += 1000;
  }

  std::cout << "Connection attempt timed out." << std::endl;
  return false;
}

bool NetworkManager::offlineUser(int timeout) {
  std::string header_str = "msg";
  zmq::message_t header(header_str.c_str(), header_str.size());

  nlohmann::json json_message;
  json_message["type"] = "offline";

  std::string json_str = json_message.dump();
  zmq::message_t message(json_str.c_str(), json_str.size());

  zmq_client.send(header, message);

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
  zmq_client.send(header, message);
}

void NetworkManager::sendPulse() {
  std::string header_str = "msg";
  zmq::message_t header(header_str.c_str(), header_str.size());
  nlohmann::json json_message;
  json_message["type"] = "pulse";
  std::string json_str = json_message.dump();
  zmq::message_t message(json_str.c_str(), json_str.size());

  zmq_client.send(header, message);
}

void NetworkManager::startHeartbeatTimer() {
  std::thread([this]() {
    while (true) {
      sendPulse();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }).detach();
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
      zmq_client.connectRecv(server_address);
      break;
    } catch (const zmq::error_t &e) {
      std::cout << "ZMQ Error: " << e.what() << ", retrying..." << std::endl;
    }
  }
  while (true) {
    try {
      zmq::message_t header;
      try {
        zmq_client.recv(header);
      } catch (const zmq::error_t &e) {
        std::cout << "ZMQ Error: " << e.what() << ", retrying..." << std::endl;
        continue;
      }
      std::string header_str(static_cast<char *>(header.data()), header.size());

      zmq::message_t message;
      try {
        zmq_client.recv(message);
      } catch (const zmq::error_t &e) {
        std::cout << "ZMQ Error: " << e.what() << ", retrying..." << std::endl;
        continue;
      }
      std::string json_message(static_cast<char *>(message.data()),
                               message.size());
      nlohmann::json parsed_message = nlohmann::json::parse(json_message);

      std::lock_guard<std::mutex> lock(handler_mutex);

      if (header_str == "msg") {
        if (parsed_message["type"] == "register success") {
          on_register = 1;
        } else if (parsed_message["type"] == "offline success") {
          recv_close = 1;
        }
      } else if (header_str == "data") {
        latest_recv_message = parsed_message;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));

    } catch (const zmq::error_t &e) {
      std::cout << "ZMQ Error: " << e.what() << std::endl;
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
