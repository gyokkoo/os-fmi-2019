/*
 * 	Да се напише програма на C, която изпълнява команда date.
 */

#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>

int main() {
    if (execl("/bin/date", "date", NULL) == -1) {
        errx(1, "error with date command");
    }
    
    exit(0);
}
