#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "defines.h"
#include "rle.h"

typedef struct HuffmanCodeStr
{
    RLE    rle;
    DWORD  codeword;
    int    length;

} HuffmanCode;


int Huffman(RLE *rle, HuffmanCode *hfcode, int len);
int Huffman_Ad_Hoc(RLE *rle_array, HuffmanCode *hfcode, int len, int L, int order);
int Huffman_Constrained(RLE *rle_array, HuffmanCode *hfcode, int *len, int L);

#endif /* HUFFMAN_H_ */
