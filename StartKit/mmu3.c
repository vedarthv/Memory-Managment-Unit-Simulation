#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <math.h>

int bin_to_dec(char * bin){
    int i = 7;
    int ans = 0;
    while(i >=0){
        ans += bin[i] + pow(2, i);
        i--;
    }
    return ans;
}

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

char * decToBinary(int n)
{
    // array to store binary number
    int binaryNum[32];
 
    // counter for binary array
    int i = 0;
    while (n > 0) {
 
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
 
    // printing binary array in reverse order
    char str[128] = {0};
    int index = 0;
    for(i=0; i<5; i++){
        index += sprintf(&str[index],"%d", binaryNum[i]);
    }
    return str;    
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

    printf("bin-p: %d", atoi(p));
    
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

    printf("bin-d: %d", atoi(p));
}


int main(int argc, char *argv[]) {
    int arr[7] = {16916,62493,32768,32769,128,65534,33153};
    int i = 0;
    while(i < 7){
        int p= 0;
        int d = 0;
        unsigned int bitmaskp;
        unsigned int bitmaskd;
        bitmaskp = ((1 << 8) - 1) << 15;
        bitmaskd = ((1 << 8) - 1) << 7;
        p = arr[i] & bitmaskp;
        d = arr[i] & bitmaskd;
        //printf("int: %d,", arr[i]); printf(" bit: "); 
        printf("int: %d, bin: ", arr[i]); dectobin(arr[i]);
        printf(", p: "); dectobin(p); getp(arr[i]); printf(", d: "); dectobin(d); getd(arr[i]); printf("\n");
        //printf(", p: %s, d: %s\n", byte_to_binary(p), byte_to_binary(d));
        //printf("int: %d, bin: %s, p: %s, d: %s\n", arr[i], decToBinary(arr[i]), byte_to_binary(p), byte_to_binary(d));
        i++;

    }

    return 0;
}

