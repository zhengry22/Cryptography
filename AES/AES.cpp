#include <iostream>
#include <string>
#include <cstdint>
#include <iomanip>
//#define MYDEBUG 
using namespace std;

class AES_Encrypt
{
public:
	void AES_ENcrypt() {	  	}
	void setkey(unsigned char* init_key) {
		keyExpansion(init_key);
	}

	//Encrypt with ctr mode
	void aes_ctr(const unsigned char* plaintext, int len, const unsigned char* iv, unsigned char* ciphertext, const unsigned char* key = nullptr) {

	}
	void encrypt(const unsigned char* plaintext, unsigned char* ciphertext, const unsigned char* key = nullptr) {
		// We assume that the key is set.

		// Initialize state
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				state[j][i] = plaintext[(i << 2) + j];
			}
		}

		// Initial round: Xor the initial state 
		addRoundKey(roundKeys[0]);

		// 10 rounds
		for (int i = 1; i < 10; i++) {
			subBytes();
			shiftRows();
			mixColumns();
			addRoundKey(roundKeys[i]);
		}
		// 1 extra round
		subBytes();
		shiftRows();
		addRoundKey(roundKeys[10]);

		// Move state into ciphertext
	}
private:
	unsigned char state[4][4];
	unsigned char roundKeys[11][4][4];
	const unsigned char EnCryp_sbox[256] = {
		// 0     1     2	 3     4     5     6     7     8     9     a    b    c    d    e    f
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // a
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // b
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // c
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // d
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // e
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16  // f
	};

	//The round constant word array
	const unsigned char rcon[11] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };
	const int mixcolmtx[4][4] = {{2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2}};
	// Transformation in the Cipher that processes the State using a nonlinear byte substitution table(S - box) that operates on each of the
	// State bytes independently.
	unsigned char mul2(unsigned char value) {
		return (value & 0x80) ? (unsigned char)((value << 1) ^ 0x1b) : (unsigned char)(value << 1);
	}
	unsigned char mul3(unsigned char value) {
		return mul2(value) ^ value;
	}
	unsigned char mul4(unsigned char value) {
		return mul2(mul2(value));
	}
	unsigned char mul_selector(int num, unsigned char value) {
		switch (num) {
			case 1:
				return value;
			case 2:
				return mul2(value);
			case 3:
				return mul3(value);
			default:
				return value;
		}
	}
	void subBytes() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				state[i][j] = EnCryp_sbox[(int)state[i][j]];
			}
		}
	}
	void shiftRows() {
		// The four rows shift to the left respectively 0, 1, 2, 3 bytes
		for (int i = 0; i < 4; i++) {
			char tmp = state[i][0];
			char tmp1 = state[i][0];
			char tmp2 = state[i][1];
			char tmp3 = state[i][3];
			switch (i) {
				case 0:
					break;
				case 1:
					for (int j = 0; j < 3; j++) {
						state[i][j] = state[i][j + 1];
					}
					state[i][3] = tmp;
					break;
				case 2:
					state[i][0] = state[i][2];
					state[i][1] = state[i][3];
					state[i][2] = tmp1;
					state[i][3] = tmp2;
					break;
				case 3:
					for (int j = 2; j >= 0; j--) {
						state[i][j + 1] = state[i][j];
					}
					state[i][0] = tmp3;
					break;
			}
		}
	}
	void mixColumns() {
		unsigned char original[4][4];
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				original[i][j] = state[i][j];
			}
		}
		// mulptiplying mtx:
		/*
		02 03 01 01
		01 02 03 01
		01 01 02 03
		03 01 01 02
		*/
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				// i-th row and j-th col
				for (int k = 0; k < 4; k++) {
					if (k == 0) {
						state[i][j] = mul_selector(mixcolmtx[i][k], original[k][j]);
					}
					else {
						state[i][j] ^= (mul_selector(mixcolmtx[i][k], original[k][j]));
					}				
				}
			}
		}
	}
	void addRoundKey(const unsigned char rndKey[4][4]) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				state[i][j] = state[i][j] ^ rndKey[i][j];
			}
		}
	}
	void keyExpansion(const unsigned char* key) {
		// Fill in the array roundKeys with the expanded key
		// The round key for the initial round is key itself
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				roundKeys[0][j][i] = key[(i << 2) + j];
			}
		}
		// 10 more rounds
		for (int i = 1; i < 11; i++) {
			unsigned char newkey[4][4];
			for (int j = 0; j < 4; j++) {
				if (j == 0) {
					// The first column needs to be dealt in a special way
					unsigned char tmp[4];
					for (int k = 0; k < 4; k++) {
						tmp[k] = roundKeys[i - 1][k][3];
					}

					// process tmp
					key_process(tmp, i);

					for (int k = 0; k < 4; k++) {
#ifdef MYDEBUG
						int value = static_cast<int>(roundKeys[i - 1][k][j]);
						std::cout << std::hex << std::setw(2) << std::setfill('0') << value << std::endl;
#endif
						newkey[k][j] = tmp[k] ^ roundKeys[i - 1][k][j];
					}
				}
				else {
					for (int k = 0; k < 4; k++) {
						newkey[k][j] = newkey[k][j - 1] ^ roundKeys[i - 1][k][j];
					}
				}
			}
			// Copy it to roundKeys[i]
			for (int m = 0; m < 4; m++) {
				for (int n = 0; n < 4; n++) {
					roundKeys[i][m][n] = newkey[m][n];
				}
			}
		}
	}
	void key_process(unsigned char* key, int round) {
		// 1. RotWord
		char tmp = key[0];
		for (int i = 0; i < 3; i++) {
			key[i] = key[i + 1];
		}
		key[3] = tmp;
		// 2. Subword
		for (int i = 0; i < 4; i++) {
			key[i] = EnCryp_sbox[(int)key[i]];
		}
		// 3. Round const Xor
		key[0] = key[0] ^ rcon[round];
	}
};

class AES_Decrypt
{
public:
	void setkey(unsigned char* init_key) {
		keyExpansion(init_key);
	}

	void decrypt(const unsigned char* ciphertext, unsigned char* decrypttext, unsigned char* key = nullptr) {

	}
private:
	unsigned char state[4][4];
	unsigned char roundKeys[11][4][4];
	const unsigned char EnCryp_sbox[256] = {
		// 0     1     2	 3     4     5     6     7     8     9     a    b    c    d    e    f
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // a
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // b
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // c
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // d
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // e
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16  // f
	};

	//The round constant word array
	const unsigned char rcon[11] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };
	const unsigned char DeCryp_SBox[256] = {
		// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f 
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, // 0
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, // 1
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, // 2
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, // 3
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, // 4
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, // 5
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, // 6
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, // 7
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, // 8
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, // 9
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, // a
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, // b
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, // c
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, // d
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, // e
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d  // f
	};

	void keyExpansion(const unsigned char* key) {

	}

	void Decryp_shiftRows() {

	}


	void Decryp_subBytes() {

	}

	//mix column matrix
	/*
	0E 0B 0D 09
	09 0E 0B 0D
	0D 09 0E 0B
	0B 0D 09 0E

	*/
	unsigned char mul2(unsigned char val) { return (val & 0x80) ? (unsigned char)((val << 1) ^ 0x1b) : (unsigned char)(val << 1); }
	unsigned char mul4(unsigned char val) { return mul2(mul2(val)); }
	unsigned char mul8(unsigned char val) { return mul2(mul2(mul2(val))); }
	unsigned char mule(unsigned char val) { return mul8(val) ^ mul4(val) ^ mul2(val); }
	unsigned char mulb(unsigned char val) { return mul8(val) ^ mul2(val) ^ val; }
	unsigned char muld(unsigned char val) { return mul8(val) ^ mul4(val) ^ val; }
	unsigned char mul9(unsigned char val) { return mul8(val) ^ val; }

	void Decryp_mixColumns() {

	}
	void addRoundKey(const unsigned char rndKey[4][4]) {

	}
};





 int main() {
 	unsigned char plaintext[16] = { 0x6B,0xC1,0xBE,0xE2,0x2E,0x40,0x9F,0x96,0xE9,0x3D,0x7E,0x11,0x73,0x93,0x17,0x2A };
 	unsigned char key[16] =		{ 0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C };
 	unsigned char plaintext2[16] = { 0xAE,0x2D,0x8A,0x57,0x1E,0x03,0xAC,0x9C,0x9E,0xB7,0x6F,0xAC,0x45,0xAF,0x8E,0x51 };
	//unsigned char key[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };
 	unsigned char iv[16] = { 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF };
 	unsigned char plaintext3[64] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A ,\
 									 0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C, 0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51 ,\
 									 0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11, 0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF ,\
 									 0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17, 0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10 };

	unsigned char plain_AES[16] = { 0x90, 0x35, 0xF7, 0x8A, 0x96, 0x33, 0xBA, 0x91, 0x4F, 0xED, 0x58, 0x5E, 0xCF, 0x8E, 0xCF, 0x11 };

 	unsigned char ciphertext[64];

 	unsigned char decryptedtext[64];

	unsigned char cipher_AES[16];

 	AES_Encrypt en;
 	AES_Decrypt de;

 	en.setkey(key);
	en.encrypt(plaintext, cipher_AES, key);
 	/*en.aes_ctr(plaintext3, 64, iv, ciphertext, key);
 	std::cout << "CTR Plain Text:\t ";
 	for (int i = 0; i < 64; ++i) {
 		std::cout << std::hex << static_cast<int>(plaintext3[i]) << " ";
 		if (i == 32)
 			cout << endl;
 	}
 	std::cout << std::endl;
 	std::cout << "CTR cypher Text:\n ";
 	for (int i = 0; i < 64; ++i) {
 		std::cout << std::hex << static_cast<int>(ciphertext[i]) << " ";
 		if (i == 32)
 			cout << endl;
 	}
 	std::cout << std::endl;*/

 	////de.setkey(key);
 	////de.aes_ctr(ciphertext, 64, iv, decryptedtext, key);
 	//en.aes_ctr(ciphertext, 64, iv, decryptedtext, key);
 	//std::cout << "CTR Decrypted text:\n";
 	//for (int i = 0; i < 64; ++i) {
 	//	std::cout << std::hex << static_cast<int>(decryptedtext[i]) << " ";
 	//	if (i == 32)
 	//		cout << endl;
 	//}
 	//std::cout << std::endl;


 	//std::cout << "Plain Text:\t ";
 	//for (int i = 0; i < 16; ++i) {
 	//	std::cout << std::hex << static_cast<int>(plaintext[i]) << " ";
 	//}
 	//std::cout << std::endl;
 	//en.encrypt(plaintext, ciphertext);
 	//std::cout << "Ciphertext: \t";
 	//for (int i = 0; i < 16; ++i) {
 	//	std::cout << std::hex << static_cast<int>(ciphertext[i]) << " ";
 	//}
 	//std::cout << std::endl;

 	//de.decrypt(ciphertext, key, decryptedtext);
 	//std::cout << "Decrypted text:\t";
 	//for (int i = 0; i < 16; ++i) {
 	//	std::cout << std::hex << static_cast<int>(decryptedtext[i])<<" ";
 	//}
 	//std::cout << std::endl;




 	//std::cout << "Plain Text:\t ";
 	//for (int i = 0; i < 16; ++i) {
 	//	std::cout << std::hex << static_cast<int>(plaintext2[i]) << " ";
 	//}
 	//std::cout << std::endl;
 	//en.encrypt(plaintext2, key, ciphertext);
 	//std::cout << "Ciphertext: \t";
 	//for (int i = 0; i < 16; ++i) {
 	//	std::cout << std::hex << static_cast<int>(ciphertext[i]) << " ";
 	//}
 	//std::cout << std::endl;

 	return 0;
 }


