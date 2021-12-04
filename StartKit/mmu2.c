#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <math.h>
#include "mmu.h"

static int PHASE_INDICATOR;
static struct AddressTable addresses[FRAME_SIZE];
static struct PageTable pageTable[FRAME_SIZE];
static int AddressTable_SIZE;
static int PageTable_SIZE;
static struct Memory memory[FRAME_SIZE]; 
static int Memory_SIZE;
static struct TLB tlb [TLB_SIZE]; // TLB = [Page Number][Frame Number] 
static int tlb_SIZE;

// Statistic Variables
static int TLB_MISS;
static int TLB_HIT;
static int PageTable_MISS;
static int PageTable_HIT;




int main(int argc, char *argv[]) {
    
    FILE *addressFile;
    FILE *backingStore;
    //FILE *outputFile;
    

    addressFile = fopen(argv[3],"r");
    backingStore = fopen(argv[2], "rb"); 
    //outputFile = fopen("output.csv", "w");
    PHASE_INDICATOR = atoi(argv[1]);


    innit_Memory(PHASE_INDICATOR);
    innit_PageTable(PHASE_INDICATOR);
    innit_TLB();
    innit_AddressTable(PHASE_INDICATOR);
    //char frame[PHASE_INDICATOR];
    char address [LOGICAL_ADDRESS_BITS] = {'\0'};
    //char backingStoreFrame[FRAME_SIZE];

    int idx = 0;
    while (fgets(address,LOGICAL_ADDRESS_BITS, addressFile) != NULL) {
        int tempAddress = atoi(address);
        printf("%d\n", tempAddress);
        AddressTableAdd(tempAddress);
        //PageTableAdd(tempAddress);
    }

    printf("---------------------------\n");
    printf("Size: %d\n", PageTable_SIZE);

    for(int i = 0; i < PHASE_INDICATOR; i++){

        // 1. Search TLB
        // 2. IF TLB miss, ask PAGETABLE
        // 3. Print result
        int tempFrame = TLBGetFrame(addresses[i].page);
        if(tempFrame > -1){
            TLB_HIT++;
            for(int j = 0; j < PageTable_SIZE; j++){
                if(addresses[i].page == pageTable[j].p){
                    pageTable[j].lastUse = i + 1;
                }
            }
        }
        else { // TBL miss, use search table
            TLB_MISS++;
            // Search page table
            int idx = PageTableGetIdx(addresses[i].page);
            
            if(idx > -1) { // Data in page table, so now add it to TLB.
                if (tlb_SIZE >= TLB_SIZE){ // TLB ADD CASE 1
                    //remove TLB FIFO 
                    for (int l = 1; l < TLB_SIZE; l++){
                        tlb[l-1].frameNumber = tlb[l].frameNumber;
                        tlb[l-1].pageNumber = tlb[l].pageNumber;
                        if(l == TLB_SIZE - 1){ // add entry at the end
                            tlb[l].frameNumber = idx;
                            tlb[l].pageNumber = addresses[i].page;
                        }
                    }
                }
                else{   //TLB ADD CASE 2
                    tlb[tlb_SIZE].frameNumber = idx;
                    tlb[tlb_SIZE].pageNumber = addresses[i].page;
                    tlb_SIZE++;
                }
                for(int j = 0; j < PageTable_SIZE; j++){
                    if(addresses[i].page == pageTable[j].p){
                        pageTable[j].lastUse = i + 1;
                    }
                }
                PageTable_HIT++;

            }
            else { // Data not in page table, need to ask BACKING STORE for Frame/data. PAGEFAULT
                // fetch data from backing store and add it to page table
                // load back -> add page table


                //1. get data from backing store
                signed char data[FRAME_SIZE];
                fseek(backingStore, addresses[i].page * FRAME_SIZE, SEEK_SET);
                fread(data, sizeof(char), FRAME_SIZE, backingStore);

                //2. add data into the page table
                //tempFrame = 
                int tempidx = PageTableGetIdx(addresses[i].page);

                if(PageTable_SIZE < PHASE_INDICATOR){
                    // insert page
                    pageTable[PageTable_SIZE].p = addresses[i].page;
                    pageTable[PageTable_SIZE].lastUse = i + 1;
                    strncpy(memory[PageTable_SIZE].data, data, FRAME_SIZE);
                    PageTable_SIZE++;

                }
                else{ // swap least recently used value
                    int m = 100000;
                    int lastMin = 100000;
                    for(int r = 0; r < PHASE_INDICATOR; r++){
                        if(m >= pageTable[r].lastUse){
                            m = pageTable[r].lastUse;
                            lastMin = r;
                        }
                    }
                    pageTable[lastMin].p = addresses[i].page;
                    pageTable[lastMin].lastUse = i + 1;
                    strncpy(memory[lastMin].data, data, FRAME_SIZE);

                    //  now update tlb

                    


                    
                }
                



                PageTable_MISS++;
            }




        }
        printf("Index: %d, address: %d, Page: %d, Offset: %d\n", i,  pageTable[i].virtualAddress, pageTable[i].p, pageTable[i].d);
    }

    fclose(addressFile);
    //fclose(backingStore);
    //fclose(outputFile);
    
    return 0;
}


static void innit_Memory(int frames) {
    for(int i = 0; i < frames; i++){
        strncpy(memory[i].data, "NULL", FRAME_SIZE);
    }
}

static void innit_PageTable(int frames) {
    for(int i = 0; i < frames; i++){
        pageTable[i].d = -1; 
        pageTable[i].p = -1; 
        pageTable[i].virtualAddress = -1;
        pageTable[i].physicalAddress = -1;
        pageTable[i].presence = 0;
        pageTable[i].reference = 0;
    }
}

static void innit_AddressTable(int frames) {
    for(int i = 0; i < frames; i++){
        addresses[i].offset = -1; 
        addresses[i].page = -1; 
        addresses[i].virtualAddress = -1;
    }
}

static void innit_TLB() {
    for(int i = 0; i < TLB_SIZE; i++){
        tlb[i].frameNumber = -1;
        tlb[i].pageNumber = -1;
    } 
}

int PageTableAdd(int tempAddress){
    //if(PageTableGet(tempAddress) == -1){
    //    return 1; // element already exits
    //}
    //else{
    pageTable[PageTable_SIZE].virtualAddress = tempAddress;
    pageTable[PageTable_SIZE].p = (tempAddress & 65535) >> 8;
    pageTable[PageTable_SIZE].d = tempAddress & 255;
    PageTable_SIZE++;
    return 1; //successful add
    //}
}

int PageTableGetIdx(int address){
    for(int i = 0; i < PageTable_SIZE; i++){
        if(pageTable[i].p == address){
            return i;
        }
    }
    return -1;
}

int TLBGetFrame(int pageNum){
    for(int i = 0; i < TLB_SIZE; i++){
        if(tlb[i].pageNumber == pageNum){
            return tlb[i].frameNumber;
        }
    }
    return -1;
}

int TLBAddFrame(int idx, int pageNumber, int frameNumber){
    if (tlb_SIZE >= TLB_SIZE){ // TLB ADD CASE 1
        //remove TLB FIFO 
        for (int l = 1; l < TLB_SIZE; l++){
            tlb[l-1].frameNumber = tlb[l].frameNumber;
            tlb[l-1].pageNumber = tlb[l].pageNumber;
            if(l == TLB_SIZE - 1){ // add entry at the end
                tlb[l].frameNumber = idx;
                tlb[l].pageNumber = pageNumber;
            }
        }
    }
    else{   //TLB ADD CASE 2
        tlb[tlb_SIZE].frameNumber = idx;
        tlb[tlb_SIZE].pageNumber = addresses[i].page;
        tlb_SIZE++;
    }
    return 1;
}


int AddressTableAdd(int tempAddress){
    //if(PageTableGet(tempAddress) == -1){
    //    return 1; // element already exits
    //}
    //else{
        if (AddressTable_SIZE == PHASE_INDICATOR){
            return -1; // Table is full
        }
        else{
            addresses[AddressTable_SIZE].virtualAddress = tempAddress;
            addresses[AddressTable_SIZE].page = (tempAddress & 65535) >> 8;
            addresses[AddressTable_SIZE].offset = tempAddress & 255; 
            AddressTable_SIZE++;
            return 1; //successful add
        }
    //}
}
void dectobin(int n){
    int c,k;
    for (c = 15; c >= 0; c--){ // c value was originally 31, but we will be using 15, 2^16 = 65536, 8+8 bits.
        k = n >> c;

        if (k & 1)
            printf("1");
        else
            printf("0");
    }
}

void getp(int n){
    int c,k;
    char str[32] = {'\0'};
    char p[9] = {'\0'};
    for (c = 31; c >= 0; c--){ // c value was originally 31, but we will be using 15, 2^16 = 65536, 8+8 bits.
        k = n >> c;

        if (k & 1){
            str[c] = '1';
            //printf("1");
        }
        else {
            str[c] = '0';
            //printf("0");
        }
    }
    int j = 0;
    for(c = 15; c >= 8; c--){
        p[j] = str[c];
        j++;
    }

    printf("dec: %d, bin: %s", atoi(p), p);
    
}

void getd(int n){
    int c,k;
    char str[32] = {'\0'};
    char p[9] = {'\0'};
    for (c = 31; c >= 0; c--){ // c value was originally 31, but we will be using 15, 2^16 = 65536, 8+8 bits.
        k = n >> c;

        if (k & 1){
            str[c] = '1';
            //printf("1");
        }
        else {
            str[c] = '0';
            //printf("0");
        }
    }
    int j = 0;
    for(c = 7; c >=0; c--){
        p[j] = str[c];
        j++;
    }

    printf("dec: %d, bin: %s", atoi(p), p);
}