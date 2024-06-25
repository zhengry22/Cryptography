#include "../include/sha256.h"
//#include "sha256.cpp"
#include "../include/common.h"
#include "../include/hmac.h"
#include <cstring>

#define MYDEBUG
/*
    Output:
        Pointer to tag
        HMAC_OUTPUT_LEN_BYTES = 32
*/
uint8_t* hmac_sha256(const uint8_t* key, size_t key_len_byte, const uint8_t* message, size_t message_len_byte) 
{
    uint8_t *tag = new uint8_t[HMAC_OUTPUT_LEN_BYTES];   
    /* Your code Here! */
    #define HASH_BLOCK_SIZE 64
    uint8_t ipad[HASH_BLOCK_SIZE];
    uint8_t opad[HASH_BLOCK_SIZE];

    // initialize ipad and opad
    memset(ipad, 0x36, sizeof(ipad));
    memset(opad, 0x5c, sizeof(opad));

    // First calculate K_0

    uint8_t *K0 = new uint8_t[HASH_BLOCK_SIZE]; // K_0's output is 64 bytes long
    memset(K0, 0, HASH_BLOCK_SIZE);
    int a1 = (int)((int)key_len_byte - (int)HASH_BLOCK_SIZE > 0);
    int a2 = (int)((int)key_len_byte - (int)HASH_BLOCK_SIZE < 0);
    int cmp = a1 - a2;


    switch(cmp) {
        case(-1): {
            // key_len_byte < 64
            memcpy(K0, key, key_len_byte);
            break;
        }
        case(0): {
            // key_len_byte = 64
            memcpy(K0, key, key_len_byte);
            break;
        }
        case(1): {
            // key_len_byte > 64
            K0 = sha_256(key, key_len_byte);
            memset(&K0[32], 0, 32);
            break;
        }
    } 

#ifdef MYDEBUG
    // Check if K0 is correct 
    cout << "K0: ";
    for (int i = 0; i < HASH_BLOCK_SIZE; ++i) {
		cout << std::hex << std::setw(2) << std::setfill('0') << (int)K0[i] << " ";
	}
    cout << endl;
#endif

    // Now we've successfully calculated K0

    size_t length1 = HASH_BLOCK_SIZE + message_len_byte;
    uint8_t* mid_val1 = new uint8_t[length1];
    for (int i = 0; i < length1; i++) {
        if (i < HASH_BLOCK_SIZE) {
            mid_val1[i] = K0[i] ^ ipad[i];
        }
        else {
            mid_val1[i] = message[i - HASH_BLOCK_SIZE];
        }
    }

    // Hash 1:
    uint8_t* hash1 = new uint8_t[HMAC_OUTPUT_LEN_BYTES];
    hash1 = sha_256(mid_val1, length1);

#ifdef MYDEBUG
    // Check if K0 is correct 
    cout << "hash1: ";
    for (int i = 0; i < HMAC_OUTPUT_LEN_BYTES; ++i) {
		cout << std::hex << std::setw(2) << std::setfill('0') << (int)hash1[i] << " ";
	}
    cout << endl;
#endif


    for (int i = 0; i < HASH_BLOCK_SIZE; i++) {
        K0[i] ^= opad[i];
    }

    size_t length2 = HASH_BLOCK_SIZE + HMAC_OUTPUT_LEN_BYTES;
    uint8_t* mid_val2 = new uint8_t[length2];
    for (int i = 0; i < length2; i++) {
        if (i < HASH_BLOCK_SIZE) {
            mid_val2[i] = K0[i];
        }
        else {
            mid_val2[i] = hash1[i - HASH_BLOCK_SIZE];
        }
    }

    // Finally calculate the hash of mid_val2 

    tag = sha_256(mid_val2, length2);
    delete[] K0;
    delete[] mid_val1;
    delete[] hash1;
    delete[] mid_val2;
    return tag;
}

// The main function below is for testing
//  int main() {
//      // 初始化 text 数组
//      uint8_t text[] = "\x53\x61\x6D\x70\x6C\x65\x20\x6D\x65\x73\x73\x61\x67\x65\x20"
//                       "\x66\x6F\x72\x20\x6B\x65\x79\x6C\x65\x6E\x3C\x62\x6C\x6F"
//                       "\x63\x6B\x6C\x65\x6E\x2C\x20\x77\x69\x74\x68\x20\x74\x72"
//                       "\x75\x6E\x63\x61\x74\x65\x64\x20\x74\x61\x67";

//      // 初始化 key 数组
//      uint8_t key[] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
//                      "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
//                      "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
//                      "\x30";
//      uint8_t* answer = new uint8_t[32];
//      answer = hmac_sha256(key, 49, text, 54);
//      cout << "answer: ";
//      for (int i = 0; i < 32; ++i) {
//  		cout << std::hex << std::setw(2) << std::setfill('0') << (int)answer[i] << " ";
//   	}
//      return 0;
//  }