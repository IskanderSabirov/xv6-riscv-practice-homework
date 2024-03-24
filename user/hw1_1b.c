#include "../kernel/types.h"
#include "user.h"

#define SLEEP_TIME 150


int main() {

    int pid, exit_code;
    pid = fork();

    if (pid == 0) {

        sleep(SLEEP_TIME);
        exit(0);

    } else if (pid > 0) {

        fprintf(1, "Родительский процесс: PID = %d, Дочерний процесс: PID = %d\n", getpid(), pid);

        if (kill(pid) == -1) {
            fprintf(2, "Ошибка в комнаде kill родительского процесса\n");
            exit(-1);
        }

        wait(&exit_code);
        fprintf(1, "Завершен дочерний процесс - kill: PID = %d, Код возврата = %d\n", pid, exit_code);

    } else {
        fprintf(2, "Ошибка при создании дочернего процесса\n");
    }

    exit(0);
}