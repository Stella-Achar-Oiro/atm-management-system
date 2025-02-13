#include <termios.h>
#include <unistd.h>
#include "header.h"

int getNextUserId() {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int maxId = 0;
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return -1;
    }
    
    const char *sql = "SELECT MAX(id) FROM users;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            maxId = sqlite3_column_int(stmt, 0);
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return maxId + 1;
}

int userExists(const char* username) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int exists = 0;
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return -1;
    }
    
    const char *sql = "SELECT 1 FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = 1;
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return exists;
}

void registerMenu(char username[50], char pass[50]) {
    struct termios oflags, nflags;
    char hashed_password[HASH_LENGTH];
    char confirm_pass[50];
    struct User newUser;
    int c;
    
    // Clear buffers
    memset(username, 0, 50);
    memset(pass, 0, 50);
    memset(confirm_pass, 0, 50);
    memset(hashed_password, 0, sizeof(hashed_password));
    memset(&newUser, 0, sizeof(struct User));  // Initialize newUser struct
    
    // Clear input buffer
    while ((c = getchar()) != '\n' && c != EOF) { }
    
    system("clear");
    printf("\n\n\t\t\t==== Register New User ====\n");

    // Username input with validation
    do {
        printf("\n\n\t\t\tEnter Username (3-50 characters): ");
        if (scanf("%49s", username) != 1) {
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("\n\t\t✖ Invalid input!\n");
            continue;
        }
        while ((c = getchar()) != '\n' && c != EOF) { }
        
        if (strlen(username) < 3) {
            printf("\n\t\t✖ Username too short (minimum 3 characters)!\n");
            continue;
        }
        
        if (userExists(username)) {
            printf("\n\t\t✖ Username already exists! Please choose another.\n");
            continue;
        }
        break;
    } while (1);

    // Password input with hidden characters
    printf("\n\t\t\tEnter Password: ");
    tcgetattr(STDIN_FILENO, &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &nflags);
    scanf("%49s", pass);
    tcsetattr(STDIN_FILENO, TCSANOW, &oflags);
    printf("\n");  // Add newline after hidden password
    
    // Confirm password
    printf("\n\t\t\tConfirm Password: ");
    tcgetattr(STDIN_FILENO, &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &nflags);
    scanf("%49s", confirm_pass);
    tcsetattr(STDIN_FILENO, TCSANOW, &oflags);
    printf("\n");  // Add newline after hidden password
    
    // Check if passwords match
    if (strcmp(pass, confirm_pass) != 0) {
        printf("\n\n\t\t✖ Passwords do not match!\n");
        printf("\n\t\tPress Enter to try again...");
        while ((c = getchar()) != '\n' && c != EOF) { }
        getchar();
        registerMenu(username, pass);
        return;
    }
    
    // Create new user
    strncpy(newUser.username, username, MAX_NAME_LENGTH - 1);
    hashPassword(pass, newUser.password);
    
    if (saveUser(&newUser)) {
        // Get the complete user data including ID
        if (getUser(username, &newUser)) {
            // Update the username and password for the calling function
            strncpy(username, newUser.username, MAX_NAME_LENGTH - 1);
            // Keep original password for login (no need to copy to itself)
            printf("\n\n\t\t✔ Registration successful!\n");
            printf("\n\t\tPress Enter to login...");
            while ((c = getchar()) != '\n' && c != EOF) { }
            getchar();
            return;
        }
    }
    
    // If we get here, something went wrong
    printf("\n\n\t\t✖ Registration failed. Please try again.\n");
    printf("\n\t\tPress Enter to continue...");
    while ((c = getchar()) != '\n' && c != EOF) { }
    getchar();
    registerMenu(username, pass);
    return;
}

const char *getPassword(struct User u) {
    static char hashed_password[HASH_LENGTH];
    sqlite3 *db;
    sqlite3_stmt *stmt;
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return "no user found";
    }
    
    const char *sql = "SELECT password FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, u.username, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *pwd = (const char*)sqlite3_column_text(stmt, 0);
            strncpy(hashed_password, pwd, sizeof(hashed_password) - 1);
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return hashed_password;
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return "no user found";
}

void loginMenu(char name[50], char pass[50]) {
    struct termios oflags, nflags;
    int c;
    char hashed_password[HASH_LENGTH];  // Add this line

    // Clear buffers
    memset(name, 0, 50);
    memset(pass, 0, 50);
    memset(hashed_password, 0, HASH_LENGTH);  // Add this line

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System");
    printf("\n\t\t\t\t\t User Login: ");
    
    if (scanf("%49s", name) != 1) {
        printf("\n\t\t✖ Invalid input!\n");
        return;
    }
    while ((c = getchar()) != '\n' && c != EOF) { }

    // Password input with hidden characters
    printf("\n\n\n\n\n\t\t\t\tEnter the password: ");
    if (tcgetattr(STDIN_FILENO, &oflags) != 0) {
        printf("\n\t\t✖ Terminal error!\n");
        return;
    }
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &nflags) != 0) {
        printf("\n\t\t✖ Terminal error!\n");
        return;
    }

    if (scanf("%49s", pass) != 1) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oflags);
        printf("\n\t\t✖ Invalid input!\n");
        return;
    }

    // Hash the password before verification
    hashPassword(pass, hashed_password);  // Add this line
    strncpy(pass, hashed_password, 49);   // Add this line

    tcsetattr(STDIN_FILENO, TCSANOW, &oflags);
}

