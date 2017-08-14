#ifndef __Dictionary_h__
#define __Dictionary_h__
  
#include "StringMap.h"
#include "StringBuffer.h"

PROJECT_NS_BEGIN

class CDictionary
{
public: 
	typedef enum {
		Mismatched = 0,
		Matched,
		PartialMatched
	} MatchResult;

public:
	CDictionary(const CString2StringMap& words);
	CDictionary(const CDictionary& dictionary);

	virtual ~CDictionary();

	MatchResult Match(PCSTR cpszWord);
	inline bool HasWord(PCSTR cpszWord) { return m_strMap.ContainsKey((PVOID)cpszWord);}
	inline String Lookup(PCSTR cpszWord = NULL) { return m_strMap.Get(cpszWord ? cpszWord : m_sbIncremental.ToString()); }
	// cpszWord NULL to get the incrmental match result
	MatchResult IncrementalMatch(PCSTR cpszPrefix) { return Match(m_sbIncremental.Append(cpszPrefix).ToString()); }
	MatchResult IncrementalMatch(char ch) { return Match(m_sbIncremental.Append(ch).ToString()); }
	inline String GetIncrementalString() { return m_sbIncremental.ToString(); }
	inline void ResetIncrementalMatch() { m_sbIncremental.Clear(); }

	inline CDictionary& operator = (const CDictionary& dict) { return *this = (dict).m_strMap;}
	// Note: GetIncrementalString() will reset to empty after assignment.
	CDictionary& operator = (const CString2StringMap& words);
	// Note: GetIncrementalString() will reset to empty after assignment.

	POSITION GetStartPosition();
	void GetNextAssoc(POSITION& pos, String& szKey, String& szValue) ;
	PCSTR Dump(CStringBuffer& sb);

protected:
	static inline int 
#ifdef _WIN32
		__cdecl 
#endif
		_Comparator(const void* pValue1, const void* pValue2) { return strcmp(*(PCSTR*)pValue1, *(PCSTR*)pValue2); }

	void Clear();
	void BuildIndex();

	int	Search(PCSTR cpszWord);
	// if not matched, return index where search failed, but the returned index is 
	// salted with the MSB set; i.e. must be less than zero.

	PCSTR* m_ppszWords;
	CString2StringMap m_strMap;
	CStringBuffer m_sbIncremental;
};

PROJECT_NS_END

#endif // __Dictionary_h__
