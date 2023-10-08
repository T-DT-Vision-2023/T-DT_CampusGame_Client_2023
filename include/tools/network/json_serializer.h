/*************************
* @note 请不要直接使用本头文件内的资源
* @note 请通过NetworkManager.h中的NetworkManager类来使用本头文件内的资源
*/
#ifndef __JSON_SERIALIZER_H__
#define __JSON_SERIALIZER_H__

#include "tools/network/network_structs.h"
#include <nlohmann/json.hpp>

namespace nlohmann {

template <>
struct adl_serializer<network::SendStruct> {
  static void to_json(json &j, const network::SendStruct &s);
  static void from_json(const json &j, network::SendStruct &s);
};

template <>
struct adl_serializer<network::RecvStruct> {
  static void to_json(json &j, const network::RecvStruct &s);
  static void from_json(const json &j, network::RecvStruct &s);
};

} // namespace nlohmann

#endif // __JSON_SERIALIZER_H__
