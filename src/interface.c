#include "header.h"
#include "ui.h"

// ============= Input/Output Functions =============

void input_hide() {
    struct termios flags;
    tcgetattr(fileno(stdin), &flags);
    flags.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), TCSANOW, &flags);
}

void input_show() {
    struct termios flags;
    tcgetattr(fileno(stdin), &flags);
    flags.c_lflag |= ECHO;
    tcsetattr(fileno(stdin), TCSANOW, &flags);
}

void input_string(User u, char *input) {
    char *nlpos;
    printf(">>> ");

    if (fgets(input, MAXINPUTLEN, stdin) != NULL) {
        nlpos = strchr(input, '\n');
        if (nlpos != NULL) {
            *nlpos = '\0';
        }
        trimlinechar(input);
        if (!isstring(input, MAXINPUTLEN)) {
            system("clear");
            printf("\n\t\tInvalid input. Please enter a valid Input.\n\n");
            Return(u);
        }
    } else {
        die();
    }
}

int input_number() {
    char input[MAXINPUTLEN];
    char *end;
    long n;

    printf(">>> ");

    while (1) {
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';

            errno = 0;
            n = strtol(input, &end, 10);
            if (*end != '\0' || input == end) {
                printf("\n\t\tEnter a number: ");
            } else if (((n == LONG_MAX || n == LONG_MIN) && errno == ERANGE) || 
                      (n < INT_MIN || n > INT_MAX)) {
                printf("\n\t\t[ERROR] Enter a valid number: ");
            } else {
                return (int)n;
            }
        } else {
            printf("\n\t\tError reading input. Please try again...");
        }
    }
}

// ============= Menu Functions ======================

void initMenu(User *u) {
    int option;
    int r = 0;
    
    clear_screen();
    print_header("Welcome to ATM System");
    
    print_menu_option(1, "Login");
    print_menu_option(2, "Register");
    print_menu_option(3, "Exit");
    printf("\n");

    while (!r) {
        print_input_prompt("Select option");
        option = input_number();
        
        switch (option) {
            case 1:
                loginMenu(u->name, u->password);
                const char* stored_pass = dbRetrievePassword(*u);
                
                if (strcmp(stored_pass, "no user found") == 0) {
                    ERROR_MSG("User not found");
                    free((void*)stored_pass);
                    exit(EXIT_FAILURE);
                }

                // Handle both hashed and plain text passwords
                if (is_hashed_password(stored_pass)) {
                    // For hashed passwords
                    if (verify_password(u->password, stored_pass)) {
                        SUCCESS_MSG("Login successful!");
                        free((void*)stored_pass);
                        r = 1;
                    } else {
                        ERROR_MSG("Invalid password");
                        free((void*)stored_pass);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    // For plain text passwords (legacy support)
                    if (strcmp(u->password, stored_pass) == 0) {
                        SUCCESS_MSG("Login successful!");
                        
                        // Upgrade to hashed password
                        char* salt = generate_salt();
                        char* hashed = hash_password(u->password, salt);
                        
                        // Update the password in database
                        sqlite3 *db = sqliteHandler(DBPATH);
                        sqlite3_stmt *stmt = NULL;
                        const char *sql = "UPDATE USERS SET password = ? WHERE username = ?;";
                        
                        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
                            sqlite3_bind_text(stmt, 1, hashed, -1, SQLITE_STATIC);
                            sqlite3_bind_text(stmt, 2, u->name, -1, SQLITE_STATIC);
                            sqlite3_step(stmt);
                            sqlite3_finalize(stmt);
                        }
                        sqlite3_close(db);
                        
                        free(salt);
                        r = 1;
                    } else {
                        ERROR_MSG("Invalid password");
                        free((void*)stored_pass);
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case 2:
                registerMenu(*u);
                r = 1;
                break;
            case 3:
                clear_screen();
                INFO_MSG("Thank you for using ATM System");
                exit(EXIT_SUCCESS);
            default:
                ERROR_MSG("Invalid option selected");
                break;
        }
    }
}

void mainMenu(User u) {
    int choice;

    while (1) {
        clear_screen();
        print_header("Main Menu");
        printf("\nWelcome, %s%s%s!\n\n", COLOR_GREEN, u.name, COLOR_RESET);

        print_menu_option(1, "Create new account");
        print_menu_option(2, "Update account information");
        print_menu_option(3, "Check account details");
        print_menu_option(4, "List all accounts");
        print_menu_option(5, "Make transaction");
        print_menu_option(6, "Remove account");
        print_menu_option(7, "Transfer ownership");
        print_menu_option(8, "Exit");
        printf("\n");

        print_input_prompt("Select option");
        choice = input_number();

        switch (choice) {
            case 1: accountCreate(u); break;
            case 2: accountUpdateInfo(u); break;
            case 3: accountDetails(u); break;
            case 4: accountList(u); break;
            case 5: accountMakeTransaction(u); break;
            case 6: accountDelete(u); break;
            case 7: accountTransfer(u); break;
            case 8: 
                clear_screen();
                SUCCESS_MSG("Thank you for using our services!");
                exit(EXIT_SUCCESS);
            default:
                ERROR_MSG("Invalid option selected");
                break;
        }
    }
}

void registerMenu(User u) {
    char confirmPwd[MAXINPUTLEN];

    while (1) {
        printf("\n\t\tEnter Your Name >>>_");
        if (fgets(u.name, sizeof(u.name), stdin) != NULL) {
            trimlinechar(u.name);

            if (!isstring(u.name, MAXINPUTLEN)) {
                system("clear");
                printf("\n\t\t\tName too long. Please input a shorter name.\n\n");
                continue;
            }
            if (!isalphabet(u.name)) {
                system("clear");
                printf("\n\t\tInvalid input. Please enter a valid name\n\n");
                continue;
            }
        } else {
            die();
        }

        if (dbUsernameExists(u.name)) {
            system("clear");
            printf("\n\t\tUsername exists. Please choose a different name.\n");
            continue;
        }
        break;
    }

    while (1) {
        input_hide();

        printf("\n\t\tNew Password >>>_");
        if (fgets(u.password, sizeof(u.password), stdin) != NULL) {
            trimlinechar(u.password);
            if (!isstring(u.password, MAXINPUTLEN)) {
                printf("\n\t\t\tInvalid input. Please enter a valid password.\n\n");
                continue;
            }
        } else {
            die();
        }

        printf("\n\t\tConfirm Password >>>_");
        if (fgets(confirmPwd, sizeof(confirmPwd), stdin) != NULL) {
            trimlinechar(confirmPwd);
            if (!isstring(confirmPwd, MAXINPUTLEN)) {
                printf("\n\t\t\tInvalid input. Please enter a valid password.\n\n");
                continue;
            }
        } else {
            die();
        }

        input_show();

        if (strcmp(u.password, confirmPwd) == 0) {
            break;
        } else {
            printf("\n\t\tPasswords do not match!!!\n");
            exit(0);
        }
    }

    dbUserRegister(u);

    printf("\n\t\tUser registered successfully!!!\n\n");
    mainMenu(u);
}

void loginMenu(char a[50], char pass[50]) {
    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login >>>_");
    if (fgets(a, 50, stdin) != NULL) {
        trimlinechar(a);

        if (!isstring(a, 50)) {
            system("clear");
            printf("\n\t\t\tInvalid input. Please enter a valid Name.\n\n");
            exit(0);
        }
        if (!isalphabet(a)) {
            system("clear");
            printf("\n\tInvalid input. Please enter a valid name\n\n");
            exit(0);
        }
    } else {
        die();
    }

    input_hide();
    printf("\n\n\n\n\n\t\t\t\tUser Password >>>_");
    if (fgets(pass, 50, stdin) != NULL) {
        trimlinechar(pass);

        if (!isstring(pass, 50)) {
            system("clear");
            printf("\n\t\t\tInvalid input. Please enter a valid Input.\n\n");
            exit(0);
        }
    } else {
        die();
    }
    input_show();
}