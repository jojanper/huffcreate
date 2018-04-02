#ifndef LIST_H_
#define LIST_H_


#include "bintree.h"
#include "rle.h"


/*
 * List-structure consists of payload field and two other pointer
 * fields. The payload in this case is a binary tree node. Furthermore,
 * 'pNext' points to its successor in the linked list and 'pPrev' to its
 * predecessor.
 */

typedef struct ListStr
{
    BinTreeNode      *pNode;
    struct ListStr   *pNext;
    struct ListStr   *pPrev;

} List;


/*
 * Head-structure informs how many elements there are in the linked list.
 * 'pFirst' points to the first, 'pCurr' to the current and 'pLast' to the
 * last element in the linked list. Pointers 'pKey1' and 'pKey2' are needed
 * when a certain type of list element is searched from the linked list. We
 * need two pointers since after the first element is found, another search
 * is performed and we can not lose the first element. For more information
 * see functions 'Find' and 'Add'.
 */

typedef struct ListHeadStr
{
    int   Total;
    List  *pFirst;
    List  *pCurr;
    List  *pLast;
    List  *pKey1;
    List  *pKey2;

} ListHead;


/*
 * Following eight functions provide us the access to the linked lists.
 * These functions are:
 *
 * InitializeList ; initializes the list header
 * First          ; sets the 'pCurr'-pointer to the first element in the list
 * Forward        ; moves one element forward in the list
 * Back           ; moves one element backward in the list
 * Create_Node    ; creates a binary tree node and adds it to the list
 * Merge_Nodes    ; combines two binary nodes as one
 *                ; (= parent node, left and right child node)
 * Find_And_Add   ; searhes a correct position for a binary tree node
 * Add            ; adds a binary tree node to the list
 * Remove         ; removes an element from the list
 * Last           ; sets the 'pCurr'-pointer to the last element in the list
 */

ListHead InitializeList(void);
int First(ListHead *pH);
int Forward(ListHead *pH);
int Back(ListHead *pH);
int Create_Node(ListHead *pH, RLE *rle);
int Merge_Nodes(ListHead *pH);
void Find_And_Add(ListHead *pH, BinTreeNode *pN);
int Add(ListHead *pH, BinTreeNode *pN);
int Remove(ListHead *pH);
int Last(ListHead *pH);


#endif /* LIST_H_ */
