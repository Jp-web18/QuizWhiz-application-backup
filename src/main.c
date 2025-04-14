#include "config.h"



int main() {
    char input_buffer[10];
    int choice;
    int running = 1; 

    while (running) {
        system(CLEAR);
        printf("%sWelcome to the QuizWhiz System%s\n\n", COLOR_YELLOW, COLOR_RESET);
        printf("%s[1] Make a Quiz%s\n", COLOR_GREEN, COLOR_RESET);
        printf("%s[2] View student's data%s\n", COLOR_GREEN, COLOR_RESET);
        printf("%s[3] View and Take Quizes%s\n", COLOR_GREEN, COLOR_RESET);
        printf("%s[4] Exit the system%s\n\n", COLOR_RED, COLOR_RESET);
        printf("%sEnter your choice:%s ", COLOR_YELLOW, COLOR_RESET);
        printf("%s", COLOR_CYAN);
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            choice = atoi(input_buffer);
        } else {
            choice = 0; // Handle potential input error
        }
        printf("%s", COLOR_RESET);

        switch (choice) {
            case 1:
                make_quiz_menu();
                break;
            case 2:
                view_student_data();
                break;
            case 3:
                view_take_quizzes();
                break;
            case 4:
                printf("\nExiting the system...\n");
                running = 0;
                break;
            default:
                printf("%sInvalid choice. Please try again.%s\n", COLOR_RED, COLOR_RESET);
                sleep(1);
        }
    }


    printf("Press Enter to exit...\n");
    getchar();

    return 0;
}