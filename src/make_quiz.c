#include "config.h"

// void xor_encrypt_decrypt(char *data, size_t length, char key) {
//     for (size_t i = 0; i < length; i++) {
//         data[i] ^= key;
//     }
// }

int login_make_quiz() {
    int attempts = 0;
    char entered_pin[MAX_PIN_LENGTH], stored_pin_encrypted[MAX_PIN_LENGTH], stored_pin[MAX_PIN_LENGTH];
    char encryption_key = 'K';
    FILE *pin_file = fopen(PIN_FILE, "rb");

    if (pin_file) {
        size_t bytes_read = fread(stored_pin_encrypted, 1, MAX_PIN_LENGTH - 1, pin_file);
        fclose(pin_file);

        if (bytes_read > 0) {
            stored_pin_encrypted[bytes_read] = '\0';
            strcpy(stored_pin, stored_pin_encrypted);
            xor_encrypt_decrypt(stored_pin, bytes_read, encryption_key);
        } else {
            printf("%sPIN file is corrupted.%s\n", COLOR_RED, COLOR_RESET);
            return 0;
        }
    } else {
        // Create default PIN if file doesn't exist
        char default_pin[] = "1234";
        xor_encrypt_decrypt(default_pin, strlen(default_pin), encryption_key);
        FILE *new_file = fopen(PIN_FILE, "wb");
        if (!new_file || fwrite(default_pin, 1, strlen(default_pin), new_file) < strlen(default_pin)) {
            perror("Failed to create default PIN file");
            return 0;
        }
        fclose(new_file);
#ifndef _WIN32
        set_file_permissions(PIN_FILE, 0600);
#endif
        strcpy(stored_pin, "1234");
    }

    while (attempts < MAX_LOGIN_ATTEMPTS) {
        system(CLEAR);
        printf("%sMake a quiz%s\n\n%s", COLOR_YELLOW, COLOR_RESET, COLOR_BLUE);
        printf("Enter PIN code: %s", COLOR_CYAN);
        if (fgets(entered_pin, MAX_PIN_LENGTH, stdin)) {
            printf("%s", COLOR_RESET);
            entered_pin[strcspn(entered_pin, "\n")] = '\0';

            if (strcmp(entered_pin, stored_pin) == 0) {
                printf("%sLogin successful.%s\n", COLOR_GREEN, COLOR_RESET);
                sleep(1);
                return 1;
            } else {
                printf("Incorrect PIN. Attempts remaining: %d\n", MAX_LOGIN_ATTEMPTS - ++attempts);
                sleep(1);
            }
        } else {
            printf("%sInvalid input.%s\n", COLOR_RED, COLOR_RESET);
            sleep(1);
            attempts++;
        }
    }

    printf("%sToo many failed login attempts. Returning to main menu.%s\n", COLOR_YELLOW, COLOR_RESET);
    sleep(2);
    return 0;
}

void change_pin() {
    char new_pin[MAX_PIN_LENGTH];
    char encryption_key = 'K';

    printf("Enter new PIN: %s", COLOR_CYAN);
    if (fgets(new_pin, MAX_PIN_LENGTH, stdin)) {
        printf("%s", COLOR_RESET);
        new_pin[strcspn(new_pin, "\n")] = '\0';
        xor_encrypt_decrypt(new_pin, strlen(new_pin), encryption_key);

        FILE *pin_file = fopen(PIN_FILE, "wb");
        if (pin_file && fwrite(new_pin, 1, strlen(new_pin), pin_file)) {
            fclose(pin_file);
#ifndef _WIN32
            set_file_permissions(PIN_FILE, 0600);
            printf("%sPIN changed and encrypted.%s\n", COLOR_GREEN, COLOR_RESET);
#else
            printf("%sPIN changed successfully (basic XOR).%s\n", COLOR_GREEN, COLOR_RESET);
#endif
        } else {
            printf("%sFailed to write PIN file.%s\n", COLOR_RED, COLOR_RESET);
        }
    } else {
        printf("%sInvalid input.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep(1);
}

void create_new_quiz() {
    char filename[100], correct_answers[100], input[100];
    int num_items, duration;

    printf("Enter quiz file name: ");
    if (!fgets(filename, sizeof(filename), stdin)) return;
    filename[strcspn(filename, "\n")] = '\0';

    if (access("quizzes", F_OK) == -1) {
#ifdef _WIN32
        if (mkdir("quizzes") != 0)
#else
        if (mkdir("quizzes", 0777) != 0)
#endif
        {
            perror("Failed to create quizzes directory");
            return;
        }
    }

    char full_filename[128];
    snprintf(full_filename, sizeof(full_filename), "quizzes/%s.quiz", filename);

    FILE *quiz_file = fopen(full_filename, "r");
    if (quiz_file) {
        fclose(quiz_file);
        printf("Quiz file exists. Overwrite? (y/n): ");
        char confirm[3];
        if (fgets(confirm, sizeof(confirm), stdin) && (confirm[0] != 'y' && confirm[0] != 'Y')) {
            printf("Quiz not saved.\n");
            return;
        }
    }

    printf("Enter time duration (minutes): ");
    if (!fgets(input, sizeof(input), stdin)) return;
    duration = atoi(input);

    printf("Enter number of items: ");
    if (!fgets(input, sizeof(input), stdin)) return;
    num_items = atoi(input);

    printf("Enter correct answers (no spaces, %d characters): ", num_items);
    if (!fgets(correct_answers, sizeof(correct_answers), stdin)) return;
    correct_answers[strcspn(correct_answers, "\n")] = '\0';

    if ((int)strlen(correct_answers) != num_items) {
        printf("Mismatch: number of answers must equal number of items.\n");
        return;
    }

    FILE *fp = fopen(full_filename, "w");
    if (!fp) {
        perror("Failed to create quiz file");
        return;
    }

    fprintf(fp, "%d\n%d\n%s\n", duration, num_items, correct_answers);
    fclose(fp);

    printf("Save quiz?\n[1] Yes\n[2] No\nChoice: ");
    char confirm_str[3];
    if (fgets(confirm_str, sizeof(confirm_str), stdin) && atoi(confirm_str) == 1) {
        printf("Quiz saved successfully.\n");
    } else {
        remove(full_filename);
        printf("Quiz discarded.\n");
    }
    sleep(1);
}

void edit_existing_quiz() {
    DIR *dir;
    struct dirent *entry;
    char filenames[100][128];
    int file_count = 0, choice;

    // Open quizzes directory
    if ((dir = opendir("quizzes")) == NULL) {
        perror("Failed to open quizzes directory");
        sleep(1);
        return;
    }

    printf("Available quizzes:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".quiz")) {
            snprintf(filenames[file_count], sizeof(filenames[file_count]), "quizzes/%s", entry->d_name);
            printf("[%d] %s\n", file_count + 1, entry->d_name);
            file_count++;
        }
    }
    closedir(dir);

    if (file_count == 0) {
        printf("No quizzes available to edit or delete.\n");
        sleep(1);
        return;
    }

    printf("Enter the number of the quiz to edit or delete: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > file_count) {
        printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
        sleep(1);
        return;
    }
    getchar(); // Consume leftover newline

    char *selected_file = filenames[choice - 1];
    char input[100], correct_answers[100];
    int duration, num_items;

    printf("What would you like to do?\n");
    printf("[1] Edit quiz\n");
    printf("[2] Delete quiz\n");
    printf("Enter your choice: ");
    if (fgets(input, sizeof(input), stdin) && input[0] == '2') {
        if (remove(selected_file) == 0) {
            printf("Quiz deleted successfully.\n");
        } else {
            perror("Failed to delete quiz");
        }
        sleep(1);
        return;
    }

    FILE *quiz_file = fopen(selected_file, "r");
    if (!quiz_file) {
        printf("Quiz file not found.\n");
        sleep(1);
        return;
    }

    // Read existing quiz details
    if (fscanf(quiz_file, "%d\n%d\n%99s", &duration, &num_items, correct_answers) != 3) {
        printf("%sFailed to read quiz file or file is corrupted.%s\n", COLOR_RED, COLOR_RESET);
        fclose(quiz_file);
        sleep(1);
        return;
    }
    fclose(quiz_file);

    printf("Current duration: %d minutes\n", duration);
    printf("Current number of items: %d\n", num_items);
    printf("Current correct answers: %s\n", correct_answers);

    printf("Enter new time duration (minutes) or press Enter to keep current: ");
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        duration = atoi(input);
    }

    printf("Enter new number of items or press Enter to keep current: ");
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        num_items = atoi(input);
    }

    printf("Enter new correct answers (no spaces, %d characters) or press Enter to keep current: ", num_items);
    if (fgets(input, sizeof(input), stdin) && input[0] != '\n') {
        input[strcspn(input, "\n")] = '\0';
        if ((int)strlen(input) != num_items) {
            printf("%sMismatch: number of answers must equal number of items.%s\n", COLOR_RED, COLOR_RESET);
            sleep(1);
            return;
        }
        strcpy(correct_answers, input);
    }

    // Save updated quiz
    quiz_file = fopen(selected_file, "w");
    if (!quiz_file) {
        perror("Failed to save quiz file\n");
        return;
    }

    fprintf(quiz_file, "%d\n%d\n%s\n", duration, num_items, correct_answers);
    fclose(quiz_file);

    printf("Quiz updated successfully.\n");
    sleep(1);
}

void make_quiz_menu() {
    if (!login_make_quiz()) return;

    int choice;
    char input[10];

    while (1) {
        system(CLEAR);
        printf("%sMake a quiz%s\n\n%s", COLOR_YELLOW, COLOR_RESET, COLOR_BLUE);
        printf("[1] Make another quiz\n[2] Edit existing quizzes\n[3] Change PIN\n[4] Back to main menu\n\n%s", COLOR_RESET);
        printf("Enter your choice: %s", COLOR_CYAN);

        if (fgets(input, sizeof(input), stdin)) {
            choice = atoi(input);
            printf("%s", COLOR_RESET);

            switch (choice) {
                case 1: create_new_quiz(); break;
                case 2: edit_existing_quiz(); break;
                case 3: change_pin(); break;
                case 4: return;
                default:
                    printf("%sInvalid choice. Try again.%s\n", COLOR_RED, COLOR_RESET);
                    sleep(1);
            }
        }
    }
}
