#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"

int
sys_dmsg(void) {
    uint64 buf;
    int n;

    argaddr(0, &buf);
    argint(1, &n);

    int len = copyout_buffer((char *) buf, n);

    if (len < 0)
        return -1;

    return len;
}