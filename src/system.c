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
    FILE *pf = fopen(RECORDS, "a+");
    
    if (pf == NULL) {
        printf("\nError opening file!\n");
        return;
    }

    // Get next available ID
    r.id = getNextRecordId(pf);
    r.userId = u.id;  // Set the user ID from the logged-in user

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date(mm/dd/yyyy):");
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);

    // Reset file position to start for checking duplicates
    rewind(pf);
    
    while (getAccountFromFile(pf, userName, &cr)) {
        if (strcmp(userName, u.name) == 0 && cr.accountNbr == r.accountNbr) {
            printf("✖ This Account already exists for this user\n\n");
            goto noAccount;
        }
    }

    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

    // Move file pointer to end for appending
    fseek(pf, 0, SEEK_END);
    saveAccountToFile(pf, u, r);

    fclose(pf);
    success(u);
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

void updateAccount(struct User u)
{
    FILE *old, *new;
    struct Record r;
    char userName[50];
    int choice, accountNum, found = 0;
    
    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\nError opening file!");
        return;
    }
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\nError creating temporary file!");
        fclose(old);
        return;
    }

    printf("\nEnter account number you want to update: ");
    scanf("%d", &accountNum);

    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\nAccount found! Current details:");
            printf("\nCountry: %s", r.country);
            printf("\nPhone: %d", r.phone);
            
            printf("\n\nWhat would you like to update?");
            printf("\n1. Country");
            printf("\n2. Phone Number");
            printf("\nEnter choice (1-2): ");
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    printf("\nEnter new country: ");
                    scanf("%s", r.country);
                    break;
                case 2:
                    printf("\nEnter new phone number: ");
                    scanf("%d", &r.phone);
                    break;
                default:
                    printf("\nInvalid choice!");
                    // Copy record without changes
                    saveAccountToFile(new, u, r);
                    continue;
            }
            printf("\nAccount updated successfully!");
        }
        saveAccountToFile(new, u, r);
    }

    if (!found) {
        printf("\nAccount not found!");
    }

    fclose(old);
    fclose(new);

    // Replace old file with new file
    remove(RECORDS);
    rename("./data/temp.txt", RECORDS);

    printf("\nPress any key to continue...");
    getchar();
    getchar();
}

// Function to check if account type allows transactions
int canMakeTransaction(const char* accountType) {
    return (strcmp(accountType, "fixed01") != 0 && 
            strcmp(accountType, "fixed02") != 0 && 
            strcmp(accountType, "fixed03") != 0);
}

void makeTransaction(struct User u)
{
    FILE *old, *new;
    struct Record r;
    char userName[50];
    int choice, accountNum, found = 0;
    double amount;
    
    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\nError opening file!");
        return;
    }
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\nError creating temporary file!");
        fclose(old);
        return;
    }

    printf("\nEnter account number for transaction: ");
    scanf("%d", &accountNum);

    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            
            // Check if account type allows transactions
            if (!canMakeTransaction(r.accountType)) {
                printf("\nError: Transactions not allowed for %s account type!", r.accountType);
                saveAccountToFile(new, u, r);
                continue;
            }

            printf("\nCurrent balance: $%.2f", r.amount);
            printf("\n1. Deposit");
            printf("\n2. Withdraw");
            printf("\nEnter choice (1-2): ");
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    printf("\nEnter amount to deposit: $");
                    scanf("%lf", &amount);
                    if (amount <= 0) {
                        printf("\nInvalid amount!");
                        break;
                    }
                    r.amount += amount;
                    // Update deposit date
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    r.deposit.day = tm.tm_mday;
                    r.deposit.month = tm.tm_mon + 1;
                    r.deposit.year = tm.tm_year + 1900;
                    printf("\nDeposit successful! New balance: $%.2f", r.amount);
                    break;

                case 2:
                    printf("\nEnter amount to withdraw: $");
                    scanf("%lf", &amount);
                    if (amount <= 0) {
                        printf("\nInvalid amount!");
                        break;
                    }
                    if (amount > r.amount) {
                        printf("\nInsufficient funds!");
                        break;
                    }
                    r.amount -= amount;
                    // Update withdraw date
                    t = time(NULL);
                    tm = *localtime(&t);
                    r.withdraw.day = tm.tm_mday;
                    r.withdraw.month = tm.tm_mon + 1;
                    r.withdraw.year = tm.tm_year + 1900;
                    printf("\nWithdrawal successful! New balance: $%.2f", r.amount);
                    break;

                default:
                    printf("\nInvalid choice!");
                    break;
            }
        }
        saveAccountToFile(new, u, r);
    }

    if (!found) {
        printf("\nAccount not found!");
    }

    fclose(old);
    fclose(new);

    // Replace old file with new file
    remove(RECORDS);
    rename("./data/temp.txt", RECORDS);

    printf("\nPress any key to continue...");
    getchar();
    getchar();
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

void checkAccountDetails(struct User u) {
    FILE *fp;
    struct Record r;
    char userName[50];
    int accountNum, found = 0;
    
    system("clear");
    printf("\n\t\t====== Account Details =====\n");
    printf("\nEnter the account number you want to check: ");
    scanf("%d", &accountNum);
    
    fp = fopen(RECORDS, "r");
    if (fp == NULL) {
        printf("\nError opening file!");
        return;
    }
    
    while (getAccountFromFile(fp, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\n\t\t===== Account Information =====");
            printf("\n\nAccount Number: %d", r.accountNbr);
            printf("\nDeposit Date: %d/%d/%d", r.deposit.month, r.deposit.day, r.deposit.year);
            printf("\nCountry: %s", r.country);
            printf("\nPhone Number: %d", r.phone);
            printf("\nBalance: $%.2f", r.amount);
            printf("\nAccount Type: %s", r.accountType);
            
            // Calculate and display interest information
            if (strcmp(r.accountType, "current") == 0) {
                printf("\n\nYou will not get interests because the account is of type current");
            } else {
                double interest = calculateInterest(r.accountType, r.amount);
                printf("\n\nYou will get $%.2f as interest on day %d of every month", 
                       interest, r.deposit.day);
            }
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nAccount not found!");
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
    
    system("clear");
    printf("\n\t\t====== Remove Account =====\n");
    printf("\nEnter the account number you want to remove: ");
    scanf("%d", &accountNum);
    
    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\nError opening file!");
        return;
    }
    
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\nError creating temporary file!");
        fclose(old);
        return;
    }
    
    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\nAccount found with the following details:");
            printf("\nAccount Number: %d", r.accountNbr);
            printf("\nBalance: $%.2f", r.amount);
            printf("\nAccount Type: %s", r.accountType);
            
            printf("\n\nAre you sure you want to remove this account? (y/n): ");
            scanf(" %c", &confirmation);
            
            if (confirmation == 'y' || confirmation == 'Y') {
                printf("\nAccount successfully removed!");
                continue;  // Skip writing this record to new file
            } else {
                printf("\nAccount removal cancelled.");
                saveAccountToFile(new, u, r);
            }
        } else {
            saveAccountToFile(new, u, r);
        }
    }
    
    fclose(old);
    fclose(new);
    
    if (!found) {
        printf("\nAccount not found!");
        remove("./data/temp.txt");  // Clean up temp file
    } else {
        // Replace old file with new file
        remove(RECORDS);
        rename("./data/temp.txt", RECORDS);
    }
    
    printf("\n\nPress any key to continue...");
    getchar();
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

void transferOwnership(struct User u) {
    FILE *old, *new;
    struct Record r;
    char userName[50];
    char newOwner[50];
    int accountNum, found = 0;
    char confirmation;
    
    system("clear");
    printf("\n\t\t====== Transfer Account Ownership =====\n");
    
    // Get account number to transfer
    printf("\nEnter the account number you want to transfer: ");
    scanf("%d", &accountNum);
    
    // Get new owner's username
    printf("\nEnter the username of the new owner: ");
    scanf("%s", newOwner);
    
    // Verify new owner exists
    if (!userExists(newOwner)) {
        printf("\nError: User '%s' does not exist!", newOwner);
        printf("\nPress any key to continue...");
        getchar();
        getchar();
        return;
    }
    
    // Cannot transfer to self
    if (strcmp(u.name, newOwner) == 0) {
        printf("\nError: Cannot transfer account to yourself!");
        printf("\nPress any key to continue...");
        getchar();
        getchar();
        return;
    }

    old = fopen(RECORDS, "r");
    if (old == NULL) {
        printf("\nError opening records file!");
        return;
    }
    
    new = fopen("./data/temp.txt", "w");
    if (new == NULL) {
        printf("\nError creating temporary file!");
        fclose(old);
        return;
    }
    
    while (getAccountFromFile(old, userName, &r)) {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum) {
            found = 1;
            printf("\nAccount found with the following details:");
            printf("\nAccount Number: %d", r.accountNbr);
            printf("\nBalance: $%.2f", r.amount);
            printf("\nAccount Type: %s", r.accountType);
            
            printf("\n\nAre you sure you want to transfer this account to %s? (y/n): ", newOwner);
            scanf(" %c", &confirmation);
            
            if (confirmation == 'y' || confirmation == 'Y') {
                // Update the record with new owner information
                r.userId = findUserIdByName(newOwner);
                strcpy(r.name, newOwner);

                // Send notification to the new owner
                sendTransferNotification(u.name, newOwner, r.accountNbr, 
                                      r.amount, r.accountType);

                printf("\nAccount successfully transferred to %s!", newOwner);
            } else {
                printf("\nTransfer cancelled.");
            }
        }
        // Save the record (either original or modified)
        fprintf(new, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                r.id,
                r.userId,
                r.name,
                r.accountNbr,
                r.deposit.month,
                r.deposit.day,
                r.deposit.year,
                r.country,
                r.phone,
                r.amount,
                r.accountType);
    }
    
    fclose(old);
    fclose(new);
    
    if (!found) {
        printf("\nAccount not found!");
        remove("./data/temp.txt");  // Clean up temp file
    } else {
        // Replace old file with new file
        remove(RECORDS);
        rename("./data/temp.txt", RECORDS);
    }
    
    printf("\n\nPress any key to continue...");
    getchar();
    getchar();
}