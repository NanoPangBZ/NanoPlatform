#include <stdint.h>
#include <stdint.h>
#include <unistd.h>

int _close(int fd) {
    (void)fd; // Suppress unused parameter warning
    return 0;
}

int _lseek(int fd, int offset, int whence) {
    (void)fd; // Suppress unused parameter warning
    (void)offset; // Suppress unused parameter warning
    (void)whence; // Suppress unused parameter warning
    return 0;
}

int _read(int fd, char *buf, int count) {
    (void)fd; // Suppress unused parameter warning
    (void)buf; // Suppress unused parameter warning
    (void)count; // Suppress unused parameter warning
    return 0;
}

int _write(int fd, char *buf, int count) {
    (void)fd; // Suppress unused parameter warning
    (void)buf; // Suppress unused parameter warning
    (void)count; // Suppress unused parameter warning
    return 0;
}


void _exit(int status) {
    (void)status; // Suppress unused parameter warning
    while (1) {
        // Infinite loop to halt execution
    }
}

void* _sbrk(int incr) {
    (void)incr; // Suppress unused parameter warning
    return (void*)-1;
}

int* __errno(void) {
    static int nano_errno;
    return &nano_errno;
}
