#include "NetworkManager.h"
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
bool reg = false;

void sigint_handler(int sig) { exit(1); }

int main() {
  signal(SIGINT, sigint_handler);

  // 创建 NetworkManager 对象
  network::NetworkManager net("127.0.0.1", 20205654, "梗小姐赢麻了", 5558,
                              5559);

  // 注册视觉程序
  reg = net.registerUser(0, 100000);



  if (!reg) {
    std::cout << "Register Failed, Exit" << std::endl;
    exit(1);
  }

  std::cout << "Register Success! the client will start at 2 second" << std::endl;

  sleep(2);

  auto time_system_start = std::chrono::system_clock::now();

  auto time_temp = std::chrono::system_clock::now();

  while (true) {


    cv::Mat img;

    auto message = net.getLatestRecvMessage();

    auto time_now = std::chrono::system_clock::now();


    if (!message.img.empty()) {
      std::cout<<"接受到消息！！！"<<std::endl;
      /*******************************************************************************/

      /*                               YOUR CODE STARTS */

      /*******************************************************************************/

      int fire = 0;
      // 发送控制信息
      //net.sendControlMessage(network::SendStruct(0.0, 0.0, fire, 0, 1280, 720));
      cv::imshow("img", message.img);
      cv::waitKey(10);

      /*******************************************************************************/
      /*                                YOUR CODE ENDS */
      /*******************************************************************************/

      /*!
       * example!
       * 这里设置为每秒旋转360度并且，一秒发射一发子弹
       */


      float  rotaion_speed = 1;

      auto time_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_now-time_temp);

      time_temp=time_now;

      int shoot_rate=1;

      std::cout<<"time "<<message.pitch<<std::endl;


      net.sendControlMessage(network::SendStruct(message.yaw+(float)time_duration.count()/1000*360, 0.0, fire, 0, 1280, 720));




    } else {
      std::cout << "Get an empty image~" << std::endl;

    }
  }
  return 0;
}
