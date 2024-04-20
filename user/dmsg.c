#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"

int main() {

    char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE + 1);

    if (!buffer) {
        fprintf(2, "Error in allocation memory\n");
        exit(-2);
    }

    int len = dmsg(buffer, BUFFER_SIZE + 1);

    if (len != BUFFER_SIZE + 1) {
        fprintf(2, "Error: incorrect length of buffer: %d, waited: %d\n", len, BUFFER_SIZE);
        free(buffer);
        exit(-1);
    }

    for (int i = 0; i < len; ++i) {
        char c = buffer[i];
        printf("%c", c);
    }

    free(buffer);

    exit(0);
}