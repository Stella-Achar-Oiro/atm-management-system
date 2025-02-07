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

// Authentication functions
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
const char *getPassword(struct User u);           // Original auth function
const char *getEncryptedPassword(struct User u);  // New encrypted version

// Password encryption functions
int updatePasswordsInFile(void);
int isLikelyHash(const char* str);

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