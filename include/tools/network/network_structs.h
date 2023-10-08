//
// Created by momo on 23-10-8.
//
#ifndef GAMECLIENT_NETWORK_STRUCTS_H
#define GAMECLIENT_NETWORK_STRUCTS_H

#include <opencv2/opencv.hpp>

namespace network {
// clang-format off
struct RecvStruct {
  double yaw;                   // 目标位置欧拉角yaw值
  double pitch;                 // 目标位置欧拉角pitch值
  int    shoot;                 // 开火状态 0->不开火 1->1的范围内一直开火 2->跳变到2开火
  double time_stamp;            // 当前视觉程序时间戳,用于时间同步,默认-1不进行同步 单位s
  int    required_image_width;  // 需要获得的视觉画面的宽度,默认-1不进行修改
  int    required_image_height; // 需要获得的视觉画面的高度,默认-1不进行修改
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
} // namespace network

#endif // GAMECLIENT_NETWORK_STRUCTS_H
