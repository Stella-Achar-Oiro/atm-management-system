#include "header.h"
#include "ui.h"

// ============= Password Encryption Functions =============

char* generate_salt(void) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    static char salt[SALT_LENGTH + 1] = "$6$";  // Using SHA-512
    
    srand((unsigned int)time(NULL));
    
    for(int i = 3; i < SALT_LENGTH; i++) {
        salt[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    salt[SALT_LENGTH] = '\0';
    
    return strdup(salt);
}

char* hash_password(const char* password, const char* salt) {
    return crypt(password, salt);
}

bool verify_password(const char* password, const char* hashed) {
    char* new_hash = crypt(password, hashed);
    return strcmp(new_hash, hashed) == 0;
}

bool is_hashed_password(const char* password) {
    return (password != NULL && strncmp(password, "$6$", 3) == 0);
}

// ============= SQLite Core Functions =============

int sqliteInit(char *dbname) {
    sqlite3 *db = NULL;
    int rc = sqlite3_open(dbname, &db);
    if (rc != SQLITE_OK) {
        print_error("Failed to open database");
        return 0;
    }

    print_processing("Initializing database");

    // Create tables if they don't exist
    const char *sqlUsersTable = 
        "CREATE TABLE IF NOT EXISTS USERS ("
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "password TEXT NOT NULL);";
    sqliteExecute(db, sqlUsersTable);

    const char *sqlRecordsTable = 
        "CREATE TABLE IF NOT EXISTS Records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userId INTEGER, "
        "name TEXT NOT NULL, "
        "country TEXT NOT NULL, "
        "phone INTEGER, "
        "accountType TEXT NOT NULL, "
        "accountNbr INTEGER, "
        "amount REAL, "
        "deposit_month INTEGER, "
        "deposit_day INTEGER, "
        "deposit_year INTEGER, "
        "withdraw_month INTEGER, "
        "withdraw_day INTEGER, "
        "withdraw_year INTEGER, "
        "Accnt_Balance REAL, "
        "FOREIGN KEY (userId) REFERENCES USERS (user_id));";
    sqliteExecute(db, sqlRecordsTable);

    const char *sqlTsTable = 
        "CREATE TABLE IF NOT EXISTS transactions ("
        "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "account_id INTEGER, "
        "transaction_type TEXT, "
        "amount REAL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY (account_id) REFERENCES Records(accountNbr));";
    sqliteExecute(db, sqlTsTable);

    sqlite3_close(db);
    return 1;
}

sqlite3 *sqliteHandler(const char *dbName) {
    sqlite3 *db = NULL;
    if (sqlite3_open(dbName, &db) != SQLITE_OK) {
        sqliteError(db, "Failed to open Database", NULL);
    }
    return db;
}

void sqliteExecute(sqlite3 *db, const char *sql) {
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqliteError(db, "Execution failed", stmt);
    }
    sqlite3_finalize(stmt);
}

void sqliteError(sqlite3 *db, const char *message, sqlite3_stmt *stmt) {
    print_error(message);
    fprintf(stderr, "%s: %s\n", message, sqlite3_errmsg(db));
    if (stmt != NULL) sqlite3_finalize(stmt);
    if (db != NULL) sqlite3_close(db);
    die();
}

// ============= User Database Operations =============

int dbRetrieveUserId(const char *username) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    int id = -1;

    const char *sql = "SELECT user_id FROM USERS WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id;
}

char *dbRetrieveUserName(int user_id) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    char *name = NULL;

    const char *sql = "SELECT username FROM USERS WHERE user_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, user_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            name = strdup((const char *)text);
            if (!name) sqliteError(db, "Memory allocation failed", stmt);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return name;
}

const char* dbRetrievePassword(User u) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    char* hashed_pass = NULL;

    const char *sql = "SELECT password FROM USERS WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        hashed_pass = strdup((const char *)sqlite3_column_text(stmt, 0));
    } else {
        hashed_pass = strdup("no user found");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return hashed_pass;
}

void dbUserRegister(User u) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;

    const char *sql = "INSERT INTO USERS (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    // Generate salt and hash password
    char* salt = generate_salt();
    char* hashed_password = hash_password(u.password, salt);
    free(salt);  // Free the salt as it's now part of the hash

    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqliteError(db, "Failed to register user", stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

bool dbUsernameExists(const char *username) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    bool exists = false;

    const char *sql = "SELECT 1 FROM USERS WHERE username = ? LIMIT 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = true;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return exists;
}

// ============= Account Database Operations =============

double accountBalance(int userId __attribute__((unused)), int accnt_id, User u __attribute__((unused))) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    double balance = 0.0;

    const char *sql = "SELECT Accnt_Balance FROM Records WHERE accountNbr = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, accnt_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return balance;
}

bool dbAccountExistsInDatabase(int accountNbr) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    bool exists = false;

    const char *sql = "SELECT COUNT(*) FROM Records WHERE accountNbr = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, accountNbr);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = (sqlite3_column_int(stmt, 0) > 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return exists;
}

bool dbAccountExistsForUser(int user_id, int account_number) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    bool exists = false;

    const char *sql = "SELECT COUNT(*) FROM Records WHERE userId = ? AND accountNbr = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, account_number);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = (sqlite3_column_int(stmt, 0) > 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return exists;
}

bool dbAccountCreate(User u, Record r) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;

    const char *sql = 
        "INSERT INTO Records (userId, name, country, phone, accountType, "
        "accountNbr, amount, deposit_month, deposit_day, deposit_year, Accnt_Balance) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, r.userId);
    sqlite3_bind_text(stmt, 2, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, r.country, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, r.phone);
    sqlite3_bind_text(stmt, 5, r.accountType, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, r.accountNbr);
    sqlite3_bind_double(stmt, 7, r.amount);
    sqlite3_bind_int(stmt, 8, r.deposit.month);
    sqlite3_bind_int(stmt, 9, r.deposit.day);
    sqlite3_bind_int(stmt, 10, r.deposit.year);
    sqlite3_bind_double(stmt, 11, r.Accnt_Balance);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

bool dbFetchAccountDetails(User u, int Accntid, Record *r, double *balance) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    int userid = dbRetrieveUserId(u.name);
    bool success = false;

    if (!dbAccountExistsForUser(userid, Accntid)) {
        print_error("Account does not exist for this user");
        sqlite3_close(db);
        return false;
    }

    *balance = accountBalance(userid, Accntid, u);

    const char *sql = 
        "SELECT id, country, userId, accountType, accountNbr, amount, "
        "deposit_month, deposit_day, deposit_year, phone "
        "FROM Records WHERE accountNbr = ? AND userId = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, Accntid);
    sqlite3_bind_int(stmt, 2, userid);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        r->id = sqlite3_column_int(stmt, 0);
        strcpy(r->country, (const char *)sqlite3_column_text(stmt, 1));
        r->userId = sqlite3_column_int(stmt, 2);
        strcpy(r->accountType, (const char *)sqlite3_column_text(stmt, 3));
        r->accountNbr = sqlite3_column_int(stmt, 4);
        r->amount = sqlite3_column_double(stmt, 5);
        r->deposit.month = sqlite3_column_int(stmt, 6);
        r->deposit.day = sqlite3_column_int(stmt, 7);
        r->deposit.year = sqlite3_column_int(stmt, 8);
        r->phone = sqlite3_column_int(stmt, 9);
        success = true;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

void dbUpdateAccntBalance(int user_id __attribute__((unused)), int accnt_id, double balance, int option) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    const char *sql;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int year = tm_info->tm_year + 1900;
    int month = tm_info->tm_mon + 1;
    int current_day = tm_info->tm_mday;

    if (option == 1) {
        sql = "UPDATE Records SET deposit_day = ?, deposit_month = ?, deposit_year = ?, Accnt_Balance = ? WHERE accountNbr = ?";
    } else {
        sql = "UPDATE Records SET withdraw_day = ?, withdraw_month = ?, withdraw_year = ?, Accnt_Balance = ? WHERE accountNbr = ?";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, current_day);
    sqlite3_bind_int(stmt, 2, month);
    sqlite3_bind_int(stmt, 3, year);
    sqlite3_bind_double(stmt, 4, balance);
    sqlite3_bind_int(stmt, 5, accnt_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqliteError(db, "Failed to update balance", stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void dbRecordTransaction(int accnt_id, char *transaction_type, double amount) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;

    const char *sql = "INSERT INTO transactions (account_id, transaction_type, amount) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, accnt_id);
    sqlite3_bind_text(stmt, 2, transaction_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, amount);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqliteError(db, "Failed to record transaction", stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

bool accountAllowsTransactions(int userId __attribute__((unused)), int accnt_id, User u __attribute__((unused))) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    bool allows = false;

    const char *sql = "SELECT accountType FROM Records WHERE accountNbr = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, accnt_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *account_type = (const char *)sqlite3_column_text(stmt, 0);
        allows = (strcmp(account_type, "saving") == 0);
        if (!allows) {
            print_error("Transactions not allowed for this account type");
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return allows;
}

bool dbUpdateAccountDetails(User u, int accountId, Record AccInfo, int option) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;
    const char *sql;
    bool success = false;

    int userid = dbRetrieveUserId(u.name);
    if (!dbAccountExistsForUser(userid, accountId)) {
        print_error("Account does not exist");
        sqlite3_close(db);
        return false;
    }

    if (option == 1) {
        sql = "UPDATE Records SET phone = ? WHERE accountNbr = ?";
    } else {
        sql = "UPDATE Records SET country = ? WHERE accountNbr = ?";
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    if (option == 1) {
        sqlite3_bind_int(stmt, 1, AccInfo.phone);
    } else {
        sqlite3_bind_text(stmt, 1, AccInfo.country, -1, SQLITE_STATIC);
    }
    sqlite3_bind_int(stmt, 2, accountId);

    success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

void dbaccountTransfer(int user_id, int accnt_id, char *name) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;

    const char *sql = "UPDATE Records SET userId = ?, name = ? WHERE accountNbr = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, accnt_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqliteError(db, "Failed to transfer account", stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

bool dbAccountDelete(int userId, int accountId) {
    sqlite3 *db = sqliteHandler(DBPATH);
    sqlite3_stmt *stmt = NULL;

    const char *sql = "DELETE FROM Records WHERE accountNbr = ? AND userId = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement", stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_int(stmt, 2, userId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (success) {
        SUCCESS_MSG("Account deleted successfully");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}