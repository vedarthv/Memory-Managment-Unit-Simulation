#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <math.h>
#include "mmu.h"

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

int main(int argc, char *argv[]) {
    FILE *in;
    FILE *backingStore;
    char *temp;
    char frame[FRAME_SIZE];
    char num[128];
    int mem = atoi(argv[1]);
    unsigned char buff[(FRAMES * FRAME_SIZE)];
    //printf("mem: %d\n", mem);
    

    backingStore = fopen(argv[2], "rb"); 

    fread(buff, sizeof(buff), 1, backingStore);

    for(int i = 0; i<(FRAMES * FRAME_SIZE); i++)
        printf("%x\n", buff[i]); 
   
    while (fgets(frame,FRAME_SIZE, backingStore) != NULL) {
    //    //temp = strdup(frame);
    //    //name = strsep(&temp,",");
    //    //priority = atoi(strsep(&temp,","));
    //    //burst = atoi(strsep(&temp,","));

    //    // add the task to the scheduler's list of tasks
        //i = 0;
        //while (i < FRAME_SIZE)
        //{
        //    printf("%x", frame[i]);
        //    i++;
        //}
        //printf("\n");
        //printf("%x\n", frame);
    //    //free(temp);
    }

    fclose(backingStore);

    in = fopen(argv[3],"r");
    while (fgets(num,128, in) != NULL) {
        //temp = strdup(frame);
        //name = strsep(&temp,",");
        //priority = atoi(strsep(&temp,","));
        //burst = atoi(strsep(&temp,","));
        
        //puts(num);
        int number = atoi(num);
        int physicalAdd = 
        // add the task to the scheduler's list of tasks
        printf("%d,", num);
        

        printf("%d", atoi(num));
        //free(temp);
    }

    fclose(in);
    


    // invoke the scheduler
    //schedule();

    return 0;
}

