#include "header.h"

// ============= Core System Functions =============

void die() {
    system("clear");
    printf("\n\n\n\n\n\t\tAn unrecoverable error occurred. Please try again later...\n");
    getchar();
    exit(1);
}

void success(User u) {
    int option;
    printf("\n\n\t\t✔ Success!\n\n");
    do {
        printf("\n\n\t\tEnter\n\t\t1) to go to the main menu\n\t\t2) to exit\n\n\t\t");
        option = input_number();
        system("clear");
        switch (option) {
            case 1: mainMenu(u); break;
            case 2: exit(1);
            default: printf("\n\n\t\tInsert a valid operation!\n"); break;
        }
    } while (option != 1 && option != 2);
}

void Return(User u) {
    int option;
    do {
        printf("\n\n\tEnter\n\t1) to go to the main menu\n\t2) to exit\n\n\t\t");
        option = input_number();
        switch (option) {
            case 1: system("clear"); mainMenu(u); break;
            case 2: system("clear"); exit(1);
            default: printf("\n\t[ERROR] Insert a valid operation!\n");
        }
    } while (option != 1 && option != 2);
}

void StayOrReturn(int notGood, void (*f)(User), User u) {
    int option;
    while (1) {
        if (notGood == 0) {
            printf("\n\n\t\tEnter\n\t\t0) to try again, \n\t\t1) to return to main menu and \n\t\t2) to exit\n\n\t\t");
            option = input_number();
            if (option == 0) {
                f(u);
                break;
            } else if (option == 1) {
                mainMenu(u);
                break;
            } else if (option == 2) {
                system("clear");
                exit(0);
            } else {
                printf("\n\n\t\tPlease insert a valid operation!\n");
            }
        } else {
            printf("\n\n\t\tEnter\n\t\t1) to go to the main menu\n\t\t0) to exit\n\n\t\t");
            option = input_number();
            if (option == 1) {
                system("clear");
                mainMenu(u);
                break;
            } else if (option == 0) {
                system("clear");
                exit(1);
            } else {
                printf("\n\n\t\tPlease insert a valid operation!\n");
            }
        }
    }
}

// ============= Utility Functions =============

bool isdate(Date date) {
    if (date.month < 1 || date.month > 12) return false;
    if (date.day < 1) return false;
    
    time_t tm = time(NULL);
    struct tm *tm_info = localtime(&tm);
    int currentYear = tm_info->tm_year + 1900;
    
    if (date.year < 1472 || date.year > currentYear) return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (date.month == 2 && isleapyear(date.year)) {
        daysInMonth[1] = 29;
    }
    if (date.day > daysInMonth[date.month - 1]) return false;

    return true;
}

void trimlinechar(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

bool ispositive(const char *str) {
    bool hasdot = false;
    const char *str_p = str;

    if (str == NULL || *str == '\0' || *str_p == '-') return false;

    while (*str_p != '\0') {
        if (*str_p == '.') {
            if (hasdot) return false;
            hasdot = true;
        } else if (!isdigit(*str_p)) {
            return false;
        }
        str_p++;
    }
    return true;
}

bool isvaliddouble(const char *str, double *dnum) {
    char *end;
    errno = 0;

    if (!ispositive(str)) return false;

    *dnum = strtod(str, &end);
    if (end == str || *end != '\0' || errno == ERANGE) return false;

    return true;
}

bool isalphabet(char *str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (!isalpha((unsigned char)str[i]) && str[i] != ' ') {
            return false;
        }
    }
    return true;
}

bool isleapyear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isstring(const char *str, size_t n) {
    if (str == NULL || strlen(str) == 0) return false;
    if (strlen(str) >= n) return false;
    return true;
}

double interest(double amount, char *AccountType) {
    double interestRate = 0.0;  // Initialize with default value
    
    if (AccountType == NULL) {
        return 0.0;  // Return 0 interest for invalid account type
    }

    if (strcmp(AccountType, "saving") == 0) {
        interestRate = 0.07;        // 7% for savings
    } else if (strcmp(AccountType, "fixed01") == 0) {
        interestRate = 0.04;        // 4% for 1-year fixed
    } else if (strcmp(AccountType, "fixed02") == 0) {
        interestRate = 0.05;        // 5% for 2-year fixed
    } else if (strcmp(AccountType, "fixed03") == 0) {
        interestRate = 0.08;        // 8% for 3-year fixed
    } else if (strcmp(AccountType, "current") == 0) {
        interestRate = 0.0;         // No interest for current accounts
    }

    return amount * interestRate;
}

// ============= Account Operations =============

void accountCreate(User u) {
    Record r;
    char input[MAXINPUTLEN];

    system("clear");
    printf("\t\t\t========= New record ========\n");

    printf("\n\tEnter the account number");
    r.accountNbr = input_number();
    if(dbAccountExistsInDatabase(r.accountNbr)) {
        printf("\n\t\t✖ The Account Number You Entered already exists\n\n");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    r.userId = dbRetrieveUserId(u.name);
    if (dbAccountExistsForUser(r.userId, r.accountNbr)) {
        printf("\n\t\t✖ The Account Number You Entered already exists\n\n");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    printf("\nEnter today's date (mm/dd/yyyy): ");
    if (scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year) != 3 || !isdate(r.deposit)) {
        printf("\n\t\tInvalid date format or invalid date. Please try again.\n\n");
        StayOrReturn(0, accountCreate, u);
        return;
    }
    while (getchar() != '\n');

    printf("\nEnter the phone number");
    r.phone = input_number();

    printf("\nEnter the country");
    input_string(u, r.country);
    if (!isalphabet(r.country)) {
        printf("\n\tInvalid input. Please enter a valid country name.\n\n");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    printf("\nEnter amount to deposit $");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
        if (!isvaliddouble(input, &r.amount)) {
            printf("\n\t\tInvalid input. Please enter a valid amount.\n\n");
            StayOrReturn(0, accountCreate, u);
            return;
        }
    } else {
        die();
    }

    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice");
    input_string(u, r.accountType);
    bool valid = (strcmp(r.accountType, "fixed01") == 0 || 
                 strcmp(r.accountType, "fixed02") == 0 || 
                 strcmp(r.accountType, "fixed03") == 0 || 
                 strcmp(r.accountType, "current") == 0 || 
                 strcmp(r.accountType, "saving") == 0);
    if (!valid) {
        printf("\n\t\tInvalid Account Type!!!\n");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    r.Accnt_Balance = r.amount;

    if (!dbAccountCreate(u, r)) {
        printf("\n\t\tAn error occurred while trying to create a new account\n");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    system("clear");
    printf("\n\n\t\t\tAccount Created Successfully!!!\n");
    success(u);
}

void accountDetails(User u) {
    int Accntid;
    Record r;
    double balance;

    system("clear");
    printf("\t\t====== Welcome %s =====\n\n", u.name);
    printf("\n\t\tEnter The Account Number you want to check");
    Accntid = input_number();

    if (!dbFetchAccountDetails(u, Accntid, &r, &balance)) {
        StayOrReturn(0, accountDetails, u);
        return;
    }

    printf("\n\n\t\t==================================\n\n");
    printf("\n\t\t====== [ Account details ] ========\n\n");
    printf("\n\t\tID:      %d\n", r.id);
    printf("\n\t\tNumber:  %d\n", r.accountNbr);
    printf("\n\t\tType:    %s\n", r.accountType);
    printf("\n\t\tAmount:  $%.2f\n", r.amount);
    printf("\n\t\tBalance: $%.2f\n", balance);
    printf("\n\t\tCountry: %s\n", r.country);
    printf("\n\t\tPhone:   %d\n", r.phone);

    if (strncmp(r.accountType, "current", 7) == 0) {
        printf("\n\t\tYou will not get interests because the account is of type current\n\n");
        StayOrReturn(1, accountDetails, u);
        return;
    }

    double interestRate = interest(balance, r.accountType);
    if (strncmp(r.accountType, "fixed01", 7) == 0) {
        printf("\n\t\tYou will get $%.2f as interest on day %d/%d/%d.\n\n",
               interestRate, r.deposit.month, r.deposit.day, r.deposit.year + 1);
    } else if (strncmp(r.accountType, "fixed02", 7) == 0) {
        printf("\n\t\tYou will get $%.2f as interest on day %d/%d/%d.\n\n",
               interestRate * 2, r.deposit.month, r.deposit.day, r.deposit.year + 2);
    } else if (strncmp(r.accountType, "fixed03", 7) == 0) {
        printf("\n\t\tYou will get $%.2f as interest on day %d/%d/%d.\n\n",
               interestRate * 3, r.deposit.month, r.deposit.day, r.deposit.year + 3);
    } else if (strncmp(r.accountType, "saving", 6) == 0) {
        printf("\n\t\tYou will get $%.2f as interest on day 10 of every month.\n\n",
               interestRate / 12);
    }

    printf("\n\t\t==================================\n");
    StayOrReturn(1, accountDetails, u);
}

void accountUpdateInfo(User u) {
    Record r;
    int option;

    system("clear");
Reselect:
    printf("\n\t\tEnter Account Number");
    r.accountNbr = input_number();

    r.userId = dbRetrieveUserId(u.name);
    if (!dbAccountExistsForUser(r.userId, r.accountNbr)) {
        printf("\n\t\t✖ The Account Number You Entered does not exist\n\n");
        Return(u);
        return;
    }

    printf("\n\t\tWhich Field You would like to Update\n\n\t\t\t[1] - Phone\n\n\t\t\t[2] - Country\n");
    option = input_number();

    switch (option) {
        case 1:
            printf("\t\tEnter your new phone number");
            r.phone = input_number();
            break;
        case 2:
            printf("\t\tEnter your new country");
            input_string(u, r.country);
            if (!isalphabet(r.country)) {
                printf("\n\t\tInvalid input. Please enter a valid country name\n\n");
                Return(u);
                return;
            }
            break;
        default:
            system("clear");
            printf("\n\n\t\tInvalid Input !!!!");
            goto Reselect;
    }

    if (!dbUpdateAccountDetails(u, r.accountNbr, r, option)) {
        printf("\n\n\t\tAn error occurred while trying to update account information\n");
        Return(u);
        return;
    }
    success(u);
}

void accountList(User u) {
    Record r;
    sqlite3_stmt *stmt = NULL;
    sqlite3 *db = sqliteHandler(DBPATH);
    if (!db) {
        sqliteError(db, "Failed to open database: ", stmt);
    }

    int userid = dbRetrieveUserId(u.name);

    const char *sqlQuery = "SELECT id, country, phone, accountType, accountNbr, amount, "
                          "deposit_month, deposit_day, deposit_year FROM Records WHERE userId = ?";
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, NULL) != SQLITE_OK) {
        sqliteError(db, "Failed to prepare statement: ", stmt);
    }

    sqlite3_bind_int(stmt, 1, userid);
    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        strcpy(r.country, (const char *)sqlite3_column_text(stmt, 1));
        r.phone = sqlite3_column_int(stmt, 2);
        strcpy(r.accountType, (const char *)sqlite3_column_text(stmt, 3));
        r.accountNbr = sqlite3_column_int(stmt, 4);
        r.amount = sqlite3_column_double(stmt, 5);
        r.deposit.month = sqlite3_column_int(stmt, 6);
        r.deposit.day = sqlite3_column_int(stmt, 7);
        r.deposit.year = sqlite3_column_int(stmt, 8);

        printf("\n\t\t\t_____________________\n");
        printf("\n\t\t\tAccount number: %d\n\t\t\tDeposit Date: %d/%d/%d \n\t\t\tCountry: %s \n\t\t\tPhone number: %d \n\t\t\tAmount deposited: $%.2f \n\t\t\tType Of Account: %s\n\t\t\tAccount ID: %d",
               r.accountNbr, r.deposit.day, r.deposit.month, r.deposit.year, r.country, r.phone, r.amount, r.accountType, r.id);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

void accountDelete(User u) {
    int userId = dbRetrieveUserId(u.name);
    int accountId;

    system("clear");
    printf("\n\t\t\tEnter the Account Number you want to delete : ");
    accountId = input_number();

    if (!dbAccountExistsForUser(userId, accountId)) {
        printf("\n\t\t\tAccount ID %d does not Exist for this User", accountId);
        StayOrReturn(1, accountDelete, u);
        return;
    }

    char confirm[5];
    printf("\n\t\tConfirm deletion of account number '%d'? (y/n): ", accountId);
    if (fgets(confirm, sizeof(confirm), stdin) != NULL) {
        trimlinechar(confirm);
        if (!isstring(confirm, 5) || !isalphabet(confirm)) {
            printf("\n\t\t\tInvalid input. Please enter a valid option.\n\n");
            StayOrReturn(0, accountDelete, u);
            return;
        }
    } else {
        printf("\n\t\t\tError reading input.\n\n");
        die();
        return;
    }

    char c = confirm[0];
    if (c == 'n' || c == 'N') {
        printf("\t\t\tAccount deletion cancelled.\n");
        StayOrReturn(0, accountDelete, u);
        return;
    }
    if (c != 'y' && c != 'Y') {
        printf("\t\t\tInvalid input.\n");
        StayOrReturn(0, accountDelete, u);
        return;
    }

    if (dbAccountDelete(userId, accountId)) {
        success(u);
    } else {
        printf("\t\t\tAn error occurred while trying to delete account.\n");
        StayOrReturn(0, mainMenu, u);
    }
}

void accountTransfer(User u) {
    int accnt_id, recipientID;
    
    system("clear");
    printf("\n\t\t\t\t\tAccount Transfer\n\n");
    printf("\t\t\t===========================================\n\n");
    printf("\n\t\tEnter the Account Number you wish to Transfer: ");
    accnt_id = input_number();

    int user_id = dbRetrieveUserId(u.name);
    if (!dbAccountExistsForUser(user_id, accnt_id)) {
        printf("\n\t\t\tAccount ID %d does not belong to user %s\n", accnt_id, u.name);
        StayOrReturn(0, accountTransfer, u);
        return;
    }

    printf("\n\t\t\tEnter the recipient's ID: ");
    recipientID = input_number();

    char *recipientName = dbRetrieveUserName(recipientID);
    if (recipientName == NULL || strlen(recipientName) == 0) {
        printf("\n\t\t\tUser ID %d does not exist.\n\n", recipientID);
        StayOrReturn(0, accountTransfer, u);
        return;
    }

    dbaccountTransfer(recipientID, accnt_id, recipientName);
    success(u);
}

void accountMakeTransaction(User u) {
    int accnt_id, option = 0;
    double amount;
    char transaction_type[10];
    char input[MAXINPUTLEN];

    int user_id = dbRetrieveUserId(u.name);

    system("clear");
    printf("\n\t\t\tEnter your Account Number: ");
    accnt_id = input_number();

    if (!dbAccountExistsForUser(user_id, accnt_id)) {
        printf("\n\t\t\t✖ Account ID %d does not exist for this user %s\n", accnt_id, u.name);
        StayOrReturn(0, accountMakeTransaction, u);
        return;
    }

    if (!accountAllowsTransactions(user_id, accnt_id, u)) {
        Return(u);
        return;
    }

retry:
    printf("\n\t\t\t\tMake Transaction");
    printf("\n\t\t=========================================================\n");
    printf("\n\t\t\t\tDo you Wish to");
    printf("\n\t\t[1]- Deposit\n");
    printf("\n\t\t[2]- Withdraw\n");
    printf("\n\t\t[3]- go to main menu\n");
    option = input_number();

    double balance = accountBalance(user_id, accnt_id, u);

    switch (option) {
        case 1:
            strcpy(transaction_type, "Deposit");
            printf("\n\t\tHow much do you wish to deposit: ");
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = '\0';
                if (!isvaliddouble(input, &amount)) {
                    system("clear");
                    printf("\n\t\tInvalid input. Please enter a valid amount.\n\n");
                    Return(u);
                    return;
                }
            } else {
                die();
            }
            balance += amount;
            dbUpdateAccntBalance(user_id, accnt_id, balance, option);
            break;

        case 2:
            strcpy(transaction_type, "Withdraw");
            printf("\n\t\tHow much do you wish to withdraw: ");
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = '\0';
                if (!isvaliddouble(input, &amount)) {
                    system("clear");
                    printf("\n\t\tInvalid input. Please enter a valid amount.\n\n");
                    Return(u);
                    return;
                }
            } else {
                die();
            }
            if (balance < amount) {
                printf("\n\t\t\tInsufficient balance!!!!\n");
                StayOrReturn(0, accountMakeTransaction, u);
                return;
            }
            balance -= amount;
            dbUpdateAccntBalance(user_id, accnt_id, balance, option);
            break;

        case 3:
            mainMenu(u);
            return;

        default:
            system("clear");
            printf("\n\t\tInvalid Input");
            goto retry;
    }

    dbRecordTransaction(accnt_id, transaction_type, amount);
    printf("\n\t\t\tTransaction successful\n");
    success(u);
}