#ifndef RLE_H_
#define RLE_H_


typedef struct RLEStr
{
    int   run;
    int   size;
    float prob;
    float original_prob;

} RLE;


typedef struct RLC_EntryStr
{
    RLE                  rle;
    struct RLC_EntryStr  *pNext;
    struct RLC_EntryStr  *pPrev;

} RLC;


typedef struct RLC_HeadStr
{
    int  Total;
    RLC  *pFirst;
    RLC  *pCurr;
    RLC  *pLast;

} RLC_Head;


RLC_Head Initialize_RLC_List(void);
int RLC_Compare(RLC_Head *pH, RLE *rle);
void RLC_Copy(RLC_Head *pH, RLC *pV);
int RLC_First(RLC_Head *pH);
int RLC_Forward(RLC_Head *pH);
int RLC_Back(RLC_Head *pH);
int RLC_Add(RLC_Head *pH, RLE *rle);
int RLC_Remove(RLC_Head *pH);
int RLC_Last(RLC_Head *pH);
int RLC_IncrementProbability(RLC_Head *pH, RLE *rle);


#endif /* RLE_H_ */
