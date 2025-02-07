#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <openssl/sha.h>

// File paths
#define RECORDS "./data/records.txt"
#define USERS "./data/users.txt"
#define DB_FILE "./data/atm.db"


// Validation constants
#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 15
#define MIN_ACCOUNT_NUMBER 10000
#define MAX_ACCOUNT_NUMBER 99999
#define MIN_AMOUNT 0.01
#define MAX_AMOUNT 1000000.00
#define MAX_RETRIES 3

// Validation return codes
#define VALID 0
#define INVALID_DATE -1
#define INVALID_ACCOUNT_NUMBER -2
#define INVALID_PHONE -3
#define INVALID_AMOUNT -4
#define INVALID_COUNTRY -5
#define INVALID_ACCOUNT_TYPE -6

// Existing structs
struct Date {
    int month, day, year;
};

struct Record {
    int id;
    int userId;
    char name[100];
    char country[100];
    int phone;
    char accountType[10];
    int accountNbr;
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

struct User {
    int id;
    char name[50];
    char password[50];
};

// ValidationResult struct
struct ValidationResult {
    int isValid;
    const char* message;
};

// Function declarations
void clearInputBuffer(void);
int validateDate(int month, int day, int year);
struct ValidationResult validateDateWithRetry(int* month, int* day, int* year);
int validateAccountNumber(int accountNum);
struct ValidationResult validateAccountNumberWithRetry(int* accountNum);
int validatePhone(const char* phone);
int validateAmount(double amount);
int validateCountry(const char* country);
int validateAccountType(const char* accountType);
int getValidatedInput(const char* prompt, const char* format, void* var, int (*validator)(void*));

// Authentication functions
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
const char *getPassword(struct User u);           // Original auth function
const char *getEncryptedPassword(struct User u);  // New encrypted version

// Password encryption functions
int updatePasswordsInFile(void);
int isLikelyHash(const char* str);

int getNextUserId(void);
int saveUserToDb(const char* username, const char* password);

// System functions
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);
void updateAccount(struct User u);
void makeTransaction(struct User u);
void checkAccountDetails(struct User u);
void removeExistingAccount(struct User u);
void transferOwnership(struct User u);

// Notification functions
void startNotificationListener(const char* username);
void sendTransferNotification(const char* sender, const char* receiver, 
                            int accountNum, double balance, const char* accountType);
void handle_signal(int sig __attribute__((unused)));

// Database functions
int initDatabase(void);
int insertUser(const char* username, const char* hashed_password);
int verifyUser(const char* username, const char* hashed_password);

// Migration functions
void runMigration(void);

// Helper functions
int canMakeTransaction(const char* accountType);
double calculateInterest(const char* accountType, double amount);
int findUserIdByName(const char* userName);
int userExists(const char* userName);
void hashPassword(const char* password, char* output);

// File operations
int getAccountFromFile(FILE *ptr, char name[50], struct Record *r);
void saveAccountToFile(FILE *ptr, struct User u, struct Record r);

#endif /* HEADER_H */