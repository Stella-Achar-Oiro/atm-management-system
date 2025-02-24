#include "header.h"
#include "ui.h"

// ============= Core System Functions =============

void die(void) {
    clear_screen();
    print_error("An unrecoverable error occurred. Please try again later...");
    getchar();
    exit(1);
}

void success(User u) {
    int option;
    SUCCESS_MSG("Operation completed successfully!");
    
    do {
        print_section_header("Next Steps");
        print_menu_option(1, "Return to main menu");
        print_menu_option(2, "Exit");
        
        print_input_prompt("Select option");
        option = input_number();
        
        if (clear_screen() != -1) {
            switch (option) {
                case 1: mainMenu(u); break;
                case 2: exit(1);
                default: print_error("Invalid operation selected"); break;
            }
        }
    } while (option != 1 && option != 2);
}

void Return(User u) {
    int option;
    do {
        print_section_header("Options");
        print_menu_option(1, "Return to main menu");
        print_menu_option(2, "Exit");
        
        print_input_prompt("Select option");
        option = input_number();
        
        switch (option) {
            case 1: 
                if (clear_screen() != -1) {
                    mainMenu(u);
                }
                break;
            case 2: 
                if (clear_screen() != -1) {
                    exit(1);
                }
                break;
            default: 
                print_error("Invalid operation selected");
        }
    } while (option != 1 && option != 2);
}

void StayOrReturn(int notGood, void (*f)(User), User u) {
    int option;
    while (1) {
        if (notGood == 0) {
            print_section_header("Options");
            print_menu_option(0, "Try again");
            print_menu_option(1, "Return to main menu");
            print_menu_option(2, "Exit");
            
            print_input_prompt("Select option");
            option = input_number();
            
            if (option == 0) {
                f(u);
                break;
            } else if (option == 1) {
                mainMenu(u);
                break;
            } else if (option == 2) {
                if (clear_screen() != -1) {
                    exit(0);
                }
            } else {
                print_error("Please insert a valid operation!");
            }
        } else {
            print_section_header("Options");
            print_menu_option(1, "Return to main menu");
            print_menu_option(0, "Exit");
            
            print_input_prompt("Select option");
            option = input_number();
            
            if (option == 1) {
                if (clear_screen() != -1) {
                    mainMenu(u);
                }
                break;
            } else if (option == 0) {
                if (clear_screen() != -1) {
                    exit(1);
                }
            } else {
                print_error("Please insert a valid operation!");
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

bool is_valid_phone(int number) {
    // Phone numbers should be positive and have a reasonable number of digits
    if (number <= 0) return false;
    
    // Count digits
    int digits = 0;
    long temp = number;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    
    // Validate number of digits (e.g., between 7 and 10 digits)
    return (digits >= 7 && digits <= 10);
}

bool is_valid_account_number(int number) {
    // Must be positive
    if (number <= 0) return false;
    
    // Count digits in the number
    int digits = 0;
    long temp = number;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    
    // Check if digits are in valid range (5-9 digits)
    if (!(digits >= 5 && digits <= 9)) {
        print_error("Account number must be between 5 and 9 digits");
        return false;
    }
    
    // Check if account number already exists
    if (dbAccountExistsInDatabase(number)) {
        print_error("Account number already exists. Please choose a different number");
        return false;
    }
    
    return true;
}

bool is_valid_username(const char* name) {
    if (!name || strlen(name) < 3) return false;  // At least 3 characters
    if (strlen(name) > 30) return false;          // Not too long
    
    // Check if it's not just spaces
    bool has_non_space = false;
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isspace(name[i])) {
            has_non_space = true;
            break;
        }
    }
    return has_non_space;
}

bool is_valid_password(const char* password) {
    // Check if password exists and meets minimum length (e.g., 6 characters)
    if (!password || strlen(password) < 6) return false;
    
    // Check if password is not just spaces
    bool has_non_space = false;
    for (size_t i = 0; i < strlen(password); i++) {
        if (!isspace(password[i])) {
            has_non_space = true;
            break;
        }
    }
    
    return has_non_space;
}

double interest(double amount, char *AccountType) {
    double interestRate = 0.0;
    
    if (AccountType == NULL) {
        return 0.0;
    }

    if (strcmp(AccountType, "saving") == 0) {
        interestRate = 0.07;
    } else if (strcmp(AccountType, "fixed01") == 0) {
        interestRate = 0.04;
    } else if (strcmp(AccountType, "fixed02") == 0) {
        interestRate = 0.05;
    } else if (strcmp(AccountType, "fixed03") == 0) {
        interestRate = 0.08;
    } else if (strcmp(AccountType, "current") == 0) {
        interestRate = 0.0;
    }

    return amount * interestRate;
}

// ============= Account Operations =============

void accountCreate(User u) {
    Record r;
    char input[MAXINPUTLEN];

    clear_screen();
    print_header("Create New Account");

    print_input_prompt("Enter account number");
    do {
        r.accountNbr = input_number();
        if (!is_valid_account_number(r.accountNbr)) {
            print_error("Account number must be positive and less than 1000000");
            }
    } while (!is_valid_account_number(r.accountNbr));
    
    if(dbAccountExistsInDatabase(r.accountNbr)) {
        print_error("The Account Number you entered already exists");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    r.userId = dbRetrieveUserId(u.name);
    if (dbAccountExistsForUser(r.userId, r.accountNbr)) {
        print_error("The Account Number you entered already exists");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    print_input_prompt("Enter today's date (mm/dd/yyyy)");
    if (scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year) != 3 || !isdate(r.deposit)) {
        print_error("Invalid date. Date must be today or in the future, and within next year.");
        print_error("Format: mm/dd/yyyy");
        StayOrReturn(0, accountCreate, u);
        return;
    }
    while (getchar() != '\n');

    print_input_prompt("Enter phone number");
    do {
        r.phone = input_number();
        if (!is_valid_phone(r.phone)) {
            print_error("Please enter a valid phone number");
        }
    } while (!is_valid_phone(r.phone));

    print_input_prompt("Enter country");
    input_string(u, r.country);
    if (!isalphabet(r.country)) {
        print_error("Invalid input. Please enter a valid country name");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    print_input_prompt("Enter amount to deposit");
    print_currency(0.00);
    printf(" ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
        if (!isvaliddouble(input, &r.amount)) {
            print_error("Invalid input. Please enter a valid amount.");
            StayOrReturn(0, accountCreate, u);
            return;
        }
    } else {
        die();
    }

    print_section_header("Account Types");
    printf("  • saving\n");
    printf("  • current\n");
    printf("  • fixed01 (1 year)\n");
    printf("  • fixed02 (2 years)\n");
    printf("  • fixed03 (3 years)\n\n");
    
    print_input_prompt("Enter your choice");
    input_string(u, r.accountType);
    
    bool valid = (strcmp(r.accountType, "fixed01") == 0 || 
                 strcmp(r.accountType, "fixed02") == 0 || 
                 strcmp(r.accountType, "fixed03") == 0 || 
                 strcmp(r.accountType, "current") == 0 || 
                 strcmp(r.accountType, "saving") == 0);
                 
    if (!valid) {
        print_error("Invalid Account Type!");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    r.Accnt_Balance = r.amount;

    if (!dbAccountCreate(u, r)) {
        print_error("An error occurred while trying to create a new account");
        StayOrReturn(0, accountCreate, u);
        return;
    }

    clear_screen();
    SUCCESS_MSG("Account Created Successfully!");
    success(u);
}

void accountDetails(User u) {
    int Accntid;
    Record r;
    double balance;

    clear_screen();
    print_header("Account Details");
    printf("\nWelcome, %s!\n", u.name);

    print_input_prompt("Enter Account Number to check");
    Accntid = input_number();

    if (!dbFetchAccountDetails(u, Accntid, &r, &balance)) {
        StayOrReturn(0, accountDetails, u);
        return;
    }

    print_section_header("Account Information");
    
    print_account_detail_numeric("ID", r.id);
    print_account_detail_numeric("Account Number", r.accountNbr);
    print_account_info("Type", r.accountType);
    print_account_detail_double("Amount", r.amount);
    print_account_detail_double("Balance", balance);
    print_account_info("Country", r.country);
    print_account_detail_numeric("Phone", r.phone);

    if (strncmp(r.accountType, "current", 7) == 0) {
        INFO_MSG("You will not get interests because the account is of type current");
        StayOrReturn(1, accountDetails, u);
        return;
    }

    double interestRate = interest(balance, r.accountType);
    print_section_header("Interest Information");

    if (strncmp(r.accountType, "fixed01", 7) == 0) {
        print_date("Interest Date", r.deposit.month, r.deposit.day, r.deposit.year + 1);
        print_account_detail_double("Interest Amount", interestRate);
    } else if (strncmp(r.accountType, "fixed02", 7) == 0) {
        print_date("Interest Date", r.deposit.month, r.deposit.day, r.deposit.year + 2);
        print_account_detail_double("Interest Amount", interestRate * 2);
    } else if (strncmp(r.accountType, "fixed03", 7) == 0) {
        print_date("Interest Date", r.deposit.month, r.deposit.day, r.deposit.year + 3);
        print_account_detail_double("Interest Amount", interestRate * 3);
    } else if (strncmp(r.accountType, "saving", 6) == 0) {
        INFO_MSG("Interest is paid monthly on day 10");
        print_account_detail_double("Monthly Interest", interestRate / 12);
    }

    print_section_header("End of Details");
    StayOrReturn(1, accountDetails, u);
}

void accountUpdateInfo(User u) {
    Record r;
    int option;

    clear_screen();
    print_header("Update Account Information");

Reselect:
    print_input_prompt("Enter Account Number");
    r.accountNbr = input_number();

    r.userId = dbRetrieveUserId(u.name);
    if (!dbAccountExistsForUser(r.userId, r.accountNbr)) {
        print_error("The Account Number you entered does not exist");
        Return(u);
        return;
    }

    print_section_header("Update Options");
    print_menu_option(1, "Update Phone Number");
    print_menu_option(2, "Update Country");
    
    print_input_prompt("Select option");
    option = input_number();

    switch (option) {
        case 1:
            print_input_prompt("Enter new phone number");
            r.phone = input_number();
            break;
        case 2:
            print_input_prompt("Enter new country");
            input_string(u, r.country);
            if (!isalphabet(r.country)) {
                print_error("Invalid input. Please enter a valid country name");
                Return(u);
                return;
            }
            break;
        default:
            clear_screen();
            print_error("Invalid Input!");
            goto Reselect;
    }

    if (!dbUpdateAccountDetails(u, r.accountNbr, r, option)) {
        print_error("An error occurred while trying to update account information");
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
    clear_screen();
    print_header("Account List");
    printf("\nAccounts for user: %s\n\n", u.name);

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

        print_section_header("Account Details");
        print_account_number(r.accountNbr);
        print_date("Deposit Date", r.deposit.month, r.deposit.day, r.deposit.year);
        print_account_info("Country", r.country);
        print_account_detail_numeric("Phone", r.phone);
        print_account_detail_double("Amount deposited", r.amount);
        print_account_info("Account Type", r.accountType);
        print_account_detail_numeric("Account ID", r.id);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

void accountDelete(User u) {
    int userId = dbRetrieveUserId(u.name);
    int accountId;

    clear_screen();
    print_header("Delete Account");
    
    print_input_prompt("Enter Account Number to delete");
    accountId = input_number();

    if (!dbAccountExistsForUser(userId, accountId)) {
        print_error("Account does not exist for this user");
        StayOrReturn(1, accountDelete, u);
        return;
    }

    char confirm[5];
    printf("\nConfirm deletion of account %d? (y/n): ", accountId);
    if (fgets(confirm, sizeof(confirm), stdin) != NULL) {
        trimlinechar(confirm);
        if (!isstring(confirm, 5) || !isalphabet(confirm)) {
            print_error("Invalid input. Please enter a valid option.");
            StayOrReturn(0, accountDelete, u);
            return;
        }
    } else {
        print_error("Error reading input.");
        die();
        return;
    }

    char c = confirm[0];
    if (c == 'n' || c == 'N') {
        INFO_MSG("Account deletion cancelled.");
        StayOrReturn(0, accountDelete, u);
        return;
    }
    if (c != 'y' && c != 'Y') {
        print_error("Invalid input.");
        StayOrReturn(0, accountDelete, u);
        return;
    }

    if (dbAccountDelete(userId, accountId)) {
        success(u);
    } else {
        print_error("Failed to delete account.");
        StayOrReturn(0, mainMenu, u);
    }
}

void accountTransfer(User u) {
    int accnt_id, recipientID;
    
    clear_screen();
    print_header("Account Transfer");
    
    print_input_prompt("Enter Account Number to transfer");
    accnt_id = input_number();

    int user_id = dbRetrieveUserId(u.name);
    if (!dbAccountExistsForUser(user_id, accnt_id)) {
        print_error("Account does not belong to current user");
        StayOrReturn(0, accountTransfer, u);
        return;
    }

    print_input_prompt("Enter recipient's ID");
    recipientID = input_number();

    char *recipientName = dbRetrieveUserName(recipientID);
    if (recipientName == NULL || strlen(recipientName) == 0) {
        print_error("Recipient user does not exist");
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

    clear_screen();
    print_header("Make Transaction");
    
    print_input_prompt("Enter Account Number");
    accnt_id = input_number();

    if (!dbAccountExistsForUser(user_id, accnt_id)) {
        print_error("Account does not exist for this user");
        StayOrReturn(0, accountMakeTransaction, u);
        return;
    }

    if (!accountAllowsTransactions(user_id, accnt_id, u)) {
        Return(u);
        return;
    }

retry:
    print_section_header("Transaction Options");
    print_menu_option(1, "Deposit");
    print_menu_option(2, "Withdraw");
    print_menu_option(3, "Return to main menu");
    
    print_input_prompt("Select option");
    option = input_number();

    double balance = accountBalance(user_id, accnt_id, u);

    switch (option) {
        case 1:
            strcpy(transaction_type, "Deposit");
            print_input_prompt("Enter deposit amount");
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = '\0';
                if (!isvaliddouble(input, &amount)) {
                    clear_screen();
                    print_error("Invalid amount entered");
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
            print_input_prompt("Enter withdrawal amount");
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = '\0';
                if (!isvaliddouble(input, &amount)) {
                    clear_screen();
                    print_error("Invalid amount entered");
                    Return(u);
                    return;
                }
            } else {
                die();
            }
            if (balance < amount) {
                print_error("Insufficient balance");
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
            clear_screen();
            print_error("Invalid option selected");
            goto retry;
    }

    dbRecordTransaction(accnt_id, transaction_type, amount);
    SUCCESS_MSG("Transaction completed successfully");
    success(u);
}

