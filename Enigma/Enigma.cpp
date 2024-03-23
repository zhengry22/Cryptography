#include <iostream>
#include <string>
#define MYDEBUG
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
        cout << tmp3;
        return tmp3;
    }

    char reverse_convert(char input) {
        char tmp1 = cyc_plus(input);
        char tmp2 = reverse_mapping[tmp1 - 'A'];
        char tmp3 = cyc_minus(tmp2);
        cout << tmp3;
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
        rotor3.rotate();
        if (pos3 == TNPs[numbers3]) {
            rotor2.rotate();
        }
        if (pos2 == TNPs[numbers2]) {
            rotor2.rotate();
            rotor1.rotate();
        }
    }
public:
    Enigma(int first, int second, int third, char posi_1, char posi_2, char posi_3, char ring_set_1, char ring_set_2, char ring_set_3):
        rotor1(mapping_rotor[first], posi_1, ring_set_1, TNPs[first], false),
        rotor2(mapping_rotor[second], posi_2, ring_set_2, TNPs[second], false),
        rotor3(mapping_rotor[third], posi_3, ring_set_3, TNPs[third], true),
        numbers1(first), numbers2(second), numbers3(third) {
    }

    char convert(char input) {
        // Rotate if needed
        Rotate_rotors();
#ifdef MYDEBUG
        cout << rotor1.get_position() << " " << rotor2.get_position() << " " << rotor3.get_position() << endl;
#endif  
        // converting
        char tmp3 = rotor3.convert(input);
        char tmp2 = rotor2.convert(tmp3);
        char tmp1 = rotor1.convert(tmp2);

        // reflect tmp1
        char reflected = reflector[tmp1 - 'A'];
        cout << reflected;

        // reverse_converting
        char rev1 = rotor1.reverse_convert(reflected);
        char rev2 = rotor2.reverse_convert(rev1);
        char rev3 = rotor3.reverse_convert(rev2);
        cout << endl << "Final conversion: " << rev3 << endl;
        return rev3;
    }
};

int main() {
    Enigma enigma(1, 2, 3, 'A', 'D', 'U', 'A', 'E', 'S');
    for (char i = 'A'; i <= 'F'; i++) {
        enigma.convert(i);
    }
    return 0;
}
