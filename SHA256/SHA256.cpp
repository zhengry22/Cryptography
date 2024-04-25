#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#define LITTLE
//#define BIG

typedef unsigned int uint32;
using namespace std;
class SHA_256 {
public:
	SHA_256() {
		a = b = c = d = e = f = g = h = 0;
	}
	uint32 concat_char(uint32 start) {
#ifdef BIG
		uint32 result = (uint32)buf[start];
		result |= ((uint32)buf[start + 1] << 8);
		result |= ((uint32)buf[start + 2] << 16);
		result |= ((uint32)buf[start + 3] << 24);
		return result;
#endif
		uint32 result2 = (uint32)buf[start + 3];
		result2 |= ((uint32)buf[start + 2] << 8);
		result2 |= ((uint32)buf[start + 1] << 16);
		result2 |= ((uint32)buf[start] << 24);
		return result2;
	}
	uint32 sigma1(uint32 x) {
		return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
	}
	uint32 sigma0(uint32 x) {
		return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
	}
	uint32 Sigma1(uint32 x) {
		return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
	}
	uint32 Sigma0(uint32 x) {
		return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
	}
	uint32 Ch(uint32 e, uint32 f, uint32 g) {
		return (e & f) ^ (~e & g);
	}
	uint32 Maj(uint32 a, uint32 b, uint32 c) {
		return (a & b) ^ (b & c) ^ (c & a);
	}
	uint32 calc_w(int i) {
		return (sigma1(w[i - 2]) + w[i - 7] + sigma0(w[i - 15]) + w[i - 16]);
	}
	uint32* calc(unsigned char* data, int len) {
		//padding first
		int total_len = padding(data, len);
		
		// At first, hash is initialized by H
		for (int i = 0; i < 8; i++) {
			hash[i] = H[i];
		}

		//each time we process 1 block (64 byte)
		for (int offset = 0; offset < total_len; offset += 64) {
			// total_len must be the mtp of 64, and one offset corresponds to 512 bits of information
			
			// step1: Prepare the message schedule {Wt}
			for (int i = 0; i < 64; i++) {
				if (i < 16) {
					w[i] = concat_char(offset + (i << 2));
				}
				else {
					w[i] = calc_w(i);
				}
			}	
			
			//step 2: Initialize the eight working variables, a, b, c, d, e, f, g, and h, with the (i-1)st hash value:
			a = hash[0];
			b = hash[1];
			c = hash[2];
			d = hash[3];
			e = hash[4];
			f = hash[5];
			g = hash[6];
			h = hash[7];

			//step 3: Main loop
			for (int i = 0; i < 64; i++) {
				uint32 T1 = h + Sigma1(e) + Ch(e, f, g) + SHA256_K[i] + w[i];
				uint32 T2 = Sigma0(a) + Maj(a, b, c);
				h = g;
				g = f;
				f = e;
				e = d + T1;
				d = c;
				c = b;
				b = a;
				a = T1 + T2;
			}


			//step 4: Compute the ith intermediate hash value H(i):
			hash[0] = a + hash[0];
			hash[1] = b + hash[1];
			hash[2] = c + hash[2];
			hash[3] = d + hash[3];
			hash[4] = e + hash[4];
			hash[5] = f + hash[5];
			hash[6] = g + hash[6];
			hash[7] = h + hash[7];
		}
		//After repeating steps one through four a total of N times(i.e., after processing M(N)), the resulting
		//	256 - bit message digest of the message
		// After N Rounds of hashing, we obtain the message digest: hash
		return hash;
	}

private:
	vector<unsigned char> buf;
	uint32 w[64];
	uint32 a, b, c, d, e, f, g, h;
	uint32 hash[8];

	const uint32 SHA256_K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};
	// For SHA-256, the initial hash value, H(0), shall consist of the following eight 32-bit words
	uint32 H[8] = {
		0x6a09e667,
		0xbb67ae85,
		0x3c6ef372,
		0xa54ff53a,
		0x510e527f,
		0x9b05688c,
		0x1f83d9ab,
		0x5be0cd19
	};
	//rotate right
	uint32 rotr(uint32 x, uint32 n) {
		return (x >> n) | (x << (32 - n));
	}
	
	void intToChars(uint32_t num, char* chars) {
		long long n = (long long)num;
		for (int i = 0; i < 8; ++i) {
			chars[i] = (n >> (i * 8)) & 0xFF;
		}
	}
	// shift right is equivalent to x >> n
	//Suppose that the length of the message, M, is L bits. Append the bit ��1�� to the end of the message, 
	//followed by k zero bits, where k is the smallest, non - negative solution to the equation
	//	L +1+ k= 448 mod 512 .  Then append the 64-bit block that is equal to the number L expressed
	//using a binary representation
	int padding(unsigned char* data, int len) {
		// len is in bytes, fill buf with the padding data
		// First, find the smallest k, so that len + 1 + k = 448 mod 512
		int first_mod = (len * 8 + 1) % 512;
		int k = (first_mod <= 448) ? (448 - first_mod) : (512 - first_mod + 448);
		for (int i = 0; i < len; i++) {
			buf.push_back(data[i]);
		}
		int pad_len = ((k + 1) >> 3);
		for (int i = 0; i < pad_len; i++) {
			if (i == 0) {
				buf.push_back((unsigned char)0x80);
			}
			else {
				buf.push_back((unsigned char)0x00);
			}
		}
		// Finally, 64 more bits for the initial length
		char chars[8];
		intToChars((len << 3), chars);
		for (int i = 7; i >= 0; i--) {
			buf.push_back(chars[i]);
		}
		return buf.size();
	}
};



int main(int argc, char* argv[]) {
	unsigned int* hashp;
	SHA_256 sa;
	unsigned char mmsg[28] = "abcdefghijklmnopqrstuvwxyz";
	string inputfile = "input.txt";
	string outputfile = "output.txt";

	//unsigned char plaintext[185] = "iauboogn iarhgi ansd adn ngkfnkajn kehg asgqeh hthe[]fhh tehw ][g rt][h wt][e h]r[t ]qe[ ]weth[ wt][]th r][j ]r[je[ ]e [][w]h [wj]] ] t[h ]wt[] wj]w[ ]ry[j]yj  w h;' gq,. g. ,,. ,.> < "; // Initial length = 3
	//uint32* digest = sa.calc(plaintext, 184);

	if (argc != 3)
	{
		cout << "invalid parameter : please use sha.exe [inputfile] [outputfile]" << endl;
		return 0;
	}
	else
	{
		inputfile = argv[1];
		outputfile = argv[2];
		cout << " inputfile: " << inputfile << " outputfile: " << outputfile << endl;
	}

	ofstream ofs(outputfile);
	ifstream ifs(inputfile);
	if (!ifs.is_open())
	{
		cout << "sorry can not find file!" << endl;
		return 0;
	}
	stringstream buff;
	string one_byte_data;
	unsigned char tmpval;
	buff << ifs.rdbuf();
	ifs.seekg(0, ios::end);
	int totallen = ifs.tellg();
	totallen = (totallen + 10) / 3;
	ifs.close();
	istringstream hexstrm(buff.str());
	unsigned char* plain_data;
	unsigned char* output_data;
	plain_data = new unsigned char[totallen];
	output_data = new unsigned char[32];
	int count = 0;
	while (hexstrm >> one_byte_data)
	{

		tmpval = ((one_byte_data[1] >= 'a') ? one_byte_data[1] - 'a' + 10 : ((one_byte_data[1] >= 'A') ? one_byte_data[1] - 'A' + 10 : one_byte_data[1] - '0'));
		tmpval += (((one_byte_data[0] >= 'a') ? one_byte_data[0] - 'a' + 10 : ((one_byte_data[0] >= 'A') ? one_byte_data[0] - 'A' + 10 : one_byte_data[0] - '0'))) << 4;
		plain_data[count++] = tmpval;

	}
	clock_t start = clock();
	hashp = sa.calc(plain_data, count);
	//hashp = sa.calc(mmsg, 26);

	clock_t end = clock();
	double elapsed_seconds = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
	cout << "the hash value is :" << endl;
	for (int i = 0; i < 32; ++i) {
		output_data[i] = (unsigned char)((hashp[i / 4] >> (24 - 8 * (i % 4))) & 0xff);
		cout << std::hex << std::setw(2) << std::setfill('0') << (int)output_data[i] << " ";
	}
	cout << endl;
	cout << "SHA 256 Use: " << elapsed_seconds << "ms" << endl;
	cout << "The Speed is: " << 8 * count / 1024 / 1024 / (elapsed_seconds / 1000) << "Mbps" << endl;

	unsigned char out_one_byte[] = "00";
	for (int i = 0; i < 32; i++) {
		unsigned char a = output_data[i];
		out_one_byte[0] = (a >> 4) >= 10 ? (a >> 4) + 'a' - 10 : (a >> 4) + '0';
		out_one_byte[1] = (a & 0x0f) >= 10 ? (a & 0x0f) + 'a' - 10 : (a & 0x0f) + '0';
		ofs << out_one_byte << " ";
	}
	ofs.close();
	delete[] plain_data;
	delete[] output_data;
	return 0;
}
