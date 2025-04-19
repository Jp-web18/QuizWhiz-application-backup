#include "config.h"



void view_take_quizzes() {
    DIR *d;
    struct dirent *dir;
    int quiz_count = 0;

retry_input:
    system(CLEAR);
    quiz_count = 0;

    d = opendir("quizzes");
    if (d) {
        printf("%sAvailable Quizzes:%s\n\n", COLOR_YELLOW, COLOR_RESET);
        
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".quiz")) {
                printf("%d. %s\n", ++quiz_count, dir->d_name);
            }
        }
        closedir(d);
    }

    if (quiz_count == 0) {
        printf("%sNo quizzes made yet.%s\n", COLOR_RED, COLOR_RESET);
#ifdef _WIN32
        Sleep(2000);
#else
        sleep(2);
#endif
        return;
    }

    char input[10];
    int choice;
    printf("\n%s[1]%s %sTake a quiz%s\n", COLOR_YELLOW, COLOR_RESET, COLOR_CYAN, COLOR_RESET);
    printf("%s[2]%s %sBack to main menu%s\n", COLOR_YELLOW, COLOR_RESET, COLOR_LIGHT_PURPLE, COLOR_RESET);
    printf("%sEnter your choice:%s ", COLOR_CYAN, COLOR_RESET);

    if (fgets(input, sizeof(input), stdin) == NULL || input[0] == '\n') {
        printf("%sInvalid input. Please press 1 or 2.%s\n", COLOR_RED, COLOR_RESET);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        goto retry_input;
    }

    char* endptr;
    choice = strtol(input, &endptr, 10);
    if (*endptr != '\n' || (choice != 1 && choice != 2)) {
        printf("%sInvalid input. Please enter a valid option.%s\n", COLOR_RED, COLOR_RESET);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        goto retry_input;
    }

    if (choice == 1) {
        take_quiz();
    }
}

void take_quiz() {
    DIR *d;
    struct dirent *dir;
    char *quiz_files[100];
    int quiz_count = 0;
    char encryption_key = 'Q';

    d = opendir("quizzes");
    if (!d) {
        printf("%sQuiz directory not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep(2);
        return;
    }

    printf("%sAvailable Quizzes:%s\n\n", COLOR_YELLOW, COLOR_RESET);
    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".quiz")) {
            printf("[%d] %s\n", ++quiz_count, dir->d_name);
            quiz_files[quiz_count - 1] = strdup(dir->d_name);
        }
    }
    closedir(d);

    if (quiz_count == 0) {
        printf("%sNo quizzes available.%s\n", COLOR_RED, COLOR_RESET);
        sleep(2);
        return;
    }

    char input[16];
    int selection = -1;
    while (1) {
        printf("\n%sEnter the number of the quiz you want to take:%s ", COLOR_CYAN, COLOR_RESET);
        if (!fgets(input, sizeof(input), stdin) || input[0] == '\n') {
            printf("%sInvalid input.%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }

        char *endptr;
        selection = strtol(input, &endptr, 10);
        if (*endptr != '\n' || selection < 1 || selection > quiz_count) {
            printf("%sPlease enter a valid quiz number.%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }

        break;
    }

    const char *selected_quiz = quiz_files[selection - 1];
    char filename[128];
    snprintf(filename, sizeof(filename), "quizzes/%s", selected_quiz);

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Unable to open quiz");
        return;
    }

    // Read and decrypt quiz data
    char quiz_data[256];
    size_t bytes_read = fread(quiz_data, 1, sizeof(quiz_data), fp);
    fclose(fp);

    if (bytes_read == 0) {
        printf("%sFailed to read quiz file or file is corrupted.%s\n", COLOR_RED, COLOR_RESET);
        sleep(2);
        return;
    }

    encrypt_decrypt_xor(quiz_data, bytes_read, encryption_key);

    // Parse decrypted data
    int duration, items;
    char correct_answers[100];
    char *quiz_data_ptr = quiz_data; // Pointer to traverse quiz_data

    // Extract duration and number of items
    if (sscanf(quiz_data_ptr, "%d\n%d\n", &duration, &items) != 2) {
        printf("%sFailed to parse quiz metadata. File may be corrupted.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    // Move pointer to the start of the correct answers
    quiz_data_ptr = strchr(quiz_data_ptr, '\n') + 1; // Skip duration line
    quiz_data_ptr = strchr(quiz_data_ptr, '\n') + 1; // Skip items line

    // Copy correct answers
    strncpy(correct_answers, quiz_data_ptr, items);
    correct_answers[items] = '\0'; // Ensure null-termination

    printf("%sTime Duration:%s %d minutes\n", COLOR_YELLOW, COLOR_RESET, duration);

    char student_name[100], section[20], pc_number[10], submission_date[11];

    while (1) {
        printf("%sEnter your name:%s ", COLOR_CYAN, COLOR_RESET);
        fgets(student_name, sizeof(student_name), stdin);
        student_name[strcspn(student_name, "\n")] = '\0';

        printf("%sEnter your section code:%s ", COLOR_CYAN, COLOR_RESET);
        fgets(section, sizeof(section), stdin);
        section[strcspn(section, "\n")] = '\0';

        printf("%sEnter your PC number:%s ", COLOR_CYAN, COLOR_RESET);
        fgets(pc_number, sizeof(pc_number), stdin);
        pc_number[strcspn(pc_number, "\n")] = '\0';

        printf("\n%sPlease confirm your information:%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("%sName:%s %s\n", COLOR_CYAN, COLOR_RESET, student_name);
        printf("%sSection:%s %s\n", COLOR_CYAN, COLOR_RESET, section);
        printf("%sPC Number:%s %s\n", COLOR_CYAN, COLOR_RESET, pc_number);
        printf("\n%sIs this information correct?%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("%s[1] Yes%s\n", COLOR_GREEN, COLOR_RESET);
        printf("%s[2] No%s\n", COLOR_RED, COLOR_RESET);
        printf("%sEnter your choice:%s ", COLOR_CYAN, COLOR_RESET);

        if (fgets(input, sizeof(input), stdin)) {
            int choice = atoi(input);
            if (choice == 1) break;
            if (choice == 2) continue;
        }

        printf("%sInvalid input. Please try again.%s\n", COLOR_RED, COLOR_RESET);
    }

    char record_file[128];
    snprintf(record_file, sizeof(record_file), "records/%s_%s.rec", selected_quiz, student_name);

    if (file_exists(record_file)) {
        printf("%sYou have already taken this quiz with the same name. Not allowed to take twice.%s\n", COLOR_RED, COLOR_RESET);
        sleep(2);
        return;
    }

    printf("%sThe quiz will start now. You have %d minutes to complete it.%s\n", COLOR_YELLOW, duration, COLOR_RESET);
    printf("%sPress ENTER to begin...%s", COLOR_CYAN, COLOR_RESET);
    fgets(input, sizeof(input), stdin);

    time_t start_time = time(NULL);
    time_t end_time = start_time + (duration * 60);

    char user_answers[100];
    for (int i = 0; i < items; i++) {
        time_t current_time = time(NULL);
        if (current_time >= end_time) {
            printf("\n%sTime is up! Submitting your answers...%s\n", COLOR_RED, COLOR_RESET);
            break;
        }

        printf("Question #%d answer: ", i + 1);
        char ans_input[4];
        if (fgets(ans_input, sizeof(ans_input), stdin)) {
            ans_input[strcspn(ans_input, "\n")] = '\0'; // Remove newline
            if (strlen(ans_input) != 1) {
                printf("%sInvalid input. Each answer must be a single character.%s\n", COLOR_RED, COLOR_RESET);
                i--; // Retry the current question
                continue;
            }
            user_answers[i] = ans_input[0]; // Store the single character answer
        } else {
            printf("%sInvalid input. Skipping question.%s\n", COLOR_RED, COLOR_RESET);
            user_answers[i] = ' '; // Default to a blank answer
        }
    }
    user_answers[items] = '\0'; // Ensure user_answers is null-terminated

    if (time(NULL) >= end_time) {
        printf("\n%sTime is up! Your quiz has been automatically submitted.%s\n", COLOR_RED, COLOR_RESET);
    } else {
        int confirmed = 0;
        while (1) {
            printf("Are you finished answering the quiz? %s[1] Yes%s %s[2] No:%s ", COLOR_YELLOW, COLOR_RESET, COLOR_YELLOW, COLOR_RESET);
            if (!fgets(input, sizeof(input), stdin) || input[0] == '\n') {
                printf("%sInvalid input.%s\n", COLOR_RED, COLOR_RESET);
                continue;
            }

            char *endptr;
            confirmed = strtol(input, &endptr, 10);
            if (*endptr != '\n' || (confirmed != 1 && confirmed != 2)) {
                printf("%sPlease enter 1 for Yes or 2 for No.%s\n", COLOR_RED, COLOR_RESET);
                continue;
            }

            break;
        }

        if (confirmed != 1) return;
    }

    int score = 0;
    for (int i = 0; i < items; i++) {
        if (tolower(user_answers[i]) == tolower(correct_answers[i])) { // Case-insensitive comparison
            score++;
        }
    }

    float percentage = ((float)score / items) * 100;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(submission_date, sizeof(submission_date), "%m/%d/%Y", &tm);

    FILE *rec = fopen(record_file, "w");
    fprintf(rec, "Name: %s\n", student_name);
    fprintf(rec, "Section: %s\n", section);
    fprintf(rec, "PC: %s\n", pc_number);
    fprintf(rec, "Score: %d/%d %s\n", score, items, submission_date);
    fprintf(rec, "Percent: %.2f%%\n", percentage);
    fprintf(rec, "Answers: %s\n", user_answers);
    fprintf(rec, "Correct: %s\n", correct_answers);
    fclose(rec);
    chmod(record_file, 0444);

    printf("%sQuiz submitted. Score:%s %d/%d (%.2f%%) on %s\n", COLOR_GREEN, COLOR_RESET, score, items, percentage, submission_date);
    printf("\n%sPress ENTER to return to the main menu...%s", COLOR_CYAN, COLOR_RESET);
    fgets(input, sizeof(input), stdin); // Wait for ENTER
}
