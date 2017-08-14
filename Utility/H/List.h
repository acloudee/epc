#ifndef __List_h__
#define __List_h__

#include "Common.h"

PROJECT_NS_BEGIN

class List
{
public:
#ifdef _WIN32
	typedef int	(__cdecl *Comparator)(const void* pValue1, const void* pValue2);	
#else
	typedef int	(*Comparator)(const void* pValue1, const void* pValue2);	
#endif

public:
	List();
	List(const List& list);
	virtual ~List();
	
	// status
	inline int GetCount() const { return m_nCount; }
	inline bool IsEmpty() const { return m_nCount == 0; }
	
	// retrieval
	inline void* GetHead() const { return GetAt((POSITION)m_pHead); }
	inline void* GetTail() const { return GetAt((POSITION)m_pTail); }
	void* GetAt(int nIdx) const ;
	void* GetAt(POSITION pos) const ;		

	// removal
	inline void* RemoveHead() { return RemoveAt((POSITION)m_pHead); }
	inline void* RemoveTail() { return RemoveAt((POSITION)m_pTail); }	
	void RemoveAll();
	void* RemoveAt(POSITION pos);
	inline void* RemoveAt(int nIdx) { return RemoveAt(FindIndex(nIdx)); }
	inline void Remove(void* pValue) { RemoveAt(Find(pValue)); }

	// add
	POSITION AddHead(void* pValue);
	void AddHead(List* pList);
	POSITION AddTail(void* pValue);
	void AddTail(List* pList);
	POSITION InsertBefore(POSITION pos, void* pValue);// pos can be NULL => same as AddHead()
	POSITION InsertAfter(POSITION pos, void* pValue); // pos can be NULL => same as AddTail()
	POSITION Insert(int nIdx, void* pValue);	// 0 and tail is allowed
			
	// search
	POSITION FindIndex(int nIdx);
	POSITION Find(void* pValue, POSITION startAfter=NULL);

	// modification
	inline void SetAt(POSITION pos, void* pValue) { if (pos) ((CNode*)pos)->m_pValue = pValue; }
	POSITION SetAt(int nIdx, void* pValue);

	// iteration
	inline POSITION GetHeadPosition() const { return (POSITION)m_pHead;}
	void* GetNext(POSITION& pos) const;
	inline POSITION GetTailPosition() const { return (POSITION)m_pTail;}	
	void* GetPrev(POSITION& pos) const;

	// sort
	List& Sort(Comparator comparator=NULL, bool bIncremental=true);

	// operator
	List& operator=(const List& list);
	inline void* operator[](int nIdx) { return GetAt(nIdx); }
	List& operator+=(List& list);
	friend List operator + (List& list1, List& list2);

protected:
	class CNode {
	public:
		CNode(void* pValue=NULL) { m_pNext = m_pPrev = NULL; m_pValue = pValue; }
		virtual ~CNode(){}
		CNode& operator=(CNode& node) {
			m_pNext = node.m_pNext;
			m_pPrev = node.m_pPrev;
			m_pValue = node.m_pValue;
			return *this;
		}
		CNode* m_pNext;
		CNode* m_pPrev;
		PVOID m_pValue;
	};

	CNode* m_pHead;
	CNode* m_pTail;
	int m_nCount;

private:
	static inline int 
#ifdef _WIN32
		__cdecl 
#endif
		_IntegerComparator(const void* pValue1, const void* pValue2) 
	{	return *(int*)pValue1 - *(int*)pValue2; }

};

extern List operator + (List& list1, List& list2);

PROJECT_NS_END

#endif //__List_h__
