#include <stdint.h>
#include <stdint.h>
#include <unistd.h>

__attribute__((weak)) int _close(int fd) {
    (void)fd; // Suppress unused parameter warning
    return 0;
}

__attribute__((weak)) int _lseek(int fd, int offset, int whence) {
    (void)fd; // Suppress unused parameter warning
    (void)offset; // Suppress unused parameter warning
    (void)whence; // Suppress unused parameter warning
    return 0;
}

__attribute__((weak)) int _read(int fd, char *buf, int count) {
    (void)fd; // Suppress unused parameter warning
    (void)buf; // Suppress unused parameter warning
    (void)count; // Suppress unused parameter warning
    return 0;
}

__attribute__((weak)) int _write(int fd, char *buf, int count) {
    (void)fd; // Suppress unused parameter warning
    (void)buf; // Suppress unused parameter warning
    (void)count; // Suppress unused parameter warning
    return 0;
}

__attribute__((weak)) void _exit(int status) {
    (void)status; // Suppress unused parameter warning
    while (1) {
        // Infinite loop to halt execution
    }
}

__attribute__((weak)) void* _sbrk(int incr) {
    (void)incr; // Suppress unused parameter warning
    return (void*)-1;
}
