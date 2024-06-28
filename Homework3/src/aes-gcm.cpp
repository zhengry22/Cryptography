#include <iostream>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "../include/aes-gcm.h"
#include "../include/aes-ctr.h"
#include "../include/aes.h"
using namespace std;
#define AES_BLOCK_SIZE 16
#define MYDEBUG
void inc_32_in_longlong(const long long X[2], long long dst[2]) {
    // The last 32 bits add one
    memcpy(dst, X, 16);
    if (X[1] & 0x1111111100000000 == 0x1111111100000000) {
        dst[1] &= 0x0000000011111111;
    }
    else {
        long long tmp = X[1] >> 32;
        uint32_t first4Bytes = *((uint32_t*)&tmp);
        uint32_t reversed4Bytes = ((first4Bytes & 0xFF000000) >> 24) |
                              ((first4Bytes & 0x00FF0000) >> 8) |
                              ((first4Bytes & 0x0000FF00) << 8) |
                              ((first4Bytes & 0x000000FF) << 24);
        
        reversed4Bytes++;

        uint32_t final4Bytes = ((reversed4Bytes & 0xFF000000) >> 24) |
                           ((reversed4Bytes & 0x00FF0000) >> 8) |
                           ((reversed4Bytes & 0x0000FF00) << 8) |
                           ((reversed4Bytes & 0x000000FF) << 24);

        dst[1] |= 0xFFFFFFFF00000000;
        long long a = ((long long)((long long)final4Bytes << 32) + 0x00000000FFFFFFFF);
        dst[1] &= a;
    }
} 

void GCTR(const uint8_t key[16], const long long ICB[2], const uint8_t *X, uint8_t* Y, const int length_of_X) {
    // Y is the output that shares the same length with X
    // length_of_X is in bits
    if (length_of_X  == 0) {
        // Y is empty
        return;
    }
    int n = (length_of_X % 128 == 0) ? (length_of_X >> 7) : (1 + (length_of_X >> 7));
    int last_length = (length_of_X % 128 == 0) ? 128 : length_of_X % 128;

    /*
        Let X1, X2, ... , Xn-1, Xn*
        denote the unique sequence of bit strings such that ∗ X = X X Xn− Xn || || ... || || 1 2 1 ; 
        X1, X2,..., Xn-1 are complete blocks.
        Each block is 16 bytes long, except for the last one
    */
    
    AES_Encrypt my_aes;
    
    long long midval[2], next_midval[2];
    for (int i = 1; i <= n; i++) {
        if (i == 1) {
            memcpy(midval, ICB, 16);
        }
        else {
            memcpy(midval, next_midval, 16);
        }
        my_aes.setkey(key);

        unsigned char tmp[16];
        memcpy(tmp, midval, 16);

        int index = (i - 1) << 4;
        unsigned char ct[16];
        my_aes.encrypt(tmp, ct);


        if (i != n) {
            for (int j = index; j < index + 16; j++) {
                Y[j] = (X[j] ^ ct[j - index]);
                //std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)X[j] << " ";
            }
        }
        else {
            for(int j = index; j < index + (last_length >> 3); j++) {
                Y[j] = (X[j] ^ ct[j - index]);
            }
        }


        inc_32_in_longlong(midval, next_midval);
    }
}

void mult_128bits(const long long X[2], const long long Y[2], long long res[2]) {
    unsigned long long Z[2] = { 0, 0 };
    unsigned long long V[2];
    memcpy(V, Y, 16);
    const unsigned long long R[2] = { 0xE100000000000000, 0x0000000000000000 };

    unsigned long long sign = 0x8000000000000000;

    unsigned long long actual_sign = sign;
    for (int i = 0; i < 128; i++) {
        if (i % 64 == 0) {
            actual_sign = sign;
        }
        else {
            actual_sign >>= 1;
        }
        // Z_{i+1}
        if (i < 64) {
            if (X[0] & actual_sign) {
                for (int j = 0; j < 2; j++) {
                    Z[j] ^= V[j];
                }
            }
        }
        else {
            if (X[1] & actual_sign) {
                for (int j = 0; j < 2; j++) {
                    Z[j] ^= V[j];
                }
            }
        }
        // V_{i+1}
        bool is_one = V[1] & 0x0000000000000001;
        unsigned long long last_bit_of_V0 = V[0] & 0x0000000000000001;
        last_bit_of_V0 <<= 63;
        V[0] >>= 1;
        V[1] >>= 1;
        V[1] |= last_bit_of_V0;
        if (is_one) {
            V[0] ^= R[0];
            V[1] ^= R[1];
        }
    }
    res[0] = Z[0];
    res[1] = Z[1];
}

void GHash(const uint8_t H[AES_BLOCK_SIZE], const uint8_t *X, const int X_len, uint8_t *Y) {
    memset(Y, 0, 16);
    int m = X_len >> 7;
    long long mult1[2], mult2[2], res[2];
    uint8_t *Z = new uint8_t[16];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < 16; j++) {
            Z[j] = Y[j] ^ X[(i << 4) + j];
        }

        uint8_t *mult1_ptr = (uint8_t*)(mult1);
        uint8_t *mult2_ptr = (uint8_t*)(mult2);

        for (int j = 0; j < 8; j++) {
            mult1_ptr[j] = H[7 - j];
        }
        for (int j = 8; j < 16; j++) {
            mult1_ptr[j] = H[23 - j];
        }

        for (int j = 0; j < 8; j++) {
            mult2_ptr[j] = Z[7 - j];
        }
        for (int j = 8; j < 16; j++) {
            mult2_ptr[j] = Z[23 - j];
        }

        mult_128bits(mult2, mult1, res);

        uint8_t* res_ptr = (uint8_t*)res;
        
        for (int j = 0; j < 8; j++) {
            Y[j] = res_ptr[7 - j];
        }
        for (int j = 8; j < 16; j++) {
            Y[j] = res_ptr[23 - j];
        }

    }
    delete[] Z;
}

/*
AES_GCM 认证加密函数
plaintext: 明文字节数组. plain_len_bits: 明文比特数.
IV: 初始向量96比特.
AD: 附加认证信息字节数组. AD_len_bits: AD比特数.
key: 128比特密钥
output_ciphertext: 输出密文数组指针地址，长度同密文 output_tag: 输出tag数组指针地址.
ciphertext长度同plaintext, tag长度为128比特(16字节).
*/
void AES_Authenticated_Encryption_GCM(const uint8_t *plaintext, const uint64_t& plain_len_bits, const uint8_t IV[12], const uint8_t *AD, const uint64_t& AD_len_bits, const uint8_t key[16], uint8_t** output_ciphertext, uint8_t** output_tag) {
    /* Your code here */

    AES_Encrypt aes;
    uint8_t H0[AES_BLOCK_SIZE] = {0};
    uint8_t H[AES_BLOCK_SIZE] = {0};
    aes.setkey(key);
    // Let H = CIPHK(0128).
    aes.encrypt(H0, H);



    /*
        Define a block, J0, as follows: 
        If len(IV)=96, then let J0 = IV || 0^31
        ||1. 
        If len(IV) ≠ 96, then let s = 128 ⎡len(IV)/128⎤-len(IV), and let 
        J0=GHASHH(IV||0s+64
        ||[len(IV)]64).     
    */
    long long J[2];
    memset(J, 0, 2);
    J[1] = 0x0100000000000000;
    memcpy(J, IV, 12);

    /*
        Let C=GCTRK(inc32(J0), P). 
    */
    long long J1[2];
    inc_32_in_longlong(J, J1);
    // inc_32(J)



    GCTR(key, J1, plaintext, *output_ciphertext, plain_len_bits);
  

    /*
        Let u =128⋅⎡len(C) 128⎤ − len(C) and let v =128⋅⎡len(A) 128⎤ − len(A).
    */
    int u = (plain_len_bits % 128 == 0) ? (plain_len_bits >> 7) : (1 + (plain_len_bits >> 7));
    u <<= 7;
    u -= plain_len_bits;

    int v = (AD_len_bits % 128 == 0) ? (AD_len_bits >> 7) : (1 + (AD_len_bits >> 7));
    v <<= 7;
    v -= AD_len_bits;

    /*
        Define a block, S, as follows: 
        S = GHASHH (A || 0v
        || C || 0u
        || [len(A)]64 || [len(C)]64). 
    */
   
    uint8_t *ghash_input = new uint8_t[(u + plain_len_bits + v + AD_len_bits + 128) >> 3];
    uint8_t *ghash_output = new uint8_t[(u + plain_len_bits + v + AD_len_bits + 128) >> 3];
    int AD_byte_length = AD_len_bits >> 3;
    memcpy(ghash_input, AD, AD_byte_length);
    memset(&ghash_input[AD_byte_length], 0, v >> 3);
    memcpy(&ghash_input[AD_byte_length + (v >> 3)], *output_ciphertext, plain_len_bits >> 3);
    memset(&ghash_input[AD_byte_length + (v >> 3) + (plain_len_bits >> 3)], 0, u >> 3);
    long long A_length = (long long)(AD_len_bits);
    long long C_length = (long long)(plain_len_bits);

    uint8_t* A_length_ptr = (uint8_t*)&A_length;
    uint8_t* C_length_ptr = (uint8_t*)&C_length;

    int add1 = AD_byte_length + (v >> 3) + (plain_len_bits >> 3) + (u >> 3);

    for (int i = 0; i < 8; i++) {
        ghash_input[add1 + i] = A_length_ptr[7 - i];
    }
    for (int i = 8; i < 16; i++) {
        ghash_input[add1 + i] = A_length_ptr[23 - i];
    }

    int add2 = add1 + 8;
    for (int i = 0; i < 8; i++) {
        ghash_input[add2 + i] = C_length_ptr[7 - i];
    }
    for (int i = 8; i < 16; i++) {
        ghash_input[add2 + i] = C_length_ptr[23 - i];
    }

    //memcpy(&ghash_input[AD_byte_length + (v >> 3) + (plain_len_bits >> 3) + (u >> 3)], &A_length, 8);
    //memcpy(&ghash_input[AD_byte_length + (v >> 3) + (plain_len_bits >> 3) + (u >> 3) + 8], &C_length, 8);
    uint8_t *S = new uint8_t[16];
    GHash(H, ghash_input, (AD_len_bits + v + plain_len_bits + u + 128), ghash_output);


    /*
        Let T (J S) K , 0 ( ).
    */
    GCTR(key, J, ghash_output, *output_tag, 128);
    delete[] ghash_input;
    delete[] ghash_output;
    delete[] S;
}

/*
AES_GCM 认证解密
ciphertext：密文字节数组 cipher_len_bits：密文比特数
IV: 初始向量96比特.
AD: 附加认证信息字节数组. AD_len_bits: AD比特数.
key: 128比特密钥
output_plaintext：明文数组，认证成功，长度同密文。如果认证失败，长度为0
Return value: true if authentication success, false if fail
*/
bool AES_Authenticated_Decryption_GCM(const uint8_t *ciphertext, const uint64_t& cipher_len_bits, const uint8_t IV[12], const uint8_t *AD, const uint64_t& AD_len_bits, const uint8_t key[16], uint8_t** output_plaintext, uint8_t* input_tag) {
 
    return 0;
}

// Function below for testing 
// int main() {
//     // Initialize key
//     unsigned char key[] = {
//         0xFE, 0xFF, 0xE9, 0x92, 0x86, 0x65, 0x73, 0x1C,
//         0x6D, 0x6A, 0x8F, 0x94, 0x67, 0x30, 0x83, 0x08
//     };

//     // Initialize IV
//     unsigned char IV[] = {
//         0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0xCE, 0xDB, 0xAD,
//         0xDE, 0xCA, 0xF8, 0x88
//     };

//     // 初始化数组 AD
//     unsigned char AD[] = {
//         0x3A, 0xD7, 0x7B, 0xB4, 0x0D, 0x7A, 0x36, 0x60,
//         0xA8, 0x9E, 0xCA, 0xF3, 0x24, 0x66, 0xEF, 0x97,
//         0xF5, 0xD3, 0xD5, 0x85
//     };

//     // 初始化数组 plaintext
//     unsigned char plaintext[] = {
//         0xD9, 0x31, 0x32, 0x25, 0xF8, 0x84, 0x06, 0xE5,
//         0xA5, 0x59, 0x09, 0xC5, 0xAF, 0xF5, 0x26, 0x9A,
//         0x86, 0xA7, 0xA9, 0x53, 0x15, 0x34, 0xF7, 0xDA,
//         0x2E, 0x4C, 0x30, 0x3D, 0x8A, 0x31, 0x8A, 0x72,
//         0x1C, 0x3C, 0x0C, 0x95, 0x95, 0x68, 0x09, 0x53,
//         0x2F, 0xCF, 0x0E, 0x24, 0x49, 0xA6, 0xB5, 0x25,
//         0xB1, 0x6A, 0xED, 0xF5, 0xAA, 0x0D, 0xE6, 0x57,
//         0xBA, 0x63, 0x7B, 0x39
//     };


//     unsigned char *ciphertext = new unsigned char[16];
//     unsigned char *tag = new unsigned char[16];

//     AES_Authenticated_Encryption_GCM(plaintext, 480, IV, AD, 160, key, &ciphertext, &tag);

// #ifdef MYDEBUG
//     cout << "Tag: ";
//     for (int i = 0; i < 16; i++) { 
//         std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)tag[i] << " ";
//     }
// #endif
//     return 0;
// }