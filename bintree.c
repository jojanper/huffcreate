#include <stdio.h>
#include <stdlib.h>

#include "bintree.h"
#include "huffman.h"
#include "list.h"
#include "rle.h"


/*
 * Initialize the head "node" of the binary tree
 */
BinTreeHead
InitializeTree(void)
{
  BinTreeHead  h;

  h.pCurr = h.pMove = h.pRoot = NULL;

  return (h);
}


/*
 * Set 'pH->pCurr' to the root of the binary tree.
 */
int
Root(BinTreeHead *pT)
{
  if(pT->pRoot == NULL)
    return (-1);

  pT->pCurr = pT->pMove = pT->pRoot;

  return (0);
}


/*
 * Set 'pH->pCurr' to the right node.
 */
int
Right(BinTreeHead *pH)
{
  if(pH->pCurr->pRight == NULL)
    return (-1);

  pH->pCurr = pH->pMove = pH->pCurr->pRight;

  return (0);
}


/*
 * Set 'pH->pCurr' to the left node.
 */
int
Left(BinTreeHead *pH)
{
  if(pH->pCurr->pLeft == NULL)
    return (-1);

  pH->pCurr = pH->pMove = pH->pCurr->pLeft;

  return (0);
}


/*
 * Set 'pH->pCurr' to the parent node.
 */
int
Parent(BinTreeHead *pT)
{
  if(pT->pMove->pParent == NULL)
    return (-1);

  pT->pMove = pT->pMove->pParent;

  return (0);
}


/*
 * Remove a node pointed by 'pH->pCurr'.
 */
int
NodeRemove(BinTreeHead *pH)
{
  BinTreeNode  *pN;

  pN = pH->pCurr;

  /* Does this node have children ? If does don't destroy it yet. */
  if(pH->pCurr->pLeft == NULL && pH->pCurr->pRight != NULL)
    return (0);

  /* Is current node root node ? */
  if(pH->pCurr->pParent == NULL)
  {
    pH->pCurr = pH->pMove = pH->pRoot = NULL;
    free(pN);
    return (0);
  }

  /* Set pH->pCurr point to the parent node of the node to be removed. */
  pH->pCurr = pH->pMove = pH->pCurr->pParent;

  /* Is the node to be removed a left or a right node of its parent. */
  if(pH->pCurr->pLeft == pN)
    pH->pCurr->pLeft = NULL;
  else
    pH->pCurr->pRight = NULL;

  /* This node can be destroyed. */
  if(!(pH->pCurr->flag & FLAG_SET))
    pH->pCurr->flag ^= FLAG_SET;

  /* For safety all the pointers of the node to be removed are set to NULL. */
  pN->pParent = NULL;

  /* Now we are ready to destroy the node. */
  free(pN);

  return (0);
}


/*
 * Create Huffman code by traversing from the child node
 * to the root. Delete node after use.
 */
int
Inorder_Tree_Walk(BinTreeHead *pH, HuffmanCode *hfword)
{
  unsigned int  i;
  HuffmanCode   *hftemp;

  hftemp = hfword;

  while(pH->pRoot != NULL)
  {
    /* Go to the leftmost (child) node. */
    while(pH->pCurr->pLeft !=NULL)
      Left(pH);

    /*
     * Are we actually in the leftmost child node ? It is possible that
     * we have reached the leftmost node but it can have a child which
     * is on the right. This can be solved by going to the child node which
     * is the right and to continue the searching of the leftmost child node
     * from there.
     */
    if(pH->pCurr->pRight == NULL)
    {
      /*
       * FLAG_SET-bit will indicate wheather we have already been here or not.
       * If it has been set, then this is not a "true" child node and it can
       * be destroyed.
       */
      if(!(pH->pCurr->flag & FLAG_SET))
      {
        /* Store the (run,size)-pair to the Huffman code structure. */
        (*hftemp).rle.run = pH->pCurr->rle.run;
        (*hftemp).rle.size = pH->pCurr->rle.size;
        (*hftemp).rle.prob = pH->pCurr->prob;
        (*hftemp).rle.original_prob = pH->pCurr->rle.original_prob;

        /*
         * Start Huffman code creation. LEFT_OR_RIGHT-bit will indicate
         * wheather we are in the left or in the right child node. It is
         * important to know that since it determines entirely the Huffman
         * code construction.
         */
        (*hftemp).codeword = (pH->pCurr->flag & LEFT_OR_RIGHT) ? 1 : 0;
        i = 2;
        /* Traverse from the child node to the root node. */
        while(pH->pMove->pParent->pParent != NULL)
        {
          Parent(pH);
          (*hftemp).codeword ^= (pH->pMove->flag & LEFT_OR_RIGHT) ? i : 0;
          i <<= 1;
        }
        /* Count the length of the created Huffman code. */
        i >>= 1;
        (*hftemp).length = 0;
        while(i > 0)
        {
          i >>= 1;
          (*hftemp).length++;
        }

        /*
         * Check the length of the codeword. For practical reasons the length
         * of the codeword have to be less than or equal to 32 bits.
         */
        if((*hftemp).length > 32)
        {
          fprintf(stderr, "codeword length too long (%i)\n", (*hftemp).length);
          exit(EXIT_FAILURE);
        }

        hftemp++;
        NodeRemove(pH); Root(pH);
      }
      /*
       * Destroy the node since it does not belong to the actual Huffman
       * code i.e. is is an intermediate node.
       */
      else
        NodeRemove(pH); Root(pH);
    }
    /*
     * Go to the child node on the right and continue searching the leftmost
     * child node.
     */
    else
      Right(pH);
  }

  return 0;
}
