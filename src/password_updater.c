#include "header.h"
#include <unistd.h>
#include <ctype.h>

// Global buffer for password storage
static char stored_password[SHA256_DIGEST_LENGTH * 2 + 1];

int updatePasswordsInFile() {
    FILE *original, *temp;
    struct User user;
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    
    // Open original file for reading
    original = fopen(USERS, "r");
    if (original == NULL) {
        printf("Error opening users file!\n");
        return -1;
    }
    
    // Create temporary file
    temp = fopen("./data/users_temp.txt", "w");
    if (temp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(original);
        return -1;
    }
    
    printf("Starting password encryption...\n");
    
    // Read each line and update passwords
    while (fscanf(original, "%d %s %s", &user.id, user.name, user.password) != EOF) {
        // Check if password is already hashed (simple check for length)
        if (strlen(user.password) != SHA256_DIGEST_LENGTH * 2) {
            // Hash the plain text password
            hashPassword(user.password, hashed_password);
            // Write with hashed password
            fprintf(temp, "%d %s %s\n", user.id, user.name, hashed_password);
            printf("Encrypted password for user: %s\n", user.name);
        } else {
            // Password already seems to be hashed, write as is
            fprintf(temp, "%d %s %s\n", user.id, user.name, user.password);
            printf("Password already encrypted for user: %s\n", user.name);
        }
    }
    
    // Close both files
    fclose(original);
    fclose(temp);
    
    // Create backup of original file
    char backup_name[100];
    snprintf(backup_name, sizeof(backup_name), "%s.bak", USERS);
    rename(USERS, backup_name);
    
    // Replace original with temp file
    rename("./data/users_temp.txt", USERS);
    
    printf("\nPassword encryption completed!\n");
    printf("Original file backed up as: %s\n", backup_name);
    
    return 0;
}

// Function to detect if a string is likely a SHA-256 hash
int isLikelyHash(const char* str) {
    // SHA-256 hash is 64 characters long in hex
    if (strlen(str) != 64) {
        return 0;
    }
    
    // Check if all characters are valid hex digits
    for (int i = 0; i < 64; i++) {
        if (!isxdigit((unsigned char)str[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Modified function name to avoid conflicts
const char *getEncryptedPassword(struct User u) {
    FILE *fp = fopen(USERS, "r");
    struct User userChecker;
    
    if (fp == NULL) {
        printf("Error opening users file!\n");
        return "no user found";
    }
    
    while (fscanf(fp, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF) {
        if (strcmp(userChecker.name, u.name) == 0) {
            fclose(fp);
            if (isLikelyHash(userChecker.password)) {
                // Password is already hashed, copy to global buffer
                strncpy(stored_password, userChecker.password, sizeof(stored_password) - 1);
                stored_password[sizeof(stored_password) - 1] = '\0';
                return stored_password;
            } else {
                // Hash the password before returning
                hashPassword(userChecker.password, stored_password);
                return stored_password;
            }
        }
    }
    
    fclose(fp);
    return "no user found";
}