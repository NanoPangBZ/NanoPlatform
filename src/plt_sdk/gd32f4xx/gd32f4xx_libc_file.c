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
