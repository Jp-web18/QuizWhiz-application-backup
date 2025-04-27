#include "config.h"


void clear_stdin_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}




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

    // printf("%student Data%s\n\n", COLOR_YELLOW, COLOR_RESET);
    printf("%s----------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
    printf("%s                     Students Data\n", COLOR_YELLOW);
    printf("%s----------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
    printf("\n");

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

        // Set default PIN code as in make_quiz
        char default_pin[] = "1234";
        char encryption_key = 'K';
        xor_encrypt_decrypt(default_pin, strlen(default_pin), encryption_key);

        FILE *new_file = fopen(PIN_FILE, "wb");
        if (new_file && fwrite(default_pin, 1, strlen(default_pin), new_file) == strlen(default_pin)) {
            printf("Default PIN file created successfully.\n");
        } else {
            printf("Failed to create default PIN file.\n");
        }
        if (new_file) fclose(new_file);

#ifndef _WIN32
        set_file_permissions(PIN_FILE, 0600);
#endif

        xor_encrypt_decrypt(default_pin, strlen(default_pin), encryption_key); // Decrypt for runtime use
        strcpy(stored_pin, default_pin);

        sleep(2);
    }

    while (attempts < MAX_LOGIN_ATTEMPTS) {
        // printf("%sStudent Data%s\n\n", COLOR_YELLOW, COLOR_RESET);
        printf("%s----------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
        printf("%s                     Students Data\n", COLOR_YELLOW);
        printf("%s----------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
        printf("\n");

        // printf("Enter PIN to view student data: ");
        printf("%sEnter PIN code: %s", COLOR_CYAN, COLOR_WHITE);
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

    struct {
        char filepath[256];
        time_t mod_time;
    } records[100];
    int record_count = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".rec")) {
            snprintf(records[record_count].filepath, sizeof(records[record_count].filepath), "records/%s", dir->d_name);

            struct stat file_stat;
            if (stat(records[record_count].filepath, &file_stat) == 0) {
                records[record_count].mod_time = file_stat.st_mtime;
                record_count++;
            }
        }
    }
    closedir(d);

    // Sort records by modification time (latest first)
    for (int i = 0; i < record_count - 1; i++) {
        for (int j = i + 1; j < record_count; j++) {
            if (records[i].mod_time < records[j].mod_time) {
                // Swap records
                char temp_filepath[256];
                time_t temp_mod_time;

                strcpy(temp_filepath, records[i].filepath);
                temp_mod_time = records[i].mod_time;

                strcpy(records[i].filepath, records[j].filepath);
                records[i].mod_time = records[j].mod_time;

                strcpy(records[j].filepath, temp_filepath);
                records[j].mod_time = temp_mod_time;
            }
        }
    }

    printf("%s-------------------------------------------------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
    printf("%sSTUDENT DATA\tSTUDENT DATA\tSTUDENT DATA\tSTUDENT DATA\tSTUDENT DATA\tSTUDENT DATA\t\n", COLOR_YELLOW);
    printf("%s-------------------------------------------------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
    printf("\n");

    printf("%sTotal Students Who Took the Quiz:\t%s%d%s\n\n", COLOR_CYAN, COLOR_WHITE, record_count, COLOR_RESET);

    printf("%s%-4s  %-30s  %-12s  %-18s  %-15s  %-8s%s\n",
        COLOR_YELLOW, "No.", "Student Name", "Section", "Quiz Name", "Date", "Score", COLOR_RESET);
    printf("----  ------------------------------  ------------  ------------------  ---------------  --------\n");
 
    for (int i = 0; i < record_count; i++) {
        FILE *fp = fopen(records[i].filepath, "r");
        if (!fp) continue;

        char line[100];
        char name[100] = "N/A"; // Default to "N/A" if missing
        char section[50] = "N/A"; // Default to "N/A" if missing
        char pc[50] = "N/A"; // Default to "N/A" if missing
        char score_str[20] = "0/0"; // Default to "0/0" if missing
        char file_date[20] = "N/A"; // Default to "N/A" if missing
        char quiz[50] = "N/A"; // Default to "N/A" if missing
        int score_val = 0, total_items = 0;

        // Parse each line of the record file
        if (fgets(line, sizeof(line), fp)) sscanf(line, "Name    : %99[^\n]", name);
        if (fgets(line, sizeof(line), fp)) sscanf(line, "Section : %49[^\n]", section);
        if (fgets(line, sizeof(line), fp)) sscanf(line, "PC      : %49[^\n]", pc);
        if (fgets(line, sizeof(line), fp)) sscanf(line, "Score   : %d/%d", &score_val, &total_items);
        if (fgets(line, sizeof(line), fp)) sscanf(line, "Date    : %19[^\n]", file_date);

        sscanf(records[i].filepath, "records/%[^_]", quiz);
        snprintf(score_str, sizeof(score_str), "%d/%d", score_val, total_items);

        // Display the parsed data
        printf("%-4d  %-30s  %-12s  %-18s  %-15s  %-8s\n", i + 1, name, section, quiz, file_date, score_str);
        fclose(fp);
    }

    char input_buffer[100];

    while (1) {
        printf("%s\nWould you like to view detailed information for a specific student? (y/n): %s", COLOR_CYAN, COLOR_WHITE);
        if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
            printf("%sInput error. Please try again.%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }
    
        // Remove trailing newline and clear buffer if needed
        if (strchr(input_buffer, '\n') == NULL)
            clear_stdin_buffer();
        input_buffer[strcspn(input_buffer, "\n")] = '\0';
    
        if (strlen(input_buffer) == 1 && (input_buffer[0] == 'y' || input_buffer[0] == 'Y')) {
            printf("%sEnter the number of the student (1 to %d): %s", COLOR_CYAN, record_count, COLOR_WHITE);
    
            if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
                printf("%sInput error. Please try again.%s\n", COLOR_RED, COLOR_RESET);
                continue;
            }
    
            if (strchr(input_buffer, '\n') == NULL)
                clear_stdin_buffer();
            input_buffer[strcspn(input_buffer, "\n")] = '\0';
    
            int student_number = atoi(input_buffer);
            if (student_number < 1 || student_number > record_count) {
                printf("%sInvalid student number. Please enter a number between 1 and %d.%s\n", COLOR_RED, record_count, COLOR_RESET);
                continue;
            }
    
            FILE *fp = fopen(records[student_number - 1].filepath, "r");
            if (fp) {
                char student_name[100] = "N/A"; // Default to "N/A" if missing
                char line[100]; // Declare the line variable
                rewind(fp); // Reset file pointer to read the name
                if (fgets(line, sizeof(line), fp)) sscanf(line, "Name    : %[^\n]", student_name); // Adjusted format string

                printf("\n%sDetailed Information for Student #%d (%s):%s\n", COLOR_YELLOW, student_number, student_name, COLOR_RESET);
                printf("%s----------------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);

                rewind(fp); // Reset file pointer to read all details
                while (fgets(line, sizeof(line), fp)) {
                    char key[50], value[100];

                    if (strchr(line, ':') != NULL && sscanf(line, "%49[^:]: %[^\n]", key, value) == 2) {
                        if (strcmp(key, "Score") == 0) {
                            char score[20], date[20];
                            if (sscanf(value, "%19s %19s", score, date) == 2) {
                                printf("%s%-20s: %s%s\n", COLOR_CYAN, "Score", COLOR_WHITE, score);
                                printf("%s%-20s: %s%s\n", COLOR_CYAN, "Date", COLOR_WHITE, date);
                            } else {
                                printf("%s%-20s: %s%s\n", COLOR_CYAN, key, COLOR_WHITE, value);
                            }
                        } else if (strcmp(key, "Answers") == 0) {
                            printf("%s%-20s:%s\n", COLOR_CYAN, "Answers", COLOR_RESET);
                            for (int i = 0; value[i] != '\0'; i++) {
                                printf("%d. %c (Correct: %c)\n", i + 1, value[i], value[i]); // Remove extra spaces
                            }
                        } else if (strcmp(key, "Correct") == 0) {
                            printf("%s%-20s:%s\n", COLOR_CYAN, "Correct", COLOR_RESET);
                            for (int i = 0; value[i] != '\0'; i++) {
                                printf("%d. %c (Correct: %c)\n", i + 1, value[i], value[i]); // Remove extra spaces
                            }
                        } else {
                            printf("%s%-20s: %s%s\n", COLOR_CYAN, key, COLOR_WHITE, value);
                        }
                    } else {
                        // Print raw lines like "1. a (Correct: a)"
                        printf("%s", line);
                    }
                }

                fclose(fp);
            } else {
                printf("%sFailed to open the record file for the selected student.%s\n", COLOR_RED, COLOR_RESET);
            }
    
        } else if (strlen(input_buffer) == 1 && (input_buffer[0] == 'n' || input_buffer[0] == 'N')) {
            break;
        } else {
            printf("%sInvalid choice. Please enter 'y' or 'n'.%s\n", COLOR_RED, COLOR_RESET);
        }
    }
    
    
    
    printf("\n\n%sPress Enter to go back to the main menu%s...\n", COLOR_GREEN, COLOR_RESET);
    printf("%s------------------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
    getchar();
}
