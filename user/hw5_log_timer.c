#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"

int
main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(2, "Not enough arguments, write <event (exec, swtch, intr, syscall)> <number of ticks>\n");
        return -1;
    }

    char *event = argv[1];
    int timer = atoi(argv[2]);

    if (strcmp(event, "exec") == 0) {
        return logger_timer(EXEC, timer);
    } else if (strcmp(event, "intr") == 0) {
        return logger_timer(INTRPT, timer);
    } else if (strcmp(event, "swtch") == 0) {
        return logger_timer(SWTCH, timer);
    } else if (strcmp(event, "syscall") == 0) {
        return logger_timer(SYSCALL, timer);
    }

    fprintf(2, "Incorrect event name: [%s], write one of (exec, swtch, intr, syscall)\n", event);
    return -3;

}