#ifndef _UI_H
#define _UI_H

#include <stdio.h>
#include <stdlib.h>

// ANSI Color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

// Text styles
#define STYLE_BOLD      "\033[1m"
#define STYLE_UNDERLINE "\033[4m"

// UI Elements
#define HEADER_LINE     "══════════════════════════════════════════════════════════════\n"
#define DIVIDER_LINE    "──────────────────────────────────────────────────────────────\n"
#define SUB_DIVIDER     "··························································\n"

// Symbols
#define SYMBOL_SUCCESS "✔"
#define SYMBOL_ERROR   "✖"
#define SYMBOL_INFO    "ℹ"
#define SYMBOL_WARN    "⚠"
#define SYMBOL_ARROW   "→"
#define SYMBOL_BULLET  "•"

// Message templates
#define SUCCESS_MSG(msg) printf(COLOR_GREEN " %s %s" COLOR_RESET "\n", SYMBOL_SUCCESS, msg)
#define ERROR_MSG(msg)   printf(COLOR_RED " %s %s" COLOR_RESET "\n", SYMBOL_ERROR, msg)
#define INFO_MSG(msg)    printf(COLOR_CYAN " %s %s" COLOR_RESET "\n", SYMBOL_INFO, msg)
#define WARN_MSG(msg)    printf(COLOR_YELLOW " %s %s" COLOR_RESET "\n", SYMBOL_WARN, msg)

// Screen clearing
static inline int clear_screen(void) {
    int result = system("clear");
    if (result == -1) {
        fprintf(stderr, "Error clearing screen\n");
    }
    return result;
}

// Header and section functions
static inline void print_header(const char *title) {
    printf(COLOR_CYAN "\n%s" COLOR_RESET, HEADER_LINE);
    printf(COLOR_BOLD "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "%s" COLOR_RESET, HEADER_LINE);
}

static inline void print_section_header(const char *section) {
    printf(COLOR_CYAN "\n  %s %s %s\n" COLOR_RESET, SYMBOL_ARROW, section, SYMBOL_ARROW);
    printf(COLOR_CYAN "  %s" COLOR_RESET, SUB_DIVIDER);
}

// Menu and input functions
static inline void print_menu_option(int number, const char *option) {
    printf(COLOR_YELLOW "  [%d]" COLOR_RESET " %s\n", number, option);
}

static inline void print_input_prompt(const char *prompt) {
    printf(COLOR_GREEN ">>> " COLOR_RESET "%s: ", prompt);
}

// Financial display functions
static inline void print_currency(double amount) {
    if (amount >= 0) {
        printf(COLOR_GREEN "$%.2f" COLOR_RESET, amount);
    } else {
        printf(COLOR_RED "-$%.2f" COLOR_RESET, -amount);
    }
}

static inline void print_balance(double amount) {
    printf("  Balance: ");
    print_currency(amount);
    printf("\n");
}

static inline void print_transaction(const char* type, double amount) {
    printf("  %s: ", type);
    print_currency(amount);
    printf("\n");
}

// Account information display
static inline void print_account_info(const char *label, const char *value) {
    printf(COLOR_CYAN "  %-20s:" COLOR_RESET " %s\n", label, value);
}

static inline void print_account_number(int number) {
    printf(COLOR_YELLOW "  Account #: %d\n" COLOR_RESET, number);
}

static inline void print_account_detail_numeric(const char *label, int value) {
    printf(COLOR_CYAN "  %-20s:" COLOR_RESET " %d\n", label, value);
}

static inline void print_account_detail_double(const char *label, double value) {
    printf(COLOR_CYAN "  %-20s:" COLOR_RESET " ", label);
    print_currency(value);
    printf("\n");
}

static inline void print_date(const char *label, int month, int day, int year) {
    printf(COLOR_CYAN "  %-20s:" COLOR_RESET " %02d/%02d/%04d\n", 
           label, month, day, year);
}

// Processing and status messages
static inline void print_processing(const char *operation) {
    printf(COLOR_CYAN "  %s Processing %s..." COLOR_RESET "\n", SYMBOL_INFO, operation);
}

static inline void print_confirmation(const char *message) {
    printf(COLOR_GREEN "  %s %s" COLOR_RESET "\n", SYMBOL_SUCCESS, message);
}

static inline void print_error(const char *message) {
    printf(COLOR_RED "  %s %s" COLOR_RESET "\n", SYMBOL_ERROR, message);
}

#endif // _UI_H