#ifndef __StringMap_h__
#define __StringMap_h__

#include "HashMap.h"
#include "Strings.h"
 
PROJECT_NS_BEGIN

/*******************************************************************************************
 *	All String related maps are derived from CHashMap.
 *	Those classes override protected virtual functions inherited from CHashMap as necessary:
 *  protect:
 *		virtual inline int HashKey(PVOID pKey) { return (int)pKey; }
 *		virtual inline int HashValue(PVOID pValue) { return (int)pValue; }
 *		virtual inline bool EqualKey(PVOID pKey1, PVOID pKey2) { return pKey1 == pKey2; }
 *		virtual inline bool EqualValue(PVOID pValue1, PVOID pValue2) { return pValue1 == pValue2; }
 *		virtual inline PVOID OnAddKey(PVOID pKey) { return pKey; }
 *		virtual inline PVOID OnAddValue(PVOID pValue) { return pValue; }
 *		virtual inline void OnRemoveKey(PVOID pKey) {}
 *		virtual inline void OnRemoveValue(PVOID pValue) {}
 *	Thus, all these string related classes can used functions below without any modification (
 *	nor should redefine them) thanks to class inheritance.
 *	    bool ContainsKey(PVOID pKey);
 *	    bool ContainsValue(PVOID pValue);
 *	    void PutAll(const CHashMap& map);
 *	    POSITION GetStartPosition() const;
 *	   void Clear();
 *	Generally, you coould treat all PVOID as PCSTR whenever appropriate.
 ********************************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// CString2StringMap
/////////////////////////////////////////////////////////////////////////////
class CString2StringMap : public CHashMap
{
public:
	CString2StringMap(int nInitialCapacity=11, float fLoadFactor = 0.75);
    CString2StringMap(const CString2StringMap& map);
	virtual ~CString2StringMap() ;
    String Get(PCSTR cpszKey);
    String Put(PCSTR cpszKey, PCSTR cpszValue);
    /* @return previous value associated with specified key, or <tt>null</tt>
     *	       if there was no mapping for key.  A <tt>null</tt> return can
     *	       also indicate that the map previously associated <tt>null</tt>
     *	       with the specified key.
     */
    String Remove(PCSTR cpszKey);
   /**
     * Removes the mapping for this key from this map if present.
     *
     * @param key key whose mapping is to be removed from the map.
     * @return previous value associated with specified key, or <tt>null</tt>
     *	       if there was no mapping for key.  A <tt>null</tt> return can
     *	       also indicate that the map previously associated <tt>null</tt>
     *	       with the specified key.
     */

	void GetNextAssoc(POSITION& pos, String& szKey, String& szValue);
	CString2StringMap& operator = (const CString2StringMap& map);
	inline String operator[](PCSTR cpszKey) { return Get(cpszKey); }
	// CString2StringMap map
	// map["hello"] = "xyz";// not supported yet!
	// String strHello = map["hello"];
	// those functions could used from CHashMap without any modifications thanks to class inheritance
	// you could treat PVOID as PCSTR
	//    bool ContainsKey(PVOID pKey);
	//    bool ContainsValue(PVOID pValue);
	//    void PutAll(const CHashMap& map);
	//    POSITION GetStartPosition() const;
	//   void Clear();

protected:
	virtual inline int HashKey(PVOID pKey) { return HashString((PCSTR)pKey); }
	virtual inline int HashValue(PVOID pValue) { return HashString((PCSTR)pValue); }
	virtual inline bool EqualKey(PVOID pKey1, PVOID pKey2) { return EqualString((PCSTR)pKey1, (PCSTR)pKey2); }
	virtual inline bool EqualValue(PVOID pValue1, PVOID pValue2) { return EqualString((PCSTR)pValue1, (PCSTR)pValue2); }
	virtual inline PVOID OnAddKey(PVOID pKey) { return NewString((PCSTR)pKey); }
	virtual inline PVOID OnAddValue(PVOID pValue) { return NewString((PCSTR)pValue); }
	virtual inline void OnRemoveKey(PVOID pKey) { RemoveString((PSTR)pKey); }
	virtual inline void OnRemoveValue(PVOID pValue)  { RemoveString((PSTR)pValue); }
};

/////////////////////////////////////////////////////////////////////////////
// CString2PtrMap
/////////////////////////////////////////////////////////////////////////////
class CString2PtrMap : public CHashMap
{
public:
	CString2PtrMap(int nInitialCapacity=11, float fLoadFactor = 0.75);
    CString2PtrMap(CString2StringMap& map);
	virtual ~CString2PtrMap();
    inline PVOID Get(PCSTR cpszKey) { return CHashMap::Get((PVOID)cpszKey); }
    inline PVOID Put(PCSTR cpszKey, PVOID pValue) { return CHashMap::Put((PVOID)cpszKey, pValue); }
    inline PVOID Remove(PCSTR cpszKey) { return CHashMap::Remove((PVOID)cpszKey); }

	void GetNextAssoc(POSITION& pos, String& szKey, PVOID& pValue);
	CString2PtrMap& operator = (const CString2PtrMap& map);
	inline PVOID	operator[](PCSTR cpszKey) { return Get(cpszKey); }
	// CString2PtrMap map
	// map["hello"] = new char[10];// not supported yet!
	// char* cptr = (char*)map["hello"];
	// functions below could used from CHashMap without any modifications thanks to class inheritance
	// you could treat PVOID as PCSTR
	//    bool ContainsKey(PVOID pKey);
	//    bool ContainsValue(PVOID pValue);
	//    void PutAll(const CHashMap& map);
	//    POSITION GetStartPosition() const;
	//   void Clear();

protected:
	virtual inline int HashKey(PVOID pKey) { return HashString((PCSTR)pKey); }
	virtual inline bool EqualKey(PVOID pKey1, PVOID pKey2) { return EqualString((PCSTR)pKey1, (PCSTR)pKey2); }
	virtual inline PVOID OnAddKey(PVOID pKey) { return NewString((PCSTR)pKey); }
	virtual inline void OnRemoveKey(PVOID pKey) { RemoveString((PSTR)pKey); }
};

/////////////////////////////////////////////////////////////////////////////
// CPtr2StringMap
/////////////////////////////////////////////////////////////////////////////
class CPtr2StringMap : public CHashMap
{
public:
	CPtr2StringMap(int nInitialCapacity=11, float fLoadFactor = 0.75) ;
    CPtr2StringMap(CString2StringMap& map);
	virtual ~CPtr2StringMap();
    String Get(PVOID pKey);
    String Put(PVOID pKey, PCSTR cpszValue);
    String Remove(PVOID pKey);

	void GetNextAssoc(POSITION& pos, PVOID& szKey, String& szValue);
	CPtr2StringMap& operator = (const CPtr2StringMap& map);
	inline String operator[](PVOID pKey) { return Get(pKey); }
	// CPtr2StringMap map
	// map[33] = "hello";// not supported yet!
	// String strHello = map[33];
	// those functions could used from CHashMap without any modifications thanks to class inheritance
	// you could treat PVOID as PCSTR
	//    bool ContainsKey(PVOID pKey);
	//    bool ContainsValue(PVOID pValue);
	//    void PutAll(const CHashMap& map);
	//    POSITION GetStartPosition() const;
	//   void Clear();


protected:
	virtual inline int HashValue(PVOID pValue) { return HashString((PCSTR)pValue); }
	virtual inline bool EqualValue(PVOID pValue1, PVOID pValue2) { return EqualString((PCSTR)pValue1, (PCSTR)pValue2); }
	virtual inline PVOID OnAddValue(PVOID pValue) { return NewString((PCSTR)pValue); }
	virtual inline void OnRemoveValue(PVOID pValue)  { RemoveString((PSTR)pValue); }
};

PROJECT_NS_END

#endif // __StringMap_h__
