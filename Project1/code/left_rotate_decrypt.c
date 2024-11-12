// c = m XOR (m rotate_left(6)) XOR (m rotate_left(10))
#include <stdio.h>
#include <stdint.h>
// Function that rotates left
uint16_t left_rotate(uint16_t num, int d){
	return (num << d) | (num >> (16-d));
}

// Encoding function 
uint16_t encode(uint16_t msg){
	return msg ^ left_rotate(msg,6) ^ left_rotate(msg,10);
}


// Decoding Function
uint16_t decode(uint16_t cipher){
	uint16_t msg = cipher;
	/*
		Why It Works
		This approach leverages the fact that repeated application of XOR with rotated values will gradually “undo” the original encoding formula. The number 5 is chosen because, experimentally, it’s sufficient for 16-bit values. 
		If we used more iterations, the result would remain the same.
	*/
	for(int i = 0; i < 3; i++){
		msg = cipher ^ left_rotate(msg,6) ^ left_rotate(msg,10);
	}
	return msg;
}


int main() {
    // Example
    uint16_t test_values[] = {0x1F3A, 0x3BCD, 0xA55A, 0x0001, 0xFFFF};
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);

    for (int i = 0; i < num_tests; i++) {
        uint16_t m = test_values[i];
        uint16_t c = encode(m);
        uint16_t decoded_m = decode(c);
        printf("Original m: 0x%04X, Encoded c: 0x%04X, Decoded m: 0x%04X\n", m, c, decoded_m);
    }

    return 0;
}
