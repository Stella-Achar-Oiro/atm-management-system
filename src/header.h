#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <crypt.h>
#include <time.h>

// Database configuration
#define DB_FILE "./data/atm.db"

// Password configuration
#define SALT_LENGTH 16
#define BCRYPT_COST 12  // Work factor for bcrypt
#define HASH_LENGTH 64  // bcrypt hash length

// Validation constants
#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 15
#define MIN_ACCOUNT_NUMBER 100000000  // 9 digits
#define MAX_ACCOUNT_NUMBER 999999999  // 9 digits
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

// Database structs
struct User {
    int id;
    char username[MAX_NAME_LENGTH];
    char password[HASH_LENGTH];  // Store bcrypt hash
    char created_at[26];  // ISO 8601 timestamp
};

struct Account {
    int id;
    int user_id;
    int account_number;
    char account_type[10];
    double balance;
    char country[100];
    char phone[MAX_PHONE_LENGTH];
    char created_at[26];  // ISO 8601 timestamp
};

// ValidationResult struct
struct ValidationResult {
    int isValid;
    const char* message;
};

// Validation functions
void clearInputBuffer(void);
int validateDate(int month, int day, int year);
int validateAccountNumber(int accountNum);
int validatePhone(const char* phone);
int validateAmount(double amount);
int validateCountry(const char* country);
int validateAccountType(const char* accountType);

// Input validation functions
struct ValidationResult getDateInput(int* month, int* day, int* year);
struct ValidationResult getAccountNumberInput(int* accountNum);
struct ValidationResult getPhoneInput(char* phone);
struct ValidationResult getAmountInput(double* amount);
struct ValidationResult getCountryInput(char* country);
struct ValidationResult getAccountTypeInput(char* accountType);

// Authentication functions
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
const char *getPassword(struct User u);
void hashPassword(const char* password, char* output);
int verifyPassword(const char* password, const char* hash);

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
int getNextAccountId(void);
int saveUser(const struct User* user);
int getUser(const char* username, struct User* user);
int createAccount(struct Account* account);
int getAccount(int account_number, struct Account* account);
int updateBalance(int account_number, double new_balance);

#endif /* HEADER_H */