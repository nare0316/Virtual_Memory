#include <stdbool.h>
#define SIZE 4096


typedef struct {
    int start;
    int end;
    int size;
} Text;

typedef struct {
    int start;
    int end;
    int size;
} Rodata;

typedef struct {
    int start;
    int end;
    int size;
} Data;

typedef struct {
    int start;
    int end;
    int size;
} Bss;

typedef struct {
    Text text;
    Rodata rodata;
    Data data;
    Bss bss;
} Pages;


typedef struct {
    int val;
    int w;
    int r;
    int vaddr;
    int paddr;
} PTE;

void initPagesSize(Pages *pages, int t, int r, int d, int b);
PTE *parse(FILE *stream, Pages *pages);
void initPTE(PTE *arr, int t, int r,  int d, int b);
bool isSeg(Pages *pages, int addr);
int checkVal(int addr, PTE *arr);