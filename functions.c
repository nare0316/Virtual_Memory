#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "myLib.h"

int count;
int ram = 4000;//start pysical address space at optional address Ox4000

//count page sizes, start and address of pages, t-text pages count, r-rodata pages count, d-data pages count, b-bss pages count
void initPagesSize(Pages *pages, int t, int r, int d, int b) {
    pages->text.start = 1000; //start virtual address space at optional address Ox1000
    pages->text.end = pages->text.start + t*SIZE - 1;
    pages->text.size = 0;

    pages->rodata.start = pages->text.end + 1;
    pages->rodata.end = pages->rodata.start + r*SIZE - 1;
    pages->rodata.size = 0;

    pages->data.start = pages->rodata.end + 1;
    pages->data.end = pages->data.start + d*SIZE - 1;
    pages->data.size = 0;

    pages->bss.start = pages->data.end + 1;
    pages->bss.end = pages->bss.start + b*SIZE -1;
    pages->bss.size = 0;

}
//initialize PTE-s
void initPTE(PTE *arr, int t, int r, int d, int b) {
    //init text PTE
    int i = 0;
    for(int j = 0; j<t; ++j, ++i) {
        arr[i].val = 0;
        arr[i].vaddr = i*SIZE+1;
        arr[i].w = 0;
        arr[i].r = 1;
        arr[i].paddr = -1;
    }
    //init rodata PTE
    for(int j = 0; j<r; ++j, ++i) {
        arr[i].val = 0;
        arr[i].vaddr = i*SIZE;
        arr[i].w = 0;
        arr[i].r = 1;
        arr[i].paddr = -1;
    }
    //init data PTE
    for(int j = 0; j<d; ++j, ++i) {
        arr[i].val = 0;
        arr[i].vaddr = i*SIZE;
        arr[i].w = 1;
        arr[i].r = 1;
        arr[i].paddr = -1;
    }
    //init bss PTE
    for(int j = 0; j<t; ++j, ++i) {
        arr[i].val = 0;
        arr[i].vaddr = i*SIZE;
        arr[i].w = 1;
        arr[i].r = 1;
        arr[i].paddr = -1;
    }
}

PTE *parse(FILE *stream, Pages *pages) {
    initPagesSize(pages, 0, 0, 0, 0); //initialize page sizes to 0 to get sizes right
    char buf[LINE_MAX];
    int c = 0;
    memset(buf, 0, sizeof(buf));
    // count the sizes of data, rodata and bss
    while((c = getc(stream)) != EOF) {
        ungetc(c, stream);
        memset(buf, 0, strlen(buf));
        if(fgets(buf, LINE_MAX, stream) == NULL) {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
        if(strlen(buf) == 0 || strlen(buf) == 1) {
            continue;
        } else if(strstr(buf, "#define") != NULL || strstr(buf,"#include") != NULL || strchr(buf, (int)'(') != NULL) {
            continue;
        } else if (strstr(buf, "const") != NULL && (strstr(buf, "int") != NULL || strstr(buf, "float") != NULL)) {
            pages->rodata.size += 4;
        } else if (strstr(buf, "const") != NULL && strstr(buf, "char") != NULL) {
            pages->rodata.size += 1;
        } else if (strstr(buf, "const") != NULL && strstr(buf, "double") != NULL) {
            pages->rodata.size += 8;
        } else if (strstr(buf, "static") != NULL && (strstr(buf, "= 0") != NULL || strchr(buf, (int)'=') == NULL) && (strstr(buf, "int") != NULL || strstr(buf, "float") != NULL)) {
            pages->bss.size += 4;
        } else if (strstr(buf, "static") != NULL && (strstr(buf, "= 0") != NULL || strchr(buf, (int)'=') == NULL) && strstr(buf, "char") != NULL) {
            pages->bss.size += 1;
        } else if (strstr(buf, "static") != NULL && (strstr(buf, "= 0") != NULL || strchr(buf, (int)'=') == NULL) && strstr(buf, "double") != NULL) {
            pages->bss.size += 8;
        }   else if (strstr(buf, "static") != NULL && strchr(buf, (int)'=') != NULL && (strstr(buf, "int") != NULL || strstr(buf, "float") != NULL)) {
            pages->data.size += 4;
        } else if (strstr(buf, "static") != NULL && strchr(buf, (int)'=') != NULL && strstr(buf, "char") != NULL) {
            pages->data.size += 1;
        } else if (strstr(buf, "static") != NULL && strchr(buf, (int)'=') != NULL && strstr(buf, "double") != NULL) {
            pages->data.size += 8;
        } else if(strstr(buf, "static") == NULL && strstr(buf, "const") == NULL && strchr(buf, (int)'=') == NULL && (strstr(buf, "int") != NULL ||strstr(buf, "float") != NULL)) { 
            pages->bss.size += 4;
        } else if(strstr(buf, "static") == NULL && strstr(buf, "const") == NULL && strchr(buf, (int)'=') == NULL && (strstr(buf, "int") != NULL ||strstr(buf, "char") != NULL)) { 
            pages->bss.size += 1;
        } else if(strstr(buf, "static") == NULL && strstr(buf, "const") == NULL && strchr(buf, (int)'=') == NULL && (strstr(buf, "int") != NULL ||strstr(buf, "double") != NULL)) { 
            pages->bss.size += 8;
        } else if(strstr(buf, "static") == NULL && strstr(buf, "const") == NULL && strchr(buf, (int)'=') != NULL && (strstr(buf, "int") != NULL ||strstr(buf, "float") != NULL)) { 
            pages->data.size += 4;
        } else if(strstr(buf, "static") == NULL && strstr(buf, "const") == NULL && strchr(buf, (int)'=') != NULL && (strstr(buf, "int") != NULL ||strstr(buf, "char") != NULL)) { 
            pages->data.size += 1;
        } else if(strstr(buf, "static") == NULL && strstr(buf, "const") == NULL && strchr(buf, (int)'=') != NULL && (strstr(buf, "int") != NULL ||strstr(buf, "double") != NULL)) { 
            pages->data.size += 8;
        }
       
    }
    // count the size of text
    rewind(stream);
    memset(buf, 0, sizeof(buf));
    while((c = fgetc(stream)) != EOF && strchr(buf, (int)'{') == NULL) {
        ungetc(c, stream);
        memset(buf, 0, strlen(buf));
        if(fgets(buf, LINE_MAX, stream) == NULL) {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
    }
    ungetc(c, stream);
    while((c = fgetc(stream)) != EOF) {
        ungetc(c, stream);
        if(fgets(buf, LINE_MAX, stream) == NULL) {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
        if(strchr(buf, (int)';') != NULL) {
            pages->text.size += 32;
        }
        memset(buf, 0, strlen(buf));
    }
    
    int textSize = pages->text.size;
    int rodataSize = pages->rodata.size;
    int dataSize = pages->data.size;
    int bssSize = pages->bss.size;

    int t = 0, r = 0, d = 0, b = 0;  // the count of each section page
    if(pages->text.size%SIZE) {
        t = pages->text.size/SIZE +1;
    } else {
        t = pages->text.size/SIZE;
    }
    if(pages->rodata.size%SIZE) {
        r = pages->rodata.size/SIZE +1;
    } else {
        r = pages->rodata.size/SIZE;
    }
    if(pages->data.size%SIZE) {
        d = pages->data.size/SIZE +1;
    } else {
        d = pages->data.size/SIZE;
    }
    if(pages->bss.size%SIZE) {
        b = pages->bss.size/SIZE +1;
    } else {
        b = pages->bss.size/SIZE;
    }
    initPagesSize(pages, t, r , d, b);
    pages->text.size = textSize;
    pages->rodata.size = rodataSize;
    pages->data.size = dataSize;
    pages->bss.size = bssSize;
    
    count = t+r+d+b;
    PTE *arr = (PTE*)malloc(sizeof(PTE)*count);
    if(arr == NULL) {
        perror("Not enought memory");
        exit(EXIT_FAILURE);
    }
    
    initPTE(arr, t, r, d, b);

    char bufer[1024];
    memset(buf, 0, 1024);
    printf("Page table entries\n");
    for(int i = 0; i<count; ++i) {
        sprintf(bufer, "VA->%.10x. PA->%.10x. w-%d. r-%d. val-%d.\n", arr[i].vaddr, arr[i].paddr, arr[i].w, arr[i].r, arr[i].val);
        printf("%s", bufer);
        memset(buf, 0, strlen(buf));
    }
    return arr;
}




bool isSeg(Pages *pages, int addr) {
    if((addr >= pages->text.start && addr <= pages->text.end) || (addr >= pages->rodata.start && addr <= pages->rodata.end) || (addr >= pages->data.start && addr <= pages->data.end) || (addr >= pages->bss.start && addr <= pages->bss.end)) {
        return true;
    }
    return false;
}

int checkVal(int addr, PTE *arr) {
    for(int i = 0; i<count; ++i) {
        if(addr >= arr[i].vaddr && addr <= arr[i].vaddr+SIZE && arr[i].val == 0) {
            printf("page fault.\n");
            arr[i].val = 1;
            return 0;
        } else if(addr >= arr[i].vaddr && addr <= arr[i].vaddr+SIZE && arr[i].val == 1) {
            arr[i].paddr = ram + addr%arr[i].vaddr;
            printf("Physical addres: %x.\n", arr[i].paddr);
            ram+=SIZE;
            return 0;
        }
    }

}
 