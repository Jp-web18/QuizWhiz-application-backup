#ifndef CONFIG_H
#define CONFIG_H

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif


/* STANDARD HEADEARS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>


/* DEVELOPER HEADEARS */
#include "make_quiz.h"
#include "quiz.h"
#include "students_data.h"
#include "file_handling.h"

/* MAKE_QUIZ.C & STUDENT_DATA.C MACROS */
#define MAX_LOGIN_ATTEMPTS 3
#define PIN_FILE "pin.bin"
#define MAX_PIN_LENGTH 100

/* MAKE_QUIZ.C & STUDENT_DATA.C PIN EBCRYPTION */
void xor_encrypt_decrypt(char *data, size_t length, char key);  // ✅ Updated declaration


// ANSI escape codes for common text colors
#define COLOR_RESET   "\033[0m"     // Reset all attributes
#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_LIGHT_PURPLE "\033[95m"

// ANSI escape codes for bright/bold text colors
#define BOLD_BLACK   "\033[1;30m"
#define BOLD_RED     "\033[1;31m"
#define BOLD_GREEN   "\033[1;32m"
#define BOLD_YELLOW  "\033[1;33m"
#define BOLD_BLUE    "\033[1;34m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_CYAN    "\033[1;36m"
#define BOLD_WHITE   "\033[1;37m"

// ANSI escape codes for background colors
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

// ANSI escape codes for bright/bold background colors (less commonly supported)
#define BGB_BLACK   "\033[1;40m"
#define BGB_RED     "\033[1;41m"
#define BGB_GREEN   "\033[1;42m"
#define BGB_YELLOW  "\033[1;43m"
#define BGB_BLUE    "\033[1;44m"
#define BGB_MAGENTA "\033[1;45m"
#define BGB_CYAN    "\033[1;46m"
#define BGB_WHITE   "\033[1;47m"

// Example of combining text color and background color
#define COLOR_RED_ON_YELLOW COLOR_RED BG_YELLOW

// Example of bold text with a background color
#define BOLD_BLUE_ON_CYAN BOLD_BLUE BG_CYAN


#endif
