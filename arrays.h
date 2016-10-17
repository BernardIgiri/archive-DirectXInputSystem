/*****************************************************************************
//////////////////////////////////////////////////////////////////////////////

  DATE	:	12/29/2003
  AUTHOR:	Bernard Igiri

  "arrays.h"

  This is a collection of templated array classes for use for use with
  dynamic memory allocation.

///////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifndef __INCLUDED_ARRAYS_H__
#define __INCLUDED_ARRAYS_H__

/*****************************************************************************

  Templates:
  template <class T,class InitT> class CSclArray_t
	A simple scalable array, type InitT* will be use as an initilizer for
	all type T objects.  Type T objecs must have a CreateMe member function
	defined as follows:
	void CreateMe(InitT* pInit,int idx,int relIdx);
	pInit is the initialization data, the idx is the index of the new
	object in the array, and relIdx is the relative index of the object
	relative to the current call to create more objects.  If you specify an
	array of objects to create, this will be the index into that array.
	CreateMe should operate just as the objects constructor does.
	CreateMe is called after the memory has been cleared, to protect CreateMe
	make it a private function and define class CSclArray_t as a friend class.
	
  template <class T,class InitT> class CPtrArray_t
	An array of pointers this array allows elements to be deleted without
	affecting the rest of the array. It allows array indexes to be reused
	Type InitT* will be use as an initializer for all type T objects.
	The size of the array in memory scales up according to step size.
	Initializing CPtrArray_t with a value for unsinged n will step the step
	size on initialization. This methods requires that objects be defined
	with an a constructor that supports the form of...
	T(InitT*,int,int);
	InitT* is the initialization data, the first int is the index of the new
	object in the array, and the second is the relative index of the object
	relative to the current call to create more objects.  If you specify an
	array of objects to create, this will be the index into that array.

******************************************************************************/


// scaling array
template <class T,class InitT> class CSclArray_t
{
public:
	T*		 m_ptr; // pointer to array
	unsigned m_counter; // number of elements in the array

	// Initializers
	CSclArray_t();
	CSclArray_t(unsigned n,InitT* pInit);
	~CSclArray_t();

	// Destroys the array's contents
	void Destroy();

	// Adds an element into the array, returns false on failure
	bool AddElmts(unsigned n,InitT* pInit);
};

// pointer array
template <class T,class InitT> class CPtrArray_t
{
public:
	// Initializers
	CPtrArray_t();
	CPtrArray_t(unsigned n,InitT* pInit);
	~CPtrArray_t();

	// Destroys the array's contents
	void Destroy();
	// Removes all the elements from the array
	void RemoveAll();

	// Adds an element into the array, returns false on failure
	bool InsertElmts(unsigned n,InitT* pInit);
	// Adds an element into the array, elmtIdx = index of last created element
	bool InsertElmts(unsigned n,InitT* pInit,unsigned *elmtIdx);
	
	// Removes specifed elements from array
	void RemoveElmts(const unsigned *pIndices,unsigned count);
	// Sets the step size for resizing the array
	void SetStepSize(unsigned value);

	// Searches for given element, returns true on success and sets idx = index of found element
	bool FindElmt(const InitT* key,unsigned *idx);
	// Returns pointer to element at given index
	const T* GetElmt(unsigned idx) const;
	
	// Returns requested unsigned integer values
	unsigned GetSize() const;
	unsigned GetStepSize() const;
	unsigned GetNumOfElmts() const;
private:
	inline void RemoveElmt(unsigned idx);
	bool IncrSize(unsigned nUnits);
	void CalcNxLwFrIdx();

	T**		 m_pList; // array
	unsigned m_nElmts; // # of spaces occupied in array
	unsigned m_nSize; // # of units in array
	unsigned m_nStepSize; // min # of units which array increases by
	unsigned m_nxLwFrIdx; // next lowest free index
};

// dynamic step sizing array
template <class T,class InitT> class CDSArray_t
{
public:
	typedef int (*COMPFUNC)(const T *key1, const T *key2);
	typedef bool (*INITFUNC)(T *obj,InitT *params,int arIdx,int relIdx);

	// Initializers
	CDSArray_t();
	CDSArray_t(unsigned n,InitT* pInit);
	~CDSArray_t();

	// Sets values for the initialization of the array, NULL values are accepted, except for with the step size
	void Init(COMPFUNC pCompFunc,INITFUNC pInitFunc,bool isOwner,unsigned stepSize);

	// Destroys the array's contents
	void Destroy();
	// Removes all the elements from the array
	void RemoveAll();

	// Adds new elements to the array and returns 1+index or NULL on failure
	unsigned AddElmts(unsigned n,InitT* pInit);
	// Increases the size of the array in memory, returns false on failure
	bool IncrSize(unsigned nUnits);

	// Returns the index+1 of the requested element, or NULL on failure
	unsigned FindElmt(const T* key) const;
	// Returns a pointer the element at id-1, or NULL on failure
	const T* GetElmt(unsigned id) const;

	COMPFUNC m_pCompFunc; // comparison function
	INITFUNC m_pInitFunc; // initialization function
	T*		 m_pArray; // array
	unsigned m_nElmts; // # of elements in array
	unsigned m_nSize; // size of array in memory
	unsigned m_nStepSize; // min # of units which array increases by
	bool	 m_isOwner; // if true object destructors willl be called upon array destruction
};

/******************************************************************************

  CSclArray_t.

******************************************************************************/

template <class T,class InitT>
CSclArray_t<T,InitT>::CSclArray_t()
{
	m_ptr=NULL;
	m_counter=0;
}

template <class T,class InitT>
CSclArray_t<T,InitT>::CSclArray_t(unsigned n,InitT* pInit)
{
	m_ptr=NULL;
	m_counter=0;
	AddElmts(n,pInit);
}

template <class T,class InitT>
CSclArray_t<T,InitT>::~CSclArray_t()
{
	Destroy();
}

template <class T,class InitT>
void CSclArray_t<T,InitT>::Destroy()
{
	// destruct elements
	while (m_counter--)
		(m_ptr + m_counter)->~T();
	// free memory
	if (m_ptr)
		free(m_ptr);
	m_counter=0;
	m_ptr = NULL;
}

template <class T,class InitT>
bool CSclArray_t<T,InitT>::AddElmts(unsigned n,InitT* pInit)
{
	// reject zeros
	if (!n)
		return true;
	// increment m_counter
	m_counter+=n;
	// allocate ram
	if (!m_ptr)
		m_ptr = (T*)malloc(sizeof(T)*m_counter);
	else
		m_ptr = (T*)realloc(m_ptr,sizeof(T)*m_counter);
	// check pointer
	if (!m_ptr)
	{
		m_counter-=n;
		return false;
	}
	// initialize objects
	memset(m_ptr+(m_counter-n),NULL,sizeof(T)*n); // clear ram
	int i = m_counter;
	while (n--) // call initializers
	{
		i--;
		(m_ptr + i)->CreateMe(pInit,i,n);
	}
	return true;
}

/******************************************************************************

  CPtrArray_t.

******************************************************************************/

template <class T,class InitT>
CPtrArray_t<T,InitT>::CPtrArray_t()
{
	m_pList	 = NULL;
	m_nElmts = 0;
	m_nSize	 = 0;
	m_nStepSize = 1;
	m_nxLwFrIdx = 0;
}

template <class T,class InitT>
CPtrArray_t<T,InitT>::CPtrArray_t(unsigned n,InitT* pInit)
{
	m_pList	 = NULL;
	m_nElmts = 0;
	m_nSize	 = 0;
	m_nStepSize = n;
	if (!m_nStepSize) // step size is at least = 1
		m_nStepSize = 1;
	m_nxLwFrIdx = 0;
	InsertElmts(n,pInit);
}

template <class T,class InitT>
CPtrArray_t<T,InitT>::~CPtrArray_t()
{
	Destroy();
}

template <class T,class InitT>
void CPtrArray_t<T,InitT>::Destroy()
{
	// destruct elements
	RemoveAll();
	// free memory
	if (m_pList)
		free(m_pList);
	m_pList = NULL;
	m_nxLwFrIdx = 0;
	m_nSize = 0;
}

template <class T,class InitT>
void CPtrArray_t<T,InitT>::RemoveAll()
{
	// destruct all elements
	unsigned i=m_nSize;
	while (m_nElmts&&(i--))
		if (*(m_pList + i))
		{
			delete *(m_pList + i);
			*(m_pList + i)=NULL;
			m_nElmts--;
		}
	m_nElmts = 0;
}

template <class T,class InitT>
bool CPtrArray_t<T,InitT>::FindElmt(const InitT* key,unsigned *idx)
{
	int i=m_nSize;
	while (i--)
		if (*(m_pList + i))
			if ((*(m_pList + i))->CompareWith((InitT*)key)==NULL)
			{
				*idx=i;
				return true;
			}
	return false;
}

template <class T,class InitT>
bool CPtrArray_t<T,InitT>::IncrSize(unsigned nUnits)
{
	// increase by m_nStepSize or more
	if (nUnits<m_nStepSize)
		nUnits=m_nStepSize;
	// if the next lowest free index >= size, set equal to size
	if (m_nxLwFrIdx>=m_nSize)
		m_nxLwFrIdx=m_nSize;
	// increment m_nSize
	m_nSize+=nUnits;
	// allocate ram for array of pointers
	if (!m_pList)
		m_pList = (T**)malloc(sizeof(T*)*m_nSize);
	else
		m_pList = (T**)realloc(m_pList,sizeof(T*)*m_nSize);
	// check pointer
	if (!m_pList)
	{
		m_nSize=0;
		return false;
	}
	// clear newly allocated ram
	memset(m_pList+(m_nSize-nUnits),NULL,sizeof(T*)*nUnits);
	return true;
}

template <class T,class InitT>
bool CPtrArray_t<T,InitT>::InsertElmts(unsigned n,InitT* pInit)
{
	// reject zeros
	if (!n)
		return true;
	// check if there is enough space left
	if ((m_nSize-m_nElmts)<n) // if not then make it
		if (!IncrSize(n-(m_nSize-m_nElmts)))
			return false; // increase size failed
	// if next lowest free index is too high, then recalc next lowest free index
	if (m_nxLwFrIdx>=m_nSize)
		CalcNxLwFrIdx();
	// create objects
	int i = m_nxLwFrIdx;
	while (n)
	{
		if (!*(m_pList + i)) // if space is blank then fill it
		{ // found blank space
			n--; // decrement number of new objects left
			*(m_pList + i) = new T(pInit,i,n); // create object
			if (!*(m_pList + i)) // if new failed
				return false; // return false, out of memory
			m_nElmts++; // increment number of elements
		}
		i++;
	}
	m_nxLwFrIdx=i;
	return true;
}

template <class T,class InitT>
bool CPtrArray_t<T,InitT>::InsertElmts(unsigned n,InitT* pInit,unsigned *elmtIdx)
{
	// reject zeros
	if (!n)
		return true;
	// check if there is enough space left
	if ((m_nSize-m_nElmts)<n) // if not then make it
		IncrSize(n-(m_nSize-m_nElmts));
	// if next lowest free index is too high, then recalc next lowest free index
	if (m_nxLwFrIdx>=m_nSize)
		CalcNxLwFrIdx();
	// create objects
	int i = m_nxLwFrIdx;
	while (n)
	{
		if (!*(m_pList + i)) // if space is blank then fill it
		{ // found blank space
			n--; // decrement number of new objects left
			// get index of element
			*elmtIdx=i;
			*(m_pList + i) = new T(pInit,i,n); // create object
			if (!*(m_pList + i)) // if new failed
				return false; // return false, out of memory
			m_nElmts++; // increment number of elements
		}
		i++;
	}
	m_nxLwFrIdx=i;
	return true;
}

template <class T,class InitT>
inline void CPtrArray_t<T,InitT>::RemoveElmt(unsigned idx)
{
	// destroy element at index
	if ( *(m_pList + idx) )
	{
		delete *(m_pList + idx);
		// nullify pointer
		*(m_pList + idx) = NULL;
		// decrement number of elements
		m_nElmts--;
	}
	// update next lowest free index
	if (idx<m_nxLwFrIdx)
		m_nxLwFrIdx = idx;
}

template <class T,class InitT>
void CPtrArray_t<T,InitT>::RemoveElmts(const unsigned *pIndices,unsigned count)
{
	if (!pIndices||!count)
		return;
	while (count--)
		if (*(pIndices+count)<m_nSize)
			RemoveElmt(*(pIndices+count));
}

template <class T,class InitT>
void CPtrArray_t<T,InitT>::SetStepSize(unsigned value)
{
	if (value)	
		m_nStepSize = value;
}

template <class T,class InitT>
unsigned CPtrArray_t<T,InitT>::GetSize() const
{
	return m_nSize;
}

template <class T,class InitT>
unsigned CPtrArray_t<T,InitT>::GetStepSize() const
{
	return m_nStepSize;
}

template <class T,class InitT>
unsigned CPtrArray_t<T,InitT>::GetNumOfElmts() const
{
	return m_nElmts;
}

template <class T,class InitT>
const T* CPtrArray_t<T,InitT>::GetElmt(unsigned idx) const
{
	if (idx<m_nSize)
		return *(m_pList + idx);
	return NULL;
}

template <class T,class InitT>
void CPtrArray_t<T,InitT>::CalcNxLwFrIdx()
{
	unsigned i=0;
	for (i=0; i<m_nSize; i++)
		if (!*(m_pList + i))
			break;
	m_nxLwFrIdx = i;
}

/******************************************************************************

  CDSArray_t.

******************************************************************************/

template <class T,class InitT>
CDSArray_t<T,InitT>::CDSArray_t()
{
	m_pCompFunc=NULL;
	m_pInitFunc=NULL;
	m_pArray=NULL;
	m_nElmts=0;
	m_nSize=0;
	m_nStepSize=1;
	m_isOwner=false;
}

template <class T,class InitT>
CDSArray_t<T,InitT>::CDSArray_t(unsigned n,InitT* pInit)
{
	m_pCompFunc=NULL;
	m_pInitFunc=NULL;
	m_pArray=NULL;
	m_nElmts=0;
	m_nSize=0;
	m_nStepSize=n;
	m_isOwner=false;
	AddElmts(n,pInit);
}

template <class T,class InitT>
CDSArray_t<T,InitT>::~CDSArray_t()
{
	Destroy();
	m_pCompFunc=NULL;
	m_pInitFunc=NULL;
	m_pArray=NULL;
	m_nElmts=0;
	m_nSize=0;
	m_nStepSize=1;
	m_isOwner=false;
}

template <class T,class InitT>
void CDSArray_t<T,InitT>::Init(COMPFUNC pCompFunc,INITFUNC pInitFunc,bool isOwner,unsigned stepSize)
{
	m_pCompFunc = pCompFunc;
	m_pInitFunc = pInitFunc;
	m_isOwner	= isOwner;
	m_nStepSize	= stepSize;
}

template <class T,class InitT>
void CDSArray_t<T,InitT>::Destroy()
{
	RemoveAll();
	if (m_pArray)
		free(m_pArray);
	m_pArray=NULL;
	m_nSize=0;
}

template <class T,class InitT>
void CDSArray_t<T,InitT>::RemoveAll()
{
	if (m_isOwner)
		while (m_nElmts--)
			(m_pArray+m_nElmts)->~T();
	m_nElmts=0;
}

template <class T,class InitT>
unsigned CDSArray_t<T,InitT>::AddElmts(unsigned n,InitT* pInit)
{
	// reject zeros
	if (!n)
		return NULL;
	// copy id of first new element
	unsigned firstID=m_nElmts+1; // id=index+1
	// check if there is enough space left
	if ((m_nSize-m_nElmts)<n) // if not then make it
		if (!IncrSize(n-(m_nSize-m_nElmts)))
		{
			m_nElmts-=n;
			return NULL; // increase size failed
		}
	// increment m_nElmts
	m_nElmts+=n;
	// initialize objects
	int i = m_nElmts;
	if (m_pInitFunc)
	{
		while (n--) // call initializers
		{
			i--;
			if (!m_pInitFunc(m_pArray+i,pInit,i,n))
			{
				m_nElmts-=n;
				return NULL;
			}
		}
	}
	return firstID;
}

template <class T,class InitT>
bool CDSArray_t<T,InitT>::IncrSize(unsigned nUnits)
{
	// increase by m_nStepSize or more
	if (nUnits<m_nStepSize)
		nUnits=m_nStepSize;
	// increment m_nSize
	m_nSize+=nUnits;
	// allocate ram
	if (!m_pArray)
		m_pArray = (T*)malloc(sizeof(T)*m_nSize);
	else
		m_pArray = (T*)realloc(m_pArray,sizeof(T)*m_nSize);
	// check pointer
	if (!m_pArray)
		return false;
	return true;
}

template <class T,class InitT>
unsigned CDSArray_t<T,InitT>::FindElmt(const T* key) const
{
	if (!m_pCompFunc)
		return NULL;
	int i=m_nElmts;
	while (i--)
		if (m_pCompFunc(m_pArray+i,key)==NULL)
			return i+1; // id=index+1
	return NULL;
}

template <class T,class InitT>
const T* CDSArray_t<T,InitT>::GetElmt(unsigned id) const
{
	if ((id<=m_nElmts)&&(id>0))
		return m_pArray+(id-1); // id=index+1
	return NULL;
}

#endif//__INCLUDED_ARRAYS_H__