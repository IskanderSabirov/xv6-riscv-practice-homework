#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"

int main() {

    char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);

    if (!buffer) {
        fprintf(2, "Error in allocation memory\n");
        exit(-2);
    }

    int len = dmsg(buffer, BUFFER_SIZE);

    if (len != BUFFER_SIZE) {
        fprintf(2, "Error: incorrect length of buffer: %d, waited: %d\n", len, BUFFER_SIZE);
        exit(-1);
    }

    printf(buffer);

    exit(0);
}