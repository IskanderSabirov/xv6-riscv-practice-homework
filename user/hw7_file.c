#include "../kernel/types.h"
#include "../user/user.h"
#include "../kernel/fcntl.h"


#define large_file_name "large_file"

const int BUFFER_SIZE = 128 * sizeof(uint64);

const uint64 MOD = 199999999, MUL = 47, ADD = 19;

// returns bool
uint64 string_to_uint64(char *str, uint64 *result) {
    *result = 0;
    while (*str != 0) {
        if (*str < '0' || *str > '9') {
            return 0;
        }
        *result = (*result) * 10 + (*str - '0');
        str++;

    }
    return 1;
}

uint64 get_next(uint64 current) {
    return (current * MUL + ADD) % MOD;
}

int write_file(uint64 size, uint64 first) {
    int file_descriptor = open(large_file_name, O_CREATE | O_WRONLY);

    uint64 current = first;
    char buffer[BUFFER_SIZE];
    int current_buffer_size = 0;

    for (uint64 i = 0; i < size; i++) {
        if (current_buffer_size == BUFFER_SIZE) {
            if (write(file_descriptor, buffer, BUFFER_SIZE) != BUFFER_SIZE) {
                close(file_descriptor);
                return -1;
            }
            current_buffer_size = 0;
        }
        memcpy(buffer + current_buffer_size, (char *) (&current), sizeof(current));
        current_buffer_size += sizeof(current);
        current = get_next(current);
    }
    if (current_buffer_size != 0 && write(file_descriptor, buffer, current_buffer_size) != current_buffer_size) {
        close(file_descriptor);
        return -1;
    }
    close(file_descriptor);
    return 0;
}

int check_file(uint64 size, uint64 first) {
    int file_descriptor = open(large_file_name, O_RDONLY);

    uint64 current = first, read_element;

    char buffer[BUFFER_SIZE];
    int count_uint = BUFFER_SIZE / sizeof(uint64);
    while (size > count_uint) {
        if (read(file_descriptor, buffer, BUFFER_SIZE) != BUFFER_SIZE) {
            close(file_descriptor);
            return -1;
        }
        for (int i = 0; i < count_uint; i++) {
            read_element = *((uint64 *) (buffer + i * sizeof(uint64)));
            if (read_element != current) {
                close(file_descriptor);
                fprintf(2, "Ошибка в элементе файлов:[%l] [%l]\n", read_element, current);
                return -2;
            }
            current = get_next(current);
        }
        size -= count_uint;
    }

    int bytes_size = size * sizeof(uint64);
    if (read(file_descriptor, buffer, bytes_size) != bytes_size) {
        close(file_descriptor);
        return -1;
    }

    for (int i = 0; i < size; i++) {
        read_element = *((uint64 *) (buffer + i * sizeof(uint64)));
        if (read_element != current) {
            close(file_descriptor);
            fprintf(2, "Ошибка в элементе файлов:[%l] [%l]\n", read_element, current);
            return -2;
        }
        current = get_next(current);
    }
    close(file_descriptor);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Ошибка в количестве параметров\n", argc - 1);
        exit(1);
    }
    uint64 size = 0;
    if (!string_to_uint64(argv[1], &size)) {
        fprintf(2, "Некорректный размер\n");
        exit(1);
    }

    uint64 first = 0;
    if (!string_to_uint64(argv[2], &first)) {
        fprintf(2, "Ошибка первого элемента\n");
        exit(1);
    }
    printf("размер = %l, первый элемент = %l\n", size, first);

    if (write_file(size, first) != 0) {
        fprintf(2, "Ошибка записи\n");
        exit(-1);
    }

    fprintf(2, "Данные записаны\n");

    if (check_file(size, first) != 0) {
        fprintf(2, "Ошибка чтения\n");
        exit(-1);
    }

    printf("Данные прочитаны\n");

    exit(0);
}
