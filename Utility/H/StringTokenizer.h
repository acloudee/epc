#ifndef __StringTokenizer_h__
#define __StringTokenizer_h__

#include "StringList.h"
 
PROJECT_NS_BEGIN
/*//////////////////////////////////////////////////////////////
	PCSTR cpszSrc = "mark,mark2,jones,albert";
	PCSTR cpszTarget = "mark";
	CStringTokenizer st(cpszSrc, _T(","), false);
	String szOutcome;
	while (st.HasMoreTokens()) {
		String szToken = st.NextToken();
		if (szToken != cpszTarget) szOutcome += szToken + _T(",");
	}
	if (szOutcome.GetLength() > 0) szOutcome.Delete(szOutcome.GetLength()-1);
*/
class CStringTokenizer
{
public:
	static PCSTR s_cpszSpaces;	// "\n\r\t\a "

public:
	CStringTokenizer();
	CStringTokenizer(PCSTR cpszSrc, 
					PCSTR cpszDelimiters=NULL, // default is space-separated by any one of s_cpszSpaces;	// "\n\r\t\a "
					bool bEmptyToken=true,
					bool bMatchAll=false);// treat all cpszDelimiters as a whole
	int Tokenize(PCSTR cpszSrc,		// discard current tokens and re-tokenize a brand new string
					PCSTR cpszDelimiters=NULL, // default is space-separated by any one of s_cpszSpaces;	// "\n\r\t\a "
					bool bEmptyToken=true,
					bool bMatchAll=false);// treat all cpszDelimiters as a whole
	/******************************************************************
	 * cpszDelimiters: each character are served as token sepatator,
						where consecutive spaces are treat as one delimiter.
						pass NULL to use s_cpszSpaces [ "\n\r\t\a "]
	 * bEmptyToken: true => take two consecutive non-space delimiters as 
							empty token, 
					false => skip empty tokens
	 ***********************************************************************/
	virtual ~CStringTokenizer();// { TRACE(LogDebug);m_strlstTokens.RemoveAll(); }
	inline bool HasMoreTokens() { return m_posNext != NULL; }
	inline int CountTokens() { return m_strlstTokens.GetCount(); }
	inline int NextInt() { return atoi(NextToken()); }
	String NextToken();
	inline void Reset() { m_posNext = m_strlstTokens.GetHeadPosition(); }
	int GetTokens(CStringList& szlstTokens);
	// append all tokens onto the tail of the szlstTokens
	inline const CStringList* GetTokens() { return &m_strlstTokens; } // be careful, never try to delete this pointer!

	// convenient funciton
	String ToString(PCSTR cpszSeparator); 
	// cpszSeparator NULL or empty string is allowed to concatenate them

	inline bool HasToken(PCSTR cpszToken, bool bFromHead=true, bool bCaseSensitive=true) { return m_strlstTokens.Find(cpszToken, bFromHead ? NULL : m_posNext, bCaseSensitive) != NULL; }
	// cpszToken NULL is invalid and will return false;
	// bFromHead true to search from head; false to search from current pos
	// bCaseSensitive false if the comparison is case-INsensitive
	bool MoveTo(PCSTR cpszToken, bool bFromHead=true, bool bCaseSensitive=true);
	// cpszToken NULL is invalid and will return false;
	// bFromHead true to search from head; false to search from current pos
	// bCaseSensitive false if the comparison is case-INsensitive
	// return true if moved and you may call NextToken() to get it
	//        false if not found and not moved.
	
	inline void TrimAll() {	m_strlstTokens.TrimAll(); Reset(); }
	// strip off all spaces from all strings
	// On return the NextToken() will be reset (same as Reset())

private:
	void MatchAll(PSTR cpszSrc, PCSTR cpszDelimiters, bool bEmptyToken);

	CStringList m_strlstTokens;
	POSITION m_posNext;
};

PROJECT_NS_END

#endif //  __StringTokenizer_h__
