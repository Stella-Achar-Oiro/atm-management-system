#include "header.h"
#include <sqlite3.h>

int migrateUserData() {
    FILE *file;
    sqlite3 *db;
    char *err_msg = 0;
    struct User user;
    char hashed_password[SHA256_DIGEST_LENGTH * 2 + 1];
    
    // Open the text file
    file = fopen(USERS, "r");
    if (file == NULL) {
        printf("Could not open users.txt\n");
        return -1;
    }
    
    // Open database
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        fclose(file);
        return -1;
    }
    
    // Begin transaction for better performance
    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, &err_msg);
    
    // Read and insert each user
    while (fscanf(file, "%d %s %s", &user.id, user.name, user.password) != EOF) {
        // Hash the password from text file
        hashPassword(user.password, hashed_password);
        
        // Prepare SQL statement
        sqlite3_stmt *stmt;
        const char *sql = "INSERT OR IGNORE INTO users (id, username, password) VALUES (?, ?, ?);";
        
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            continue;
        }
        
        // Bind values
        sqlite3_bind_int(stmt, 1, user.id);
        sqlite3_bind_text(stmt, 2, user.name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, hashed_password, -1, SQLITE_STATIC);
        
        // Execute
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            printf("Failed to insert user %s: %s\n", user.name, sqlite3_errmsg(db));
        }
        
        sqlite3_finalize(stmt);
    }
    
    // Commit transaction
    sqlite3_exec(db, "COMMIT", 0, 0, &err_msg);
    
    fclose(file);
    sqlite3_close(db);
    return 0;
}

int migrateAccountData() {
    FILE *file;
    sqlite3 *db;
    char *err_msg = 0;
    struct Record record;
    char userName[50];
    
    // Open the text file
    file = fopen(RECORDS, "r");
    if (file == NULL) {
        printf("Could not open records.txt\n");
        return -1;
    }
    
    // Open database
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        fclose(file);
        return -1;
    }
    
    // Begin transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, &err_msg);
    
    // Read and insert each record
    while (getAccountFromFile(file, userName, &record)) {
        sqlite3_stmt *stmt;
        const char *sql = "INSERT OR IGNORE INTO accounts "
                         "(id, user_id, account_number, account_type, balance, country, phone) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?);";
        
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            continue;
        }
        
        // Bind values
        sqlite3_bind_int(stmt, 1, record.id);
        sqlite3_bind_int(stmt, 2, record.userId);
        sqlite3_bind_int(stmt, 3, record.accountNbr);
        sqlite3_bind_text(stmt, 4, record.accountType, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 5, record.amount);
        sqlite3_bind_text(stmt, 6, record.country, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 7, record.phone);
        
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            printf("Failed to insert account %d: %s\n", record.accountNbr, sqlite3_errmsg(db));
        }
        
        sqlite3_finalize(stmt);
    }
    
    // Commit transaction
    sqlite3_exec(db, "COMMIT", 0, 0, &err_msg);
    
    fclose(file);
    sqlite3_close(db);
    return 0;
}

// Function to run migration
void runMigration() {
    printf("Starting data migration...\n");
    
    if (migrateUserData() == 0) {
        printf("Users data migration completed successfully\n");
    } else {
        printf("Error migrating users data\n");
    }
    
    if (migrateAccountData() == 0) {
        printf("Accounts data migration completed successfully\n");
    } else {
        printf("Error migrating accounts data\n");
    }
    
    printf("Migration process completed\n");
}