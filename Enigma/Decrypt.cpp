#include <iostream>
#include <string>
#include <fstream>
#include <vector>
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
        } else if (retval < 'A') {
            retval += 26;
        }
        return retval;
    }

    char cyc_minus(char s) {
        char retval = s - ((position - 'A') - (ring_setting - 'A'));
        if (retval < 'A') {
            retval += 26;
        } else if (retval > 'Z') {
            retval -= 26;
        }
        return retval;
    }
public:
    char get_position() {
        return this->position;
    }

    void rotate() {
        if (position != 'Z') {
            position++;
        }
        else {
            position = 'A';
        }
    }

    Rotor(const string& mapping, char position, char ring_setting, char TNP, bool if_rotate): 
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
    const string mapping_rotor[4] = {"", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", "AJDKSIRUXBLHWTMCQGZNPYFVOE", "BDFHJLCPRTXVZNYEIWGAKMUSQO"};

    // TNPs
    const char TNPs[4] = {' ', 'Q', 'E', 'V'};

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
            if (!carry) {rotor2.rotate();}          
            rotor1.rotate();
        }
    }
public:
    Enigma(int first, int second, int third, char posi_1, char posi_2, char posi_3, char ring_set_1, char ring_set_2, char ring_set_3):
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
    if (retval < 'A') {retval += 26;}
    else if (retval > 'Z') {retval -= 26;}
    return retval;
}

int main() {
    // For all the possibilities
    std::ofstream outfile("DecryptEnigma.txt");
    std::streambuf* old_cout_streambuf = std::cout.rdbuf();
    std::cout.rdbuf(outfile.rdbuf());
    cout << "Combinations that satisfies the condition: " << endl;
    
    string cipher_text = "E****A*A****ETT**G*";
    string encrypt_text = "L****O*O****GHH**L*";

    vector<string> msgvec, encvec;
    
    for (int i = 0; i < 26; i++) {
        string txt = cipher_text;
        for (auto & e : txt) {
            if (e != '*') {
                e = cyc_shift(e, i);
            }
        }
        msgvec.push_back(txt);
    }

    for (int i = 0; i < 26; i++) {
        string txt = encrypt_text;
        for (auto & e : txt) {
            if (e != '*') {
                e = cyc_shift(e, i);
            }
        }
        encvec.push_back(txt);
    }

    for (int first_rotor = 1; first_rotor <= 3; first_rotor++) {
        for (int second_rotor = 1; second_rotor <= 3; second_rotor++) {
            for (int third_rotor = 1; third_rotor <= 3; third_rotor++) {
                if ((first_rotor == second_rotor)
                    || (second_rotor == third_rotor)
                    || (third_rotor == first_rotor)) {continue;}
                // 在每一种给定的转子排列下
                for (char init_pos1 = 'A'; init_pos1 <= 'Z'; init_pos1++) {
                    for (char init_pos2 = 'A'; init_pos2 <= 'Z'; init_pos2++) {
                        for (char init_pos3 = 'A'; init_pos3 <= 'Z'; init_pos3++) {

                            Enigma enigma1(first_rotor, second_rotor, third_rotor, init_pos1, init_pos2, init_pos3, 'D', 'E', 'S');
                            Enigma enigma2(enigma1);
                            Enigma enigma3(enigma1);
                            Enigma enigma4(enigma1);
                            Enigma enigma5(enigma1);
                            Enigma enigma6(enigma1);
                            for (int i = 0; i < 26; i++) {
                                string tmpmsg = msgvec[i];
                                string first_convert = enigma1.convert_string(tmpmsg);
                                // 进行变换
                                tmpmsg[7] = first_convert[5];
                                tmpmsg[14] = first_convert[13];
                                tmpmsg[17] = first_convert[0];
                                // 再变换
                                string second_convert = enigma2.convert_string(tmpmsg);
                                if (tmpmsg[5] == second_convert[7] && tmpmsg[13] == second_convert[14]) {
                                    tmpmsg[12] = second_convert[17];
                                    string third_convert = enigma3.convert_string(tmpmsg);
                                    if (tmpmsg[0] == third_convert[12]) {
                                        // Consider the next type of strings
                                        string revmsg = encvec[i];
                                        string rev_first_convert = enigma4.convert_string(revmsg);
                                        revmsg[7] = rev_first_convert[5];
                                        revmsg[14] = rev_first_convert[13];
                                        revmsg[12] = rev_first_convert[0];

                                        string rev_second_convert = enigma5.convert_string(revmsg);
                                        if (revmsg[5] == rev_second_convert[7] || revmsg[13] == rev_second_convert[14]) {
                                            // Last judgement!
                                            revmsg[17] = rev_second_convert[12];
                                            string rev_third_convert = enigma6.convert_string(revmsg);
                                            if (revmsg[0] == rev_third_convert[17]) {
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
            }
        }
    }
}