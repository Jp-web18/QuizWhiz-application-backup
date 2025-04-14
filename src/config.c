#include "config.h"

void xor_encrypt_decrypt(char *data, size_t length, char key) {
    for (size_t i = 0; i < length; i++) {
        data[i] ^= key;
    }
}
