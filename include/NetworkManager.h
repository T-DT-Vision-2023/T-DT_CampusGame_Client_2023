/*******************
 * @name NetworkManager
 * @author T-DT Algorithm 2024 黄子墨
 * @version 1.0
 * @date 2023-10-6
 * @brief 用于与Unity服务端通信
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include "tools.h"
#include "tools/core/base64.h"
#include "tools/network/json_serializer.h"
#include "tools/network/network_structs.h"
#include <zmq.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <vector>

namespace network {

class NetworkManager {
private:


  zmq::context_t* context;
  zmq::socket_t* send_socket;
  zmq::socket_t* recv_socket;


  int student_id;
  std::string team_name;
  std::string server_address;

  std::mutex handler_mutex;
  std::thread handler_thread;
  tools::Base64 base64;
  bool running = true;
  bool on_register = 0;
  bool recv_close = 0;
  bool on_game = 0;
  bool close = 0;


  RecvStruct latest_recv_message;

  zmq::message_t *latest_recv_raw_message=new zmq::message_t();


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
  NetworkManager(const std::string &server_ip, int student_id,
                 std::string team_name, int server_port, int client_port);

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
  bool registerUser(float cnt_time, int timeout = 5000);

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
  RecvStruct getLatestRecvMessage();

  /************************************
   * @name    接收数据帧处理线程
   * @brief   用于接收数据帧的处理线程
   * @return  (void)
   */

  bool get__latest_raw_message();
  /*!
   *@brief 用于从消息队列获取最新的数据
   */

  void recvHandler();

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
   * @name    启动心跳包定时器
   * @brief   启动心跳包定时器
   */
  void startHeartbeatTimer();

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

} // namespace network

#endif // __NETWORKMANAGER_H__
