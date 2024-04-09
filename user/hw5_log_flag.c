#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"

int
main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(2, "Not enough arguments\n");
        return -1;
    }

    int flag = atoi(argv[1]);
    int val = atoi(argv[2]);

    if (tune_log(flag, val) < 0) {
        fprintf(2, "Error\n");;
        return -1;
    }

    return 0;

}