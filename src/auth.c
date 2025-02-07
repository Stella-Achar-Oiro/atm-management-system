#include <termios.h>
#include "header.h"

void registerMenu(char name[50], char pass[50]) {
    struct termios oflags, nflags;
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];  // For hex string
    
    system("clear");
    printf("\n\n\t\t\t==== Register New User ====\n");
    
    printf("\n\n\t\t\tEnter Username: ");
    scanf("%s", name);
    
    // Check if username exists in database
    char temp_pass[50] = "temp";
    hashPassword(temp_pass, hashed_password);
    if (verifyUser(name, hashed_password) != 0) {
        printf("\n\t\tUsername already exists! Please choose another.\n");
        printf("\n\t\tPress any key to continue...");
        getchar();
        getchar();
        registerMenu(name, pass);
        return;
    }
    
    // Disable echo for password input
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return;
    }

    printf("\n\n\t\t\tEnter Password: ");
    scanf("%s", pass);

    // Restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        return;
    }
    
    // Hash password and store in database
    hashPassword(pass, hashed_password);
    if (insertUser(name, hashed_password) < 0) {
        printf("\n\t\tError registering user! Please try again.\n");
        return;
    }
    
    printf("\n\n\t\t\tRegistration successful!\n");
    printf("\n\n\t\t\tPress any key to continue...");
    getchar();
    getchar();
}

const char *getPassword(struct User u) {
    static char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];  // For hex string
    char temp_pass[50];
    
    FILE *fp = fopen(USERS, "r");
    if (fp == NULL) {
        printf("Error opening users file!\n");
        return "no user found";
    }
    
    while (fscanf(fp, "%s %s", u.name, temp_pass) != EOF) {
        if (strcmp(u.name, u.name) == 0) {
            fclose(fp);
            hashPassword(temp_pass, hashed_password);
            return hashed_password;
        }
    }
    
    fclose(fp);
    return "no user found";
}

void loginMenu(char a[50], char pass[50]) {
    struct termios oflags, nflags;
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    scanf("%s", a);

    // Disable echo for password
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return;
    }

    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    scanf("%s", pass);

    // Hash the password for verification
    hashPassword(pass, hashed_password);

    // Verify against database
    if (!verifyUser(a, hashed_password)) {
        printf("\n\t\tInvalid username or password!\n");
        printf("\n\t\tPress any key to continue...");
        getchar();
        getchar();
        loginMenu(a, pass);
        return;
    }

    // Restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        return;
    }
}