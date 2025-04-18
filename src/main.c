#include "config.h"

void animate_welcome() {
    const char welcome_text[] = "WELCOME TO THE QUIZWHIZ SYSTEM";
    const char* colors[] = { COLOR_CYAN, COLOR_GREEN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE };
    const int color_count = 5;
    const int name_frames = 20;
    const int confetti_frames = 10;
    const int total_frames = name_frames + confetti_frames;
    const int max_bar_length = 30;

    for (int i = 0; i < total_frames; i++) {
        system(CLEAR);
        printf("%s", COLOR_LIGHT_PURPLE);

        // Phase 1: Welcome animation
        if (i < name_frames) {
            for (int k = 0; k < 6; k++) {
                for (int j = 0; j < i; j++) printf(" ");
                const char* dynamic_color = colors[(i + k) % color_count];
                printf("%s%s%s\n", dynamic_color, welcome_text, COLOR_RESET);
            }
        }

        // Phase 2: Confetti animation
        else {
            for (int row = 0; row < 10; row++) {
                for (int col = 0; col < 40; col++) {
                    if (rand() % 10 < 2) {
                        printf("%s%c%s", COLOR_YELLOW, '*', COLOR_RESET);
                        printf("%s%c%s", COLOR_LIGHT_PURPLE, '*', COLOR_RESET);
                        printf("%s%c%s", COLOR_WHITE, '*', COLOR_RESET);
                    } else {
                        printf(" ");
                    }
                }
                printf("\n");
            }
        }

        // Move cursor to fixed position
        printf("\033[12;1H");

        // Loading bar
        int progress = (i + 1) * max_bar_length / total_frames;
        printf("%sLoading [", COLOR_CYAN);
        for (int p = 0; p < max_bar_length; p++) {
            printf(p < progress ? "=" : " ");
        }
        printf("]%s", COLOR_RESET);
        fflush(stdout);

#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
    }

    // Final done message
    printf("\033[13;1H%sDone!%s\n", COLOR_GREEN, COLOR_RESET);
#ifdef _WIN32
    Sleep(500);
#else
    usleep(500000);
#endif
}

/*
void animate_names_on_exit(int choice) {
    if (choice != 4) return;

    const char* name_colors[] = { COLOR_CYAN, COLOR_GREEN, COLOR_MAGENTA };
    const char* names[] = {
        "\tJohn Paul D. Abecia",
        "\tGwyneth Miasco",
        "\tSharmagne Faith Gamboa"
    };
    const int name_count = 3;

    for (int i = 0; i < 20; i++) {
        system(CLEAR);

        // Top greeting
        for (int s = 0; s < i; s++) printf(" ");
        printf("%sThank you for using the QuizWhiz System!%s\n", COLOR_YELLOW, COLOR_RESET);

        // Animated names
        for (int n = 0; n < name_count; n++) {
            for (int s = 0; s < i; s++) printf(" ");
            printf("%s%s%s\n", name_colors[(i + n + i) % 3], names[n], COLOR_RESET);
        }

        // Bottom greeting
        for (int s = 0; s < i; s++) printf(" ");
        printf("%sThank you for using the QuizWhiz System!%s\n", COLOR_YELLOW, COLOR_RESET);

#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
    }

    printf("\nPress Enter to exit...");
    getchar(); // absorb newline from prior input
}

*/


int main() {
    char input_buffer[10];
    int choice;
    int running = 1; 
    int first_time = 1; // Flag to check if it's the first time

    while (running) {
        system(CLEAR);
        if (first_time) {
            animate_welcome();
            first_time = 0; // Set the flag to 0 after the first display
        }
        system(CLEAR);
        printf("%s====================================%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
        printf("%s   WELCOME TO THE QUIZWHIZ SYSTEM%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("%s====================================%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
        printf("%s[1] %sMake a Quiz%s\n", COLOR_YELLOW, COLOR_CYAN, COLOR_RESET); 
        printf("%s[2] %sView Student's Data%s\n", COLOR_YELLOW, COLOR_CYAN, COLOR_RESET);
        printf("%s[3] %sView and Take Quizzes%s\n", COLOR_YELLOW, COLOR_CYAN, COLOR_RESET);
        printf("%s[4] Exit the system%s\n", COLOR_RED, COLOR_RESET);
        printf("%s------------------------------------%s\n", COLOR_LIGHT_PURPLE, COLOR_RESET);
        printf("%sEnter your choice:%s ", COLOR_CYAN, COLOR_RESET);
        printf("%s", BOLD_WHITE);
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
                // animate_names_on_exit(choice); 
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