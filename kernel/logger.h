#ifndef XV6_RISCV_PRACTICE_HOMEWORK_LOGGER_H
#define XV6_RISCV_PRACTICE_HOMEWORK_LOGGER_H

#include "param.h"
#include "types.h"
#include "spinlock.h"

struct logging_flags {
    int sys_call;
    int interrupt;
    int process_switch;
    int exec;
    struct spinlock lock;

    uint64 syscall_start, syscall_timer;
    uint64 interrupt_start, interrupt_timer;
    uint64 swtch_start, swtch_timer;
    uint64 exec_start, exec_timer;

};

#endif //XV6_RISCV_PRACTICE_HOMEWORK_LOGGER_H
