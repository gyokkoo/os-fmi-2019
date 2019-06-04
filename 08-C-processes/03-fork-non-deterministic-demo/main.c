#include <stdio.h>
#include <unistd.h>

const int N = 10000;

// Occasionally you may see child output appearing
// AFTER the prompt. This is expected behaviour.
int main() {
    int pid = fork();
    if (pid == 0) {
        for (int i = 0; i < N; ++i) {
	        printf("**     CHILD %d \n", getpid());
        }
    } else {
        for (int i = 0; i < N; ++i) {
	        printf("  **   PARENT  %d \n", getpid());
        }
    }
}
