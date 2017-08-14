#ifndef __CircularBucket_h__
#define __CircularBucket_h__

#include "Common.h"
#include "ReEntrantMutex.h"
 
PROJECT_NS_BEGIN

/****************************************************************************************
 * Thread safe circular buffer, 
 * on write it will overwrite the oldest buffer as necessary,
 * on read it will read the oldest buffer first unless no newer buffer could be read			
****************************************************************************************/
class CircularBucket 
{
public:
	CircularBucket(int nBuckets=0, int nBucketSize=0); // to be a meaningfull circular buffer the nBuckets must > 0; i.e., at least 1
	virtual ~CircularBucket();
	bool Resize(int nBuckets, int nBucketSize);// to be a meaningfull circular buffer the nBuckets must > 0; i.e., at least 1
	int Write(const PBYTE pbData, int nSize=-1); // -1 for the whole bucket, return the size copied
	// on write it will overwrite the oldest buffer as necessary,
	// nSize -1 for writing in unit of the bucket size,  
	//			if nSize is larger than nBucketSize, it will only write nBucketSize data.
	// return	the size writtern into bucket,
	int Read(PBYTE pbData, int nSize=-1); 
	// on read it will read the oldest buffer first unless no newer buffer could be read
	// nSize -1 for reading the whole bucket size,  
	//			if nSize is larger than nBucketSize, it will only reade nBucketSize data.
	// return	the valid size read from next bucket,
	int GetValidRead();
	// return # of consecutive could be performed without any other write.
	// the range would be between 0 and nBucketSize (inclusive
	inline int GetBuckets() { return m_nBuckets ? m_nBuckets - 1 : 0; }
	// the actual bucket consumed will be one larger than usable buckets
	// this is due to the characteristics of ring/circular buffer
	inline int GetBucketSize() { return m_nBucketSize - sizeof(int); }
	

private:
	CReEntrantMutex m_mutex;
	int m_nBucketSize;
	int m_nBuckets;
	PBYTE m_pbRing, m_pbTail, m_pbHead, m_pbLimit;
};

PROJECT_NS_END
#endif // __CircularBucket_h__
