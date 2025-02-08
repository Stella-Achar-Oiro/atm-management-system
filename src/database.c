#include "header.h"
#include <sqlite3.h>

// Remove DB_FILE define since it's already in header.h

int initDatabase() {
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(DB_FILE, &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    // Create users table
    const char *sql_users = "CREATE TABLE IF NOT EXISTS users ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                           "username TEXT UNIQUE NOT NULL,"
                           "password TEXT NOT NULL,"
                           "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);";
    
    rc = sqlite3_exec(db, sql_users, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    
    // Create accounts table
    const char *sql_accounts = "CREATE TABLE IF NOT EXISTS accounts ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "user_id INTEGER,"
                             "account_number INTEGER UNIQUE,"
                             "account_type TEXT,"
                             "balance REAL,"
                             "country TEXT,"
                             "phone TEXT,"
                             "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                             "FOREIGN KEY(user_id) REFERENCES users(id));";
    
    rc = sqlite3_exec(db, sql_accounts, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    
    sqlite3_close(db);
    return 0;
}

int insertUser(const char* username, const char* hashed_password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    // Remove unused err_msg variable
    
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        return -1;
    }
    
    const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_password, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return (rc == SQLITE_DONE) ? 1 : -1;
}

int verifyUser(const char* username, const char* password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int verified = 0;
    
    // Open database
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    // Get stored hash for user
    const char *sql = "SELECT password FROM users WHERE username = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    
    // Bind the username parameter
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char* stored_hash = (const char*)sqlite3_column_text(stmt, 0);
        verified = verifyPassword(password, stored_hash);
    }
    
    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return verified;
}

int getNextAccountId() {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int maxId = 10000;  // Start from 10001
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return -1;
    }
    
    const char *sql = "SELECT MAX(account_number) FROM accounts;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int result = sqlite3_column_int(stmt, 0);
            if (result > maxId) {
                maxId = result;
            }
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return maxId + 1;
}

int createAccount(struct Account* account) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;
    int success = 0;
    
    printf("\nDEBUG: Attempting to create account for user_id: %d\n", account->user_id);
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("DEBUG: Database opened successfully\n");
    
    // First ensure the accounts table exists
    const char *check_table = "CREATE TABLE IF NOT EXISTS accounts ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "user_id INTEGER NOT NULL,"
                            "account_number INTEGER UNIQUE NOT NULL,"
                            "account_type TEXT NOT NULL,"
                            "balance REAL NOT NULL,"
                            "country TEXT NOT NULL,"
                            "phone TEXT NOT NULL,"
                            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                            "FOREIGN KEY(user_id) REFERENCES users(id));";
                            
    if (sqlite3_exec(db, check_table, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 0;
    }
    printf("DEBUG: Accounts table exists/created\n");
    
    // Now insert/update the account
    const char *sql = "INSERT OR REPLACE INTO accounts "
                     "(user_id, account_number, account_type, balance, country, phone) "
                     "VALUES (?, ?, ?, ?, ?, ?);";
                     
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, account->user_id);
    sqlite3_bind_int(stmt, 2, account->account_number);
    sqlite3_bind_text(stmt, 3, account->account_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, account->balance);
    sqlite3_bind_text(stmt, 5, account->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, account->phone, -1, SQLITE_STATIC);
    
    printf("DEBUG: Account details:\n");
    printf("DEBUG: user_id: %d\n", account->user_id);
    printf("DEBUG: account_number: %d\n", account->account_number);
    printf("DEBUG: account_type: %s\n", account->account_type);
    printf("DEBUG: balance: %.2f\n", account->balance);
    printf("DEBUG: country: %s\n", account->country);
    printf("DEBUG: phone: %s\n", account->phone);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        success = 1;
        printf("DEBUG: Account saved successfully\n");
    } else {
        fprintf(stderr, "Failed to save account: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

int getAccount(int account_number, struct Account* account) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int found = 0;
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return 0;
    }
    
    const char *sql = "SELECT id, user_id, account_number, account_type, balance, "
                     "country, phone, created_at FROM accounts WHERE account_number = ?;";
                     
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, account_number);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            account->id = sqlite3_column_int(stmt, 0);
            account->user_id = sqlite3_column_int(stmt, 1);
            account->account_number = sqlite3_column_int(stmt, 2);
            strncpy(account->account_type, (const char*)sqlite3_column_text(stmt, 3), 
                   sizeof(account->account_type) - 1);
            account->balance = sqlite3_column_double(stmt, 4);
            strncpy(account->country, (const char*)sqlite3_column_text(stmt, 5), 
                   sizeof(account->country) - 1);
            strncpy(account->phone, (const char*)sqlite3_column_text(stmt, 6), 
                   sizeof(account->phone) - 1);
            strncpy(account->created_at, (const char*)sqlite3_column_text(stmt, 7), 
                   sizeof(account->created_at) - 1);
            found = 1;
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}

int updateBalance(int account_number, double new_balance) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int success = 0;
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        return 0;
    }
    
    const char *sql = "UPDATE accounts SET balance = ? WHERE account_number = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, new_balance);
        sqlite3_bind_int(stmt, 2, account_number);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = 1;
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

int getUser(const char* username, struct User* user) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int found = 0;
    
    printf("\nDEBUG: Attempting to get user: %s\n", username);
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("DEBUG: Database opened successfully\n");
    
    const char *sql = "SELECT id, username, password, created_at FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        printf("DEBUG: Query prepared successfully\n");
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user->id = sqlite3_column_int(stmt, 0);
            strncpy(user->username, (const char*)sqlite3_column_text(stmt, 1), 
                   sizeof(user->username) - 1);
            strncpy(user->password, (const char*)sqlite3_column_text(stmt, 2), 
                   sizeof(user->password) - 1);
            strncpy(user->created_at, (const char*)sqlite3_column_text(stmt, 3), 
                   sizeof(user->created_at) - 1);
            found = 1;
            printf("DEBUG: User found with ID: %d\n", user->id);
        } else {
            printf("DEBUG: User not found\n");
        }
    } else {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}

int saveUser(const struct User* user) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;
    int success = 0;
    
    printf("\nDEBUG: Attempting to save user: %s\n", user->username);
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("DEBUG: Database opened successfully\n");
    
    // First ensure the users table exists
    const char *check_table = "CREATE TABLE IF NOT EXISTS users ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "username TEXT UNIQUE NOT NULL,"
                            "password TEXT NOT NULL,"
                            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);";
                            
    if (sqlite3_exec(db, check_table, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 0;
    }
    printf("DEBUG: Users table exists/created\n");
    
    // Now insert the user
    const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user->password, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        success = 1;
        printf("DEBUG: User saved successfully\n");
    } else {
        fprintf(stderr, "Failed to insert user: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}