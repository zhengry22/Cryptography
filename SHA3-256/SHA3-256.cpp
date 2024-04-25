#include <iostream>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

typedef unsigned long long uint64;

class SHA3_256 {
public:
	SHA3_256() {
		init();
	}
	void init() {
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				state[i][j] = 0;
		for (int i = 0; i < 32; i++)
			hash[i] = 0;
		buf.clear();
	}

	void extract_block(int start, uint64* storage) {
		for (int i = start; i < start + r; i+=8) {
			int index = (i - start) >> 3;
			// 136 bytes per round
			/*for (int j = i + 7; j >= i; j--) {
				if (j == i + 7) {
					storage[index] = (uint64)buf[start + j];
				}
				else {
					storage[index] |= ((uint64)buf[start + j] << ((i + 7 - j) << 3));
				}
			}*/
			for (int j = i; j < i + 8; j++) {
				if (j == i) {
					storage[index] = (uint64)buf[j];
				}
				else {
					storage[index] |= ((uint64)buf[j] << ((j - i) << 3));
				}
			}
		}
	}

	unsigned char* calc(unsigned char* data, int len) {
		// Step 1: padding
		int total_length = padding(data, len);

		int block_num = total_length / 136;

		// Need to take heed of how the string is formed by the state:
		// The format is: state[0][0] || state[1][0] || state[2][0] || ... || state[4][0] || state[0][1] || ... || state[0][4] || state[1][4] || ... || state[4][4]
		// Each state[i][j] is a uint64 type variable, which takes 8 bytes, which is 64 bits.

		for (int i = 0; i < block_num; i++) {
			// The first 17 bytes to be Xored with message
			uint64 message_block[17];
			extract_block(r * i, message_block);

			// Impl Xor
			for (int j = 0; j < 17; j++) {
				int row = j % 5;
				int col = j / 5;
				state[row][col] = state[row][col] ^ message_block[j];
			}

			keccakF();
		}
		
		squeeze();

		return hash;
	}

private:
	const uint64 RC[24] = {
		0x0000000000000001, 0x0000000000008082, 0x800000000000808a, 0x8000000080008000,
		0x000000000000808b, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
		0x000000000000008a, 0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
		0x000000008000808b, 0x800000000000008b, 0x8000000000008089, 0x8000000000008003,
		0x8000000000008002, 0x8000000000000080, 0x000000000000800a, 0x800000008000000a,
		0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
	};
	int rho_table[5][5] = { { 0, 36, 3, 105, 210},\
							{ 1, 300, 10, 45, 66},\
							{ 190, 6, 171, 15, 253},\
							{ 28, 55, 153, 21, 120},\
							{ 91, 276, 231, 136, 78} };
	int pi_table[5][5] = { {0, 3, 1, 4, 2},\
						   {6, 9, 7, 5, 8}, \
						   {12, 10, 13, 11, 14},\
						   {18, 16, 19, 17, 15},\
						   {24, 22, 20, 23, 21}};
	uint64 state[5][5];
	unsigned char hash[32];
	int r = 136; //rate 136*8 bit
	int b = 200; //block 200*8 bit
	vector<unsigned char> buf;

	int padding(unsigned char* data, int len) {
		// In SHA3-256 standard, we pad the message with 10*1.
		// Following the rules of byte padding, first calculate q. note that the rate r used in this calculation stands for the number of bytes we use
		for (int i = 0; i < len; i++) {
			buf.push_back(data[i]);
		}
		int q = r - (len % r);
		if (q == 1) {
			buf.push_back((unsigned char)(0x86));
		}
		else if (q == 2) {
			buf.push_back((unsigned char)(0x06));
			buf.push_back((unsigned char)(0x80));
		}
		else {
			buf.push_back((unsigned char)(0x06));
			for (int i = 0; i < q - 2; i++) {
				buf.push_back((unsigned char)(0x00));
			}
			buf.push_back((unsigned char)(0x80));
		}
		return buf.size();
	}

	uint64 C(uint64 x, uint64 tmp_state[5][5]) {
		uint64 ret;
		for (int i = 0; i < 5; i++) {
			if (i == 0) {
				ret = tmp_state[x][i];
			}
			else {
				ret ^= tmp_state[x][i];
			}
		}
		return ret;
	}

	//rotate left function
	uint64 rotl(const uint64 val, unsigned int pos) {
		return (val << pos) | (val >> (64 - pos));
	}

	//rotate right function
	uint64 rotr(uint64 x, uint64 n) {
		return (x >> n) | (x << (64 - n));
	}

	void theta() {
		uint64 tmp_state[5][5];
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				tmp_state[i][j] = state[i][j];
			}
		}
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				state[i][j] = tmp_state[i][j] ^ C((i + 4) % 5, tmp_state) ^ rotl(C((i + 1) % 5, tmp_state), 1);
			}
		}
	}

	void Rou() {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				state[i][j] = rotl(state[i][j], rho_table[i][j]);
			}
		}
	}

	void Pi() {
		// We can also build a pi table ahead of time
		uint64 tmp = state[1][0];
		int this_index = 1, next_index = pi_table[1][0];
		while (next_index != 1) {
			state[this_index % 5][this_index / 5] = state[next_index % 5][next_index / 5];
			this_index = next_index;
			next_index = pi_table[this_index % 5][next_index / 5];
		}
		state[this_index % 5][this_index / 5] = tmp;
	}

	void Chi() {
		uint64 tmp_state[5];
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				tmp_state[j] = state[j][i];
			}
			for (int j = 0; j < 5; j++) {
				state[j][i] = tmp_state[j] ^ (~tmp_state[(j + 1) % 5] & tmp_state[(j + 2) % 5]);
			}
		}
	}

	void IotA(int round) {
		state[0][0] ^= RC[round];
	}

	void keccakF() {
		for (int i = 0; i < 24; i++) {
			// Impl 5 functions
			theta();
			Rou();
			Pi();
			Chi();
			IotA(i);
		}
	}

	void squeeze() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 8; j++) {
				hash[(i << 3) + j] = (unsigned char)(state[i][0] >> (j << 3)) & (unsigned char)(0xFF);
			}
		}
	}

};

int main(int argc, char* argv[]) {
	SHA3_256 sh;
    unsigned char* hashp;
	// int datalen = 121; 
	// //unsigned char aa[300] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	// //unsigned char aa[2] = "";
	// unsigned char aa[130] = "ego noef[][g g[] [ad[] sagr k [] tear[y ]et[h]df[g r]u 4w6] w4[t] e]hety[ ]3yea][ha [h ukyh ts h;rs'jem; .lm./,m ljlejy;s";

	// unsigned char* p;
	// p = sh.calc(aa, datalen);
	// for (int i = 0; i < 32; i++)
	// 	cout << std::hex << std::setw(2) << std::setfill('0') << (int)p[i] << " ";
	// cout << endl;
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
	hashp = sh.calc(plain_data, count);
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
