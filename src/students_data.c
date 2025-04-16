#include "config.h"


int login() {
    char input_pin[MAX_PIN_LENGTH];
    char stored_pin_encrypted[MAX_PIN_LENGTH];
    char stored_pin[MAX_PIN_LENGTH];
    // char encryption_key = 0xAB;
    char encryption_key = 'K';
    int attempts = 0;

    FILE *pin_file = fopen(PIN_FILE, "rb");
    if (!pin_file) {
        printf(COLOR_RED "PIN file not found. Please set your PIN first.\n" COLOR_RESET);
        return 0;
    }

    size_t bytes_read = fread(stored_pin_encrypted, 1, MAX_PIN_LENGTH - 1, pin_file);
    fclose(pin_file);

    if (bytes_read > 0) {
        memcpy(stored_pin, stored_pin_encrypted, bytes_read);
        xor_encrypt_decrypt(stored_pin, bytes_read, encryption_key);
        stored_pin[bytes_read] = '\0';

        // Trim newline if it was stored
        size_t len = strlen(stored_pin);
        if (len > 0 && stored_pin[len - 1] == '\n') {
            stored_pin[len - 1] = '\0';
        }
    } else {
        printf(COLOR_RED "Failed to read PIN file.\n" COLOR_RESET);
        return 0;
    }

    printf("%student Data%s\n\n", COLOR_YELLOW, COLOR_RESET);
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        printf("Enter your PIN: ");
        if (fgets(input_pin, MAX_PIN_LENGTH, stdin) != NULL) {
            size_t len = strlen(input_pin);
            if (len > 0 && input_pin[len - 1] == '\n') {
                input_pin[len - 1] = '\0';
            }

            if (strcmp(input_pin, stored_pin) == 0) {
                printf(COLOR_GREEN "Login successful.\n" COLOR_RESET);
                return 1;
            } else {
                printf(COLOR_RED "Incorrect PIN. Try again.\n" COLOR_RESET);
                attempts++;
            }
        } else {
            printf(COLOR_RED "Failed to read input.\n" COLOR_RESET);
            attempts++;
        }
    }

    printf(COLOR_RED "Maximum login attempts exceeded.\n" COLOR_RESET);
    return 0;
}


void view_student_data() {
    system(CLEAR);

    int attempts = 0;
    char entered_pin[MAX_PIN_LENGTH];
    char stored_pin[MAX_PIN_LENGTH];
    char encryption_key = 'K';
    FILE *pin_file;

    pin_file = fopen(PIN_FILE, "rb");
    if (pin_file != NULL) {
        size_t bytes_read = fread(stored_pin, 1, MAX_PIN_LENGTH - 1, pin_file);
        if (bytes_read > 0) {
            xor_encrypt_decrypt(stored_pin, bytes_read, encryption_key);
            stored_pin[bytes_read] = '\0';
        } else {
            stored_pin[0] = '\0';
        }
        fclose(pin_file);
    } else {
        stored_pin[0] = '\0';
        printf("Warning: PIN file not found or cannot be opened.\n");
        printf("Using default login behavior (no PIN required).\n");
        sleep(2);
    }

    while (attempts < MAX_LOGIN_ATTEMPTS) {
        printf("%sStudent Data%s\n\n", COLOR_YELLOW, COLOR_RESET);
        printf("Enter PIN to view student data: ");
        if (fgets(entered_pin, MAX_PIN_LENGTH, stdin) != NULL) {
            size_t len = strlen(entered_pin);
            if (len > 0 && entered_pin[len - 1] == '\n') {
                entered_pin[len - 1] = '\0';
            }

            if (stored_pin[0] == '\0' || strcmp(entered_pin, stored_pin) == 0) {
                break;
            } else {
                attempts++;
                printf("%sIncorrect PIN. Attempts remaining: %d%s\n", COLOR_RED, MAX_LOGIN_ATTEMPTS - attempts, COLOR_RESET);
                if (attempts >= MAX_LOGIN_ATTEMPTS) {
                    printf("%sToo many failed login attempts. Returning to main menu.%s\n", COLOR_RED, COLOR_RESET);
                }
                sleep(2);
                system(CLEAR);
            }
        } else {
            attempts++;
            printf("Invalid input.\n");
            sleep(1);
            system(CLEAR);
        }
    }

    if (attempts >= MAX_LOGIN_ATTEMPTS && stored_pin[0] != '\0') {
        return;
    }

    system(CLEAR);
    DIR *d = opendir("records");
    struct dirent *dir;

    if (!d) {
        printf("No student records found.\n");
        sleep(2);
        return;
    }

    printf("%s%-20s%s %s%-20s%s %s%-10s%s %-10s %s%-15s%s\n", 
           COLOR_YELLOW, "Student Name", COLOR_RESET, 
           COLOR_MAGENTA, "Quiz Name", COLOR_RESET, 
           COLOR_BLUE, "Score", COLOR_RESET, 
           "Date", 
           COLOR_GREEN, "Section", COLOR_RESET);
    printf("-------------------- -------------------- ---------- ---------- ---------------\n");

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".rec")) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "records/%s", dir->d_name);
            FILE *fp = fopen(filepath, "r");

            if (!fp) continue;

            char line[100];
            char name[100] = "";
            char section[50] = "";
            char pc[50] = "";
            char score_str[20] = "";
            char file_date[11] = "";
            char quiz[50] = "";
            int score_val, total_items;

            if (fgets(line, sizeof(line), fp)) sscanf(line, "Name: %99[^\n]", name);
            if (fgets(line, sizeof(line), fp)) sscanf(line, "Section: %49[^\n]", section);
            if (fgets(line, sizeof(line), fp)) sscanf(line, "PC: %49[^\n]", pc);
            if (fgets(line, sizeof(line), fp)) sscanf(line, "Score: %d/%d %10[^\n]", &score_val, &total_items, file_date);

            sscanf(dir->d_name, "%[^_]", quiz);
            snprintf(score_str, sizeof(score_str), "%d/%d", score_val, total_items);

            printf("%-20s %-20s %-10s %-10s %-15s\n", name, quiz, score_str, file_date, section);
            // printf("%s%-20s%s %-20s %s%-10s%s %-10s %-15s\n", COLOR_YELLOW, name, COLOR_RESET, quiz, COLOR_BLUE, score_str, COLOR_RESET, file_date, section);
            fclose(fp);
        }
    }

    closedir(d);
    printf("\nPress Enter to go back to the main menu...\n");
    getchar();
}
