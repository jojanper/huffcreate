#include <stdio.h>
#include <stdlib.h>

#include "bintree.h"
#include "defines.h"
#include "list.h"
#include "rle.h"


/*
 * Initialize the head element of the linked list.
 */
ListHead
InitializeList(void)
{
  ListHead  h;

  h.pFirst = h.pCurr = h.pLast = NULL;
  h.pKey1 = h.pKey2 = NULL;
  h.Total = 0;

  return (h);
}


/*
 * Set 'pH->pCurr' to the first element in the linked list.
 */
int
First(ListHead *pH)
{
  /* List is empty. */
  if(pH->pFirst == NULL)
    return (-1);

  pH->pCurr = pH->pFirst;

  return (0);
}


/*
 * Set 'pH->pCurr' to the next element in the linked list.
 */
int
Forward(ListHead *pH)
{
  /* List is empty or we are in the last element. */
  if(pH->pCurr->pNext == NULL)
    return (-1);

  pH->pCurr = pH->pCurr->pNext;

  return (0);
}


/*
 * Set 'pH->pCurr' to the previous element in the linked list.
 */
int
Back(ListHead *pH)
{
  /* List is empty or we are in the first element. */
  if(pH->pCurr->pPrev == NULL)
    return -1;

  pH->pCurr = pH->pCurr->pPrev;

  return 0;
}


int
Create_Node(ListHead *pH, RLE *rle)
{
  BinTreeNode  *pN;

  /* Create a node. */
  pN = (BinTreeNode *)malloc(sizeof(BinTreeNode));
  if(pN == NULL)
    return -1;

  pN->pParent = NULL;

  pN->prob = pN->rle.prob = rle->prob;
  pN->rle.original_prob = rle->original_prob;
  pN->rle.run = rle->run;
  pN->rle.size = rle->size;
  pN->pRight = pN->pLeft = pN->pParent = NULL;

  /* Add the new node to the list. */
  Find_And_Add(pH, pN);

  return (0);

}


void
Find_And_Add(ListHead *pH, BinTreeNode *pN)
{
  int i;

  First(pH);

  for(i = 0;  i < pH->Total; i++)
  {
    if(pH->pCurr->pNode->prob < pN->prob && pH->pCurr->pNext != NULL)
    {
      Forward(pH);
      continue;
    }
    else
      break;
  }

  Add(pH, pN);
}


int
Merge_Nodes(ListHead *pH)
{
  BinTreeNode  *pN;


  pN = (BinTreeNode *)malloc(sizeof(BinTreeNode));
  if(pN == NULL)
    return (-1);

  pN->pParent = NULL;

  /* Create left and right node. */
  if(pH->pFirst->pNode->prob >= pH->pFirst->pNext->pNode->prob)
  {
    pN->pLeft = pH->pFirst->pNode;
    pN->pRight = pH->pFirst->pNext->pNode;
    pH->pFirst->pNode->flag = LEFT_OR_RIGHT;
    pH->pFirst->pNext->pNode->flag = 0;
  }
  else
  {
    pN->pLeft = pH->pFirst->pNext->pNode;
    pN->pRight = pH->pFirst->pNode;
    pH->pFirst->pNode->flag = 0;
    pH->pFirst->pNext->pNode->flag = LEFT_OR_RIGHT;
  }

  pN->prob = pH->pFirst->pNext->pNode->prob + pH->pFirst->pNode->prob;
  pH->pFirst->pNode->pParent = pH->pFirst->pNext->pNode->pParent = pN;

  /*
   * Remove the first and second element from the list. Note that this
   * doesn't delete the binary tree nodes.
   */
  First(pH);
  Remove(pH);

  First(pH);
  Remove(pH);

  /* Add the new node to the list. */
  Find_And_Add(pH, pN);

  return (0);
}


int
Add(ListHead *pH, BinTreeNode *pN)
{
  List  *pL;
  float prob1, prob2;


  pL = (List*)(malloc(sizeof(List)));
  if(pL == NULL)
    return (-1);

  pL->pNode = pN;

  if(pH->Total != 0)
    prob1 = pH->pCurr->pNode->prob;

  prob2 = pN->prob;

  /*
   * There are four possible cases, which all have to cover, when adding
   * a binary tree node to the list.
   *   - list is empty (1)
   *   - adding to first (2)
   *   - adding to last (3)
   *   - none of the above (4)
   */

  if(pH->Total == 0)                                  /* (1) */
  {
    pL->pNext = pL->pPrev = NULL;
    pH->pFirst = pH->pCurr = pH->pLast = pL;
  }
  else if(pH->pCurr->pPrev == NULL && prob1 >= prob2) /* (2) */
  {
    pL->pNext = pH->pFirst;
    pL->pPrev = NULL;
    pH->pCurr->pPrev = pL;
    pH->pCurr = pH->pFirst = pL;
  }
  else if(pH->pCurr->pNext == NULL && prob1 < prob2)  /* (3) */
  {
    pL->pNext = NULL;
    pL->pPrev = pH->pCurr;
    pH->pLast = pH->pCurr->pNext = pL;
  }
  else                                                /* (4) */
  {
    pL->pNext = pH->pCurr;
    pL->pPrev = pH->pCurr->pPrev;
    pH->pCurr->pPrev->pNext = pL;
    pH->pCurr->pPrev = pL;
  }

  pH->Total++;

  return (0);
}


/*
 * Remove an element pointed by 'pH->pCurr' from the linked list.
 */
int
Remove(ListHead *pH)
{
  List  *pL;

  /* List is empty. */
  if(pH->Total == 0)
    return (-1);

  /*
   * There are four possible cases, which all have to cover, when removing
   * an element from the list.
   *  - removing the first and only element (1)
   *  - removing the first element (2)
   *  - removing the last element (3)
   *  - none of the above (4)
   */

  if(pH->Total == 1)                 /* (1) */
  {
    pH->pFirst = pH->pLast = NULL;
    pL = pH->pCurr;
    pH->pCurr = NULL;
  }
  else if(pH->pCurr->pPrev == NULL)  /* (2) */
  {
    pH->pCurr->pNext->pPrev = NULL;
    pH->pFirst = pH->pCurr->pNext;
    pL = pH->pCurr;
    pH->pCurr = pH->pFirst;
  }
  else if(pH->pCurr->pNext == NULL)  /* (3) */
  {
    pH->pCurr->pPrev->pNext = NULL;
    pL = pH->pCurr;
    pH->pCurr = pH->pLast = pH->pCurr->pPrev;
  }
  else                               /* (4) */
  {
    pL = pH->pCurr;
    pL->pNext->pPrev = pL->pPrev;
    pL->pPrev->pNext = pL->pNext;
    pH->pCurr = pH->pCurr->pPrev;
  }

  pL->pNext = pL->pPrev = NULL;
  pL->pNode = NULL;

  free(pL);

  pH->Total--;

  return (0);
}


/*
 * Set 'pH->pCurr' to the last element in the linked list.
 */
int
Last(ListHead *pH)
{
  /* List is empty. */
  if(pH->pLast == NULL)
    return (-1);

  pH->pCurr = pH->pLast;

  return (0);
}
