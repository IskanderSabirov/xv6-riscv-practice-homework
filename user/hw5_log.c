#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"

int
main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(2, "Not enough arguments, write <event (exec, swtch, intr, syscall)> <on/off>\n");
        return -1;
    }

    char *event = argv[1];
    char *on_off = argv[2];
    int op = 0;

    if (strcmp(on_off, "on") == 0) {
        op = 1;
    } else if (strcmp(on_off, "off") == 0) {
        op = 0;
    } else {
        fprintf(2, "Incorrect 2nd argument: [%s], write 'on' or 'off'\n", on_off);
        return -2;
    }

    if (strcmp(event, "exec") == 0) {
        return tune_log(EXEC, op);
    } else if (strcmp(event, "intr") == 0) {
        return tune_log(INTRPT, op);
    } else if (strcmp(event, "swtch") == 0) {
        return tune_log(SWTCH, op);
    } else if (strcmp(event, "syscall") == 0) {
        return tune_log(SYSCALL, op);
    }

    fprintf(2, "Incorrect event name: [%s], write one of (exec, swtch, intr, syscall)\n", event);
    return -3;

}