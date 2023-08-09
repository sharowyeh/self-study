// just a entry for the code playground...

#include <iostream>

void binary_fractions(double val) {
    printf("double:%f, ", val);
    // learned before but not memory well, https://www.electronics-tutorials.ws/binary/binary-fractions.html
    
    // integer parts divide 2 recursively to get quotient for 2^1/2/4/8 bits (LSB->MSB)
    // fraction parts multipy 2 recursively to get integer for 2^-1/-2/-4/-8 bits (MSB->LSB)
    unsigned char integer_hex = 0;
    unsigned char fraction_hex = 0;

    int integer_part = (int)val;
    double fraction_part = val - integer_part;
    // only support a unsigned char
    for (int i = 0; i < 7; i++) {
        if (integer_part % 2 != 0) {
            integer_hex += (1 << i);
        }
        integer_part /= 2;
    }
    for (int i = 7; i >= 0; i--) {
        fraction_part *= 2;
        if (fraction_part >= 1.0f) {
            fraction_hex += (1 << i);
            fraction_part -= 1.0f;
        }
    }
    printf("integer:0x%02x, fraction:0x%02x\n", integer_hex, fraction_hex);
}

int main()
{
    std::cout << "Hello World!\n";

    binary_fractions(54.6875);
    binary_fractions(0.91415926789);
}
