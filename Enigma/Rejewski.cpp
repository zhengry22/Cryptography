#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
// #define MYDEBUG
using namespace std;

const string test = "BDFHJLCPRTXVZNYEIWGAKMUSQO";

class Rotor {
private:
    string mapping;
    string reverse_mapping;
    char position;
    char ring_setting;
    char TNP;
    bool if_rotate;

    char cyc_plus(char s) {
        char retval = s + ((position - 'A') - (ring_setting - 'A'));
        if (retval > 'Z') {
            retval -= 26;
        }
        else if (retval < 'A') {
            retval += 26;
        }
        return retval;
    }

    char cyc_minus(char s) {
        char retval = s - ((position - 'A') - (ring_setting - 'A'));
        if (retval < 'A') {
            retval += 26;
        }
        else if (retval > 'Z') {
            retval -= 26;
        }
        return retval;
    }
public:
    char get_position() {
        return this->position;
    }

    void set_position(char pos) {
        position = pos;
    }

    void rotate() {
        if (position != 'Z') {
            position++;
        }
        else {
            position = 'A';
        }
    }

    Rotor(const string& mapping, char position, char ring_setting, char TNP, bool if_rotate) :
        mapping(mapping), position(position), ring_setting(ring_setting), TNP(TNP), if_rotate(if_rotate) {
        this->reverse_mapping.resize(mapping.length());
        for (int i = 0; i < mapping.length(); i++) {
            reverse_mapping[mapping[i] - 'A'] = 'A' + i;
        }
    }

    char convert(char input) {
        char tmp1 = cyc_plus(input);
        char tmp2 = mapping[tmp1 - 'A'];
        char tmp3 = cyc_minus(tmp2);
#ifdef MYDEBUG
        cout << tmp3;
#endif
        return tmp3;
    }

    char reverse_convert(char input) {
        char tmp1 = cyc_plus(input);
        char tmp2 = reverse_mapping[tmp1 - 'A'];
        char tmp3 = cyc_minus(tmp2);
#ifdef MYDEBUG
        cout << tmp3;
#endif
        return tmp3;
    }
};

class Enigma {
private:
    // 4 Settings
    const string mapping_rotor[4] = { "", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", "AJDKSIRUXBLHWTMCQGZNPYFVOE", "BDFHJLCPRTXVZNYEIWGAKMUSQO" };

    // TNPs
    const char TNPs[4] = { ' ', 'Q', 'E', 'V' };

    // Rotors
    Rotor rotor1, rotor2, rotor3; // rotor1 is the slowest, rotor 2 is the medium one, rotor 3 is the fastest
    int numbers1, numbers2, numbers3; // Numbers of each rotor
    // Plugboard
    char plugboard[26];

    // Reflector
    const string reflector = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

    void Rotate_rotors() {
        char pos1 = rotor1.get_position();
        char pos2 = rotor2.get_position();
        char pos3 = rotor3.get_position();
        rotor3.rotate(); // Whatever the situation is, rotor3 must rotate
        bool carry = false; // Note that their might be a possibility that all three rotors rotate at the same time
        if (pos3 == TNPs[numbers3]) {
            rotor2.rotate();
            carry = true;
        }
        if (pos2 == TNPs[numbers2]) {
            if (!carry) { rotor2.rotate(); }
            rotor1.rotate();
        }
    }
public:
    Enigma(int first, int second, int third, char posi_1, char posi_2, char posi_3, char ring_set_1, char ring_set_2, char ring_set_3) :
        rotor1(mapping_rotor[first], posi_1, ring_set_1, TNPs[first], false),
        rotor2(mapping_rotor[second], posi_2, ring_set_2, TNPs[second], false),
        rotor3(mapping_rotor[third], posi_3, ring_set_3, TNPs[third], true),
        numbers1(first), numbers2(second), numbers3(third) {
        for (int i = 0; i < 26; i++) {
            plugboard[i] = 'A' + i;
        }
    }

    char convert(char input) {
        // Rotate if needed
        Rotate_rotors();
#ifdef MYDEBUG
        cout << rotor1.get_position() << " " << rotor2.get_position() << " " << rotor3.get_position() << endl;
#endif  
        // plugboard
        input = plugboard[input - 'A'];

        // converting
        char tmp3 = rotor3.convert(input);
        char tmp2 = rotor2.convert(tmp3);
        char tmp1 = rotor1.convert(tmp2);

        // reflect tmp1
        char reflected = reflector[tmp1 - 'A'];
#ifdef MYDEBUG
        cout << reflected;
#endif
        // reverse_converting
        char rev1 = rotor1.reverse_convert(reflected);
        char rev2 = rotor2.reverse_convert(rev1);
        char rev3 = rotor3.reverse_convert(rev2);

        char output = plugboard[rev3 - 'A'];
        // cout << endl << "Final conversion: " << output << endl;
        return output;
    }

    void plugboard_setting() {
        cout << "Set your plugboard: enter the number you want to shift and then the pairs" << endl;
        int n;
        cin >> n;
        char a, b;
        for (int i = 0; i < n; i++) {
            cin >> a >> b;
            plugboard[a - 'A'] = b;
            plugboard[b - 'A'] = a;
        }
    }

    void receive_input() {
        cout << "Input the string that you want to encrypt: " << endl;
        string input;
        cin >> input;
        for (int i = 0; i < input.length(); i++) {
            this->convert(input[i]);
        }
    }

    string show_rotor() {
        string a;
        a.push_back(this->rotor1.get_position());
        a.push_back(this->rotor2.get_position());
        a.push_back(this->rotor3.get_position());
        return a;
    }

    void set_rotor_pos(string pos) {
        rotor1.set_position(pos[0]);
        rotor2.set_position(pos[1]);
        rotor3.set_position(pos[2]);
    }

    string convert_string(string input) {
        string retval;
        for (int i = 0; i < input.length(); i++) {
            retval.push_back(convert(input[i]));
        }
        return retval;
    }
};

char cyc_shift(char a, int shift) {
    char retval = a + shift;
    if (retval < 'A') { retval += 26; }
    else if (retval > 'Z') { retval -= 26; }
    return retval;
}

int array_1_4[26] = { 5, 5, 1, 6, 5, 6, 6, 5, 5, 5, 1, 5, 6, 5, 6, 5, 1, 6, 5, 6, 6, 6, 6, 1, 6, 6 }; // 2 个 5，2 个 6，4 个 1
vector<int> length_1_4 = { 1, 1, 1, 1, 5, 5, 6, 6};
int array_2_5[26] = { 4, 4, 4, 4, 4, 4, 2, 3, 4, 4, 4, 3, 4, 2, 2, 3, 4, 4, 3, 3, 3, 2, 4, 4, 4, 4 }; // 4 个 4，2 个 3，2 个 2
vector<int> length_2_5 = { 2, 2, 3, 3, 4, 4, 4, 4};
int array_3_6[26] = { 13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13 }; // 2 个 13
vector<int> length_3_6 = { 13, 13 };

char next_1_4[26] = { 'E','L','C','O','N','W','D','I','A','P','K','S','Z','H','F','B','Q','T','J','Y','R','G','V','X','M','U' };
char next_2_5[26] = { 'M','R','W','J','F','D','V','S','Q','E','X','U','C','O','N','H','B','I','P','L','T','G','A','Y','Z','K' };
char next_3_6[26] = { 'W','A','D','F','R','P','O','L','N','T','V','C','H','M','Y','B','J','Q','I','G','E','U','S','K','Z','X' };


int main() {
    // For all the possibilities
    std::ofstream outfile("DecryptEnigma.txt");
    std::streambuf* old_cout_streambuf = std::cout.rdbuf();
    std::cout.rdbuf(outfile.rdbuf());
    cout << "Combinations that satisfies the condition: " << endl;

    // 首先要先找到1-4, 2-5, 3-6 的循环圈

    for (int first_rotor = 1; first_rotor <= 3; first_rotor++) {
        for (int second_rotor = 1; second_rotor <= 3; second_rotor++) {
            for (int third_rotor = 1; third_rotor <= 3; third_rotor++) {
                if ((first_rotor == second_rotor)
                    || (second_rotor == third_rotor)
                    || (third_rotor == first_rotor)) {
                    continue;
                }
                // 在每一种给定的转子排列下
                for (char init_pos1 = 'A'; init_pos1 <= 'Z'; init_pos1++) {
                    for (char init_pos2 = 'A'; init_pos2 <= 'Z'; init_pos2++) {
                        for (char init_pos3 = 'A'; init_pos3 <= 'Z'; init_pos3++) {
                            Enigma enigma1(first_rotor, second_rotor, third_rotor, init_pos1, init_pos2, init_pos3, 'D', 'E', 'S');
                            bool flag0 = false;

                            char actual_1_4[26], actual_2_5[26], actual_3_6[26]; // 记录每个环下的字母是什么
                            bool check_1_4[26], check_2_5[26], check_3_6[26]; // 作为环长度校验使用
                            for (int i = 0; i < 26; i++) {
                                actual_1_4[i] = 0;
                                actual_2_5[i] = 0;
                                actual_3_6[i] = 0;
                                check_1_4[i] = 0;
                                check_2_5[i] = 0;
                                check_3_6[i] = 0;
                            }

                            for (char letter = 'A'; letter <= 'Z'; letter++) {
                                // 每一次都要先重置转子
                                string pos_reset;
                                pos_reset.push_back(init_pos1);
                                pos_reset.push_back(init_pos2);
                                pos_reset.push_back(init_pos3);
                                enigma1.set_rotor_pos(pos_reset);

                                string input;
                                for(int i = 0; i < 6; i++) {
                                    input.push_back(letter);
                                }

                                string enc = enigma1.convert_string(input);
                                actual_1_4[enc[0] - 'A'] = enc[3];
                                actual_2_5[enc[1] - 'A'] = enc[4];
                                actual_3_6[enc[2] - 'A'] = enc[5];                                
                            }

                            vector<int> vec_1_4, vec_2_5, vec_3_6;

                            // 然后统计圈长
                            for (int i = 0; i < 26; i++) {
                                if (check_1_4[i]) { continue; }
                                int cnt = 0;
                                bool flag = 0;
                                char next = 'A' + i;
                                while(next != ('A' + i) || (!flag)) {
                                    check_1_4[next - 'A'] = true;
                                    next = actual_1_4[next - 'A'];
                                    cnt++;
                                    if (next == ('A' + i)) {flag = true;}
                                }
                                vec_1_4.push_back(cnt);
                            }

                            for (int i = 0; i < 26; i++) {
                                if (check_2_5[i]) { continue; }
                                int cnt = 0;
                                bool flag = 0;
                                char next = 'A' + i;
                                while(next != ('A' + i) || (!flag)) {
                                    check_2_5[next - 'A'] = true;
                                    next = actual_2_5[next - 'A'];
                                    cnt++;
                                    if (next == ('A' + i)) {flag = true;}
                                }
                                vec_2_5.push_back(cnt);
                            }

                            for (int i = 0; i < 26; i++) {
                                if (check_3_6[i]) { continue; }
                                int cnt = 0;
                                bool flag = 0;
                                char next = 'A' + i;
                                while(next != ('A' + i) || (!flag)) {
                                    check_3_6[next - 'A'] = true;
                                    next = actual_3_6[next - 'A'];
                                    cnt++;
                                    if (next == ('A' + i)) {flag = true;}
                                }
                                vec_3_6.push_back(cnt);
                            }

                            // 判断vec_1_4 与 length_1_4 是否相等，剩下两个也是同理
                            sort(vec_1_4.begin(), vec_1_4.end());
                            sort(vec_2_5.begin(), vec_2_5.end());
                            sort(vec_3_6.begin(), vec_3_6.end());
                            if (equal(vec_1_4.begin(), vec_1_4.end(), length_1_4.begin(),length_1_4.end())
                            && equal(vec_2_5.begin(), vec_2_5.end(), length_2_5.begin(),length_2_5.end())
                            && equal(vec_3_6.begin(), vec_3_6.end(), length_3_6.begin(),length_3_6.end())) {
                                cout << first_rotor << " " << second_rotor << " " << third_rotor << " "
                                << init_pos1 << " " << init_pos2 << " " << init_pos3 << endl;
                            }


                        }
                    }
                }
            }
        }
    }
}