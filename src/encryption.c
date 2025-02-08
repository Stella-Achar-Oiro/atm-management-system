#include "header.h"
#include <unistd.h>

void generateSalt(char* salt) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    int charset_length = strlen(charset);
    
    // Seed the random number generator
    srand(time(NULL) ^ (getpid() << 16));
    
    // Generate random salt
    for (int i = 0; i < SALT_LENGTH - 1; i++) {
        salt[i] = charset[rand() % charset_length];
    }
    salt[SALT_LENGTH - 1] = '\0';
}

void hashPassword(const char* password, char* output) {
    char salt[SALT_LENGTH];
    char settings[32];
    
    // Generate salt
    generateSalt(salt);
    
    // Create bcrypt settings string
    snprintf(settings, sizeof(settings), "$2b$%02d$%s", BCRYPT_COST, salt);
    
    // Hash the password using bcrypt
    char* hash = crypt(password, settings);
    if (hash == NULL) {
        fprintf(stderr, "Error hashing password\n");
        strncpy(output, "", HASH_LENGTH - 1);
        return;
    }
    
    // Copy the hash to output
    strncpy(output, hash, HASH_LENGTH - 1);
    output[HASH_LENGTH - 1] = '\0';
}

int verifyPassword(const char* password, const char* hash) {
    char* computed_hash = crypt(password, hash);
    if (computed_hash == NULL) {
        return 0;  // Error in hashing
    }
    return strcmp(computed_hash, hash) == 0;
}