#include <iostream>
#include <string>
using namespace std;

// Take heed that the letters that we push in the Rotor machine should be uppercased 
const string mapping_rotor_3 = "BDFHJLCPRTXVZNYEIWGAKMUSQO";

class Rotor{
private:
    string mapping; // The mapping pattern that is fixed for each rotor
    char position; // The position for each rotor
    char ring_setting; // "Minus initial position"

    void rotate() {
        if (position != 'Z') {
            position++;
        }
        else {
            position = 'A';
        }
    }

    char cyc_plus(char s) {
        // Plus position
        char retval = s + ((position - 'A') - (ring_setting - 'A'));
        if (retval > 'Z') {
            retval -= 26;
        }
        return retval;
    }

    char cyc_minus(char s) {
        // Minus position
        char retval = s - ((position - 'A') - (ring_setting - 'A'));
        if (retval < 'A') {
            retval += 26;
        }
        return retval;
    }
public:
    Rotor(const string mapping, char position, char ring_setting): mapping(mapping), position(position), ring_setting(ring_setting){
        // Note that the string we use for mapping should have a length of 26
    }

    char convert(char input) {
        // First, rotate the rotor
        this->rotate();
        char tmp1 = cyc_plus(input);
        char tmp2 = mapping[tmp1 - 'A'];
        char tmp3 = cyc_minus(tmp2);
        cout << tmp3;
        return tmp3;
    }
};

int main() {
    Rotor number_3(mapping_rotor_3, 'A');
    for (char i = 'A'; i <= 'F'; i++) {
        number_3.convert(i);
    }
    return 0;
}