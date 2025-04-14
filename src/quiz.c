#include "config.h"




void view_take_quizzes() {
    system(CLEAR);

    DIR *d;
    struct dirent *dir;
    int quiz_count = 0;

    d = opendir("quizzes");
    if (d) {
        printf("Available Quizzes:\n\n");
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".quiz")) {
                printf("[%d] %s\n", ++quiz_count, dir->d_name);
            }
        }
        closedir(d);
    }

    if (quiz_count == 0) {
        printf("No quizzes made yet.\n");
        sleep(2);
        return;
    }

    int choice;
    printf("\n[1] Take a quiz\n[2] Back to main menu\nEnter your choice: ");
    scanf("%d", &choice);
    getchar(); // Clear the newline character from the input buffer

    if (choice == 1) {
        take_quiz();
    }
}

void take_quiz() {
    char filename[100];
    char student_name[100];
    char section[20];
    char pc_number[10];
    char quizname[100];
    char submission_date[11];

    printf("Enter quiz filename (without .quiz): ");
    scanf("%s", quizname);
    getchar(); // Clear the newline character from the input buffer

    snprintf(filename, sizeof(filename), "quizzes/%s.quiz", quizname);

    if (!file_exists(filename)) {
        printf("Quiz not found.\n");
        sleep(2);
        return;
    }

    char record_file[128];
    snprintf(record_file, sizeof(record_file), "records/%s_%s.rec", quizname, getenv("USER"));

    if (file_exists(record_file)) {
        printf("You have already taken this quiz. Not allowed to take twice.\n");
        sleep(2);
        return;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Unable to open quiz");
        return;
    }

    int duration, items, score = 0;
    char correct_answers[100];
    char user_answers[100];

    fscanf(fp, "%d\n%d\n%s", &duration, &items, correct_answers);
    fclose(fp);

    printf("Time Duration: %d minutes\n", duration);
    printf("Enter your name: ");
    scanf(" %[^\n]", student_name);
    printf("Enter your section code: ");
    scanf("%s", section);
    printf("Enter your PC number: ");
    scanf("%s", pc_number);

    getchar(); // Clear the newline character from the input buffer

    for (int i = 0; i < items; i++) {
        printf("Question #%d answer: ", i + 1);
        scanf(" %c", &user_answers[i]);
    }
    user_answers[items] = '\0';

    printf("Are you finish answering the quiz? [1] Yes [2] No: ");
    int done;
    scanf("%d", &done);
    if (done != 1) return;

    for (int i = 0; i < items; i++) {
        if (user_answers[i] == correct_answers[i]) score++;
    }

    float percentage = ((float)score / items) * 100;

    // Get the current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(submission_date, sizeof(submission_date), "%m/%d/%Y", &tm);

    // Save the result to a record file, aligning with view_student_data
    FILE *rec = fopen(record_file, "w");
    fprintf(rec, "Name: %s\n", student_name);
    fprintf(rec, "Section: %s\n", section);
    fprintf(rec, "PC: %s\n", pc_number);
    fprintf(rec, "Score: %d/%d %s\n", score, items, submission_date); // Combined score and date
    fprintf(rec, "Percent: %.2f%%\n", percentage);
    fprintf(rec, "Answers: %s\n", user_answers);
    fprintf(rec, "Correct: %s\n", correct_answers);
    fclose(rec);
    chmod(record_file, 0444); // Make the record file read-only

    printf("Quiz submitted. Score: %d/%d (%.2f%%) on %s\n", score, items, percentage, submission_date);
    sleep(2);
}