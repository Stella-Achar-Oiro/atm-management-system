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

struct ValidationResult getDateInput(int* month, int* day, int* year) {
    struct ValidationResult result = {0, NULL};
    char dateStr[20];

    printf("\nEnter date (mm/dd/yyyy): ");
    if (scanf("%s", dateStr) != 1) {
        clearInputBuffer();
        printf("\n\t\t✖ Invalid input format!\n");
        printf("\n\t\tPress any key to return to main menu...");
        getchar();
        result.message = "format_error";
        return result;
    }

    // Check format using sscanf
    if (sscanf(dateStr, "%d/%d/%d", month, day, year) != 3) {
        printf("\n\t\t✖ Invalid date format! Please use mm/dd/yyyy\n");
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "format_error";
        return result;
    }

    if (validateDate(*month, *day, *year) != VALID) {
        printf("\n\t\t✖ Invalid date! Please enter a valid date.\n");
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "invalid_date";
        return result;
    }

    result.isValid = 1;
    return result;
}

int validateAccountNumber(int accountNum) {
    if (accountNum < MIN_ACCOUNT_NUMBER || accountNum > MAX_ACCOUNT_NUMBER) {
        return INVALID_ACCOUNT_NUMBER;
    }
    return VALID;
}

struct ValidationResult getAccountNumberInput(int* accountNum) {
    struct ValidationResult result = {0, NULL};

    printf("\nEnter account number (9 digits): ");  // Updated prompt
    if (scanf("%d", accountNum) != 1) {
        clearInputBuffer();
        printf("\n\t\t✖ Invalid format! Account number must be numeric.\n");
        printf("\n\t\tPress any key to return to main menu...");
        getchar();
        result.message = "format_error";
        return result;
    }

    if (validateAccountNumber(*accountNum) != VALID) {
        printf("\n\t\t✖ Account number must be exactly 9 digits.\n");  // Updated message
        printf("\n\t\tValid range: %d to %d\n", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "invalid_number";
        return result;
    }

    result.isValid = 1;
    return result;
}

struct ValidationResult getPhoneInput(char* phone) {
    struct ValidationResult result = {0, NULL};

    printf("\nEnter phone number: ");
    if (scanf("%s", phone) != 1) {
        clearInputBuffer();
        printf("\n\t\t✖ Invalid input format!\n");
        printf("\n\t\tPress any key to return to main menu...");
        getchar();
        result.message = "format_error";
        return result;
    }

    if (validatePhone(phone) != VALID) {
        printf("\n\t\t✖ Invalid phone number! Use only digits, +, and -\n");
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "invalid_phone";
        return result;
    }

    result.isValid = 1;
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

struct ValidationResult getAmountInput(double* amount) {
    struct ValidationResult result = {0, NULL};

    printf("\nEnter amount: $");
    if (scanf("%lf", amount) != 1) {
        clearInputBuffer();
        printf("\n\t\t✖ Invalid amount format!\n");
        printf("\n\t\tPress any key to return to main menu...");
        getchar();
        result.message = "format_error";
        return result;
    }

    if (validateAmount(*amount) != VALID) {
        printf("\n\t\t✖ Invalid amount! Must be between $%.2f and $%.2f\n", 
               MIN_AMOUNT, MAX_AMOUNT);
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "invalid_amount";
        return result;
    }

    result.isValid = 1;
    return result;
}

int validateAmount(double amount) {
    if (amount < MIN_AMOUNT || amount > MAX_AMOUNT) {
        return INVALID_AMOUNT;
    }
    return VALID;
}

struct ValidationResult getCountryInput(char* country) {
    struct ValidationResult result = {0, NULL};

    printf("\nEnter country: ");
    if (scanf("%s", country) != 1) {
        clearInputBuffer();
        printf("\n\t\t✖ Invalid input format!\n");
        printf("\n\t\tPress any key to return to main menu...");
        getchar();
        result.message = "format_error";
        return result;
    }

    if (validateCountry(country) != VALID) {
        printf("\n\t\t✖ Invalid country name! Use only letters, spaces, and hyphens.\n");
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "invalid_country";
        return result;
    }

    result.isValid = 1;
    return result;
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

struct ValidationResult getAccountTypeInput(char* accountType) {
    struct ValidationResult result = {0, NULL};

    printf("\nChoose the type of account:\n");
    printf("\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n");
    printf("\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n");
    printf("\nEnter your choice: ");
    
    if (scanf("%s", accountType) != 1) {
        clearInputBuffer();
        printf("\n\t\t✖ Invalid input format!\n");
        printf("\n\t\tPress any key to return to main menu...");
        getchar();
        result.message = "format_error";
        return result;
    }

    if (validateAccountType(accountType) != VALID) {
        printf("\n\t\t✖ Invalid account type! Please choose from the list.\n");
        printf("\n\t\tPress any key to return to main menu...");
        clearInputBuffer();
        getchar();
        result.message = "invalid_type";
        return result;
    }

    result.isValid = 1;
    return result;
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