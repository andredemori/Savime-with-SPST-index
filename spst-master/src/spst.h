#ifndef SPST
#define SPST

#include <cstdint>
typedef int64_t ElementType;
#include <vector>
#include <string>
#include <list>

struct SPSTNode;

typedef struct SPSTNode* PositionSPST;
typedef struct SPSTNode* SPSTTree;

struct ElementList{
    int position;
};

struct SPSTNode{
    ElementType Element;
    int64_t offset;
    SPSTTree lchild;
    SPSTTree rchild;
    std::list<ElementList> list; //(Element, position)

    std::list<ElementList> resultsearchlist;

};

struct int_pair {
    int64_t first;
    int64_t second;
};
typedef struct int_pair *IntPair;

SPSTTree New_Node(int64_t offset,ElementType key);
inline SPSTTree RR_Rotate(SPSTTree k2);
inline SPSTTree LL_Rotate(SPSTTree k2);
SPSTTree Splay(int key, SPSTTree root);

SPSTTree copyTree(SPSTTree original);
SPSTTree copyNewTreeToResultList(SPSTTree original);

SPSTTree Splay2(double key, std::string op, SPSTTree root, SPSTTree ResultList);

SPSTTree IntervalSplay(int keyL,int keyR, SPSTTree root);
SPSTTree PruneLeaves(SPSTTree node);
SPSTTree Insert(int64_t offset,ElementType key, SPSTTree root);
SPSTTree Delete(ElementType key, SPSTTree root);
SPSTTree Search(ElementType key, SPSTTree root);

SPSTTree Filter_spst(ElementType key, std::string op, SPSTTree root, SPSTTree ResultList);
SPSTTree Print(SPSTTree root); //imprime o nó
SPSTTree Greater(SPSTTree root, SPSTTree ResultList);
SPSTTree Less(SPSTTree root, SPSTTree ResultList);

SPSTTree Two_childs(SPSTTree root1, SPSTTree root2, SPSTTree ResultList);

SPSTTree One_child(SPSTTree root1, SPSTTree ResultList);


SPSTTree PrintTwoSons(SPSTTree root);

void InOrder(SPSTTree root);
void PostOrder(SPSTTree p, int indent);
SPSTTree FindLastPiece(SPSTTree T);
SPSTTree FindNeighborsLTFinal(ElementType X, SPSTTree T);
std::vector<SPSTTree> GetNodesInOrder(SPSTTree T);
SPSTTree FindNeighborsGTFinal(ElementType X, SPSTTree T);
SPSTTree  FindFirstPiece(SPSTTree T);
IntPair FindNeighborsLT(ElementType X, SPSTTree T, ElementType limit);
IntPair  FindNeighborsGTE(ElementType X, SPSTTree T, ElementType limit);
#endif