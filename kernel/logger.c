#include "buffer.h"
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include <stdarg.h>

struct logging_flags logger;

void
logger_init() {
    initlock(&logger.lock, "logger_lock");
    logger.exec = 0;
    logger.interrupt = 0;
    logger.process_switch = 0;
    logger.sys_call = 0;
}

// flags: 1 - exec, 2 - intrpt, 3 - swtch, 4 - syscall, there are defines in param.h
// types: 1 - on, 0 - off
int
tune_logger(int flag, int type) {
    if (type != 1 && type != 0)
        return -1;
    if (flag <= 0 || flag > 5)
        return -2;

    acquire(&logger.lock);

    switch (flag) {
        case 1:
            logger.exec = type;
            break;
        case 2:
            logger.interrupt = type;
            break;
        case 3:
            logger.process_switch = type;
            break;
        case 4:
            logger.sys_call = type;
            break;
    }

    release(&logger.lock);

    return 0;
}

int
logger_flag(int flag) {
    if (flag <= 0 || flag > 5)
        return -1;

    acquire(&logger.lock);

    switch (flag) {
        case 1:
            release(&logger.lock);
            return logger.exec;
        case 2:
            release(&logger.lock);
            return logger.interrupt;
        case 3:
            release(&logger.lock);
            return logger.process_switch;
        case 4:
            release(&logger.lock);
            return logger.sys_call;
    }

    release(&logger.lock);

    return -2;
}

void
log_exec(int pid, char *path) {
    pr_msg("Exec: pid = [%d], program = [%s]", pid, path);
}

void
log_swtch(int pid, char *name, struct trapframe t, struct context c) {
    pr_msg("Switch proc with pid: [%d], name: [%s],\n"
           "trapframe:\nkernel_satp: [%d], kernel_sp: [%d], kernel_trap: [%d], epc: [%d], kernel_hartid: [%d], ra : [%d], sp: [%d], gp: [%d], tp: [%d]\n"
           "t0: [%d], t1: [%d], t2: [%d], s0: [%d], s1: [%d],\n"
           "a0: [%d], a1: [%d], a2: [%d], a3: [%d], a4: [%d], a5: [%d], a6: [%d], a7: [%d],\n"
           "s2: [%d], s3: [%d], s4: [%d], s5: [%d], s6: [%d], s7: [%d], s8: [%d], s9: [%d], s10: [%d], s11: [%d],\n"
           "t3: [%d], t4: [%d], t5: [%d], t6: [%d],\n"
           "context:\nra: [%d], sp: [%d],\ns0: [%d], s1: [%d], s2: [%d], s3: [%d], s4: [%d], s5: [%d], s6: [%d], s7: [%d], s8: [%d], s9: [%d], s10: [%d], s11: [%d]",
           pid, name,
           t.kernel_satp, t.kernel_sp, t.kernel_trap, t.epc, t.kernel_hartid, t.ra, t.sp, t.gp, t.tp,
           t.t0, t.t1, t.t2, t.s0, t.s1,
           t.a0, t.a1, t.a2, t.a3, t.a4, t.a5, t.a6, t.a7,
           t.s2, t.s3, t.s4, t.s5, t.s6, t.s7, t.s8, t.s9, t.s10, t.s11,
           t.t3, t.t4, t.t5, t.t6,
           c.ra, c.sp, c.s0, c.s1, c.s2, c.s3, c.s4, c.s5, c.s6, c.s7, c.s8, c.s9, c.s10, c.s11);
}

void
log_syscall(int call_num, int pid, char *name) {
    pr_msg("Sys call: [%d], in pid: [%d], name: [%s]", call_num, pid, name);
}

void
log_unknown_syscall(int call_num, int pid, char *name) {
    pr_msg("%d %s: unknown sys call %d", call_num, pid, name);
}

void log_virtiointr() {
    pr_msg("Virtio interrupt with number: [%d]", VIRTIO0_IRQ);
}

void log_uartintr(char c) {
    if (c == 'D') {
        pr_msg("UART interrupt with number: [%d], reason: [console line end]", UART0_IRQ);
        return;
    }
    char buf[2] = {c, '\0'};
    pr_msg("UART interrupt with number: [%d], reason: [%s]", UART0_IRQ, buf);
}