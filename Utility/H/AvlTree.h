//:
/* 
CAVLTree and CSizeAVLTree can take char*, int*, void* or any other points in one tree.
eg: CAVLTree tree; 
	tree.Insert((int*)a); 
	tree.Insert((char*)b);
	

*/

#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

#include "Common.h"
#include "ArrayList.h"
#include "Mutex.h"
//#include "Strings.h"

PROJECT_NS_BEGIN
	
#define L_CHILD		1
#define R_CHILD		2
#define LR_CHILD	3
#define LL_CHILD	4
#define RL_CHILD	5
#define RR_CHILD	6

#ifdef _DEBUG_TREE_
typedef enum {
	AVL_PRINTF_L_M_R = 1,	//left-middle-right
	AVL_PRINTF_R_M_L,		//right-middle-left
	AVL_PRINTF_M_L_R,		//middle-left-right
	AVL_PRINTF_M_R_L		//middle-right-left
} AVL_PRINTF_TYPE;
#endif	//_DEBUG_TREE_

typedef int  (*PAVL_COMPARE_FUNC)(PVOID pValue1, PVOID pValue2);
typedef void (*PAVL_COPY_FUNC)(PVOID pDstValue, PVOID pSrcValue);
typedef bool (*PAVL_VALIDATE_FUNC)(PVOID pValue);
typedef void (*PAVL_DEL_FUNC)(PVOID pValue);

class CAVLTree  
{
public:
	typedef struct _AVL_NODE{
		struct _AVL_NODE* pLeft;		//left child
		struct _AVL_NODE* pRight;	//right child
		struct _AVL_NODE* pParent;	//parent
		BYTE	Height;				//the height of the node
		PVOID	pValue;				//the point of value or the identifier of the real value.
#if 0
		PVOID	pHashValue;			//the real value or null. null means not use this.
#endif
		char	bf;					//balance factor
	}AVL_NODE, *PAVL_NODE;

public:
	CAVLTree();		// it need call bool Init(PAVL_COMPARE_FUNC, PAVL_COPY_FUNC, PAVL_DEL_FUNC) later, 
					//otherwise it will use default compare function (DefaultCompare)

	CAVLTree(PAVL_COMPARE_FUNC lpfnCompare, PAVL_COPY_FUNC lpfnCopy = NULL, PAVL_DEL_FUNC lpfnDel = NULL);
	virtual ~CAVLTree();

	bool Init(PAVL_COMPARE_FUNC lpfnCompare, PAVL_COPY_FUNC lpfnCopy = NULL, PAVL_DEL_FUNC lpfnDel = NULL);
//	static CAVLTree* _NewInstance(PAVL_COMPARE_FUNC lpfnCompare, PAVL_COPY_FUNC lpfnCopy = NULL, PAVL_DEL_FUNC lpfnDel = NULL) {return NULL;}; 
	//lfnfCompare: compare function		
	//		if	pValue1 > pValue2	: lpfnCompare return 1
	//			pValue1 == pValue2	: lpfnCompare return 0
	//			pValue1 < pValue2	: lpfnCompare return -1
	//lpfnCopy: copy function, when you use Insert, it nedd copy the value of pSrc to the pDst 

public:
	int Insert(PVOID pValue, bool bUseCopyFun = false);	
	//insert a node to the tree, which AVLNODE.pValue == pValue
	//pValue: the value you want to insert
	//bUseCopyFun: true: use lpfnCopy when insert
	//				false: not use lpfnCopy when insert
	//return	1: insert succeed
	//			0: this value is already exist on the tree
	//		   -1: insert failed

	virtual bool Insert(PAVL_NODE pAVLTree);
	//insert node pAVLTree and it's children nodes to the current tree.

	virtual PVOID Find(PVOID pValue);
	//find a node which (the value of AVLNODE.pValue point to) == (the value of pValue point to)
	//return: the value(address) of AVLNODE.pValue

	PVOID Remove(PVOID pValue, bool bUseDelFun = false);
	//remove a node from the tree, which AVLNODE.pValue == pValue
	//return: the point of AVLNODE.pValue

	void RemoveAll(bool bUseDelFun = false);
	//remove all nodes from the tree

	PAVL_NODE GetRoot();
	//return: the root node of this tree

	inline PAVL_NODE GetLeftChild(PAVL_NODE pInNode) { return pInNode ? pInNode->pLeft : NULL; }
	inline PAVL_NODE GetRightChild(PAVL_NODE pInNode) { return pInNode ? pInNode->pRight : NULL; }
	inline PAVL_NODE GetParent(PAVL_NODE pInNode) {return pInNode ? pInNode->pParent : NULL ;}

	int GetCount();
	//return: the number of all nodes

	int CountNodes(PAVL_NODE pNode);
	//return: the number of nodes below pNode (include pNode)

	CAVLTree& operator+=(PVOID pValue);
	//if you want to use pHashValue you must use Insert(PVOID, PVOID).
	CAVLTree& operator-=(PVOID pValue);
	
	int Popup(unsigned long* pPoint, int nMaxSize);
	//store AVL_NODE.pValue to the array pPoint
	//pPoint: the array to store point
	//nMaxSize: the max size of array pPoint
	//return the used size of this array

protected:
	PAVL_NODE FindNode(PVOID pValue);

	virtual PAVL_NODE OnNewNode();
	virtual bool OnDeleteNode(PAVL_NODE pNode);
	PAVL_NODE InsertValue(PVOID pValue, bool bUseCopyFun = false);	//insert a node 
	PAVL_NODE InsertNode(PAVL_NODE pNode);
	virtual void UseCopyFun(PAVL_NODE pNode, PVOID pValue, bool bUseCopyFun = false);
	
	virtual void RemoveAllNode(PAVL_NODE Node, bool bUseDelFun = false);
	virtual void DeleteTree();

	//functions about inserting. 
	bool LLRotate(PAVL_NODE Node);
	bool LRRotate(PAVL_NODE Node);
	bool RLRotate(PAVL_NODE Node);
	bool RRRotate(PAVL_NODE Node);
	bool Balance(PAVL_NODE Child, unsigned long RotateType, BYTE Height);

	//functions about removing.
	bool RemoveNodeFromTree(PAVL_NODE Node);
	bool BalanceAfterRemove(PAVL_NODE Node);

	static int DefaultCompare(PVOID nValueL, PVOID nValueR);

	PAVL_COMPARE_FUNC m_pCompare;
	PAVL_COPY_FUNC m_lpfnCopy;
	PAVL_DEL_FUNC m_lpfnDel;

	PAVL_NODE	m_pRoot;	//the root node.
	CMutex	m_mTx;

private:
	int PopupM(PAVL_NODE pTreeNode, unsigned long* pPoint, int nMaxSize, int nIndex);
	PAVL_NODE SearchLeftM(PAVL_NODE pTreeNode);
#ifdef _DEBUG_TREE_
	void PrintTreeM(void (*lpfnPrintf)(PVOID, PVOID),AVL_PRINTF_TYPE nType, PAVL_NODE T);
#endif
};

class CSizedAVLTree : public CAVLTree
{
public:
	CSizedAVLTree(PAVL_COMPARE_FUNC lpfnCompare, 
					int nNodeCount,	// max node count of this tree
					int nEntrySize = 0,
					PVOID pValueArr[] = NULL, 
					PAVL_COPY_FUNC lpfnCopy = NULL,
					PAVL_VALIDATE_FUNC lpfnValidator = NULL);
	~CSizedAVLTree();

	CSizedAVLTree();
	//if use this construction, you should call CSizedAVLTree::Init(PAVL_COMPARE_FUNC, int, int, PVOID, PAVL_COPY_FUNC, PAVL_VALIDATE_FUNC).

	bool Init(PAVL_COMPARE_FUNC lpfnCompare, 
		int nNodeCount,	// max node count of this tree
		int nEntrySize = 0,
		PVOID pValueArr[] = NULL, 
		PAVL_COPY_FUNC lpfnCopy = NULL,
		PAVL_VALIDATE_FUNC lpfnValidator = NULL);


	PVOID Insert(PVOID pValue, bool bUseCopyFun = false);	//insert a node 
	PVOID Find(PVOID pValue);

private:
	virtual bool InitArray(PVOID pValueArr[], int nEntrySize, int nCount);
	//init a tree
	//it will add the value in the pValueArr to the tree

	void UseCopyFun(PAVL_NODE pNode, PVOID pValue, bool bUseCopyFun = false);
	PAVL_NODE OnNewNode();
	bool OnDeleteNode(PAVL_NODE pNode);

	virtual void RemoveAllNode(PAVL_NODE Node, bool bUseDelFun = false);
	virtual void DeleteTree();

	bool Release(PAVL_NODE Node);

	CArrayList m_listNode;
	PAVL_NODE	m_pNodeArray;
	PAVL_VALIDATE_FUNC m_lpfnValidator;
	int m_nEntrySize;
};

PROJECT_NS_END

#endif //__AVL_TREE_H__

///:~

