#include "NetworkManager.h"
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

bool reg = false;

void error_handle(int error_id, std::string message);

Net::NetworkManager net("127.0.0.1", 20205654, "梗小姐赢麻了", 25562, 25564,
                        error_handle);

void sigint_handler(int sig) {
  // 退出程序
  exit(1);
}

int main() {
  signal(SIGINT, sigint_handler);

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

    // 获取最新的图像数据
    cv::Mat img;
    auto message = net.getNewestRecvMessage();

    if (!message.img.empty()) {
      // 在这里添加图像处理逻辑
      // 根据图像内容来修改 fire 的值
      // 例如，检测敌人并决定是否开火
      int fire = 0;

      // 发送控制信息
      net.sendControlMessage(
          Net::SendStruct(0.0, 0.0, fire, 0, 20.0, 0, 0.0, 0.0, 1280, 720));

      // 显示图像
      cv::imshow("img", message.img);
    } else {
      std::cout << "Get an empty image" << std::endl;
      cv::waitKey(100);
    }

    // net.sendPulse();
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
