// validation.c
#include "header.h"
#include <ctype.h>
#include <time.h>

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int validateDate(int month, int day, int year) {
    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    
    // Check basic ranges
    if (month < 1 || month > 12) return INVALID_DATE;
    if (day < 1 || day > 31) return INVALID_DATE;
    if (year < (current->tm_year + 1900)) return INVALID_DATE;
    
    // Check specific month lengths
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0) daysInMonth[1] = 29; // Leap year
    
    if (day > daysInMonth[month - 1]) return INVALID_DATE;
    
    return VALID;
}

struct ValidationResult validateDateWithRetry(int* month, int* day, int* year) {
    int retries = 0;
    struct ValidationResult result = {0, NULL};
    
    while (retries < MAX_RETRIES) {
        printf("\nEnter date (mm/dd/yyyy): ");
        if (scanf("%d/%d/%d", month, day, year) != 3) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid date format! Use mm/dd/yyyy (%d attempts remaining)\n", 
                   MAX_RETRIES - retries - 1);
            retries++;
            continue;
        }

        if (validateDate(*month, *day, *year) == VALID) {
            result.isValid = 1;
            return result;
        }

        printf("\n\t\t✖ Invalid date! (%d attempts remaining)\n",
               MAX_RETRIES - retries - 1);
        retries++;
    }

    result.message = "\n\t\t✖ Maximum attempts exceeded for date input.";
    return result;
}

int validateAccountNumber(int accountNum) {
    if (accountNum < MIN_ACCOUNT_NUMBER || accountNum > MAX_ACCOUNT_NUMBER) {
        return INVALID_ACCOUNT_NUMBER;
    }
    return VALID;
}

struct ValidationResult validateAccountNumberWithRetry(int* accountNum) {
    int retries = 0;
    struct ValidationResult result = {0, NULL};
    
    while (retries < MAX_RETRIES) {
        printf("\nEnter account number (5 digits): ");
        if (scanf("%d", accountNum) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Invalid format! (%d attempts remaining)\n",
                   MAX_RETRIES - retries - 1);
            retries++;
            continue;
        }

        if (validateAccountNumber(*accountNum) == VALID) {
            result.isValid = 1;
            return result;
        }

        printf("\n\t\t✖ Account number must be 5 digits! (%d attempts remaining)\n",
               MAX_RETRIES - retries - 1);
        retries++;
    }

    result.message = "\n\t\t✖ Maximum attempts exceeded for account number.";
    return result;
}

int validatePhone(const char* phone) {
    if (strlen(phone) < 7 || strlen(phone) > MAX_PHONE_LENGTH) {
        return INVALID_PHONE;
    }
    
    for (size_t i = 0; phone[i] != '\0'; i++) {
        if (!isdigit(phone[i]) && phone[i] != '-' && phone[i] != '+') {
            return INVALID_PHONE;
        }
    }
    return VALID;
}

int validateAmount(double amount) {
    if (amount < MIN_AMOUNT || amount > MAX_AMOUNT) {
        return INVALID_AMOUNT;
    }
    return VALID;
}

int validateCountry(const char* country) {
    if (strlen(country) < 2 || strlen(country) > MAX_NAME_LENGTH) {
        return INVALID_COUNTRY;
    }
    
    for (size_t i = 0; country[i] != '\0'; i++) {
        if (!isalpha(country[i]) && country[i] != ' ' && country[i] != '-') {
            return INVALID_COUNTRY;
        }
    }
    return VALID;
}

int validateAccountType(const char* accountType) {
    const char* validTypes[] = {"saving", "current", "fixed01", "fixed02", "fixed03"};
    int numTypes = sizeof(validTypes) / sizeof(validTypes[0]);
    
    for (int i = 0; i < numTypes; i++) {
        if (strcmp(accountType, validTypes[i]) == 0) {
            return VALID;
        }
    }
    return INVALID_ACCOUNT_TYPE;
}

int getValidatedInput(const char* prompt, const char* format, void* var, int (*validator)(void*)) {
    int retries = 0;
    
    while (retries < MAX_RETRIES) {
        printf("%s", prompt);
        if (scanf(format, var) != 1) {
            clearInputBuffer();
            printf("Invalid input format. Please try again (%d attempts remaining).\n", 
                   MAX_RETRIES - retries - 1);
            retries++;
            continue;
        }
        
        clearInputBuffer();
        if (validator && validator(var) != VALID) {
            printf("Invalid input. Please try again (%d attempts remaining).\n", 
                   MAX_RETRIES - retries - 1);
            retries++;
            continue;
        }
        
        return VALID;
    }
    
    return -1; // Max retries exceeded
}

struct ValidationResult getDateInput(int* month, int* day, int* year) {
    struct ValidationResult result = {0, NULL};
    char dateStr[20];
    int retries = 0;

    while (retries < MAX_RETRIES) {
        printf("\nEnter date (mm/dd/yyyy): ");
        if (scanf("%s", dateStr) != 1) {
            clearInputBuffer();
            printf("\n\t\t✖ Error reading input. Please try again (%d attempts remaining)\n", 
                   MAX_RETRIES - retries - 1);
            retries++;
            continue;
        }

        // Check format using sscanf
        if (sscanf(dateStr, "%d/%d/%d", month, day, year) != 3) {
            printf("\n\t\t✖ Invalid date format! Please use mm/dd/yyyy (%d attempts remaining)\n", 
                   MAX_RETRIES - retries - 1);
            clearInputBuffer();
            retries++;
            continue;
        }

        // Validate the date values
        if (validateDate(*month, *day, *year) == VALID) {
            result.isValid = 1;
            return result;
        }

        printf("\n\t\t✖ Invalid date! Please enter a valid date (%d attempts remaining)\n", 
               MAX_RETRIES - retries - 1);
        retries++;
    }

    result.message = "\n\t\t✖ Maximum attempts exceeded. Returning to main menu...";
    return result;
}