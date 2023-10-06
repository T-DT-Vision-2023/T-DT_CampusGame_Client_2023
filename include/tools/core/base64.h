/*************************
 * @note 请不要直接使用本头文件内的资源
 * @note 请通过NetworkManager.h中的NetworkManager类来使用本头文件内的资源
 */
#ifndef __BASE64_H__
#define __BASE64_H__
#include <cstring>
#include <string>
namespace tools {
    class Base64 {
    private:
        char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        char base64_reverse_table[123];

        static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }

    public:
        Base64();
        std::string encode(const char *data, int data_len);
        char       *decode(std::string data, int data_len);
    };
} // namespace tools

#endif