#include "buffer.h"
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include <stdarg.h>

struct diagnostic_buffer buffer;
struct logging_flags logger;

void
buffer_init() {
    initlock(&buffer.lock, "diagnostic_buffer");
    buffer.data[0] = '\n';
    buffer.tail = buffer.data + 1;
    for (int i = 1; i < BUFFER_SIZE; ++i)
        buffer.data[i] = '\0';
}

void
logger_init() {
    initlock(&buffer.lock, "logger_lock");
    logger.exec = 0;
    logger.interrupt = 0;
    logger.process_switch = 0;
    logger.sys_call = 0;
}

// flags: 1 - exec, 2 - intrpt, 3 - swtch, 4 - syscall
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
buffer_write(char symb) {

    if (*buffer.tail != '\0') {
        char *current = buffer.tail;
        while (1) {
            char was = *current;
            *current = '\0';
            current++;
            if (current == buffer.data + BUFFER_SIZE)
                current = buffer.data;
            if (was == '\n')
                break;
        }
    }

    *buffer.tail = symb;
    buffer.tail++;

    if (buffer.tail == buffer.data + BUFFER_SIZE)
        buffer.tail = buffer.data;

}

static char digits[] = "0123456789abcdef";

void
printint_buffer(int xx, int base, int sign) {
    char buf[16];
    int i;
    uint x;

    if (sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0) {
        buffer_write(buf[i]);
    }
}

void
printptr_buffer(uint64 x) {
    int i;
    buffer_write('0');
    buffer_write('x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        buffer_write(digits[x >> (sizeof(uint64) * 8 - 4)]);
}


void pr_msg(const char *fmt, ...) {

    acquire(&buffer.lock);

    acquire(&tickslock);
    uint cur_locks = ticks;
    release(&tickslock);

    buffer_write('[');
    printint_buffer(cur_locks, 10, 1);
    buffer_write(']');
    buffer_write(' ');

    va_list ap;
    int i, c;
    char *s;


    if (fmt == 0)
        panic("null fmt");

    va_start(ap, fmt);
    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            buffer_write(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if (c == 0)
            break;
        switch (c) {
            case 'd':
                printint_buffer(va_arg(ap, int), 10, 1);
                break;
            case 'x':
                printint_buffer(va_arg(ap, int), 16, 1);
                break;
            case 'p':
                printptr_buffer(va_arg(ap, uint64));
                break;
            case 's':
                if ((s = va_arg(ap, char*)) == 0)
                    s = "(null)";
                for (; *s; s++)
                    buffer_write(*s);
                break;
            case '%':
                buffer_write('%');
                break;
            default:
                // Print unknown % sequence to draw attention.
                buffer_write('%');
                buffer_write(c);
                break;
        }
    }

    va_end(ap);

    buffer_write('\n');


    release(&buffer.lock);
}

int
copyout_buffer(char *buf, int size) {

    if (!buf)
        return -3;

    if (size <= 0)
        return -1;

    uint64 copy_len = ((size < BUFFER_SIZE + 1) ? size - 1 : BUFFER_SIZE);

    acquire(&buffer.lock);

    uint64 begin = 0;
    while (begin < (copy_len - 1) && buffer.data[begin] != '\n')
        ++begin;

    // свдигаем на следующий символ после '\n'
    ++begin;

    // копируем, что было после первого '\n'
    if (copyout(myproc()->pagetable, (uint64) (buf), (buffer.data + begin), sizeof(char) * (copy_len - begin)) != 0) {
        release(&buffer.lock);
        return -2;
    }

    // копируем начало, что было до первого '\n'
    if (copyout(myproc()->pagetable, (uint64) (buf + copy_len - begin), (buffer.data), sizeof(char) * (begin)) != 0) {
        release(&buffer.lock);
        return -2;
    }

    char null = '\0';

    if (copyout(myproc()->pagetable, (uint64) (buf + copy_len), &null, sizeof(char)) != 0) {
        release(&buffer.lock);
        return -2;
    }

    release(&buffer.lock);

    return copy_len + 1;
}