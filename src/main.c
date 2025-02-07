#include "header.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

pid_t notification_pid = -1;

void cleanup() {
    if (notification_pid > 0) {
        kill(notification_pid, SIGINT);
    }
}

void printUsage() {
    printf("\nATM Management System - Usage:\n");
    printf("./atm [OPTION]\n\n");
    printf("Options:\n");
    printf("  --migrate            Migrate data to SQLite database\n");
    printf("  --encrypt-passwords  Encrypt passwords in users.txt\n");
    printf("  --help              Display this help message\n\n");
}

void mainMenu(struct User u)
{
    int option;
    while (1) {  // Keep showing menu until user exits
        system("clear");
        printf("\n\n\t\t╔════════════════════════════╗");
        printf("\n\t\t║         ATM SYSTEM         ║");
        printf("\n\t\t╚════════════════════════════╝\n");
        printf("\n\t\t-->> Welcome, %s <<--\n", u.name);
        printf("\n\t\tChoose an option below:");
        printf("\n\t\t╔════════════════════════════╗");
        printf("\n\t\t║ 1. Create New Account      ║");
        printf("\n\t\t║ 2. Update Account Info     ║");
        printf("\n\t\t║ 3. Check Account Details   ║");
        printf("\n\t\t║ 4. List All Accounts       ║");
        printf("\n\t\t║ 5. Make Transaction        ║");
        printf("\n\t\t║ 6. Remove Account          ║");
        printf("\n\t\t║ 7. Transfer Ownership      ║");
        printf("\n\t\t║ 8. Exit                    ║");
        printf("\n\t\t╚════════════════════════════╝");
        printf("\n\n\t\tEnter your choice: ");
        
        if (scanf("%d", &option) != 1) {
            // Clear input buffer if invalid input
            while (getchar() != '\n');
            printf("\n\t\tPlease enter a valid number!\n");
            sleep(2);
            continue;
        }

        switch (option)
        {
        case 1:
            createNewAcc(u);
            break;
        case 2:
            updateAccount(u);
            break;
        case 3:
            checkAccountDetails(u);
            break;
        case 4:
            checkAllAccounts(u);
            break;
        case 5:
            makeTransaction(u);
            break;
        case 6:
            removeExistingAccount(u);
            break;
        case 7:
            transferOwnership(u);
            break;
        case 8:
            printf("\n\t\tThank you for using our ATM System!\n");
            sleep(2);
            exit(0);
        default:
            printf("\n\t\tInvalid operation! Please try again.\n");
            sleep(2);
        }
    }
}

void initMenu(struct User *u)
{
    int r = 0;
    int option;

    while (!r) {
        system("clear");
        printf("\n\n\t\t╔════════════════════════════╗");
        printf("\n\t\t║         ATM SYSTEM         ║");
        printf("\n\t\t╚════════════════════════════╝");
        printf("\n\n\t\tWelcome! Please choose an option:");
        printf("\n\t\t╔════════════════════════════╗");
        printf("\n\t\t║ 1. Login                   ║");
        printf("\n\t\t║ 2. Register                ║");
        printf("\n\t\t║ 3. Exit                    ║");
        printf("\n\t\t╚════════════════════════════╝");
        printf("\n\n\t\tEnter your choice: ");

        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n');
            printf("\n\t\tPlease enter a valid number!\n");
            sleep(2);
            continue;
        }

        switch (option)
        {
        case 1:
            loginMenu(u->name, u->password);
            if (strcmp(u->password, getPassword(*u)) == 0) {
                printf("\n\n\t\t✔ Login successful!");
                printf("\n\t\tWelcome back, %s!\n", u->name);
                sleep(2);
                r = 1;
            } else {
                printf("\n\t\t✖ Wrong password or username!\n");
                sleep(2);
            }
            break;
        case 2:
            registerMenu(u->name, u->password);
            r = 1;
            break;
        case 3:
            printf("\n\t\tThank you for using our ATM System!\n");
            sleep(2);
            exit(0);
        default:
            printf("\n\t\tPlease enter a valid option!\n");
            sleep(2);
        }
    }
}

int main(int argc, char *argv[]) {
    struct User u;
    pid_t pid;
    
    // Command line argument handling
    if (argc > 1) {
        printf("Processing command: %s\n", argv[1]);  // Debug output
        
        if (strcmp(argv[1], "--help") == 0) {
            printUsage();
            return 0;
        }
        else if (strcmp(argv[1], "--encrypt-passwords") == 0) {
            printf("Starting password encryption process...\n");
            int result = updatePasswordsInFile();
            if (result == 0) {
                printf("Password encryption completed successfully!\n");
            } else {
                printf("Error during password encryption! (Error code: %d)\n", result);
            }
            return result;
        }
        else if (strcmp(argv[1], "--migrate") == 0) {
            if (initDatabase() != 0) {
                printf("Error initializing database. Exiting...\n");
                return 1;
            }
            runMigration();
            return 0;
        }
        else {
            printf("Unknown option: %s\n", argv[1]);
            printUsage();
            return 1;
        }
    }
    
    // Normal ATM operation...
    atexit(cleanup);
    initMenu(&u);
    
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        startNotificationListener(u.name);
        exit(0);
    } else {
        notification_pid = pid;
        mainMenu(u);
    }
    
    return 0;
}