#include "tools/network/json_serializer.h"

namespace tools {
namespace network {

std::string JSONSerializer::serialize(const Json::Value& data) {
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, data);
}

Json::Value JSONSerializer::deserialize(const std::string& json_string) {
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::istringstream iss(json_string);
    std::string errs;
    Json::parseFromStream(reader, iss, &root, &errs);
    return root;
}

} // namespace network
} // namespace tools
