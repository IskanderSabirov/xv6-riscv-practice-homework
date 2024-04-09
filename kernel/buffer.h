#ifndef XV6_RISCV_PRACTICE_HOMEWORK_BUFFER_H
#define XV6_RISCV_PRACTICE_HOMEWORK_BUFFER_H

#include "param.h"
#include "types.h"
#include "spinlock.h"

struct diagnostic_buffer {
    char data[BUFFER_SIZE]; // Сам буфер
    char *tail;             // Указатель на конец данных
    struct spinlock lock;   // Спин-замок для защиты доступа к буферу
};

struct logging_flags {
    int sys_call;
    int interrupt;
    int process_switch;
    int exec;
    struct spinlock lock;
};

#endif //XV6_RISCV_PRACTICE_HOMEWORK_BUFFER_H
