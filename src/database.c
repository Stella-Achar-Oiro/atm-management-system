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

int verifyUser(const char* username, const char* hashed_password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int verified = 0;
    
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        return 0;
    }
    
    const char *sql = "SELECT id FROM users WHERE username = ? AND password = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_password, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        verified = 1;
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return verified;
}