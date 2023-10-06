/*************************
 * @note 请不要直接使用本头文件内的资源
 * @note 请通过NetworkManager.h中的NetworkManager类来使用本头文件内的资源
 */
#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include <json/json.h>
#include <string>

namespace tools {
namespace network {

class JSONSerializer {
public:
  static std::string serialize(const Json::Value &data);
  static Json::Value deserialize(const std::string &json_string);
};

} // namespace network
} // namespace tools

#endif
