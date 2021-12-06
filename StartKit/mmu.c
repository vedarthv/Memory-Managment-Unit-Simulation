#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include "mmu.h"

static int PHASE_INDICATOR;
static struct AddressTable addresses[ADDRESS_TABLE_SIZE];
static struct PageTable pageTable[FRAME_SIZE];
static int AddressTable_SIZE;
static int PageTable_SIZE;
static struct Memory memory[FRAME_SIZE]; 
static int Memory_SIZE;
static struct TLB tlb [TLB_SIZE]; // TLB = [Page Number][Frame Number] 
static int tlb_SIZE;

// Statistic Variables
static int TLB_HIT;
static int PageTable_MISS;


int main(int argc, char *argv[]) {
    
    FILE *addressFile;
    FILE *backingStore;
    FILE *outputFile;
    
    PHASE_INDICATOR = atoi(argv[1]);
    addressFile = fopen(argv[3],"r");
    backingStore = fopen(argv[2], "rb"); 
    char outputName[14];
    snprintf(outputName, sizeof(outputName), "output%d.csv", PHASE_INDICATOR);
    outputFile = fopen(outputName, "w");
    


    innit_Memory(PHASE_INDICATOR);
    innit_PageTable(PHASE_INDICATOR);
    innit_TLB();
    innit_AddressTable(PHASE_INDICATOR);
    //char frame[PHASE_INDICATOR];
    char address [LOGICAL_ADDRESS_BITS] = {'\0'};
    //char backingStoreFrame[FRAME_SIZE];

    
    while (fgets(address,LOGICAL_ADDRESS_BITS, addressFile) != NULL) {
        int tempAddress = atoi(address);
        AddressTableAdd(tempAddress);
        //PageTableAdd(tempAddress);
    }

    for(int i = 0; i < AddressTable_SIZE; i++){
        // 1. Search TLB
        // 2. IF TLB miss, ask PAGETABLE
        // 3. Print result
        int tempFrame = TLBGetFrame(addresses[i].page);
        int outF = tempFrame;
        if(tempFrame > -1){
            TLB_HIT++;
            for(int j = 0; j < PageTable_SIZE; j++){
                if(addresses[i].page == pageTable[j].p){
                    pageTable[j].lastUse = i + 1;
                }
            }
        }
        else { // TBL miss, use search table
            // Search page table
            int idx = PageTableGetFrame(addresses[i].page);
            outF = idx;
            if(idx > -1) { // Data in page table, so now add it to TLB.
                TLBAdd(addresses[i].page, idx);
                for(int j = 0; j < PageTable_SIZE; j++){
                    if(addresses[i].page == pageTable[j].p){
                        pageTable[j].lastUse = i + 1;
                    }
                }
            }
            else { // Data not in page table, need to ask BACKING STORE for Frame/data. PAGEFAULT
                // fetch data from backing store and add it to page table
                // load back -> add page table
                PageTable_MISS++;
                //1. get data from backing store
                signed char data[FRAME_SIZE];
                fseek(backingStore, addresses[i].page * FRAME_SIZE, SEEK_SET);
                fread(data, sizeof(signed char), FRAME_SIZE, backingStore);
                //2. add data into the page table
                // PAGE TABLE ADD and also adds to TLB
                outF = PageTableAdd(addresses[i].page, addresses[i].offset, i, data);
            }
        }
        fprintf(outputFile, "%d,%d,%d\n", addresses[i].virtualAddress, outF * FRAME_SIZE + addresses[i].offset, memory[outF].data[addresses[i].offset]);
    }
    fprintf(outputFile, "Page Faults Rate, %0.2f%%,\n", ((double)(PageTable_MISS * 1.0) / (double)(AddressTable_SIZE * 1.0)) * 100.0);
    fprintf(outputFile, "TLB Hits Rate, %0.2f%%,", ((double) TLB_HIT * 1.0 /  (double) AddressTable_SIZE * 1.0) * 100.0);

    fclose(addressFile);
    fclose(backingStore);
    fclose(outputFile);
    
    return 0;
}


void innit_Memory(int frames) {
    for(int i = 0; i < frames; i++){
        for(int j = 0; j < FRAME_SIZE; j++){
            memory[i].data[j] = '\0';
        }
    }
}

void innit_PageTable(int frames) {
    for(int i = 0; i < frames; i++){
        pageTable[i].d = -1; 
        pageTable[i].p = -1; 
        pageTable[i].virtualAddress = -1;
        pageTable[i].physicalAddress = -1;
        pageTable[i].presence = -1;
        pageTable[i].reference = -1;
    }
}

void innit_AddressTable(int frames) {
    for(int i = 0; i < PHASE_INDICATOR; i++){
        addresses[i].offset = -1; 
        addresses[i].page = -1; 
        addresses[i].virtualAddress = -1;
    }
}

void innit_TLB() {
    for(int i = 0; i < TLB_SIZE; i++){
        tlb[i].frameNumber = -1;
        tlb[i].pageNumber = -1;
    } 
}

int PageTableAdd(int pageNumber, int offset, int count, signed char diskData[]){
    if(PageTableGetFrame(pageNumber) != -1) { // add case 1, pg already has the data
        return PageTableGetFrame(pageNumber);
    }
    int out = PageTable_SIZE;
    if(PageTable_SIZE < PHASE_INDICATOR){  // add case 2, pg is not full
        // insert page
        pageTable[PageTable_SIZE].p = pageNumber;
        pageTable[PageTable_SIZE].d = offset;
        pageTable[PageTable_SIZE].lastUse = count + 1;
        for(int stridx = 0; stridx < FRAME_SIZE; stridx++){
            memory[PageTable_SIZE].data[stridx] = diskData[stridx];
        }
    
        TLBAdd(pageNumber, PageTable_SIZE);
        PageTable_SIZE++;
        Memory_SIZE++;

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
        pageTable[lastMin].p = pageNumber;
        pageTable[lastMin].d = offset;
        pageTable[lastMin].lastUse = count + 1;
        
        for (int j = 0; j < FRAME_SIZE; j++){
            memory[lastMin].data[j] = diskData[j];
        }
        //  now update tlb
        out = lastMin;
        TLBAdd(pageNumber, lastMin);
    } 
    return out;
}

int PageTableGetFrame(int address){
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

int TLBAdd(int pageNum, int frameNum){
   
    if (TLBGetFrame(pageNum) != -1){ // TLB ADD CASE, checking if duplicate
        return TLBGetFrame(pageNum);
    }
    
    if (tlb_SIZE >= TLB_SIZE){ // TLB ADD CASE 2
        //remove TLB FIFO 
        for (int l = 1; l < TLB_SIZE; l++){
            tlb[l-1].frameNumber = tlb[l].frameNumber;
            tlb[l-1].pageNumber = tlb[l].pageNumber;
            if(l == TLB_SIZE - 1){ // add entry at the end
                tlb[l].frameNumber = frameNum;
                tlb[l].pageNumber = pageNum;
                return frameNum;
            }      
        }
    }
    else{   //TLB ADD CASE 3
        tlb[tlb_SIZE].frameNumber = frameNum;
        tlb[tlb_SIZE].pageNumber = pageNum;
        tlb_SIZE++;
        return frameNum;
    }
    return -1;
    
}


int AddressTableAdd(int tempAddress){
    addresses[AddressTable_SIZE].virtualAddress = tempAddress;
    addresses[AddressTable_SIZE].page = (tempAddress & 65535) >> 8;
    addresses[AddressTable_SIZE].offset = tempAddress & 255; 
    AddressTable_SIZE++;
    return 1; 
}

void dectobin(int n){
    int c,k;
    for (c = 15; c >= 0; c--){ // c value was originally 31, but we will be using 15, 2^16 = 65536, 8+8 bits.
        k = n >> c;

        if (k & 1){
            printf("1");
        }
        else{
             printf("0");
        }
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