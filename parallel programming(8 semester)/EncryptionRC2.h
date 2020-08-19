#pragma once
#include <iostream>
#include <openssl/rc2.h>



unsigned char* encriptRC2(unsigned char* text, const unsigned char* key, const int length_key);
unsigned char* decryptRC2(unsigned char* cipher, const unsigned char* key, const int length_key);