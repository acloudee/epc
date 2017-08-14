#ifndef __HashMap_h__
#define __HashMap_h__

#include "Common.h"
#include <string.h>

PROJECT_NS_BEGIN

class CHashMap 
{
public:
    /**
     * Constructs a new, empty map with the specified initial 
     * capacity and the specified load factor. 
     *
     * @param      initialCapacity   the initial capacity of the HashMap.
     * @param      loadFactor        the load factor of the HashMap
     */
	CHashMap(int nInitialCapacity=11, float fLoadFactor = 0.75);

    /**
     * Constructs a new map with the same mappings as the given map.  The
     * map is created with a capacity of twice the number of mappings in
     * the given map or 11 (whichever is greater), and a default load factor,
     * which is <tt>0.75</tt>.
     */
    CHashMap(const CHashMap& map);
	virtual ~CHashMap(); 
    inline int GetSize() const { return m_nCount; }
    inline bool IsEmpty() { return m_nCount == 0; }

    /**
     * Returns <tt>true</tt> if this map maps one or more keys to the
     * specified value.
     *
     * @param value value whose presence in this map is to be tested.
     * @return <tt>true</tt> if this map maps one or more keys to the
     *         specified value.
     */
    bool ContainsValue(const PVOID pValue);

    /**
     * Returns <tt>true</tt> if this map contains a mapping for the specified
     * key.
     * 
     * @return <tt>true</tt> if this map contains a mapping for the specified
     * key.
     * @param key key whose presence in this Map is to be tested.
     */
    bool ContainsKey(const PVOID pKey);
    /**
     * Returns the value to which this map maps the specified key.  Returns
     * <tt>null</tt> if the map contains no mapping for this key.  A return
     * value of <tt>null</tt> does not <i>necessarily</i> indicate that the
     * map contains no mapping for the key; it's also possible that the map
     * explicitly maps the key to <tt>null</tt>.  The <tt>containsKey</tt>
     * operation may be used to distinguish these two cases.
     *
     * @return the value to which this map maps the specified key.
     * @param key key whose associated value is to be returned.
     */
    PVOID Get(PVOID pKey);
    /**
     * Associates the specified value with the specified key in this map.
     * If the map previously contained a mapping for this key, the old
     * value is replaced.
     *
     * @param key key with which the specified value is to be associated.
     * @param value value to be associated with the specified key.
     * @return previous value associated with specified key, or <tt>null</tt>
     *	       if there was no mapping for key.  A <tt>null</tt> return can
     *	       also indicate that the HashMap previously associated
     *	       <tt>null</tt> with the specified key.
     */
    PVOID Put(PVOID pKey, PVOID pValue);

    /**
     * Removes the mapping for this key from this map if present.
     *
     * @param key key whose mapping is to be removed from the map.
     * @return previous value associated with specified key, or <tt>null</tt>
     *	       if there was no mapping for key.  A <tt>null</tt> return can
     *	       also indicate that the map previously associated <tt>null</tt>
     *	       with the specified key.
     */
    PVOID Remove(PVOID pKey);
    /**
     * Copies all of the mappings from the specified map to this one.
     * 
     * These mappings replace any mappings that this map had for any of the
     * keys currently in the specified Map.
     *
     * @param t Mappings to be stored in this map.
     */
    void PutAll(const CHashMap& map);

	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& pos, PVOID& pKey, PVOID& pValue);
	CHashMap& operator = (const CHashMap& map);
	PVOID operator[](PVOID pKey) { return Get(pKey); }
	// CHashMap map
	// map[33] = new char[10];	// not supported yet!
	// char* cptr = (char*)map[33];

    /**
     * Removes all mappings from this map.
     */
    void Clear();

protected:
	virtual inline int HashKey(PVOID pKey) { return (int)pKey; }
	virtual inline int HashValue(PVOID pValue) { return (int)pValue; }
	virtual inline bool EqualKey(PVOID pKey1, PVOID pKey2) { return pKey1 == pKey2; }
	virtual inline bool EqualValue(PVOID pValue1, PVOID pValue2) { return pValue1 == pValue2; }
	virtual inline PVOID OnAddKey(PVOID pKey) { return pKey; }
	virtual inline PVOID OnAddValue(PVOID pValue) { return pValue; }
	virtual inline void OnRemoveKey(PVOID pKey) {}
	virtual inline void OnRemoveValue(PVOID pValue) {}

/**
     * Returns a hashcode for this string. The hashcode for a 
     * <code>String</code> object is computed as
     * <blockquote><pre> 
     * s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]
     * </pre></blockquote>
     * using <code>int</code> arithmetic, where <code>s[i]</code> is the 
     * <i>i</i>th character of the string, <code>n</code> is the length of 
     * the string, and <code>^</code> indicates exponentiation. 
     * (The hash value of the empty string is zero.)
     *
     * @return  a hash code value for this object. 
     */
	int HashString(PCSTR cpszValue);
	bool EqualString(PCSTR cpszValue1, PCSTR cpszValue2);
	PSTR NewString(PCSTR cpszValue);
	void RemoveString(PSTR pszValue);

private:
	class CEntry
	{
	public:
		CEntry(int nHash, PVOID pKey, PVOID pValue, CEntry* pNext);
	// Map.Entry Ops 
		inline PVOID GetKey() { return m_pKey; }
		inline PVOID GetValue() { return m_pValue; }
		inline int GetHash() { return m_nHash;}
		inline CEntry* GetNext() { return m_pNext; }
		PVOID SetValue(PVOID pValue);
		inline void SetNext(CEntry* pNext) { m_pNext = pNext; }

		inline bool operator == (CEntry& entry) { return m_pKey == entry.GetKey() && m_pValue == entry.GetValue();}
	//	inline int hashCode() { return m_nHash ^ (m_pValue ? CHashMap::HashValue(m_pValue) : 0); }

	protected:
		CEntry* Clone(); 
	private:
		CEntry* m_pNext;
		int m_nHash;
		PVOID m_pKey;
		PVOID m_pValue;
	};

private:
    /**
     * The number of times this HashMap has been structurally modified
     * Structural modifications are those that change the number of mappings in
     * the HashMap or otherwise modify its internal structure (e.g.,
     * rehash).  This field is used to make iterators on Collection-views of
     * the HashMap fail-fast.  (See ConcurrentModificationException).
     */
	void Init(int nInitialCapacity=11, float fLoadFactor=0.75);
    /**
     * Rehashes the contents of this map into a new <tt>HashMap</tt> instance
     * with a larger capacity. This method is called automatically when the
     * number of keys in this map exceeds its capacity and load factor.
     */
	void Rehash();

private:
	CEntry** m_ppEntries;	// the hash table data
	int m_nCount;	// The total number of mappings in the hash table.
	int m_nCapacity;
	int m_nThreshold;	
	// The table is rehashed when its size exceeds this threshold.  (The
     // value of this field is (int)(capacity * loadFactor).)
	float m_fLoadFactor; // The load factor for the hashtable.
	int m_nModCount;
};

PROJECT_NS_END

#endif // __HashMap_h__
