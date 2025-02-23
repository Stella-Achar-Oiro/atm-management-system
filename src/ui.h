#ifndef _UI_H
#define _UI_H

#include <stdio.h>

// ANSI Color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

// UI Elements
#define HEADER_LINE   "══════════════════════════════════════════════════════════════\n"
#define DIVIDER_LINE  "──────────────────────────────────────────────────────────────\n"

// Message templates
#define SUCCESS_MSG(msg) printf(COLOR_GREEN "✔ %s" COLOR_RESET "\n", msg)
#define ERROR_MSG(msg)   printf(COLOR_RED "✖ %s" COLOR_RESET "\n", msg)
#define INFO_MSG(msg)    printf(COLOR_CYAN "ℹ %s" COLOR_RESET "\n", msg)
#define WARN_MSG(msg)    printf(COLOR_YELLOW "⚠ %s" COLOR_RESET "\n", msg)

// UI Functions
static inline void clear_screen(void) {
    printf("\033[2J\033[H");
}

static inline void print_header(const char *title) {
    printf(COLOR_CYAN "\n%s" COLOR_RESET, HEADER_LINE);
    printf(COLOR_BOLD "  %s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "%s" COLOR_RESET, HEADER_LINE);
}

static inline void print_menu_option(int number, const char *option) {
    printf(COLOR_YELLOW "  [%d]" COLOR_RESET " %s\n", number, option);
}

static inline void print_input_prompt(const char *prompt) {
    printf(COLOR_GREEN ">>> " COLOR_RESET "%s: ", prompt);
}

static inline void print_currency(double amount) {
    printf(COLOR_YELLOW "$%.2f" COLOR_RESET, amount);
}

static inline void print_account_info(const char *label, const char *value) {
    printf("  %-20s: %s\n", label, value);
}

static inline void print_account_number(int number) {
    printf(COLOR_CYAN "  Account #: %d\n" COLOR_RESET, number);
}

#endif // _UI_H