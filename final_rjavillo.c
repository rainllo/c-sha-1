/*
 * final_rjavillo.c
 * Final project is to implement the Secure Hash Algorithm (SHA-1) in C
 * Rainier Javillo
 * 5/13/2021
 * External sources used: https://en.wikipedia.org/wiki/SHA-1 for pseudocode to create computeMessageDigest() function
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "printbits.h"              // import printbits.h for debugging
#define MAX_SIZE 1048576            // max size of array

// function prototypes
unsigned int readFile(unsigned char[]);
unsigned int calculateBlocks(unsigned int);
void convertCharArrayToIntArray(unsigned char[], unsigned int[], unsigned int);
void addBitCountToLastBlock(unsigned int[], unsigned int, unsigned int);
void computeMessageDigest(unsigned int[], unsigned int);
unsigned int f(unsigned int, unsigned int, unsigned int, unsigned int);
unsigned int K(unsigned int) ;
unsigned int charPacker(unsigned char, unsigned char, unsigned char, unsigned char);
unsigned int circleLeft(unsigned int, unsigned int);

// main function
int main(int argc, char *argv[]){
    // declare and initialize variables
    unsigned char buffer[MAX_SIZE] = {'\0'};
    unsigned int size = 0;
    unsigned int blocks = 0;
    unsigned int message[MAX_SIZE] = {0};
       
    // call the functions
    size = readFile(buffer);
    blocks = calculateBlocks(size);    
    convertCharArrayToIntArray(buffer, message, size);
    addBitCountToLastBlock(message, size, blocks);
    computeMessageDigest(message, blocks);
    
    return 0;
}

// function definitions
/*
 * readFile()
 * takes in contents of file stored in buffer[] array
 * returns the number of characters in the file (in bytes) 
 */
unsigned int readFile(unsigned char buffer[]){
    char letter = 0;
    unsigned int count = 0;

    letter = getchar();                 // get first character
    // count the characters from standard input until the end of file
    while(letter != EOF){
        buffer[count++] = letter;       // buffer array stores the characters, increment count to go to next
        letter = getchar();             // get another character       
        // error checking to stop the program if input file is too big
        if (count > MAX_SIZE){
            printf("ERROR: The input file is too big!\n");
            exit(1);                    // end the program
        }
    }
    buffer[count] = 0x80;               // append 0x80 to last character in buffer array
    
    // TEST readFile()
    //printf("TEST: readFile() buffer[]:\n");
    //printbits(buffer[0]);
    //printbits(buffer[1]);
    //printbits(buffer[2]);
    //printbits(buffer[3]);
    //printf("TEST: readFile() count = %i\n", count);
    
    return count;
}

/*
 * calculateBlocks()
 * takes in the number of characters in the file
 * returns the block count 
 */
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes){
    unsigned int blockCount = 0;

    blockCount = (((8 * sizeOfFileInBytes) + 1) / 512) + 1;         // calculate the block count
    if((((8 * sizeOfFileInBytes) + 1) % 512) > (512 - 64)){         // if statement will determine if an extra block needs to be added or not:
        blockCount = blockCount + 1;
    }

    return blockCount;
}

/*
 * convertCharArrayToIntArray()
 * converts array of unsigned characters into an equivalent array of unsigned integers
 */
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes){
    int i = 0;
    int numBlocks = calculateBlocks(sizeOfFileInBytes);     // find how many blocks to process
    unsigned char temp[4] = {'\0'};                         // temp array used to store four chars for charPackers
    
    // for loop packs puts four chars into each unsigned int array index, use charPacker() function
    for (i = 0; i < (16*numBlocks); i++){                   // multiply numBlocks by 16 since there's 16 integers
        temp[0] = buffer[i * 4];                            // 0, 4, etc
        temp[1] = buffer[(i * 4) + 1];                      // 1, 5, etc
        temp[2] = buffer[(i * 4) + 2];                      // 2, 6, etc
        temp[3] = buffer[(i * 4) + 3];                      // 3, 7, etc
        message[i] = charPacker(temp[0], temp[1], temp[2], temp[3]);
        
        // TEST convertCharArrayToIntArray
        //printbits(message[i]);
    }   
}

/*
 * addBitCountToLastBlock()
 * insert the size of the file in bits into the last index of the last block
 */
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount){    
    int sizeOfFileInBits = sizeOfFileInBytes * 8;           // calculate size of file in bits (1 byte = 8 bits)
    int indexOfEndOfLastBlock = (blockCount * 16) - 1;      // find the index of the last word
    message[indexOfEndOfLastBlock] = sizeOfFileInBits;      // insert size of file in bits to last index of message array

    // TEST addBitCountToLastBlock()
    //for (int i = 0; i < (16*blockCount); i++){
    //    printbits(message[i]);
    //}
}

/*
 * computeMessageDigest()
 * final step is to compute the message digest
 * Help from https://en.wikipedia.org/wiki/SHA-1 for pseudocode
 */
void computeMessageDigest(unsigned int message[], unsigned int blockCount){
    // initialize variables
    unsigned int H0 = 0x67452301;
    unsigned int H1 = 0xEFCDAB89;
    unsigned int H2 = 0x98BADCFE;
    unsigned int H3 = 0x10325476;
    unsigned int H4 = 0xC3D2E1F0;
    unsigned int W[80] = {0};
    unsigned int tmp = 0;
    int i = 0;
    int j = 0; 
    int l = 0;

    // for each chunk
    for (i = 0; i < blockCount; i++){
        for (j = 0; j < 80; j++){
            // divide message[] into 16 words W[0] to W[15]
            if (j < 16){        
                W[j] = message[j + (16 * i)];
                // PRINT: hex of words of the block. Only if block size <= 2
                if (blockCount <= 2){
                    printf("W[%d] = %.8X\n", j, W[j]);
                }
            } else {        //extend the sixteen 32-bit words into eighty 32-bit words:
                W[j] = circleLeft(1, (W[j-3] ^ W[j-8] ^ W[j-14] ^ W[j-16]));
            }
        }
        // initialize hash values for this chunk
        unsigned int A = H0;
        unsigned int B = H1;
        unsigned int C = H2;
        unsigned int D = H3;
        unsigned int E = H4;
        
        // main loop 
        for (l = 0; l < 80; l++){
            tmp = circleLeft(5, A) + f(l, B, C, D) + E + K(l) + W[l];
            E = D;
            D = C;
            C = circleLeft(30, B);
            B = A;
            A = tmp;
            
            // PRINT: hex for A, B, C, D and E from t = 0 to 79. Only if block size <= 2
            if (blockCount <= 2){
                printf("t = %2i: %.8X   %.8X   %.8X   %.8X   %.8X\n", l, A, B, C, D, E);
            }
        }
        if (blockCount <= 2){
            // PRINT: hex for H_i and A_i and their sum. Only if block size <= 2
            printf("H_i:   %.8X %.8X %.8X %.8X %.8X\n", H0, H1, H2, H3, H4);
            printf("A_i: + %.8X %.8X %.8X %.8X %.8X\n", A, B, C, D, E);
            printf("     = %.8X %.8X %.8X %.8X %.8X\n", H0+A, H1+B, H2+C, H3+D, H4+E);
        }
        // add this chunk's hash to the result so far
        H0 += A;
        H1 += B;
        H2 += C;
        H3 += D;
        H4 += E;
    }
    // print out the message digest
    printf("Message digest = %.8X %.8X %.8X %.8X %.8X\n", H0, H1, H2, H3, H4);
}

// helper functions f() and K()
unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D){
    unsigned int F = 0;
    if (0<=t && t<=19){
        F = ((B & C) | ((~B) & D));
    } else if (20<=t && t<=39){
        F = (B ^ C ^ D);
    } else if (40<=t && t<=59){
        F = ((B & C) | (B & D) | (C & D));
    } else if (60<=t && t<=79){
        F = (B ^ C ^ D);
    }

    return F;
}

unsigned int K(unsigned int t){
    unsigned int k = 0;
    if (0<=t && t<=19){
        k = 0x5A827999;
    } else if (20<=t && t<=39){
        k = 0x6ED9EBA1;
    } else if (40<=t && t<=59){
        k = 0x8F1BBCDC;
    } else if (60<=t && t<=79){
        k = 0xCA62C1D6;
    }

    return k;
}

// functions from Bitwisdom homework
/*
 * charPacker()
 * packs four chars into one int
 */
unsigned int charPacker(unsigned char letter1, unsigned char letter2, unsigned char letter3, unsigned char letter4){
    unsigned int charpack = 0;    
    charpack = charpack | letter4;                                  // left shift operator (<<) moves bits into correct place
    charpack = charpack | (letter3 << 8);                           // use 8, 16 and 24 since each char is 8 bits in binary
    charpack = charpack | (letter2 << 16);                          // inclusive OR (|) packs them into one 32bit long integer
    charpack = charpack | (letter1 << 24);

    return charpack;                                                // return the packed characters
}

/*
 * circleLeft()
 * performs a circular left shift of bits
 * first integer is number of bits to shift left by
 * second integer is number to be shifted
 */
unsigned int circleLeft(unsigned int number1, unsigned int number2){
    unsigned int shift = 0;
    shift = (number2 << number1) | (number2 >> (32 - number1));

    return shift;
}
 
