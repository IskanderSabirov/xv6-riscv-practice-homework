#include "buffer.h"
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include <stdarg.h>

struct diagnostic_buffer buffer;

void
buffer_init() {
    initlock(&buffer.lock, "diagnostic_buffer");
    buffer.tail = buffer.data;

}

void
buffer_write(char symb) {

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