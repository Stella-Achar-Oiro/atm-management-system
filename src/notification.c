#include "header.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define FIFO_PATH "/tmp/atm_notifications"
#define MAX_MSG_SIZE 256

// Structure for transfer notifications
struct TransferNotification {
    char sender[50];
    char receiver[50];
    int accountNumber;
    double balance;
    char accountType[10];
};

// Global flag for notification listener
volatile sig_atomic_t keep_running = 1;

// Signal handler for graceful shutdown
void handle_signal(int sig __attribute__((unused))) {
    keep_running = 0;
}

// Function to start notification listener
void startNotificationListener(const char* username) {
    int fd;
    struct TransferNotification notification;
    signal(SIGINT, handle_signal);

    // Create FIFO if it doesn't exist
    mkfifo(FIFO_PATH, 0666);

    printf("\nStarting notification listener for user: %s\n", username);
    
    while (keep_running) {
        fd = open(FIFO_PATH, O_RDONLY);
        if (fd == -1) {
            perror("Error opening FIFO");
            continue;
        }

        // Read notifications
        ssize_t bytes_read = read(fd, &notification, sizeof(notification));
        close(fd);

        if (bytes_read > 0 && strcmp(notification.receiver, username) == 0) {
            // Clear line and print notification
            printf("\r\033[K"); // Clear current line
            printf("\n🔔 New Account Received!\n");
            printf("From: %s\n", notification.sender);
            printf("Account Number: %d\n", notification.accountNumber);
            printf("Balance: $%.2f\n", notification.balance);
            printf("Account Type: %s\n", notification.accountType);
            printf("\nPress Enter to continue...\n");
        }
    }
}

// Function to send notification
void sendTransferNotification(const char* sender, const char* receiver, 
                            int accountNum, double balance, const char* accountType) {
    int fd;
    struct TransferNotification notification;

    // Prepare notification
    strncpy(notification.sender, sender, sizeof(notification.sender) - 1);
    strncpy(notification.receiver, receiver, sizeof(notification.receiver) - 1);
    notification.accountNumber = accountNum;
    notification.balance = balance;
    strncpy(notification.accountType, accountType, sizeof(notification.accountType) - 1);

    // Send notification through FIFO
    fd = open(FIFO_PATH, O_WRONLY);
    if (fd != -1) {
        write(fd, &notification, sizeof(notification));
        close(fd);
    }
}