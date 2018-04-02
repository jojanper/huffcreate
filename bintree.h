#ifndef BINARY_TREE_H_
#define BINARY_TREE_H_

#include "huffman.h"
#include "rle.h"

typedef struct BinTreeNodeStr
{
    BYTE   flag;
    float  prob;
    RLE    rle;
    struct BinTreeNodeStr *pParent;
    struct BinTreeNodeStr *pRight;
    struct BinTreeNodeStr *pLeft;

} BinTreeNode;


typedef struct BinaryTreeHeadStr
{
    BinTreeNode *pRoot;
    BinTreeNode *pCurr;
    BinTreeNode *pMove;

} BinTreeHead;


BinTreeHead InitializeTree(void);
int Root(BinTreeHead *pT);
int Right(BinTreeHead *pT);
int Left(BinTreeHead *pT);
int Parent(BinTreeHead *pT);
int NodeRemove(BinTreeHead *pT);
int Inorder_Tree_Walk(BinTreeHead *pT, HuffmanCode *hcword);

#endif /* BIN_TREE_H_ */
