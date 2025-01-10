#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "myLib.h"


int main(int argc, char *argv[]) {
    if(argc < 2) {
        perror("insufficient number of arguments");
        exit(EXIT_FAILURE);
    }

//open the source file
    FILE *stream = fopen(argv[1], "r");
    if(stream == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
//

    Pages pages;
    PTE *arrPTE = parse(stream, &pages); //return the array of PTE

    // print sizes of pages
    printf("The size of each page\n");
    printf("The size of text section is: %.20d\n", pages.text.size);
    printf("The size of rodata section is: %.20d\n", pages.rodata.size);
    printf("The size of data section is: %.20d\n", pages.data.size);
    printf("The size of bss section is: %.20d\n", pages.bss.size);
    //

    
    // Enter addresses 
    int addr = 0;
    printf("Enter virtual address to access. start-1000: Enter 'q' for exit.\n");
    while(scanf("%d", &addr) == 1) {
        if(!isSeg(&pages, addr)) {
            printf("Segmentation fault.\n");
        }   else {
            checkVal(addr, arrPTE);
        }   
    
    }
}