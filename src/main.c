#include "header.h"
#include <sys/stat.h>
#include <sys/types.h>

// Function to ensure data directory exists
static void ensure_data_directory() {
    struct stat st = {0};
    if (stat("./data", &st) == -1) {
        #ifdef _WIN32
            mkdir("./data");
        #else
            mkdir("./data", 0700);
        #endif
    }
}

int main() {
    User u;

    // Ensure data directory exists
    ensure_data_directory();

    // Initialize database
    if (!sqliteInit(DBPATH)) {
        printf("\n\tFailed to initialize database. Exiting...\n");
        return EXIT_FAILURE;
    }

    // Start the ATM system
    initMenu(&u);
    mainMenu(u);

    return EXIT_SUCCESS;
}