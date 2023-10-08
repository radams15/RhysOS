#include "string.hpp"

extern "C" {
#include "malloc.h"
#include "string.h"
}

string::string() : length(0) {}

string::string(int len) : length(len) {
    rbuf = (char*)malloc(length);
    clear();
}

string::string(const char* buffer) {
    length = strlen((char*)buffer);
    rbuf   = (char*)malloc(length);
    strcpy(rbuf, (char*)buffer, length);
    rbuf[length] = 0;
}

void string::set(const char* buffer) {
    if (length < strlen((char*)buffer)) {
        length = strlen((char*)buffer);
        free(rbuf);
        rbuf = (char*)malloc(length);
    }

    strcpy(rbuf, (char*)buffer, length);
    rbuf[length] = 0;
}

void string::clear() {
    for (int i = 0; i < length; i++)
        rbuf[i] = 0;
}

const char* string::c_str() {
    return (const char*)rbuf;
}

const char* string::c_copy() {
    int str_len = strlen((char*)rbuf);
    char* out   = (char*)malloc(str_len * sizeof(char));

    strcpy((char*)out, rbuf, str_len);

    return out;
}
