#ifndef __Accessor_h__
#define __Accessor_h__
 
#include "Mutex.h"

PROJECT_NS_BEGIN

class CAccessor
// allows for multiple reader concurrently  but 
// only one writer at a time.
{
public:
	CAccessor();
	virtual ~CAccessor();
	TAKE_STATUS_CODE TakeReader(int nTimeoutMs=INFINITE); //Wait for Reader
	void ReleaseReader();
	inline TAKE_STATUS_CODE TakeWriter(int nTimeoutMs=INFINITE) { return m_mtxWriter.Take(nTimeoutMs); }
	inline void ReleaseWriter() { m_mtxWriter.Give(); }
	
protected:
	CMutex m_mtxReader;
	CMutex m_mtxWriter;
	unsigned int m_nReader;	
};


PROJECT_NS_END

#endif // __Accessor_h__
