#include "header.h"
#include <time.h>

int getAccountFromFile(FILE *ptr, char name[50], struct Record *r)
{
    return fscanf(ptr, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r->id,
		  &r->userId,
		  name,
                  &r->accountNbr,
                  &r->deposit.month,
                  &r->deposit.day,
                  &r->deposit.year,
                  r->country,
                  &r->phone,
                  &r->amount,
                  r->accountType) != EOF;
}

// Get the next available record ID
int getNextRecordId(FILE *fp) {
    struct Record r;
    char userName[50];
    int maxId = -1;
    
    while (getAccountFromFile(fp, userName, &r)) {
        if (r.id > maxId) {
            maxId = r.id;
        }
    }
    return maxId + 1;
}

// Fixed version of saveAccountToFile
void saveAccountToFile(FILE *ptr, struct User u, struct Record r) {
    fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
            r.id,          
            u.id,          
            u.name,        
            r.accountNbr,
            r.deposit.month,
            r.deposit.day,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
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
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void createNewAcc(struct User u) {
    struct Record r;
    struct Record cr;
    char userName[50];
    char phoneStr[MAX_PHONE_LENGTH + 1];
    struct ValidationResult result;
    
    FILE *pf = fopen(RECORDS, "a+");
    if (pf == NULL) {
        printf("\n\t\t✖ Error opening file!\n");
        printf("\nPress any key to return to main menu...");
        getchar();
        mainMenu(u);
        return;
    }

    r.id = getNextRecordId(pf);
    r.userId = u.id;

    system("clear");
    printf("\t\t\t===== New record =====\n");

    // Get and validate date
    result = getDateInput(&r.deposit.month, &r.deposit.day, &r.deposit.year);
    if (!result.isValid) {
        fclose(pf);
        mainMenu(u);
        return;
    }

    // Get and validate account number
    result = getAccountNumberInput(&r.accountNbr);
    if (!result.isValid) {
        fclose(pf);
        mainMenu(u);
        return;
    }

    // Check for duplicate account numbers
    rewind(pf);
    while (getAccountFromFile(pf, userName, &cr)) {
        if (cr.accountNbr == r.accountNbr) {
            printf("\n\t\t✖ This account number already exists!\n");
            printf("\nPress any key to return to main menu...");
            getchar();
            fclose(pf);
            mainMenu(u);
            return;
        }
    }

    // Get and validate country
    result = getCountryInput(r.country);
    if (!result.isValid) {
        fclose(pf);
        mainMenu(u);
        return;
    }

    // Get and validate phone
    result = getPhoneInput(phoneStr);
    if (!result.isValid) {
        fclose(pf);
        mainMenu(u);
        return;
    }
    r.phone = atoi(phoneStr);

    // Get and validate amount
    result = getAmountInput(&r.amount);
    if (!result.isValid) {
        fclose(pf);
        mainMenu(u);
        return;
    }

    // Get and validate account type
    result = getAccountTypeInput(r.accountType);
    if (!result.isValid) {
        fclose(pf);
        mainMenu(u);
        return;
    }

    // Save the account
    fseek(pf, 0, SEEK_END);
    saveAccountToFile(pf, u, r);
    fclose(pf);

    printf("\n\t\t✔ Account created successfully!\n");
    printf("\nPress any key to return to main menu...");
    getchar();
    mainMenu(u);
}

void checkAllAccounts(struct User u)
{
    char userName[100];
    struct Record r;

    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0)
        {
            printf("_____________________\n");
            printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
                   r.accountNbr,
                   r.deposit.day,
                   r.deposit.month,
                   r.deposit.year,
                   r.country,
                   r.phone,
                   r.amount,
                   r.accountType);
        }
    }
    fclose(pf);
    success(u);
}

// updateAccount function
void updateAccount(struct User u) {
    FILE *old, *new;
    struct Record r;
    char userName[50];
    char phoneStr[MAX_PHONE_LENGTH + 1];
    int choice, accountNum, found = 0;
    int status;
    
    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\n\t\t✖ Error opening file!\n");
        return;
    }
    
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\n\t\t✖ Error creating temporary file!\n");
        fclose(old);
        return;
    }

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

    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\n\t\t✔ Account found! Current details:");
            printf("\n\t\tCountry: %s", r.country);
            printf("\n\t\tPhone: %d", r.phone);
            
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
                            scanf("%s", r.country);
                            clearInputBuffer();
                            status = validateCountry(r.country);
                            if (status != VALID) {
                                printf("\n\t\t✖ Invalid country name! Use only letters, spaces, and hyphens.\n");
                            }
                        } while (status != VALID);
                        break;

                    case 2:
                        do {
                            printf("\nEnter new phone number: ");
                            scanf("%s", phoneStr);
                            clearInputBuffer();
                            status = validatePhone(phoneStr);
                            if (status != VALID) {
                                printf("\n\t\t✖ Invalid phone number! Use only digits, +, and -\n");
                                continue;
                            }
                            r.phone = atoi(phoneStr);
                            break;
                        } while (1);
                        break;

                    case 3:
                        printf("\n\t\t✖ Update cancelled.\n");
                        saveAccountToFile(new, u, r);
                        goto cleanup;

                    default:
                        printf("\n\t\t✖ Invalid choice! Please enter 1, 2, or 3.\n");
                        continue;
                }
                break;
            } while (1);
            
            printf("\n\t\t✔ Account updated successfully!\n");
        }
        saveAccountToFile(new, u, r);
    }

cleanup:
    if (!found) {
        printf("\n\t\t✖ Account not found!\n");
        remove("./data/temp.txt");
    } else {
        remove(RECORDS);
        rename("./data/temp.txt", RECORDS);
    }

    fclose(old);
    fclose(new);

    printf("\nPress any key to continue...");
    getchar();
}

// makeTransaction function
void makeTransaction(struct User u) {
    FILE *old, *new;
    struct Record r;
    char userName[50];
    int choice, accountNum, found = 0;
    double amount;
    int status;
    
    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\n\t\t✖ Error opening file!\n");
        return;
    }
    
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\n\t\t✖ Error creating temporary file!\n");
        fclose(old);
        return;
    }

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
            printf("\n\t\t✖ Invalid account number! Must be 5 digits.\n");
        } else {
            break;
        }
    } while (1);

    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            
            // Check if account type allows transactions
            if (!canMakeTransaction(r.accountType)) {
                printf("\n\t\t✖ Error: Transactions not allowed for %s account type!\n", r.accountType);
                saveAccountToFile(new, u, r);
                continue;
            }

            printf("\n\t\tCurrent balance: $%.2f", r.amount);
            
            do {
                printf("\n1. Deposit");
                printf("\n2. Withdraw");
                printf("\n3. Cancel Transaction");
                printf("\nEnter choice (1-3): ");
                
                if (scanf("%d", &choice) != 1) {
                    clearInputBuffer();
                    printf("\n\t\t✖ Invalid choice! Please enter a number.\n");
                    continue;
                }
                clearInputBuffer();

                if (choice < 1 || choice > 3) {
                    printf("\n\t\t✖ Invalid choice! Please enter 1, 2, or 3.\n");
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
                            printf("\n\t\t✖ Invalid amount! Must be between $%.2f and $%.2f\n",
                                   MIN_AMOUNT, MAX_AMOUNT);
                            continue;
                        }
                        r.amount += amount;
                        // Update deposit date
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        r.deposit.day = tm.tm_mday;
                        r.deposit.month = tm.tm_mon + 1;
                        r.deposit.year = tm.tm_year + 1900;
                        printf("\n\t\t✔ Deposit successful! New balance: $%.2f\n", r.amount);
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
                            printf("\n\t\t✖ Invalid amount! Must be between $%.2f and $%.2f\n",
                                   MIN_AMOUNT, MAX_AMOUNT);
                            continue;
                        }
                        if (amount > r.amount) {
                            printf("\n\t\t✖ Insufficient funds! Available balance: $%.2f\n", r.amount);
                            continue;
                        }
                        r.amount -= amount;
                        // Update withdraw date
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        r.withdraw.day = tm.tm_mday;
                        r.withdraw.month = tm.tm_mon + 1;
                        r.withdraw.year = tm.tm_year + 1900;
                        printf("\n\t\t✔ Withdrawal successful! New balance: $%.2f\n", r.amount);
                        break;
                    } while (1);
                    break;

                case 3:  // Cancel
                    printf("\n\t\t✖ Transaction cancelled.\n");
                    break;
            }
        }
        saveAccountToFile(new, u, r);
    }

    if (!found) {
        printf("\n\t\t✖ Account not found!\n");
        remove("./data/temp.txt");
    } else {
        remove(RECORDS);
        rename("./data/temp.txt", RECORDS);
    }

    fclose(old);
    fclose(new);

    printf("\nPress any key to continue...");
    getchar();
}

// Function to check if account type allows transactions
int canMakeTransaction(const char* accountType) {
    return (strcmp(accountType, "fixed01") != 0 && 
            strcmp(accountType, "fixed02") != 0 && 
            strcmp(accountType, "fixed03") != 0);
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


// Updated checkAccountDetails function
void checkAccountDetails(struct User u) {
    FILE *fp;
    struct Record r;
    char userName[50];
    int accountNum, found = 0;
    int status;
    
    system("clear");
    printf("\n\t\t====== Account Details =====\n");

    // Account number validation
    do {
        printf("\nEnter the account number you want to check: ");
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
    
    fp = fopen(RECORDS, "r");
    if (fp == NULL) {
        printf("\n\t\t✖ Error opening file!\n");
        return;
    }
    
    while (getAccountFromFile(fp, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\n\t\t===== Account Information =====");
            printf("\n\n\t\tAccount Number: %d", r.accountNbr);
            printf("\n\t\tDeposit Date: %d/%d/%d", r.deposit.month, r.deposit.day, r.deposit.year);
            printf("\n\t\tCountry: %s", r.country);
            printf("\n\t\tPhone Number: %d", r.phone);
            printf("\n\t\tBalance: $%.2f", r.amount);
            printf("\n\t\tAccount Type: %s", r.accountType);
            
            // Calculate and display interest information
            if (strcmp(r.accountType, "current") == 0) {
                printf("\n\n\t\tYou will not get interests because the account is of type current");
            } else {
                double interest = calculateInterest(r.accountType, r.amount);
                printf("\n\n\t\tYou will get $%.2f as interest on day %d of every month", 
                       interest, r.deposit.day);
            }
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\n\t\t✖ Account not found!\n");
    }
    
    printf("\n\nPress any key to continue...");
    getchar();
    getchar();
}

void removeExistingAccount(struct User u) {
    FILE *old, *new;
    struct Record r;
    char userName[50];
    int accountNum, found = 0;
    char confirmation;
    int status;
    
    system("clear");
    printf("\n\t\t====== Remove Account =====\n");

    // Account number validation
    do {
        printf("\nEnter the account number you want to remove: ");
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
    
    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\n\t\t✖ Error opening file!\n");
        return;
    }
    
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\n\t\t✖ Error creating temporary file!\n");
        fclose(old);
        return;
    }
    
    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\n\t\tAccount found with the following details:");
            printf("\n\t\tAccount Number: %d", r.accountNbr);
            printf("\n\t\tBalance: $%.2f", r.amount);
            printf("\n\t\tAccount Type: %s", r.accountType);
            
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
                if (r.amount > 0) {
                    printf("\n\t\t⚠ Warning: Account has remaining balance of $%.2f", r.amount);
                    do {
                        printf("\nAre you absolutely sure you want to remove this account? (y/n): ");
                        scanf(" %c", &confirmation);
                        clearInputBuffer();
                        
                        if (confirmation != 'y' && confirmation != 'Y' && 
                            confirmation != 'n' && confirmation != 'N') {
                            printf("\n\t\t✖ Invalid input! Please enter 'y' or 'n'.\n");
                            continue;
                        }
                        break;
                    } while (1);
                }
                
                if (confirmation == 'y' || confirmation == 'Y') {
                    printf("\n\t\t✔ Account successfully removed!\n");
                    continue;  // Skip writing this record to new file
                }
            }
            // If we get here, either initial confirmation was 'n' or second confirmation was 'n'
            printf("\n\t\t✖ Account removal cancelled.\n");
            saveAccountToFile(new, u, r);
        } else {
            saveAccountToFile(new, u, r);
        }
    }
    
    fclose(old);
    fclose(new);
    
    if (!found) {
        printf("\n\t\t✖ Account not found!\n");
        remove("./data/temp.txt");
    } else {
        remove(RECORDS);
        rename("./data/temp.txt", RECORDS);
    }
    
    printf("\nPress any key to continue...");
    getchar();
}

// Helper function to find user ID by name
int findUserIdByName(const char* userName) {
    FILE *fp;
    struct User user;
    int userId = -1;

    fp = fopen(USERS, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fscanf(fp, "%d %s %s", &user.id, user.name, user.password) != EOF) {
        if (strcmp(user.name, userName) == 0) {
            userId = user.id;
            break;
        }
    }

    fclose(fp);
    return userId;
}

// Helper function to check if user exists
int userExists(const char* userName) {
    return findUserIdByName(userName) != -1;
}

// transferOwnership function
void transferOwnership(struct User u) {
    FILE *old, *new;
    struct Record r;
    char userName[50];
    char newOwner[50];
    int accountNum, found = 0;
    char confirmation;
    int status;
    
    system("clear");
    printf("\n\t\t====== Transfer Account Ownership =====\n");
    
    // Account number validation
    do {
        printf("\nEnter the account number you want to transfer: ");
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
    
    // Get and validate new owner's username
    do {
        printf("\nEnter the username of the new owner: ");
        if (scanf("%s", newOwner) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid username format!\n");
            continue;
        }
        clearInputBuffer();
        
        // Check username length
        if (strlen(newOwner) < 3 || strlen(newOwner) > MAX_NAME_LENGTH) {
            printf("\n\t\t✖ Invalid username length! Must be between 3 and %d characters.\n", 
                   MAX_NAME_LENGTH);
            continue;
        }
        
        // Verify new owner exists
        if (!userExists(newOwner)) {
            printf("\n\t\t✖ Error: User '%s' does not exist!\n", newOwner);
            continue;
        }
        
        // Cannot transfer to self
        if (strcmp(u.name, newOwner) == 0) {
            printf("\n\t\t✖ Error: Cannot transfer account to yourself!\n");
            continue;
        }
        
        break;
    } while (1);

    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\n\t\t✖ Error opening records file!\n");
        return;
    }
    
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\n\t\t✖ Error creating temporary file!\n");
        fclose(old);
        return;
    }
    
    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\n\t\tAccount found with the following details:");
            printf("\n\t\tAccount Number: %d", r.accountNbr);
            printf("\n\t\tBalance: $%.2f", r.amount);
            printf("\n\t\tAccount Type: %s", r.accountType);
            
            do {
                printf("\n\nAre you sure you want to transfer this account to %s? (y/n): ", newOwner);
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
                // Update the record with new owner information
                r.userId = findUserIdByName(newOwner);
                strncpy(r.name, newOwner, sizeof(r.name) - 1);
                r.name[sizeof(r.name) - 1] = '\0';

                // Send notification to the new owner
                sendTransferNotification(u.name, newOwner, r.accountNbr, 
                                      r.amount, r.accountType);

                printf("\n\t\t✔ Account successfully transferred to %s!\n", newOwner);
            } else {
                printf("\n\t\t✖ Transfer cancelled.\n");
            }
        }
        saveAccountToFile(new, u, r);
    }
    
    fclose(old);
    fclose(new);
    
    if (!found) {
        printf("\n\t\t✖ Account not found!\n");
        remove("./data/temp.txt");
    } else {
        remove(RECORDS);
        rename("./data/temp.txt", RECORDS);
    }
    
    printf("\nPress any key to continue...");
    getchar();
}