#include <stdio.h>
    const int rodata_var = 42;
    static int data_var = 10;
    int bss_var;
    double bss_var;

void some_function() {
    // Code representing 32 bytes
    printf("Hello, World!\n");
    printf("Hello, World!\n");
    static int data_var = 10;
}

int main() {
    some_function();
    return 0;
}