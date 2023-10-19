#include "NetworkManager.h"
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <utility>
#include <unistd.h>



namespace network {

std::vector<uchar> base64_decode(const std::string& encoded_data)
{
  // Replace this with your Base64 class instance
  tools::Base64 base64;
  char* decoded_data = base64.decode(encoded_data, encoded_data.length());

  // Convert char array to vector
  std::vector<uchar> result(decoded_data, decoded_data + encoded_data.length());

  // Free the allocated memory
//  free(decoded_data);

  return result;
}

NetworkManager::NetworkManager(const std::string &server_ip, int student_id,
                               std::string team_name, int server_port,
                               int client_port)

{



  student_id=student_id;

  team_name=team_name;

  server_address="tcp://" + server_ip + ":" + std::to_string(server_port);

  std::cout<<"尝试链接服务器地址"<<server_address<<std::endl;


  this->context= new zmq::context_t(1);

  this->send_socket = new zmq::socket_t(*context,ZMQ_PUSH);

  this->recv_socket = new zmq::socket_t(*context,ZMQ_PULL);

  this->send_socket->connect(server_address);

  this->recv_socket->bind("tcp://*:"+std::to_string(client_port));


}


NetworkManager::~NetworkManager() {
//  Close();
}

bool NetworkManager::registerUser(float cnt_time, int timeout) {


    nlohmann::json json_message;
    json_message["type"] = "register";
    json_message["info"] = team_name;
    json_message["id"] = student_id;
    json_message["time"] = cnt_time;
    json_message["pwd"] = "T-DT ALGORITHM 2024";
    json_message["version"] = 1.2;


    nlohmann::json regist_success_json_message;
    regist_success_json_message["type"] = "regist success!";
    regist_success_json_message["info"] = team_name;
    regist_success_json_message["id"] = student_id;
    regist_success_json_message["time"] = cnt_time;
    regist_success_json_message["pwd"] = "T-DT ALGORITHM 2024";
    regist_success_json_message["version"] = 1.2;

    std::string json_str = json_message.dump();
    std::string  success=regist_success_json_message.dump();
    zmq::message_t message(json_str.c_str(), json_str.size());
    zmq::message_t recv_message;
    zmq::message_t temp_message;

    send_socket->send(message,zmq::send_flags::none);

  /*
   *  需要一个确认是否成功链接的机制
   *
   *  等待添加接受代码
   */
    std::cout<<"等待一段时间接受注册消息，防止出现遗漏 "<<std::endl;

    sleep(1);

    this->get__latest_raw_message();

    //我应该有一个获取最新帧的代码

    std::string recv_string =  std::string((char *)this->latest_recv_raw_message->data());

    if (recv_string=="regist success!"){

      std::cout<<"注册成功"<<std::endl;


      zmq::message_t success_message(success.c_str(), success.size());

      this->send_socket->send(success_message,zmq::send_flags::none);

      return true;

    }

  std::cout<<"注册失败，请检查网络ip和端口，以及端口占用等网络情况"<<std::endl;
  return false;

}


RecvStruct NetworkManager::getLatestRecvMessage() {


  RecvStruct ret;

  if(get__latest_raw_message()){

      nlohmann::json  json_message;

      std::string recv_string =  std::string(static_cast<char*>(this->latest_recv_raw_message->data()),
                                            this->latest_recv_raw_message->size());
      /*
       * struct RecvStruct {
        double  yaw;                  // 当前相机欧拉角的yaw值
        double  pitch;                // 当前相机欧拉角的pitch值
        cv::Mat img;                  // 当前视觉相机图像
        double  time_stamp;           // 当前帧信息对应时间戳 单位s
        double  enemy_hp;             // 敌方当前血量
        double  my_hp;                // 我方当前血量
        int     rest_bullets;         // 当前比赛剩余弹量
        int     rest_time;            // 当前比赛剩余时间 单位s
        int     buff_over_time;       // 当前触发大符已用时间 单位s
      };
       */
      /*!
       * 这里我认为所有的都是json，然后从末尾删除，直到遇到"}"
       */

      std::cout<<recv_string.end().operator--().base()<<std::endl;

      auto iter =recv_string.end().operator--();

      while ((int)*iter.base()!=(int)'}'){
        iter.operator--();
        std::cout<<iter.base()<<std::endl;
      }

      iter.operator++();
      recv_string.erase(iter,recv_string.end());


      try {

        json_message = nlohmann::json::parse(recv_string);

      } catch (nlohmann::detail::parse_error) {

        std::cout<<"当前帧解析错误："<<recv_string<<std::endl;

        return ret;
      }

      ret.yaw=json_message["yaw"];
      ret.pitch=json_message["pitch"];
      ret.time_stamp=json_message["time_stamp"];
      ret.enemy_hp=json_message["enemy_hp"];
      ret.my_hp=json_message["my_hp"];
      ret.rest_bullets=json_message["rest_bullets"];
      ret.buff_over_time=json_message["buff_over_time"];
      ret.rest_time=json_message["rest_time"];


      std::string temp =json_message["img"];

;
//      temp.erase(temp.begin(),temp.begin().operator++());
//      temp.erase(temp.end().operator--(),temp.end());




      std::vector<uchar> data= base64_decode(temp);

      if (json_message["hasimg"]){

        ret.img =   cv::imdecode(data,cv::IMREAD_UNCHANGED);
      }




      latest_recv_message=ret;

      return ret;


  } else{

  return this->latest_recv_message;

  }


}

bool NetworkManager::get__latest_raw_message() {

  int counter =0;

  while (true){

      zmq::message_t *temp = new zmq::message_t();

      zmq::recv_result_t  reult = recv_socket->recv(*temp,zmq::recv_flags::dontwait);

//      std::cout<<reult.has_value()<<std::endl;

      if (!reult.has_value()){

        break;

      }else{
//        std::cout<<"尝试改变最新值"<<std::endl;
//        std::cout<<"temp地址"<<temp<<std::endl;
//        std::cout<<"latest_recv_raw_message地址"<<latest_recv_raw_message<<std::endl;
        delete(this->latest_recv_raw_message);
        this->latest_recv_raw_message=temp;

        counter++;
      }

//      std::cout<<reult.value()<<std::endl;

      std::string recv_string =  std::string((char *)this->latest_recv_raw_message->data());


//      std::cout<<"接受到的消息"<<recv_string<<std::endl;
  }

  if (counter>0) {
      return true;
  } else {
      return false;
  }


}


void NetworkManager::sendControlMessage(const SendStruct &send_message) {




  nlohmann::json json_message = send_message;

  json_message["type"] = "control";
//
  std::cout<<"尝试发送控制消息"<<json_message<<std::endl;

  std::string json_str = json_message.dump();

  zmq::message_t message(json_str.c_str(), json_str.size());

  this->send_socket->send(message);


}


//bool NetworkManager::offlineUser(int timeout) {
//  std::string header_str = "msg";
//  zmq::message_t header(header_str.c_str(), header_str.size());
//
//  nlohmann::json json_message;
//  json_message["type"] = "offline";
//
//  std::string json_str = json_message.dump();
//  zmq::message_t message(json_str.c_str(), json_str.size());
//
//  zmq_client.send(header, message);
//
//  int elapsed_time = 0;
//  while (elapsed_time < timeout) {
//    std::lock_guard<std::mutex> lock(handler_mutex);
//    if (recv_close) {
//      recv_close = 0;
//      return true;
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    elapsed_time += 10;
//  }
//  return false;
//}
//



//
//void NetworkManager::sendPulse() {
//  std::string header_str = "msg";
//  zmq::message_t header(header_str.c_str(), header_str.size());
//  nlohmann::json json_message;
//  json_message["type"] = "pulse";
//  std::string json_str = json_message.dump();
//  zmq::message_t message(json_str.c_str(), json_str.size());
//
//  zmq_client.send(header, message);
//}
//
//void NetworkManager::startHeartbeatTimer() {
//  std::thread([this]() {
//    while (true) {
//      sendPulse();
//      std::this_thread::sleep_for(std::chrono::seconds(1));
//    }
//  }).detach();
//}
//
//bool NetworkManager::getGameStatus() {
//  bool ret;
//  handler_mutex.lock();
//  ret = on_game;
//  handler_mutex.unlock();
//  return ret;
//}
//
//void NetworkManager::recvHandler() {
//  while (true) {
//    try {
//      zmq_client.connectRecv(server_address);
//      break;
//    } catch (const zmq::error_t &e) {
//      std::cout << "ZMQ Error in connecting recv server: " << e.what()
//                << ", retrying..." << std::endl;
//    }
//  }
//  while (true) {
//    zmq::message_t header;
//    try {
//      zmq_client.recv(header);
//    } catch (const zmq::error_t &e) {
//      std::cout << "ZMQ Error in receiving header: " << e.what()
//                << ", retrying..." << std::endl;
//
//      continue;
//    }
//    std::string header_str(static_cast<char *>(header.data()), header.size());
//
//    zmq::message_t message;
//    try {
//      zmq_client.recv(message);
//    } catch (const zmq::error_t &e) {
//      std::cout << "ZMQ Error in receiving message: " << e.what()
//                << ", retrying..." << std::endl;
//      continue;
//    }
//    std::string json_message(static_cast<char *>(message.data()),
//                             message.size());
//    nlohmann::json parsed_message = nlohmann::json::parse(json_message);
//
//    std::lock_guard<std::mutex> lock(handler_mutex);
//
//    if (header_str == "msg") {
//      if (parsed_message["type"] == "register success") {
//        on_register = true;
//      } else if (parsed_message["type"] == "offline success") {
//        recv_close = true;
//      }
//    } else if (header_str == "data") {
//      latest_recv_message = parsed_message;
//    }
//  }
//}

//void NetworkManager::Close() {
//  if (!close) {
//    offlineUser(100);
//    //    zmq_client->Close();
//    close = 1;
//  } else {
//    std::cout << "[Error] NetworkManager has been closed." << std::endl;
//  }
//}

} // namespace network
