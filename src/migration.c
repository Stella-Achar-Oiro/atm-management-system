#include "header.h"

int main() {
    sqlite3 *db;
    char *err_msg = NULL;
    char hashed_password[HASH_LENGTH];
    sqlite3_stmt *stmt;
    
    // Initialize database first
    if (initDatabase() != 0) {
        printf("Failed to initialize database\n");
        return 1;
    }
    
    // Open database
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Begin transaction
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to begin transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    // Prepare statement for users
    const char *users_sql = "INSERT OR REPLACE INTO users (id, username, password) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, users_sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Insert Alice with encrypted password
    hashPassword("q1w2e3r4t5y6", hashed_password);
    sqlite3_bind_int(stmt, 1, 0);  // id
    sqlite3_bind_text(stmt, 2, "Alice", -1, SQLITE_STATIC);  // username
    sqlite3_bind_text(stmt, 3, hashed_password, -1, SQLITE_STATIC);  // encrypted password
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert Alice: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_reset(stmt);

    // Insert Michel with encrypted password
    hashPassword("q1w2e3r4t5y6", hashed_password);
    sqlite3_bind_int(stmt, 1, 1);  // id
    sqlite3_bind_text(stmt, 2, "Michel", -1, SQLITE_STATIC);  // username
    sqlite3_bind_text(stmt, 3, hashed_password, -1, SQLITE_STATIC);  // encrypted password
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert Michel: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_finalize(stmt);

    // Insert accounts
    const char *accounts_sql = 
        "INSERT OR REPLACE INTO accounts (id, user_id, account_number, account_type, balance, country, phone, created_at) VALUES "
        "(0, 0, 0, 'saving', 22432.52, 'Africa', '291321234', '2012-10-10'),"
        "(1, 1, 2, 'fixed01', 10023.23, 'Portugal', '123543455', '2001-02-05'),"
        "(2, 1, 123, 'saving', 12345.30, 'UK', '1234123', '2020-10-10');";

    if (sqlite3_exec(db, accounts_sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        sqlite3_close(db);
        return 1;
    }

    // Commit transaction
    if (sqlite3_exec(db, "COMMIT;", 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to commit: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    printf("Data migration completed successfully with encrypted passwords!\n");
    sqlite3_close(db);
    return 0;
}