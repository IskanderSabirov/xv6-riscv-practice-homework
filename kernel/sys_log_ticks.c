#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"

int
sys_log_ticks(void) {

    acquire(&tickslock);
    uint cur_locks = ticks;
    release(&tickslock);
    pr_msg("Current ticks: %d", cur_locks);


    return 0;
}