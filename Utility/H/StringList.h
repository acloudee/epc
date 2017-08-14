#ifndef __StringList_h__
#define __StringList_h__

#include "List.h"
#include "Strings.h"
 
PROJECT_NS_BEGIN
 
class CStringList
{
public:
	// String comparator function used in Sort()
	static inline int 
#ifdef _WIN32
		__cdecl 
#endif
			_StringComparator(const void* pValue1, const void* pValue2) { 
				return strcmp(*(PCSTR*)pValue1, *(PCSTR*)pValue2); 
			}
	// default sort comparator

	static inline int 
#ifdef _WIN32
		__cdecl 
#endif
			_StringComparatorIngoreCase(const void* pValue1, const void* pValue2) { 
				return String::_CompareNoCase(*(PCSTR*)pValue1, *(PCSTR*)pValue2); 
			}

public:
	CStringList();
	CStringList(const CStringList& list);
	virtual ~CStringList();
	
	// status
	inline int GetCount() const { return m_list.GetCount(); }
	inline bool IsEmpty() const { return m_list.IsEmpty(); }
	
	// retrieval
	inline String GetHead() const { return (PCSTR)m_list.GetHead(); }
	inline String GetTail() const { return (PCSTR)m_list.GetTail(); }
	inline String GetAt(int nIdx) const { return (PCSTR)m_list.GetAt(nIdx); }
	inline String GetAt(POSITION pos) const { return (PCSTR)m_list.GetAt(pos); }	

	// removal
	String RemoveHead();
	String RemoveTail();
	void RemoveAll();
	String RemoveAt(POSITION pos);
	inline String RemoveAt(int nIdx) { return RemoveAt(FindIndex(nIdx)); }
	inline void Remove(PCSTR cpszValue) { RemoveAt(Find(cpszValue)); }

	// add
	POSITION AddHead(PCSTR cpszValue);
	void AddHead(CStringList* pszList);
	POSITION AddTail(PCSTR cpszValue);
	void AddTail(CStringList* pszList);
	POSITION InsertBefore(POSITION pos, PCSTR cpszValue);// pos can be NULL => same as AddHead()
	POSITION InsertAfter(POSITION pos, PCSTR cpszValue); // pos can be NULL => same as AddTail()
	POSITION Insert(int nIdx, PCSTR cpszValue);	// 0 and tail is allowed
			
	// search
	inline POSITION FindIndex(int nIdx)  { return m_list.FindIndex(nIdx); }
	POSITION Find(PCSTR cpszValue, POSITION startAfter=NULL, bool bCaseSensitive=true) const;

	// modification
	void SetAt(POSITION pos,PCSTR cpszValue);
	POSITION SetAt(int nIdx, PCSTR cpszValue);

	void TrimAll(); // strip off all spaces from all strings

	// iteration
	inline POSITION GetHeadPosition() const { return m_list.GetHeadPosition();}
	inline String GetNext(POSITION& pos) const {	return (PCSTR)m_list.GetNext(pos); }
	inline POSITION GetTailPosition() const { return m_list.GetTailPosition();}	
	inline String GetPrev(POSITION& pos) const {	return (PCSTR)m_list.GetPrev(pos); }

	// sort
	CStringList& Sort(List::Comparator comparator=_StringComparator, bool bIncremental=true);
	// comparator	String comparator for sort, 
	//				NULL to use default comparator _StringComparator
	//				You may use _StringComparatorIngoreCase to sort string list case-insensitively.

	// operator 
	CStringList& operator=(const CStringList& list);
	inline String operator[](int nIdx) const { return GetAt(nIdx); }
	CStringList& operator+=(CStringList& list);
	friend CStringList operator + (const CStringList& list1, const CStringList& list2);

protected:
	char*	CopyString(PCSTR cpszValue);
	List m_list;
};

extern CStringList operator + (CStringList& list1, CStringList& list2);

PROJECT_NS_END
#endif //__StringList_h__
