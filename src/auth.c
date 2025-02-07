#include <termios.h>
#include "header.h"

int getNextUserId() {
    FILE *fp = fopen(USERS, "r");
    int maxId = 0;
    struct User temp;
    
    if (fp == NULL) {
        return 1; // Start with ID 1 if file doesn't exist
    }
    
    while (fscanf(fp, "%d %s %s", &temp.id, temp.name, temp.password) != EOF) {
        if (temp.id > maxId) {
            maxId = temp.id;
        }
    }
    
    fclose(fp);
    return maxId + 1;
}

int saveUserToDb(const char* username, const char* password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    
    // Hash the password
    hashPassword(password, hashed_password);
    
    // Open database
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return -1;
    }
    
    // Prepare SQL statement
    const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }
    
    // Bind values
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_password, -1, SQLITE_STATIC);
    
    // Execute statement
    int rc = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return (rc == SQLITE_DONE) ? 0 : -1;
}

// Modify the registerMenu function in auth.c
void registerMenu(char name[50], char pass[50]) {
    struct termios oflags, nflags;
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    FILE *fp;
    
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
    
    // Get password (with hidden input)
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

    // Initialize database if it doesn't exist
    if (initDatabase() != 0) {
        printf("\n\t\tError initializing database!\n");
        return;
    }
    
    // Save to database
    if (saveUserToDb(name, pass) != 0) {
        printf("\n\t\tError saving to database! Please try again.\n");
        return;
    }
    
    // Save to text file
    fp = fopen(USERS, "a");
    if (fp == NULL) {
        printf("\n\t\tError opening users file!\n");
        return;
    }
    
    int userId = getNextUserId();
    hashPassword(pass, hashed_password);
    fprintf(fp, "%d %s %s\n", userId, name, hashed_password);
    fclose(fp);
    
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

void loginMenu(char name[50], char pass[50]) {
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System");
    printf("\n\t\t\t\t\t User Login: ");
    scanf("%s", name);
    clearInputBuffer();  // Clear any extra input

    // Only disable echo for password input
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return;
    }

    printf("\n\n\n\n\n\t\t\t\tEnter the password to login: ");
    scanf("%s", pass);
    clearInputBuffer();

    // Restore terminal settings
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
    }
}

