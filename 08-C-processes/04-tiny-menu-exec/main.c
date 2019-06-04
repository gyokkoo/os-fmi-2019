#include <stdio.h>
#include <unistd.h>
int main() {
    static const char* commands[] = { 
        "ps",
        "ls",
        "date",
        "chernobil"
    };

    printf("Choose\n");
    printf("0=ps, 1=ls, 2=date, 3=chernobil : ");
    
    int i;
    scanf("%d", &i);

    execlp(commands[i], commands[i], NULL);

    printf("Command not found! \n");
}
