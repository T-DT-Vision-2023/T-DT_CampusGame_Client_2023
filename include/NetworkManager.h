/*******************
 * @name NetworkManager
 * @author T-DT Algorithm 2024 黄子墨
 * @version 1.0
 * @date 2023-10-6
 * @brief 用于与unity服务端通信
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include "tools/core/base64.h"
#include "tools.h"
#include <mutex>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <vector>

namespace Net
{
struct RecvStruct
{
  double yaw;            //当前相机欧拉角的yaw值
  double pitch;          //当前相机欧拉角的pitch值
  cv::Mat img;           //当前视觉相机图像
  double recv_time;      //当前帧信息对应时间戳 单位s
  int health[6];         //所有单位血量 友方135对应0-2 敌方135对应3-5
  int rest_time;         //当前比赛剩余时间 单位s
  int cnt_bullet_num;    //当前弹夹内子弹
  int other_bullet_num;  //当前弹夹外子弹
  bool on_reload;        //当前是否在换弹
  float bullet_speed;    //当前弹速 单位m/s
  int buff_over_time;    //当前触发大符已用时间 单位s
  int score;             //当前分数
  RecvStruct(){};
  RecvStruct(const RecvStruct& other);
};

struct SendStruct
{
  double yaw;          // 当前相机欧拉角的yaw值
  double pitch;        // 当前相机欧拉角的pitch值
  cv::Mat img;         // 当前视觉相机图像
  double time_stamp;   // 当前帧信息对应时间戳 单位s
  double enemy_hp;     // 敌方当前血量
  double my_hp;        // 我方当前血量
  int rest_bullets;    // 当前比赛剩余弹量
  int rest_time;       // 当前比赛剩余时间 单位s
  int buff_over_time;  // 当前触发大符已用时间 单位s
};

class NetworkManager
{
  //不用在意private内的内容, 看public内的函数即可
private:
  UDPClient* client;
  std::string team_name;
  int student_id;
  std::string server_ip;
  int server_port;
  std::mutex handler_mutex;
  std::thread handler_thread;
  tools::Base64 base64;
  bool is_running = 1;
  bool on_register = 0;
  bool recv_close = 0;
  bool on_game = 0;
  bool close = 0;

  std::string* image_buff;
  int image_buff_size = 1;

  RecvStruct newest_recv_message;

  void (*error_handle)(int error_id, std::string message);

private:
  void recv_handler();

public:
  /************************************
   * @name    NetworkManager
   * @brief   构造函数
   * @param   [in] team_name 队伍名称
   * @param   [in] student_id 学号
   * @param   [in] server_ip 服务器ip
   * @param   [in] server_port 服务器端口
   * @param   [in] client_port 客户端端口(在端口不冲突的前提下随意填写)
   * @param   [in] error_handle 错误处理回调函数, 具体要求见本头文件中说明
   * @return
   */
  NetworkManager(std::string server_ip, int student_id, std::string team_name, int server_port, int client_port,
                 void (*error_handle)(int, std::string) = nullptr);

  /************************************
   * @name    error_handle
   * @brief   错误处理函数, 当通信线程遇到错误后会调用该函数并传入相应错误信息
   * @param   [in] error_id(int)      错误的id
   * @param   [in] message(string)    错误的信息
   * @note    错误id: 1->用户未注册, 可能由服务端错误重启造成, 请重新注册
   * @note    错误id: 2->当前客户端版本不满足服务端对客户端版本要求
   * @note    错误id: 3->服务端对客户端校验错误, 正常使用不可能出现这个错误, 就别想着作弊了奥
   * @note    错误id: 4->注册失败, 当前服务端存在另一用户已先行注册, 可重启服务端后重试
   * @note    错误id: 5->客户端已因超时被服务端自动下线, 请重新注册
   * @note    错误id: 100->未在上诉原因中的异常下线, 请联系管理员
   */

  /************************************
   * @name    ~NetworkManager
   * @brief   析构函数
   * @return
   */
  ~NetworkManager();

  /************************************
   * @name    registerUser
   * @brief   向服务端注册视觉程序, 服务端只会向注册后的视觉程序发送数据
   * @param   [in] cnt_time   当前视觉程序时间戳,用于时间同步 单位s
   * @param   [in] timeout    超时时间 单位ms
   * @return  1->注册成功 0->注册失败
   */
  bool registerUser(double cnt_time, int timeout = 5000);

  /************************************
   * @name    offlineUser
   * @brief   从服务端下线并解除注册占用
   * @param   [in] timeout    超时时间 单位ms
   * @return  1->下线成功 0->下线失败
   */
  bool offlineUser(int timeout = 5000);

  /************************************
   * @name    getNewestRecvMessage
   * @brief   获取当前最新的一帧信息
   * @return  最新一帧信息
   * @note    该函数会获得当前最新的一帧信息, 但不保证一定获得比上一次调用函数时获得的信息更新,
   * 可以自行使用recv_time判断是否为两帧相同信息
   * @note    正如你的猜测, 在程序刚开始运行还没获得服务端信息时, 该函数会返回一个空的RecvStruct
   */
  RecvStruct getNewestRecvMessage();

  /************************************
   * @name    sendControlMessage
   * @brief   向服务端发送数据
   * @param   [in] send_message 发送的数据
   */
  void sendControlMessage(const SendStruct& send_message);

  /************************************
   * @name    sendPulse
   * @brief   向服务器发送不包含信息的心跳包, 避免因超时未发送信息被服务器下线
   */
  void sendPulse();

  /************************************
   * @name    getGameStatus
   * @brief   获取当前游戏运行状态
   * @return  0->游戏未开始或已结束 1->游戏进行中
   */
  bool getGameStatus();

  /************************************
   * @name    Close
   * @brief   关闭通信线程, 释放资源, 从服务器下线
   */
  void Close();
};
}  // namespace Net

#endif  // __NETWORKMANAGER_H__