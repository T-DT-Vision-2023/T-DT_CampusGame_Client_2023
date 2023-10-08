#include "NetworkManager.h"
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

bool reg = false;

void error_handle(int error_id, std::string message);

network::NetworkManager net("127.0.0.1", 20205654, "梗小姐赢麻了", 25562, 25564,
                            error_handle);

void sigint_handler(int sig) { exit(1); }

int main() {
  signal(SIGINT, sigint_handler);

  // 创建 NetworkManager 对象
  network::NetworkManager net("127.0.0.1", 20205654, "梗小姐赢麻了", 5555,
                              25564, error_handle);

  // 注册视觉程序
  reg = net.registerUser(0);
  while (!reg) {
    std::cout << "Register failed, retrying..." << std::endl;
    reg = net.registerUser(0);
  }
  std::cout << "Register success" << std::endl;

  while (true) {
    while (!reg) {
      std::cout << "Register failed, retrying..." << std::endl;
      reg = net.registerUser(0);
    }

    cv::Mat img;
    auto message = net.getLatestRecvMessage();

    if (!message.img.empty()) {
      //////////////////////////////////////////////////////////////////////
      ///在下面补充你的代码
      /////////////////////////////////////////////////////////////////////////
      int fire = 0;

      // 发送控制信息
      net.sendControlMessage(network::SendStruct(0.0, 0.0, fire, 0, 1280, 720));

      cv::imshow("img", message.img);

      //////////////////////////////////////////////////////////////////////
      ///在上面补充你的代码
      /////////////////////////////////////////////////////////////////////////
    } else {
      std::cout << "Get an empty image" << std::endl;
      cv::waitKey(100);
    }
  }
  return 0;
}

// 错误处理函数
void error_handle(int error_id, std::string message) {
  if (error_id == 1 || error_id == 5) {
    reg = false; // 注册失败时设置为false
  }
  std::cout << "Error: " << error_id << " " << message << std::endl;
}
