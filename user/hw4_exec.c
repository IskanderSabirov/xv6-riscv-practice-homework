#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"


int main() {

    int pid = fork();

    int exit_code;

    if (pid == 0) {

        char *args[] = {"echo", "Echo done successfully!", 0};

        exec("echo", args);

        printf("Exec failed\n");
        exit(-1);

    } else if (pid > 0) {

        wait(&exit_code);

        char *buf = malloc(sizeof(char) * BUFFER_SIZE);
        printf("Buffer size - %d\n", dmsg(buf, BUFFER_SIZE));
        printf("%s", buf);

    } else {

        fprintf(2, "Error in fork\n");
        exit(-1);

    }

    exit(0);

}