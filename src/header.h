#ifndef _HEADER_H
#define _HEADER_H

// System includes
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>
#include <sqlite3.h>
#include <unistd.h>
#include <crypt.h>

// Constants
#define MAXINPUTLEN 100
#define DBPATH "./data/atm.db"
#define SALT_LENGTH 16
#define HASH_LENGTH 64

// Structures
typedef struct {
    int month, day, year;
} Date;

typedef struct {
    int id;
    int userId;
    char name[100];
    char country[100];
    int phone;
    char accountType[100];
    int accountNbr;
    double amount;
    Date deposit;
    Date withdraw;
    double Accnt_Balance;
} Record;

typedef struct {
    int uid;
    char name[50];
    char password[HASH_LENGTH];
} User;

void safe_clear_screen(void);
bool is_valid_phone(int number);
bool is_valid_account_number(int number);
bool is_valid_username(const char* name);
bool is_valid_password(const char* password);

// Password encryption functions
char* generate_salt(void);
char* hash_password(const char* password, const char* salt);
bool verify_password(const char* password, const char* hashed);
bool is_hashed_password(const char* password);

// Database Function Declarations
sqlite3 *sqliteHandler(const char *dbName);
int sqliteInit(char *dbname);
void sqliteExecute(sqlite3 *db, const char *sql);
void sqliteError(sqlite3 *db, const char *message, sqlite3_stmt *stmt);

// User Management Functions
int dbRetrieveUserId(const char *username);
char *dbRetrieveUserName(int user_id);
const char *dbRetrievePassword(User u);
bool dbUsernameExists(const char *username);
void dbUserRegister(User u);

// Account Management Functions
bool dbAccountExistsForUser(int user_id, int account_number);
bool dbAccountExistsInDatabase(int accountNbr);
double accountBalance(int userId, int accnt_id, User u);
bool dbAccountCreate(User u, Record r);
bool dbFetchAccountDetails(User u, int Accntid, Record *r, double *balance);
bool dbUpdateAccountDetails(User u, int accountId, Record AccInfo, int option);
bool dbAccountDelete(int userId, int accountId);
void dbaccountTransfer(int user_id, int accnt_id, char *name);
bool accountAllowsTransactions(int userId, int accnt_id, User u);
void dbRecordTransaction(int accnt_id, char *transaction_type, double amount);
void dbUpdateAccntBalance(int user_id, int accnt_id, double balance, int option);

// System Utility Functions
bool isdate(Date date);
void trimlinechar(char *str);
bool ispositive(const char *str);
bool isvaliddouble(const char *str, double *dnum);
bool isalphabet(char *str);
bool isleapyear(int year);
bool isstring(const char *str, size_t n);
double interest(double amount, char *AccountType);

// Interface Functions
void initMenu(User *u);
void mainMenu(User u);
void registerMenu(User u);
void loginMenu(char a[50], char pass[50]);
void accountCreate(User u);
void accountDetails(User u);
void accountUpdateInfo(User u);
void accountList(User u);
void accountDelete(User u);
void accountTransfer(User u);
void accountMakeTransaction(User u);

// Input/Output Functions
void input_hide(void);
void input_show(void);
int input_number(void);
int input_number_with_validation(const char* prompt, bool (*validator)(int));
void input_string(User u, char *input);

// System Control Functions
void Return(User u);
void success(User u);
void StayOrReturn(int notGood, void (*f)(User), User u);
void die(void);

#endif // _HEADER_H