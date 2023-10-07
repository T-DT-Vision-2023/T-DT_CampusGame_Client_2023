/*******************
 * @name NetworkManager
 * @author T-DT Algorithm 2024 黄子墨
 * @version 1.0
 * @date 2023-10-6
 * @brief 用于与unity服务端通信
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include "tools.h"
#include "tools/core/base64.h"
#include "tools/network/json_serializer.h"
#include "tools/network/zmq_client.h"
#include <mutex>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <vector>

namespace Net {
// clang-format off
struct RecvStruct {
  double yaw;                   // 目标位置欧拉角yaw值
  double pitch;                 // 目标位置欧拉角pitch值
  int    shoot;                 // 开火状态 0->不开火 1->1的范围内一直开火 2->跳变到2开火
  double time_stamp;            // 当前视觉程序时间戳,用于时间同步,默认-1不进行同步 单位s
  int    required_image_width;  // 需要获得的视觉画面的宽度,默认-1不进行修改
  int    required_image_height; // 需要获得的视觉画面的高度,默认-1不进行修改
  RecvStruct(){};
  RecvStruct(const RecvStruct &other);
};

struct SendStruct {
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
// clang-format on

class NetworkManager {
private:
  // 私有成员变量和方法

public:
  /************************************
   * @name    构造函数
   * @brief   创建NetworkManager对象
   * @param   [in] server_ip (std::string) 服务器的IP地址
   * @param   [in] student_id (int) 学生ID
   * @param   [in] team_name (std::string) 队伍名称
   * @param   [in] server_port (int) 服务器端口号
   * @param   [in] client_port (int) 客户端端口号（可选）
   * @param   [in] error_handle (void (*)(int, std::string))
   * 错误处理回调函数（可选）
   * @return  NetworkManager对象
   */
  NetworkManager(std::string server_ip, int student_id, std::string team_name,
                 int server_port, int client_port,
                 void (*error_handle)(int, std::string) = nullptr);

  /************************************
   * @name    析构函数
   * @brief   销毁NetworkManager对象
   */
  ~NetworkManager();

  /************************************
   * @name    注册视觉程序
   * @brief   向服务端注册视觉程序
   * @param   [in] cnt_time (double) 当前视觉程序时间戳，用于时间同步 单位s
   * @param   [in] timeout (int) 超时时间 单位ms
   * @return  (bool) 注册成功返回true，失败返回false
   */
  bool registerUser(double cnt_time, int timeout = 5000);

  /************************************
   * @name    下线视觉程序
   * @brief   从服务端下线并解除注册占用
   * @param   [in] timeout (int) 超时时间 单位ms
   * @return  (bool) 下线成功返回true，失败返回false
   */
  bool offlineUser(int timeout = 5000);

  /************************************
   * @name    获取最新一帧信息
   * @brief   从服务端获取最新的数据帧信息
   * @return  (RecvStruct) 最新的数据帧信息
   */
  RecvStruct getNewestRecvMessage();

  /************************************
   * @name    向服务端发送控制信息
   * @brief   向服务端发送控制信息
   * @param   [in] send_message (const SendStruct &) 待发送的控制信息
   */
  void sendControlMessage(const SendStruct &send_message);

  /************************************
   * @name    发送心跳包
   * @brief   向服务端发送心跳包以维持连接
   */
  void sendPulse();

  /************************************
   * @name    获取游戏状态
   * @brief   从服务端获取当前游戏状态
   * @return  (bool) 游戏运行中返回true，否则返回false
   */
  bool getGameStatus();

  /************************************
   * @name    关闭通信线程和释放资源
   * @brief   关闭通信线程并释放相关资源
   */
  void Close();
};
} // namespace Net

#endif // __NETWORKMANAGER_H__