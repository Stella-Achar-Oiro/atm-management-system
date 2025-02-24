#include "header.h"
#include "ui.h"
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
        print_error("Failed to initialize database. Exiting...");
        return EXIT_FAILURE;
    }

    // Clear screen and start the ATM system
    clear_screen();
    initMenu(&u);
    mainMenu(u);

    return EXIT_SUCCESS;
}