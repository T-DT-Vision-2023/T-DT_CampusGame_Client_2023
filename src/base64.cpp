#include "tools/core/base64.h"
namespace tools {
    Base64::Base64() {
        for (int i = 0; i < 123; i++) {
            base64_reverse_table[i] = -1;
        }
        for (int i = 0; i < 64; i++) {
            base64_reverse_table[base64_table[i]] = i;
        }
    }

    std::string Base64::encode(const char *data, int data_len) {
        std::string   ret;
        int           i = 0;
        int           j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (data_len--) {
            char_array_3[i++] = *(data++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++) {
                    ret += base64_table[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++) {
                ret += base64_table[char_array_4[j]];
            }

            while ((i++ < 3)) {
                ret += '=';
            }
        }

        return ret;
    }

    char *Base64::decode(std::string data, int data_len) {
        int           in_len = data_len;
        int           i      = 0;
        int           j      = 0;
        int           in_    = 0;
        unsigned char char_array_4[4], char_array_3[3];
        char         *ret = (char *)malloc(data_len);

        while (in_len-- && (data[in_] != '=') && is_base64(data[in_])) {
            char_array_4[i++] = data[in_];
            in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++) {
                    char_array_4[i] = base64_reverse_table[char_array_4[i]];
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++) {
                    ret[j++] = char_array_3[i];
                }
                i = 0;
            }
        }

        if (i) {
            for (int k = 0; k < 4; k++) {
                char_array_4[k] = base64_reverse_table[char_array_4[k]];
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (int k = 0; (k < i - 1); k++) {
                ret[j++] = char_array_3[k];
            }
        }
        return ret;
    }

} // namespace tools