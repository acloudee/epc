#ifndef __ArrayList_h__
#define __ArrayList_h__

#include "Common.h"

PROJECT_NS_BEGIN

class CArrayList
{
#ifdef _WIN32
	typedef int	(__cdecl *ArrayComparatorFunc)(const void* pValue1, const void* pValue2);	
#else
	typedef int	(*ArrayComparatorFunc)(const void* pValue1, const void* pValue2);	
#endif


public:
	CArrayList(int nMaxArraySize);
	CArrayList(const CArrayList& list);
	virtual ~CArrayList();

	bool ReInit(int nMaxArraySize);
	
	// status
	inline int GetCount() const { return m_nCount; }
	inline bool IsEmpty() const { return m_nCount == 0; }
	// array staus
	inline int GetArraySize() const { return m_nArraySize; }
	inline bool IsFull() const { return m_nFreeNode == 0; }
	inline int GetAvailableArrayCount() const { return m_nFreeNode; }
	
	// retrieval
	inline PVOID GetHead() const { return GetAt((POSITION)m_pHead); }
	inline PVOID GetTail() const { return GetAt((POSITION)m_pTail); }
	PVOID GetAt(int nIdx) const ;
	PVOID GetAt(POSITION pos) const ;		

	// removal
	inline PVOID RemoveHead() { return RemoveAt((POSITION)m_pHead); }
	inline PVOID RemoveTail() { return RemoveAt((POSITION)m_pTail); }	
	void RemoveAll();
	PVOID RemoveAt(POSITION pos);
	inline PVOID RemoveAt(int nIdx) { return RemoveAt(FindIndex(nIdx)); }
	inline void Remove(PVOID pValue) { RemoveAt(Find(pValue)); }

	// add
	POSITION AddHead(PVOID pValue);
	void AddHead(CArrayList* pList);
	POSITION AddTail(PVOID pValue);
	void AddTail(CArrayList* pList);
	POSITION InsertBefore(POSITION pos, PVOID pValue);// pos can be NULL => same as AddHead()
	POSITION InsertAfter(POSITION pos, PVOID pValue); // pos can be NULL => same as AddTail()
	POSITION Insert(int nIdx, PVOID pValue);	// 0 and tail is allowed
			
	// search
	POSITION FindIndex(int nIdx);
	POSITION Find(PVOID pValue, POSITION startAfter=NULL);

	// modification
	inline void SetAt(POSITION pos, PVOID pValue) { if (pos) ((CNode*)pos)->m_pValue = pValue; }
	POSITION SetAt(int nIdx, PVOID pValue);

	// iteration
	inline POSITION GetHeadPosition() const { return (POSITION)m_pHead;}
	PVOID GetNext(POSITION& pos) const;
	inline POSITION GetTailPosition() const { return (POSITION)m_pTail;}	
	PVOID GetPrev(POSITION& pos) const;

	// sort
	CArrayList& Sort(ArrayComparatorFunc comparator=NULL, bool bIncremental=true);

	// operator
	CArrayList& operator=(const CArrayList& list);
	inline PVOID operator[](int nIdx) { return GetAt(nIdx); }
	CArrayList& operator+=(CArrayList& list);

protected:
	class CNode {
	public:
		CNode(PVOID pValue=NULL) { Reset(pValue); m_nIdx = 0; }
		virtual ~CNode(){}
		CNode& operator=(CNode& node) {
			m_pNext = node.m_pNext;
			m_pPrev = node.m_pPrev;
			m_pValue = node.m_pValue;
			return *this;
		}
		void Reset(PVOID pValue) {
			m_pNext = m_pPrev = NULL; 
			m_pValue = pValue; 
		}
		CNode* m_pNext;
		CNode* m_pPrev;
		PVOID m_pValue;
		int m_nIdx;
	};

	CNode* m_pHead;
	CNode* m_pTail;
	int m_nCount;

	int m_nArraySize;
	CNode* m_pNodeArray;
	int* m_pnFreeNodeArrayIdx;
	int m_nFreeNode;

private:
	CNode* OnNewNode(PVOID pValue);
	void OnDeleteNode(CNode* pNode);

	static inline int 
#ifdef _WIN32
		__cdecl 
#endif
		_IntegerComparator(const void* pValue1, const void* pValue2) 
	{	return *(int*)pValue1 - *(int*)pValue2; }

};

extern CArrayList operator + (CArrayList& list1, CArrayList& list2);

PROJECT_NS_END

#endif //__ArrayList_h__
