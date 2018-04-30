#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bintree.h"
#include "defines.h"
#include "huffman.h"
#include "list.h"
#include "rle.h"


#define ASCENDING 1
#define DESCENDING 2


void
Insertion_Sort(RLE *rle, int start, int end)
{
  int  i, j;
  RLE  rle_tmp;

  for(j=start+1; j<end; ++j)
  {
    rle_tmp = rle[j];
    i = j-1;
    while(i >= start && rle[i].prob > rle_tmp.prob)
    {
      rle[i+1] = rle[i];
      i--;
    }
    rle[i+1] = rle_tmp;
  }

}

void
Insertion_Sort2(HuffmanCode *hfcode, int start, int end, int order)
{
  int         i, j;
  HuffmanCode hfcode_tmp;

  for(j=start+1; j<end; ++j)
  {
    hfcode_tmp = hfcode[j];
    i = j-1;

    if(order == ASCENDING)
    {
      while(i >= start && hfcode[i].rle.prob <= hfcode_tmp.rle.prob)
      {
        hfcode[i+1] = hfcode[i];
        i--;
      }
    }
    else /* descending */
      while(i >= start && hfcode[i].rle.prob > hfcode_tmp.rle.prob)
      {
        hfcode[i+1] = hfcode[i];
        i--;
      }

    hfcode[i+1] = hfcode_tmp;
  }

}


void
Insertion_Sort3(HuffmanCode *hfcode, int start, int end)
{
  int    i, j;
  int    key1;
  DWORD  key2;

  for(j=start+1; j<end; ++j)
  {
    key1 = hfcode[j].length;
    key2 = hfcode[j].codeword;
    i = j-1;
    while(i >= start && hfcode[i].length > key1)
    {
      hfcode[i+1].length = hfcode[i].length;
      hfcode[i+1].codeword = hfcode[i].codeword;
      i--;
    }
    hfcode[i+1].length = key1;
    hfcode[i+1].codeword = key2;
  }

}


int Create_Tree_And_Print(ListHead *pH, HuffmanCode *hfcode);


/*
 * Huffman code construction method, that can be used
 * to perform lossless compression.
 */
int
Huffman(RLE *rle_array, HuffmanCode *hfcode, int len)
{
  int        i;
  ListHead   ListHe = InitializeList();


  Insertion_Sort(rle_array, 0, len);

  for(i = 0; i < len; i++)
    Create_Node(&ListHe, &rle_array[len - i - 1]);

  while(ListHe.Total > 2)
    Merge_Nodes(&ListHe);

  Create_Tree_And_Print(&ListHe, hfcode);

  return (0);
}


int
Huffman_Ad_Hoc(RLE *rle_array, HuffmanCode *hfcode, int len, int L, int order)
{
  int        i;
  float      prob_threshold;
  ListHead   ListHe = InitializeList();

  prob_threshold = pow(2, -L);

  for(i = 0; i < len; i++)
    if(rle_array[i].prob <= prob_threshold)
      rle_array[i].prob = prob_threshold;

  Insertion_Sort(rle_array, 0, len);

  for(i = 0; i < len; i++)
    Create_Node(&ListHe, &rle_array[len - i - 1]);

  while(ListHe.Total > 2)
    Merge_Nodes(&ListHe);

  Create_Tree_And_Print(&ListHe, hfcode);

  for(i = 0; i < len; i++)
    hfcode[i].rle.prob = hfcode[i].rle.original_prob;

  if(order)
  {
    Insertion_Sort2(hfcode, 0, len, ASCENDING);
    Insertion_Sort3(hfcode, 0, len);
  }

  return (0);
}


void
Swap_RLE_Array(RLE *dest, RLE *src, int len)
{
  int i;

  for(i = 0; i < len; i++)
    dest[i] = src[len-i-1];
  for(i = 0; i < len; i++)
    src[i] = dest[i];
}


int
Huffman_Constrained(RLE *rle_array, HuffmanCode *hfcode, int *len, int L)
{
  int        i, j, k, m, len2;
  int        codeword, clen;
  float      prob_threshold, sum;
  ListHead   ListHe = InitializeList();
  RLE        *rle;


  rle = (RLE *)calloc(*len + 1, sizeof(RLE));
  if(rle == NULL)
  {
    fprintf(stderr, "out of memory\n");
    exit(EXIT_FAILURE);
  }

  Insertion_Sort(rle_array, 0, *len);

  Swap_RLE_Array(rle, rle_array, *len);

  prob_threshold = pow(2, -L);

  for(i = 0; i < *len; i++)
    if(rle[i].prob < prob_threshold)
    {
      len2 = i + 1;
      rle[i].run = -1;
      rle[i].size = -1;
      rle[i].prob = prob_threshold;
      break;
    }

  sum = 0.0;
  for(i = len2-1; i < *len; i++)
    sum += rle_array[i].prob;

  prob_threshold = sum;
  rle[len2-1].prob = prob_threshold;

  for(i = 0; i < len2; i++)
    Create_Node(&ListHe, &rle[len2 - i - 1]);

  while(ListHe.Total > 2)
    Merge_Nodes(&ListHe);

  Create_Tree_And_Print(&ListHe, hfcode);

  Insertion_Sort2(hfcode, 0, len2, DESCENDING);

  {
    HuffmanCode *hftemp;

    hftemp = (HuffmanCode *)calloc(*len, sizeof(HuffmanCode));
    if(hftemp == NULL)
    {
      fprintf(stderr, "out of memory\n");
      exit(EXIT_FAILURE);
    }

    for(i = 0; i < len2; i++)
      hftemp[i] = hfcode[len2-i-1];
    for(i = 0; i < len2; i++)
      hfcode[i] = hftemp[i];

    k = 1;
    j = 0;
    for(i = 0; i < len2; i++)
    {
      hftemp[j++] = hfcode[i];
      if(hfcode[i].rle.prob == prob_threshold)
      {
        codeword = hfcode[i].codeword;
        clen = hfcode[i].length;
        j--;
        for(m = len2-1; m < *len; m++)
        {
          hftemp[j].rle.run = rle_array[m].run;
          hftemp[j].rle.size = rle_array[m].size;
          hftemp[j].rle.prob = rle_array[m].prob;
          hftemp[j].length =  clen + L;
          hftemp[j].codeword = codeword;
          hftemp[j].codeword <<= L;
          hftemp[j++].codeword |= k++;
        }
      }
    }
    for(i = 0; i < *len; i++)
      hfcode[i] = hftemp[i];

    free(hftemp);
  }

  free(rle);

  return (0);
}


int
Create_Tree_And_Print(ListHead *pH, HuffmanCode *hfcode)
{
  BinTreeNode    *Root = NULL;
  BinTreeHead    BinTreeHe = InitializeTree();


  Root = (BinTreeNode *) calloc(1, sizeof(BinTreeNode));
  if(Root == NULL)
    return (-1);

  Root->pParent = NULL;
  Root->prob = 0.0;

  if(pH->Total == 2)
  {
    if(pH->pFirst->pNode->prob >= pH->pFirst->pNext->pNode->prob)
    {
      Root->pLeft = pH->pFirst->pNode;
      Root->pRight = pH->pFirst->pNext->pNode;
      pH->pFirst->pNode->flag = LEFT_OR_RIGHT;
      pH->pFirst->pNext->pNode->flag = 0;
    }
    else
    {
      Root->pRight = pH->pFirst->pNode;
      Root->pLeft = pH->pFirst->pNext->pNode;
      pH->pFirst->pNode->flag = 0;
      pH->pFirst->pNext->pNode->flag = LEFT_OR_RIGHT;
    }
    pH->pFirst->pNext->pNode->pParent = pH->pFirst->pNode->pParent = Root;
  }

  BinTreeHe.pCurr = BinTreeHe.pRoot = BinTreeHe.pMove = Root;

  Last(pH);
  while(pH->Total)
  {
    Remove(pH);
    if(pH->Total)
      Back(pH);
  }

  Inorder_Tree_Walk(&BinTreeHe, hfcode);

  return (0);
}
