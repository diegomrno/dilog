#define DILOG_IMPLEMENTATION
#include "../dilog.h"

int main() {
    dilog_init();
    dilog_console_print("main", "info", "Hello, world!");
    dilog_console_print("main", "error", "Hello, world!");
    return 0;
}
