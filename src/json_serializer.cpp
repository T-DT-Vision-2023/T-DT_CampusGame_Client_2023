#include "tools/network/json_serializer.h"
#include <opencv2/opencv.hpp>

namespace nlohmann {

void adl_serializer<network::RecvStruct>::to_json(json &j, const network::RecvStruct &s) {
  j = {
      {"yaw", s.yaw},
      {"pitch", s.pitch},
      {"time_stamp", s.time_stamp},
      {"enemy_hp", s.enemy_hp},
      {"my_hp", s.my_hp},
      {"rest_bullets", s.rest_bullets},
      {"rest_time", s.rest_time},
      {"buff_over_time", s.buff_over_time}
  };
  if (!s.img.empty()) {
    std::vector<uchar> imgData;
    cv::imencode(".jpeg", s.img, imgData);
    j["img"] = imgData;
  }
}

void adl_serializer<network::RecvStruct>::from_json(const json &j, network::RecvStruct &s) {
  s.yaw = j["yaw"];
  s.pitch = j["pitch"];
  s.time_stamp = j["time_stamp"];
  s.enemy_hp = j["enemy_hp"];
  s.my_hp = j["my_hp"];
  s.rest_bullets = j["rest_bullets"];
  s.rest_time = j["rest_time"];
  s.buff_over_time = j["buff_over_time"];
  if (j.contains("img") && j["img"].is_array()) {
    const auto &imgData = j["img"];
    std::vector<uchar> imgBuffer(imgData.begin(), imgData.end());
    s.img = cv::imdecode(imgBuffer, cv::IMREAD_COLOR);
  }
}

void adl_serializer<network::SendStruct>::to_json(json &j, const network::SendStruct &s) {
  j = {
      {"yaw", s.yaw},
      {"pitch", s.pitch},
      {"shoot", s.shoot},
      {"time_stamp", s.time_stamp},
      {"required_image_width", s.required_image_width},
      {"required_image_height", s.required_image_height}
  };
}

void adl_serializer<network::SendStruct>::from_json(const json &j, network::SendStruct &s) {
  s.yaw = j["yaw"];
  s.pitch = j["pitch"];
  s.shoot = j["shoot"];
  s.time_stamp = j["time_stamp"];
  s.required_image_width = j["required_image_width"];
  s.required_image_height = j["required_image_height"];
}

} // namespace nlohmann
