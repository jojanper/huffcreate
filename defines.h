#ifndef DEFINES_H_
#define DEFINES_H_

typedef unsigned char BYTE;
typedef unsigned long DWORD;

#define FLAG_SET 1
#define LEFT_OR_RIGHT 2

typedef enum HUFFMAN_METHOD
{
    HUFFMAN_BASIC,
    HUFFMAN_CONSTRAINED,
    HUFFMAN_AD_HOC,
    HUFFMAN_AD_HOC_ORDERED,
    HUFFMAN_VOORHIS

} HUFFMAN_METHOD;

#endif /* DEFINES_H_ */
