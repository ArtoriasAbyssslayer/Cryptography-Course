#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26
#define MAX_TEXT_LENGTH 100000  // Ensures we stay within safe bounds
#define THRESHOLD_ALPHA 0.06667 // Threshold for Index of Coincidence (for English text)

// Function to load text from a file safely
int load_text_from_file(const char *filepath, char *buffer, int max_length) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", filepath);
        return -1;
    }

    int length = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF && length < max_length - 1) {
        if (isalpha(ch)) {
            buffer[length++] = toupper(ch); // Store only alphabetic characters, uppercase
        }
    }
    buffer[length] = '\0'; // Null-terminate the string

    fclose(file);
    return length;
}

// Calculate the Index of Coincidence for a given text segment
double calculate_index_of_coincidence(const char *segment, int length) {
    int frequency[ALPHABET_SIZE] = {0};
    int total_pairs = length * (length - 1);
    int total_coincidences = 0;

    // Count occurrences of each letter
    for (int i = 0; i < length; i++) {
        if (isalpha(segment[i])) {
            frequency[toupper(segment[i]) - 'A']++;
        }
    }

    // Calculate coincidences
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        total_coincidences += frequency[i] * (frequency[i] - 1);
    }

    // Return Index of Coincidence
    return (double) total_coincidences / total_pairs;
}

// Divide ciphertext into segments based on key length and check IC
// Friedman test is implemented based on Textbook notes
int friedman_test(const char *ciphertext, int text_length, double alpha) {
    int rho = 2; // Initial key length guess
    bool found = false;

    while (!found && rho < text_length) {
        // For each potential key length, create a column vector
        for (int i = 0; i < rho; i++) {
            char segment[MAX_TEXT_LENGTH / ALPHABET_SIZE];
            int segment_length = 0;

            // Construct the i-th column (characters spaced by rho)
            for (int j = i; j < text_length; j += rho) {
                segment[segment_length++] = ciphertext[j];
            }
            segment[segment_length] = '\0';

            // Calculate the Index of Coincidence for this column
            double ic = calculate_index_of_coincidence(segment, segment_length);

            // Check if IC is close to alpha
            if (ic >= alpha - 0.001 && ic <= alpha + 0.001) {
                found = true;
                return rho;
            }
        }
        rho++;
    }

    return -1; // If no key length is found
}
char find_most_common_letter(const char *segment, int length) {
    int frequency[ALPHABET_SIZE] = {0};
    int max_freq = 0;
    char most_common = 'A';

    // Count occurrences of each letter
    for (int i = 0; i < length; i++) {
        if (isalpha(segment[i])) {
            int index = toupper(segment[i]) - 'A';
            frequency[index]++;
            if (frequency[index] > max_freq) {
                max_freq = frequency[index];
                most_common = 'A' + index;
            }
        }
    }

    return most_common;
}
//estimate key function based on estimated key length
void estimate_key(const char *ciphertext, int text_length, int key_length, char *key){
    for(int i = 0; i < key_length; i++){
        char segment[MAX_TEXT_LENGTH/ALPHABET_SIZE] = {0};
        int segment_length = 0;
        
        // Extract the i-th segment from the ciphertext
        for (int j=i; j < text_length; j+=key_length){
            segment[segment_length++] = ciphertext[j];
        }
        // Find most common letter in th segment
        char most_common =  find_most_common_letter(segment, segment_length);
        // Assumming the most common letter in English alphabet is 'E' 
        key[i] = (most_common - 'E' + ALPHABET_SIZE) % ALPHABET_SIZE + 'A';
    }
    key[key_length] ='\0'; //null terminate key string

}
// decrypt function
void vigenere_decrypt(const char *ciphertext, char* plaintext, const char* key, int text_length, int estimated_key_length){
    for(int i=0; i < text_length; i++){
        char shift = key[i % estimated_key_length] - 'A';
        plaintext[i] = ((ciphertext[i]-'A'-shift+ALPHABET_SIZE)% ALPHABET_SIZE) + 'A';
    }
    plaintext[text_length] = '\0'; // Null-terminate the plaintext
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    // Allocate memory for the ciphertext
    char ciphertext[MAX_TEXT_LENGTH] = {0};  // Initialize to zero
    const char *filepath = argv[1];
    double alpha = THRESHOLD_ALPHA;

    // Load ciphertext from file
    int length = load_text_from_file(filepath, ciphertext, MAX_TEXT_LENGTH);
    if (length == -1) {
        return -1; // Exit if file loading failed
    }

    // Estimate key length
    int estimated_key_length = friedman_test(ciphertext, length, alpha);

    if (estimated_key_length != -1) {
        printf("Estimated Key Length: %d\n", estimated_key_length);
    } else {
        printf("Key length could not be determined.\n");
    }
    char plaintext[MAX_TEXT_LENGTH];
    char key[estimated_key_length + 1];
    estimate_key(ciphertext,length,estimated_key_length,key);
    printf("Estimated Key: %s\n", key);
    vigenere_decrypt(ciphertext,plaintext,key,length,estimated_key_length);
    printf("Decrypted Text: %s\n", plaintext);
    return 0;
}
