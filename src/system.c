#include "header.h"
#include <time.h>

void pauseExecution() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
    getchar();
}

void createNewAcc(struct User u) {
    struct Account acc;
    int status;
    char phone[MAX_PHONE_LENGTH];
    
    system("clear");
    printf("\n\t\t====== New Account Registration ======\n");

    // Initialize account with user information
    acc.user_id = u.id;
    acc.account_number = getNextAccountId();

    // Display the assigned account number prominently
    printf("\n\t\t╔════════════════════════════════════════╗");
    printf("\n\t\t║   Your Account Number: %d", acc.account_number);
    printf("\n\t\t║   PLEASE SAVE THIS NUMBER!             ║");
    printf("\n\t\t╚════════════════════════════════════════╝\n");
    printf("\n\t\tYou will need this number for all future transactions.\n\n");

    // Get account type
    do {
        printf("\nSelect account type:\n");
        printf("1. Saving\n");
        printf("2. Current\n");
        printf("3. Fixed01 (1 year)\n");
        printf("4. Fixed02 (2 years)\n");
        printf("5. Fixed03 (3 years)\n");
        printf("Enter choice (1-5): ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid choice! Please enter a number.\n");
            continue;
        }
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                strncpy(acc.account_type, "saving", sizeof(acc.account_type) - 1);
                break;
            case 2:
                strncpy(acc.account_type, "current", sizeof(acc.account_type) - 1);
                break;
            case 3:
                strncpy(acc.account_type, "fixed01", sizeof(acc.account_type) - 1);
                break;
            case 4:
                strncpy(acc.account_type, "fixed02", sizeof(acc.account_type) - 1);
                break;
            case 5:
                strncpy(acc.account_type, "fixed03", sizeof(acc.account_type) - 1);
                break;
            default:
                printf("\n\t\t✖ Invalid choice!\n");
                continue;
        }
        break;
    } while (1);

    // Get country
    do {
        printf("\nEnter country: ");
        scanf("%s", acc.country);
        clearInputBuffer();
        status = validateCountry(acc.country);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid country name!\n");
        }
    } while (status != VALID);

    // Get phone number
    do {
        printf("\nEnter phone number: ");
        scanf("%s", phone);
        clearInputBuffer();
        status = validatePhone(phone);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid phone number!\n");
            continue;
        }
        strncpy(acc.phone, phone, sizeof(acc.phone) - 1);
        break;
    } while (1);

    // Get initial deposit amount
    do {
        printf("\nEnter initial deposit amount: $");
        if (scanf("%lf", &acc.balance) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid amount format!\n");
            continue;
        }
        status = validateAmount(acc.balance);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid amount!\n");
            continue;
        }
        break;
    } while (1);

    // Save account to database
    if (createAccount(&acc)) {
        printf("\n\t\t✔ Account created successfully!\n");
        printf("\n\t\t╔════════════════════════════════════════╗");
        printf("\n\t\t║   IMPORTANT: SAVE YOUR ACCOUNT INFO    ║");
        printf("\n\t\t║   Account Number: %d", acc.account_number);
        printf("\n\t\t║   Type: %s", acc.account_type);
        printf("\n\t\t║   Initial Balance: $%.2f", acc.balance);
        printf("\n\t\t║   Created: %s", acc.created_at);
        printf("\n\t\t╚════════════════════════════════════════╝\n");
        
        pauseExecution();
    } else {
        printf("\n\t\t✖ Failed to create account!\n");
        pauseExecution();
    }
    
    printf("\nPress any key to continue...");
    getchar();
}

void checkAllAccounts(struct User u) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int found = 0;
    
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        printf("\n\t\t✖ Database error\n");
        return;
    }
    
    const char *sql = "SELECT account_number, account_type, balance, country, phone "
                     "FROM accounts WHERE user_id = ?;";
                     
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("\n\t\t✖ Database error\n");
        sqlite3_close(db);
        return;
    }
    
    sqlite3_bind_int(stmt, 1, u.id);
    
    system("clear");
    printf("\n\t\t====== All Accounts ======\n\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = 1;
        printf("\t\tAccount Number: %d\n", sqlite3_column_int(stmt, 0));
        printf("\t\tType: %s\n", sqlite3_column_text(stmt, 1));
        printf("\t\tBalance: $%.2f\n", sqlite3_column_double(stmt, 2));
        printf("\t\tCountry: %s\n", sqlite3_column_text(stmt, 3));
        printf("\t\tPhone: %s\n", sqlite3_column_text(stmt, 4));
        printf("\t\t------------------------\n");
    }
    
    if (!found) {
        printf("\t\tNo accounts found.\n");
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    printf("\n\t\tPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    getchar();
}

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        exit(1);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("\n✖ Insert a valid operation!\n");
        goto invalid;
    }
}

// Calculate interest based on account type
double calculateInterest(const char* accountType, double amount) {
    if (strcmp(accountType, "saving") == 0) {
        return amount * 0.07;  // 7% for savings
    } else if (strcmp(accountType, "fixed01") == 0) {
        return amount * 0.04;  // 4% for 1 year fixed
    } else if (strcmp(accountType, "fixed02") == 0) {
        return amount * 0.05;  // 5% for 2 year fixed
    } else if (strcmp(accountType, "fixed03") == 0) {
        return amount * 0.08;  // 8% for 3 year fixed
    }
    return 0.0;  // No interest for current account
}

// Function to check if account type allows transactions
int canMakeTransaction(const char* accountType) {
    return (strcmp(accountType, "fixed01") != 0 && 
            strcmp(accountType, "fixed02") != 0 && 
            strcmp(accountType, "fixed03") != 0);
}

void updateAccount(struct User u) {
    struct Account acc;
    int accountNum;
    char phone[MAX_PHONE_LENGTH];
    char country[100];
    int choice;
    int status;
    
    system("clear");
    printf("\n\t\t====== Update Account ======\n");

    // Account number validation
    do {
        printf("\nEnter account number you want to update: ");
        if (scanf("%d", &accountNum) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid account number format!\n");
            continue;
        }
        status = validateAccountNumber(accountNum);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid account number! Must be 5 digits.\n");
        } else {
            break;
        }
    } while (1);

    // Get account from database
    if (!getAccount(accountNum, &acc) || acc.user_id != u.id) {
        printf("\n\t\t✖ Account not found or unauthorized access!\n");
        return;
    }

    printf("\n\t\t✔ Account found! Current details:");
    printf("\n\t\tCountry: %s", acc.country);
    printf("\n\t\tPhone: %s", acc.phone);
    
    do {
        printf("\n\nWhat would you like to update?");
        printf("\n1. Country");
        printf("\n2. Phone Number");
        printf("\n3. Cancel Update");
        printf("\nEnter choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid choice! Please enter a number.\n");
            continue;
        }
        clearInputBuffer();

        switch(choice) {
            case 1:
                do {
                    printf("\nEnter new country: ");
                    scanf("%s", country);
                    clearInputBuffer();
                    status = validateCountry(country);
                    if (status != VALID) {
                        printf("\n\t\t✖ Invalid country name!\n");
                    }
                } while (status != VALID);
                strncpy(acc.country, country, sizeof(acc.country) - 1);
                break;

            case 2:
                do {
                    printf("\nEnter new phone number: ");
                    scanf("%s", phone);
                    clearInputBuffer();
                    status = validatePhone(phone);
                    if (status != VALID) {
                        printf("\n\t\t✖ Invalid phone number!\n");
                        continue;
                    }
                    strncpy(acc.phone, phone, sizeof(acc.phone) - 1);
                    break;
                } while (1);
                break;

            case 3:
                printf("\n\t\t✖ Update cancelled.\n");
                return;

            default:
                printf("\n\t\t✖ Invalid choice! Please enter 1, 2, or 3.\n");
                continue;
        }
        break;
    } while (1);
    
    // Update account in database
    if (createAccount(&acc)) {  // This will update existing account
        printf("\n\t\t✔ Account updated successfully!\n");
    } else {
        printf("\n\t\t✖ Failed to update account!\n");
    }
    
    printf("\nPress any key to continue...");
    getchar();
}

void makeTransaction(struct User u) {
    struct Account acc;
    int accountNum, choice;
    double amount;
    int status;
    
    system("clear");
    printf("\n\t\t====== Make Transaction ======\n");

    // Account number validation
    do {
        printf("\nEnter account number for transaction: ");
        if (scanf("%d", &accountNum) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid account number format!\n");
            continue;
        }
        status = validateAccountNumber(accountNum);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid account number!\n");
        } else {
            break;
        }
    } while (1);

    // Get account from database
    if (!getAccount(accountNum, &acc) || acc.user_id != u.id) {
        printf("\n\t\t✖ Account not found or unauthorized access!\n");
        return;
    }

    // Check if account type allows transactions
    if (!canMakeTransaction(acc.account_type)) {
        printf("\n\t\t✖ Transactions not allowed for %s account type!\n", acc.account_type);
        return;
    }

    printf("\n\t\tCurrent balance: $%.2f", acc.balance);
    
    do {
        printf("\n1. Deposit");
        printf("\n2. Withdraw");
        printf("\n3. Cancel Transaction");
        printf("\nEnter choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid choice!\n");
            continue;
        }
        clearInputBuffer();

        if (choice < 1 || choice > 3) {
            printf("\n\t\t✖ Invalid choice!\n");
            continue;
        }
        break;
    } while (1);

    switch(choice) {
        case 1:  // Deposit
            do {
                printf("\nEnter amount to deposit: $");
                if (scanf("%lf", &amount) != 1) {
                    clearInputBuffer();
                    printf("\n\t\t✖ Invalid amount format!\n");
                    continue;
                }
                status = validateAmount(amount);
                if (status != VALID) {
                    printf("\n\t\t✖ Invalid amount!\n");
                    continue;
                }
                acc.balance += amount;
                if (updateBalance(accountNum, acc.balance)) {
                    printf("\n\t\t✔ Deposit successful! New balance: $%.2f\n", acc.balance);
                } else {
                    printf("\n\t\t✖ Failed to process deposit!\n");
                }
                break;
            } while (1);
            break;

        case 2:  // Withdraw
            do {
                printf("\nEnter amount to withdraw: $");
                if (scanf("%lf", &amount) != 1) {
                    clearInputBuffer();
                    printf("\n\t\t✖ Invalid amount format!\n");
                    continue;
                }
                status = validateAmount(amount);
                if (status != VALID) {
                    printf("\n\t\t✖ Invalid amount!\n");
                    continue;
                }
                if (amount > acc.balance) {
                    printf("\n\t\t✖ Insufficient funds!\n");
                    continue;
                }
                acc.balance -= amount;
                if (updateBalance(accountNum, acc.balance)) {
                    printf("\n\t\t✔ Withdrawal successful! New balance: $%.2f\n", acc.balance);
                } else {
                    printf("\n\t\t✖ Failed to process withdrawal!\n");
                }
                break;
            } while (1);
            break;

        case 3:  // Cancel
            printf("\n\t\t✖ Transaction cancelled.\n");
            break;
    }

    printf("\nPress any key to continue...");
    getchar();
}

void checkAccountDetails(struct User u) {
    struct Account acc;
    int accountNum;
    int status;
    
    system("clear");
    printf("\n\t\t====== Account Details ======\n");

    // Account number validation
    do {
        printf("\nEnter the account number: ");
        if (scanf("%d", &accountNum) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid account number format!\n");
            continue;
        }
        status = validateAccountNumber(accountNum);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid account number!\n");
        } else {
            break;
        }
    } while (1);

    // Get account from database
    if (!getAccount(accountNum, &acc) || acc.user_id != u.id) {
        printf("\n\t\t✖ Account not found or unauthorized access!\n");
        return;
    }

    printf("\n\t\t===== Account Information =====");
    printf("\n\t\tAccount Number: %d", acc.account_number);
    printf("\n\t\tAccount Type: %s", acc.account_type);
    printf("\n\t\tBalance: $%.2f", acc.balance);
    printf("\n\t\tCountry: %s", acc.country);
    printf("\n\t\tPhone: %s", acc.phone);
    printf("\n\t\tCreated: %s", acc.created_at);
    
    // Calculate and display interest information
    if (strcmp(acc.account_type, "current") == 0) {
        printf("\n\n\t\tYou will not get interests because the account is of type current");
    } else {
        double interest = calculateInterest(acc.account_type, acc.balance);
        printf("\n\n\t\tYou will get $%.2f as interest on the first of every month", interest);
    }
    
    printf("\n\nPress any key to continue...");
    getchar();
    getchar();
}

void removeExistingAccount(struct User u) {
    struct Account acc;
    int accountNum;
    char confirmation;
    int status;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    
    system("clear");
    printf("\n\t\t====== Remove Account ======\n");

    // Account number validation
    do {
        printf("\nEnter the account number to remove: ");
        if (scanf("%d", &accountNum) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid account number format!\n");
            continue;
        }
        status = validateAccountNumber(accountNum);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid account number!\n");
        } else {
            break;
        }
    } while (1);

    // Get account from database
    if (!getAccount(accountNum, &acc) || acc.user_id != u.id) {
        printf("\n\t\t✖ Account not found or unauthorized access!\n");
        return;
    }

    printf("\n\t\tAccount found with the following details:");
    printf("\n\t\tAccount Number: %d", acc.account_number);
    printf("\n\t\tBalance: $%.2f", acc.balance);
    printf("\n\t\tAccount Type: %s", acc.account_type);
    
    do {
        printf("\n\nAre you sure you want to remove this account? (y/n): ");
        scanf(" %c", &confirmation);
        clearInputBuffer();
        
        if (confirmation != 'y' && confirmation != 'Y' && 
            confirmation != 'n' && confirmation != 'N') {
            printf("\n\t\t✖ Invalid input! Please enter 'y' or 'n'.\n");
            continue;
        }
        break;
    } while (1);
    
    if (confirmation == 'y' || confirmation == 'Y') {
        if (acc.balance > 0) {
            printf("\n\t\t⚠ Warning: Account has remaining balance of $%.2f", acc.balance);
            do {
                printf("\nAre you absolutely sure? (y/n): ");
                scanf(" %c", &confirmation);
                clearInputBuffer();
                
                if (confirmation != 'y' && confirmation != 'Y' && 
                    confirmation != 'n' && confirmation != 'N') {
                    printf("\n\t\t✖ Invalid input!\n");
                    continue;
                }
                break;
            } while (1);
        }
        
        if (confirmation == 'y' || confirmation == 'Y') {
            // Delete account from database
            if (sqlite3_open(DB_FILE, &db) == SQLITE_OK) {
                const char *sql = "DELETE FROM accounts WHERE account_number = ? AND user_id = ?;";
                if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
                    sqlite3_bind_int(stmt, 1, accountNum);
                    sqlite3_bind_int(stmt, 2, u.id);
                    
                    if (sqlite3_step(stmt) == SQLITE_DONE) {
                        printf("\n\t\t✔ Account successfully removed!\n");
                    } else {
                        printf("\n\t\t✖ Failed to remove account!\n");
                    }
                }
                sqlite3_finalize(stmt);
                sqlite3_close(db);
            }
        }
    } else {
        printf("\n\t\t✖ Account removal cancelled.\n");
    }
    
    printf("\nPress any key to continue...");
    getchar();
}

void transferOwnership(struct User u) {
    struct Account acc;
    struct User newOwner;
    int accountNum;
    char newUsername[MAX_NAME_LENGTH];
    char confirmation;
    int status;
    
    system("clear");
    printf("\n\t\t====== Transfer Account Ownership ======\n");

    // Account number validation
    do {
        printf("\nEnter the account number to transfer: ");
        if (scanf("%d", &accountNum) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid account number format!\n");
            continue;
        }
        status = validateAccountNumber(accountNum);
        if (status != VALID) {
            printf("\n\t\t✖ Invalid account number!\n");
        } else {
            break;
        }
    } while (1);

    // Get account from database
    if (!getAccount(accountNum, &acc) || acc.user_id != u.id) {
        printf("\n\t\t✖ Account not found or unauthorized access!\n");
        return;
    }

    // Get and validate new owner's username
    do {
        printf("\nEnter the username of the new owner: ");
        scanf("%s", newUsername);
        clearInputBuffer();
        
        if (strlen(newUsername) < 3 || strlen(newUsername) > MAX_NAME_LENGTH) {
            printf("\n\t\t✖ Invalid username length!\n");
            continue;
        }
        
        if (!getUser(newUsername, &newOwner)) {
            printf("\n\t\t✖ User '%s' does not exist!\n", newUsername);
            continue;
        }
        
        if (strcmp(u.username, newUsername) == 0) {
            printf("\n\t\t✖ Cannot transfer to yourself!\n");
            continue;
        }
        
        break;
    } while (1);

    printf("\n\t\tAccount details:");
    printf("\n\t\tAccount Number: %d", acc.account_number);
    printf("\n\t\tBalance: $%.2f", acc.balance);
    printf("\n\t\tAccount Type: %s", acc.account_type);
    
    do {
        printf("\n\nConfirm transfer to %s? (y/n): ", newUsername);
        scanf(" %c", &confirmation);
        clearInputBuffer();
        
        if (confirmation != 'y' && confirmation != 'Y' && 
            confirmation != 'n' && confirmation != 'N') {
            printf("\n\t\t✖ Invalid input!\n");
            continue;
        }
        break;
    } while (1);
    
    if (confirmation == 'y' || confirmation == 'Y') {
        // Update account ownership
        acc.user_id = newOwner.id;
        if (createAccount(&acc)) {  // This will update the existing account
            sendTransferNotification(u.username, newUsername, acc.account_number,
                                  acc.balance, acc.account_type);
            printf("\n\t\t✔ Account successfully transferred to %s!\n", newUsername);
        } else {
            printf("\n\t\t✖ Failed to transfer account!\n");
        }
    } else {
        printf("\n\t\t✖ Transfer cancelled.\n");
    }
    
    printf("\nPress any key to continue...");
    getchar();
}