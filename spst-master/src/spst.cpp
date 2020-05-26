#include "spst.h"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <list>

#include "../core/include/util.h"
#include "../core/include/metadata.h"
#include "../core/include/savime_hash.h"
#include "../core/include/query_data_manager.h"
#include "../core/include/storage_manager.h"
#include "../core/include/abstract_storage_manager.h"
#include "default_storage_manager.h"
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <cstdlib>

inline SPSTTree RR_Rotate(SPSTTree k2)
{
    SPSTTree k1 = k2->lchild;
    k2->lchild = k1->rchild;
    k1->rchild = k2;
    return k1;
}

inline SPSTTree LL_Rotate(SPSTTree k2)
{
    SPSTTree k1 = k2->rchild;
    k2->rchild = k1->lchild;
    k1->lchild = k2;
    return k1;
}

SPSTTree IntervalSplay(int ElementL,int ElementR, SPSTTree root){
    int midValue;
    root = Splay(ElementL,root);
    root = Splay(ElementR,root);
    midValue = (ElementL + ElementR) / 2; //Down-bound
    root = Splay(midValue,root);

    return root;

}


SPSTTree Splay2(double Element, std::string op, SPSTTree root, SPSTTree ResultList)
{
    if(!root)
        return NULL;
    SPSTNode header;
    /* header.rchild points to L tree; header.lchild points to R Tree */
    header.lchild = header.rchild = NULL;
    SPSTTree LeftTreeMax = &header;
    SPSTTree RightTreeMin = &header;

    /* loop until root->lchild == NULL || root->rchild == NULL; then break!
       (or when find the Element, break too.)
     The zig/zag mode would only happen when cannot find Element and will reach
     null on one side after RR or LL Rotation.
     */

    while(1)
    {

        if(Element < root->Element)
        {
            if(!root->lchild)
                break;

            if(Element < root->lchild->Element) //se for menor segue à esquerda senão segue à direita
            {

                root = RR_Rotate(root); /* only zig-zig mode need to rotate once,
										   because zig-zag mode is handled as zig
										   mode, which doesn't require rotate,
										   just linking it to R Tree */
                if(!root->lchild)
                    break;
            }
            /* Link to R Tree */
            RightTreeMin->lchild = root;
            RightTreeMin = RightTreeMin->lchild;
            root = root->lchild;
            RightTreeMin->lchild = NULL;
        }
        else if(Element > root->Element)
        {

            if(!root->rchild)
                break;
            if(Element > root->rchild->Element)
            {

                root = LL_Rotate(root);/* only zag-zag mode need to rotate once,
										  because zag-zig mode is handled as zag
										  mode, which doesn't require rotate,
										  just linking it to L Tree */
                if(!root->rchild)
                    break;
            }
            /* Link to L Tree */
            LeftTreeMax->rchild = root;
            LeftTreeMax = LeftTreeMax->rchild;
            root = root->rchild;
            LeftTreeMax->rchild = NULL;
        }
        else
            break;
    }
    /* assemble L Tree, Middle Tree and R tree together */
    LeftTreeMax->rchild = root->lchild;
    RightTreeMin->lchild = root->rchild;
    root->lchild = header.rchild;
    root->rchild = header.lchild;

    //root->lchild //todos menores que ele estão aqui

    //root ->rchild //todos os maiores que ele estão aqui

    if(op.compare(">") == 0) {
        if((Element != root->Element) && (root->Element > Element)){
            //printf("%i\n", root->Element);
            std::copy(root->list.begin(), root->list.end(), std::back_inserter(ResultList->resultsearchlist));
            Greater(root, ResultList);
        }else{
            Greater(root, ResultList);
        }
    }else if(op.compare("<") == 0)
    {
        if((Element != root->Element) && (root->Element < Element)) {
            //printf("%i\n", root->Element);
            std::copy(root->list.begin(), root->list.end(), std::back_inserter(ResultList->resultsearchlist));
            Less(root, ResultList);
        }else{
            Less(root, ResultList);
        }
    }else if(op.compare("=") == 0)
    {
        //for (int j = 0; j < root->qtd_element; j++) {
            //printf("%i\n", root->Element);
        //}
        //copiar os itens da lista do elemento selecionado para a lista de resultados
        std::copy(root->list.begin(), root->list.end(), std::back_inserter(ResultList->resultsearchlist));

    }else if(op.compare(">=") == 0)
    {
        //for(int j = 0 ; j < root->qtd_element ; j++) {
            //printf("%i\n", root->Element);
        //}
        std::copy(root->list.begin(), root->list.end(), std::back_inserter(ResultList->resultsearchlist));
        Greater(root, ResultList);
    }else if(op.compare("<=") == 0)
    {
        //for(int j = 0 ; j < root->qtd_element ; j++) {
            //printf("%i\n", root->Element);
        //}
        std::copy(root->list.begin(), root->list.end(), std::back_inserter(ResultList->resultsearchlist));
        Less(root, ResultList);
    }

    // Copy non-pointer values
    ResultList->Element = root->Element;
    ResultList->offset = root->offset;
    std::copy(root->list.begin(), root->list.end(), std::back_inserter(ResultList->list));

    // Recursively copy subtrees
    if(root->lchild) {
        ResultList->lchild = copyTree(root->lchild);
    } else {
        ResultList->lchild = NULL;
    }
    if(root->rchild) {
        ResultList->rchild = copyTree(root->rchild);
    } else {
        ResultList->rchild = NULL;
    }

    return root;

}


SPSTTree copyTree(SPSTTree original)
{
    // Allocate here your copy using the same way you do in your code, as it is not specified use malloc as an example
    SPSTTree copy = new SPSTNode();

    // Copy non-pointer values
    copy->Element = original->Element;
    copy->offset = original->offset;
    std::copy(original->list.begin(), original->list.end(), std::back_inserter(copy->list));

    // Recursively copy subtrees
    if(original->lchild) {
        copy->lchild = copyTree(original->lchild);
    } else {
        copy->lchild = NULL;
    }
    if(original->rchild) {
        copy->rchild = copyTree(original->rchild);
    } else {
        copy->rchild = NULL;
    }

    return copy;
}

// método para imprimir o Element do nó
//SPSTTree Print(SPSTTree root)
//{
    //for(int i = 0 ; i < root->qtd_element ; i++)
    //{
        //printf("%i\n", root->Element);
    //}
//}
/*
SPSTTree NotEqual(SPSTTree root, SPSTTree ResultList){

    if(root->rchild != NULL) {
        SPSTTree x = root->rchild;
        if(x->Element != root->Element) {
            Print(x);
            std::copy(root->rchild->list.begin(), root->rchild->list.end(), std::back_inserter(ResultList->list));
        }
        if ((x->rchild != NULL) && (x->lchild != NULL)) {
            if(x->rchild->Element != root->Element){
                Print(x->rchild);
                std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->list));
            }
            if(x->lchild->Element != root->Element) {
                Print(x->lchild);
                std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->list));
            }
            Two_childs(x->rchild, x->lchild, ResultList);
        }
        if ((x->rchild != NULL) && (x->lchild == NULL))
        {
            if(x->rchild->Element != root->Element) {
                Print(x->rchild);
                std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->list));
            }
            One_child(x->rchild, ResultList);
        }
        if ((x->lchild != NULL) && (x->rchild == NULL)){
            if(x->lchild->Element != root->Element){
                Print(x->lchild);
                std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->list));
            }
            One_child(x->lchild, ResultList);
        }
    }else if(root->lchild != NULL){
        SPSTTree x = root->lchild;
        if(x->Element != root->Element) {
            Print(x);
            std::copy(root->lchild->list.begin(), root->lchild->list.end(), std::back_inserter(ResultList->list));
        }
        if ((x->rchild != NULL) && (x->lchild != NULL)) {
            if(x->rchild->Element != root->Element) {
                Print(x->rchild);
                std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->list));
            }
            if(x->lchild->Element != root->Element){
                Print(x->lchild);
                std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->list));
            }
            Two_childs(x->rchild, x->lchild, ResultList);
        }
        if ((x->rchild != NULL) && (x->lchild == NULL))
        {
            if(x->rchild->Element != root->Element){
                Print(x->rchild);
                std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->list));
            }
            One_child(x->rchild, ResultList);
        }
        if ((x->lchild != NULL) && (x->rchild == NULL)){
            if(x->lchild->Element != root->Element) {
                Print(x->lchild);
                std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->list));
            }
            One_child(x->lchild, ResultList);
        }
    }
}
*/

SPSTTree Greater(SPSTTree root, SPSTTree ResultList){ //a entrada vai ser a saída de splay = Splay2(60, T)

    if(root->rchild != NULL) {
        SPSTTree x = root->rchild;
        //Print(x);
        std::copy(root->rchild->list.begin(), root->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
        if ((x->rchild != NULL) && (x->lchild != NULL)) {
            //Print(x->rchild);
            std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            //Print(x->lchild);
            std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            Two_childs(x->rchild, x->lchild, ResultList);
        }
        if ((x->rchild != NULL) && (x->lchild == NULL))
        {
            //Print(x->rchild);
            std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            One_child(x->rchild, ResultList);
        }
        if ((x->lchild != NULL) && (x->rchild == NULL)){
            //Print(x->lchild);
            std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            One_child(x->lchild, ResultList);
        }
    }
}

SPSTTree Less(SPSTTree root, SPSTTree ResultList){ //a entrada vai ser a saída de splay = Splay2(60, T)

    if(root->lchild != NULL) {
        SPSTTree x = root->lchild;
        //Print(x);
        std::copy(root->lchild->list.begin(), root->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));

        if ((x->rchild != NULL) && (x->lchild != NULL)) {
            //Print(x->rchild);
            std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            //Print(x->lchild);
            std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            Two_childs(x->rchild, x->lchild, ResultList);
        }
        if ((x->rchild != NULL) && (x->lchild == NULL))
        {
            //Print(x->rchild);
            std::copy(x->rchild->list.begin(), x->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            One_child(x->rchild, ResultList);
        }
        if ((x->lchild != NULL) && (x->rchild == NULL)){
            //Print(x->lchild);
            std::copy(x->lchild->list.begin(), x->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
            One_child(x->lchild, ResultList);
        }
    }
}


SPSTTree Two_childs(SPSTTree root1, SPSTTree root2, SPSTTree ResultList)  { //a entrada vai ser a saída de splay = Splay2(60, T)

    //se não tiver o par rodar Bigger3 para 1 parametro apenas;

    if((root1->rchild != NULL)) {
       //Print(root1->rchild);
        std::copy(root1->rchild->list.begin(), root1->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
    }
    if((root1->lchild != NULL)) {
        //Print(root1->lchild);
        std::copy(root1->lchild->list.begin(), root1->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
    }

    if((root2->rchild != NULL)) {
        //Print(root2->rchild);
        std::copy(root2->rchild->list.begin(), root2->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
    }
    if((root2->lchild != NULL)) {
        //Print(root2->lchild);
        std::copy(root2->lchild->list.begin(), root2->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
    }

    SPSTTree w = root1;
    SPSTTree z = root2; //não serve pra nada

    //executar em paralelo com z

    if((w->rchild != NULL) && (w->lchild != NULL))
    {
        Two_childs(w->rchild, w->lchild, ResultList);
        //Bigger2(z->rchild, z->lchild);
    }else if((w->rchild != NULL) && (w->lchild == NULL))
    {
        One_child(w->rchild, ResultList);
    }else if((w->lchild != NULL) && (w->rchild == NULL))
    {
        One_child(w->lchild, ResultList);
    }

    //executar em paralelo com w


    if((z->rchild != NULL) && (z->lchild != NULL))
    {
        Two_childs(z->rchild, z->lchild, ResultList);
    }else if((z->rchild != NULL) && (z->lchild == NULL))
    {
        One_child(z->rchild, ResultList);
    }else if((z->lchild != NULL) && (z->rchild == NULL))
    {
        One_child(z->lchild, ResultList);
    }

}

SPSTTree One_child(SPSTTree root1, SPSTTree ResultList) {

    if((root1->rchild != NULL)) {
        //Print(root1->rchild);
        std::copy(root1->rchild->list.begin(), root1->rchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
    }
    if((root1->lchild != NULL)) {
        //Print(root1->lchild);
        std::copy(root1->lchild->list.begin(), root1->lchild->list.end(), std::back_inserter(ResultList->resultsearchlist));
    }

    SPSTTree w = root1;

    if((w->rchild != NULL) && (w->lchild != NULL))
    {
        Two_childs(w->rchild, w->lchild, ResultList);
    }else if((w->rchild != NULL) && (w->lchild == NULL))
    {
        One_child(w->rchild, ResultList);
    }else if((w->lchild != NULL) && (w->rchild == NULL))
    {
        One_child(w->lchild, ResultList);
    }
}


SPSTTree Splay(int Element, SPSTTree root)
{
    if(!root)
        return NULL;
    SPSTNode header;
    /* header.rchild points to L tree; header.lchild points to R Tree */
    header.lchild = header.rchild = NULL;
    SPSTTree LeftTreeMax = &header;
    SPSTTree RightTreeMin = &header;

    /* loop until root->lchild == NULL || root->rchild == NULL; then break!
       (or when find the Element, break too.)
     The zig/zag mode would only happen when cannot find Element and will reach
     null on one side after RR or LL Rotation.
     */

    while(1)
    {
        if(Element < root->Element)
        {
            if(!root->lchild)
                break;

            if(Element < root->lchild->Element) //se for menor segue à esquerda senão segue à direita
            {

                root = RR_Rotate(root); /* only zig-zig mode need to rotate once,
										   because zig-zag mode is handled as zig
										   mode, which doesn't require rotate,
										   just linking it to R Tree */
                if(!root->lchild)
                    break;

            }
            /* Link to R Tree */
            RightTreeMin->lchild = root;
            RightTreeMin = RightTreeMin->lchild;
            root = root->lchild;
            RightTreeMin->lchild = NULL;
        }
        else if(Element > root->Element)
        {

            if(!root->rchild)
                break;
            if(Element > root->rchild->Element)
            {
                root = LL_Rotate(root);/* only zag-zag mode need to rotate once,
										  because zag-zig mode is handled as zag
										  mode, which doesn't require rotate,
										  just linking it to L Tree */
                if(!root->rchild)
                    break;
            }
            /* Link to L Tree */
            LeftTreeMax->rchild = root;
            LeftTreeMax = LeftTreeMax->rchild;
            root = root->rchild;
            LeftTreeMax->rchild = NULL;
        }
        else
            break;
    }
    /* assemble L Tree, Middle Tree and R tree together */
    LeftTreeMax->rchild = root->lchild;
    RightTreeMin->lchild = root->rchild;
    root->lchild = header.rchild;
    root->rchild = header.lchild;

    return root;
}

SPSTTree New_Node(int64_t offset,ElementType Element)
{
    SPSTTree p_node = new SPSTNode;
    if(!p_node)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    p_node->Element = Element;
    p_node->offset = offset;

    p_node->lchild = p_node->rchild = NULL;
    return p_node;
}


SPSTTree Insert(int64_t offset,ElementType Element, SPSTTree root)
{
    static SPSTTree p_node = NULL;
    if(!p_node) {
        p_node = New_Node(offset, Element);

        ElementList EL;
        EL.position = p_node->offset;
        p_node->list.push_back(EL);
    } else {
        p_node->offset = offset;
        p_node->Element = Element;

        ElementList EL;
        EL.position = p_node->offset;
        p_node->list.push_back(EL);

    }// could take advantage of the node remains because of there was duplicate Element before.
    if(!root)
    {
        root = p_node;
        p_node = NULL;
        return root;
    }
    root = Splay(Element, root);
    /* This is BST that, all Elements <= root->Element is in root->lchild, all Elements >
       root->Element is in root->rchild. (This BST doesn't allow duplicate Elements.) */
    if(Element < root->Element)
    {
        p_node->lchild = root->lchild;
        p_node->rchild = root;
        root->lchild = NULL;
        root = p_node;
    }
    else if(Element > root->Element)
    {
        p_node->rchild = root->rchild;
        p_node->lchild = root;
        root->rchild = NULL;
        root = p_node;
    }
    else if(Element == root->Element) {

        ElementList EL;
        EL.position = p_node->offset;
        root->list.push_back(EL);

    }
    else
        return root;
    p_node = NULL;

    return root;

}

SPSTTree PruneLeaves(SPSTTree node){
    if (node == NULL)
        return NULL;
    if (node->lchild == NULL && node->rchild == NULL){
        free(node);
        return NULL;
    }
    node->lchild = PruneLeaves(node->lchild);
    node->rchild = PruneLeaves(node->rchild);
    return node;

}


SPSTTree Delete(ElementType Element, SPSTTree root)
{
    SPSTTree temp;
    if(!root)
        return NULL;
    root = Splay(Element, root);
    if(Element != root->Element) // No such node in splay tree
        return root;
    else
    {
        if(!root->lchild)
        {
            temp = root;
            root = root->rchild;
        }
        else
        {
            temp = root;
            /*Note: Since Element == root->Element, so after Splay(Element, root->lchild),
              the tree we get will have no right child tree. (Element > any Element in
              root->lchild)*/
            root = Splay(Element, root->lchild);
            root->rchild = temp->rchild;
        }
        free(temp);
        return root;
    }
}


SPSTTree Filter_spst(ElementType Element, std::string op,  SPSTTree root, SPSTTree ResultList)
{
    return Splay2(Element, op, root, ResultList);
}


SPSTTree Search(ElementType Element, SPSTTree root)
{
    return Splay(Element, root);
}



void InOrder(SPSTTree root)
{
    if(root)
    {
        InOrder(root->lchild);
        std::cout<< "Element: " <<root->Element;
        if(root->lchild)
            std::cout<< " | left child: "<< root->lchild->Element;
        if(root->rchild)
            std::cout << " | right child: " << root->rchild->Element;
        std::cout<< "\n";
        InOrder(root->rchild);
    }
}

void PostOrder(SPSTTree p, int indent=0)
{
    if(p != NULL) {
        std::cout<< p->Element << "\n ";
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if(p->lchild)
            PostOrder(p->lchild, indent+4);
        if(p->rchild)
            PostOrder(p->rchild, indent+4);

    }
}

SPSTTree
FindLastPiece(SPSTTree T) {
    SPSTTree auxNode;
    while (T != NULL) {
        auxNode = T;
        T = T->rchild;
    }

    return auxNode;
}
PositionSPST
FindMax(SPSTTree T) {
    if (T != NULL)
        while (T->rchild != NULL)
            T = T->rchild;

    return T;
}
SPSTTree FindNeighborsLTFinal(ElementType X, SPSTTree T) {
    SPSTTree aux = NULL;
    SPSTTree original;
    original = T;
    while (T != NULL) {
        if (X < T->Element) {
            aux = T;
            T = T->lchild;
        } else if (X > T->Element) {
            T = T->rchild;
        } else {
            aux = T;
            if (T->lchild != NULL) {
                aux = FindMax(T->lchild);
            }
            break;
        }
    }
    if (!aux) {
        aux = FindMax(original);
    }
    return aux;
}

void _GetNodesInOrder(SPSTTree T, std::vector<SPSTTree> &vector) {
    if (T->lchild) {
        _GetNodesInOrder(T->lchild, vector);
    }
    vector.push_back(T);
    if (T->rchild) {
        _GetNodesInOrder(T->rchild, vector);
    }
}


std::vector<SPSTTree> GetNodesInOrder(SPSTTree T) {
    std::vector<SPSTTree> nodesOrder;
    _GetNodesInOrder(T, nodesOrder);
    return nodesOrder;
}

SPSTTree FindNeighborsGTFinal(ElementType X, SPSTTree T) {
    SPSTTree aux = NULL;
    while (T != NULL) {
        if (X < T->Element) {
            T = T->lchild;
        } else if (X > T->Element) {
            aux = T;
            T = T->rchild;
        } else {
            aux = T;
            break;
        }
    }

    return aux;
}

SPSTTree
FindFirstPiece(SPSTTree T) {
    SPSTTree auxNode;
    while (T != NULL) {
        auxNode = T;
        T = T->lchild;
    }

    return auxNode;
}

IntPair createOffsetPair(PositionSPST first, PositionSPST second, ElementType limit) {
    IntPair op = (IntPair) malloc(sizeof(struct int_pair));
    if (first && second) {
        if (first->offset == 0)
            op->first = first->offset;
        else
            op->first = first->offset + 1;
        op->second = second->offset;
    } else if (first) {
        if (first->offset == 0)
            op->first = first->offset;
        else
            op->first = first->offset + 1;
        op->second = limit;
    } else if (second) {
        op->first = 0;
        op->second = second->offset;
    } else {
        op->first = 0;
        op->second = limit;
    }
    return op;
}

IntPair
FindNeighborsLT(ElementType X, SPSTTree T, ElementType limit) {
    PositionSPST first = 0, second = 0;
    //if( T == NULL )
    //	return NULL;

    while (T != NULL) {
        if (X < T->Element) {
            second = T;
            T = T->lchild;
        } else if (X > T->Element) {
            first = T;
            T = T->rchild;
        } else {
            second = T;
            if (T->lchild != NULL) {
                first = FindMax(T->rchild);
            }
            break;
        }
    }

    return createOffsetPair(first, second, limit);
}

IntPair
FindNeighborsGTE(ElementType X, SPSTTree T, ElementType limit) {
    PositionSPST first = 0, second = 0;

    while (T != NULL) {
        if (X < T->Element) {
            second = T;
            T = T->lchild;
        } else if (X > T->Element) {
            first = T;
            T = T->rchild;
        } else {
            first = T;        // this is the only difference from FindNeighborsLT !
            break;
        }
    }

    return createOffsetPair(first, second, limit);
}

