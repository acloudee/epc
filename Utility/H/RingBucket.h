#ifndef __RingBucket_h__
#define __RingBucket_h__

#include "CircularBucket.h"
#include "Semaphore.h"

PROJECT_NS_BEGIN

/****************************************************************************************
 * Thread safe ring buffer, 
 * on write it will wait until an empty buffer is available
 * on read it will wait until a bucket has been written			
****************************************************************************************/

class CRingBucket // Thread safe ring buffer
{
public:
	CRingBucket(int nBuckets, int nBucketSize); // to be a meaningfull circular buffer the nBuckets must > 0
	virtual ~CRingBucket();
	int Write(const PBYTE pbData, int nTimeoutMs=INFINITE, int nSize=-1); // -1 for the whole bucket, return the size written
	// on write it will wait until an empty buffer is available
	// nSize -1 for writing in unit of the bucket size,  
	//			if nSize is larger than nBucketSize, it will only write nBucketSize data.
	// return	the size writtern into bucket,
	int Read(PBYTE pbDataint, int nTimeoutMs=INFINITE, int nSize=-1); // -1 for the whole bucket, return the size written
	// on read it will wait until a bucket has been written			
	// nSize -1 for reading the whole bucket size,  
	//			if nSize is larger than nBucketSize, it will only reade nBucketSize data.
    // return	the valid size read from next 
	inline int GetValidRead() { return m_circularBucket.GetValidRead(); }
	// return # of consecutive could be performed without any other write.
	// the range would be between 0 and nBucketSize (inclusive

	inline int GetBuckets() { return m_circularBucket.GetBuckets(); }
	// the actual bucket consumed will be one larger than usable buckets
	// this is due to the characteristics of ring/circular buffer
	inline int GetBucketSize() { return m_circularBucket.GetBucketSize(); }

private:
	CSemaphore m_semaReadBuckets, m_semaWriteBuckets;
	CircularBucket m_circularBucket;
};

PROJECT_NS_END

#endif // __RingBucket_h__
