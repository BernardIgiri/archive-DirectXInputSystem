/*****************************************************************************
//////////////////////////////////////////////////////////////////////////////

  DATE	:	12/12/2003
  AUTHOR:	Bernard Igiri

  "advdtypes/advdtypes.h"

  This is a set of templated classes for collections of data such as
  lists, stacks, etc...

///////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifndef __INCLUDED_ADVDTYPES_H__
#define __INCLUDED_ADVDTYPES_H__

#include "../arrays.h"

template <class T> class List_t
{
public:
	/***********************************
	ListElmt class.
	***********************************/
	class ListElmt_t
	{
	public:
		ListElmt_t()
		{
			m_data = NULL;
			m_next = NULL;
		};
		~ListElmt_t() {};

		T*			m_data;
		ListElmt_t* m_next;
	};

	ListElmt_t*	m_head;
	ListElmt_t*	m_tail;
	unsigned	m_size;
	bool		m_isOwner; // if true list class will takeover destruction of objects 

	/***********************************
	List Constructor.
	***********************************/
	List_t()
	{
		// nullify data
		m_head=m_tail=NULL;
		m_size=0;
		m_isOwner = true;
	};

	/***********************************
	List Constructor.
	***********************************/
	List_t(bool isOwner)
	{
		// nullify data
		m_head=m_tail=NULL;
		m_size=0;
		m_isOwner = isOwner;
	};

	/***********************************
	List Destructor.
	***********************************/
	~List_t()
	{
		T *data;
		// Remove each element.
		while (m_size)
			if (RemNext(NULL,&data)&&m_isOwner)
				delete data;
		// nullify data
		m_head=m_tail=NULL;
	};

	/***********************************
	Sorts the elements in the list.
	***********************************/
	void Sort()
	{
		if (m_size<2)
			return;
		bool progress = false;
		ListElmt_t *key0=NULL, *key1=m_head, *key2=m_head->m_next;
		bool done = false;
		unsigned long int count = 0, limit = (m_size*m_size)/2;
		while (!done)
		{
			int comp = key1->m_data->CompareWith(key2->m_data);
			// make swap if key1 is greater that key2
			if (comp>NULL)
			{
				progress=true;
				if (NULL==key0)
					m_head = key2;
				else
					key0->m_next = key2;
				key1->m_next = key2->m_next;
				key2->m_next = key1;
				if (key1->m_next == NULL) // if now the tail set list tail to point to it
					m_tail=key1;
			}
			// advance
			key0=key1;
			key1=key2;
			key2=key2->m_next;
			if (key1->m_next == NULL)
			{			
				if (count++>limit)// prevent infinite loop
					done = true;
				if (!progress)
					done=true; // end if last cycle thru showed nothing out of sequential order
				else
				{ // restart at begining reset progress to check status of new pass
					key0=NULL;
					key1=m_head;
					key2=m_head->m_next;
					progress=false;
				}
			}
		}
	};

	/***********************************
	Sets result to the matching elmt.
	Or to NULL on failure
	***********************************/
	void Search(T *data,ListElmt_t **result)
	{
		*result=NULL;
		if (m_size==0)
			return;
		ListElmt_t *curElmt = m_head;
		do
		{
			if (curElmt->m_data->CompareWith(data)==NULL)
			{
				*result=curElmt;
				return;
			}
			curElmt = curElmt->m_next;
		} while (curElmt!=NULL);
	};

	/***********************************
	Inserts data after given elmt.
	Returns false on failure.
	***********************************/
	bool InsNext(ListElmt_t *element,const T *data)
	{
		ListElmt_t *newElmt;
		// Allocate storage for the element.
		if ((newElmt = new ListElmt_t) == NULL)
			return false;
		// Insert the element into the list.
		newElmt->m_data = (T*)data;
		if (element == NULL)
		{// Handle insertion at the head of the list.
			if (m_size == 0)
				m_tail = newElmt;
			newElmt->m_next = m_head;
			m_head = newElmt;
		}
		else
		{
			// Handle insertion somewhere other than at the head.
			if (element->m_next == NULL)
				m_tail = newElmt;
			newElmt->m_next = element->m_next;
			element->m_next = newElmt;
		}
		// Adjust the size of the list to account for the inserted element.
		m_size++;
		return true;
	};

	/***********************************
	Inserts list after given elmt.
	***********************************/
	void MergeNext(ListElmt_t *element,List_t *list)
	{
		// reject self copy
		if (list==this)
			return;
		// reject empty lists
		if (!list)
			return;
		if (!list->m_size)
			return;
		if (!element)
		{ // Handle insertion into the head of the list
			if (!m_size)
				m_tail=list->m_tail;
			list->m_tail->m_next=m_head;
			m_head = list->m_head;
		}
		else
		{ // Handle insertion somewhere other than at the head.
			if (element->m_next == NULL)
				m_tail=list->m_tail;
			list->m_tail->m_next=element->m_next;
			element->m_next=list->m_head;
		}
		// Adjust the size of the list to account for the inserted elements
		m_size+=list->m_size;
		// Reset the inserted list so that it will not modify its elements
		list->m_head=list->m_tail=NULL;
		list->m_size=0;
	};

	/***********************************
	Inserts data in sorted order.
	Returns false on failure.
	***********************************/
	bool SortedInsert(const T *data)
	{
		if (!m_head)
			return InsNext(NULL,data);
		ListElmt_t *prevElmt=m_head;
		int comp = m_head->m_data->CompareWith(data);
		// if < head insert before head
		if (NULL<comp)
			return InsNext(NULL,data);
		// if = to head insert after
		if (comp==NULL)
			return InsNext(prevElmt,data);
		while (prevElmt->m_next)
		{
			if (prevElmt->m_next) // if next elmt is greater insert before it
				if (NULL<prevElmt->m_next->m_data->CompareWith(data))
					break;
			prevElmt = prevElmt->m_next;
		}
		return InsNext(prevElmt,data);
	};

	/***********************************
	Removes the elmt after given elmt.
	On Success data is set to the
	data of the removed elmt.
	Returns false on failure.
	***********************************/
	bool RemNext(ListElmt_t *element,T **data)
	{
		ListElmt_t *oldElmt;
		//  Do not allow removal from an empty list.
		if (m_size == 0)
			return false;
		// Remove the element from the list.
		if (element == NULL)
		{
			// Handle removal from the head of the list.
			*data = m_head->m_data;
			oldElmt = m_head;
			m_head = m_head->m_next;
			if (m_size == 1)
				m_tail = NULL;
		}
		else
		{
			// Handle removal from somewhere other than the head.
			if (element->m_next == NULL)
				return false;
			*data = element->m_next->m_data;
			oldElmt = element->m_next;
			element->m_next = element->m_next->m_next;
			if (element->m_next == NULL)
				m_tail = element;
		}
		// Free storage allocated by the abstract data type.
		delete oldElmt;
		// Adjust the size of the list to account for the removed element.
		m_size--;
		return true;
	};
};

template <class T> class Stack_t : public List_t<T>
{
public:
	bool Push(const T *data)
	{
		return InsNext(NULL,data);
	};
	bool Pop(T **data)
	{
		return RemNext(NULL,data);
	};
	T* Peek()
	{
		if (!m_head)
			return NULL;
		return m_head->m_data;
	};
};

template <class T> class Queue_t : public List_t<T>
{
public:
	bool Push(const T *data)
	{
		return InsNext(m_tail,data);
	};
	bool Pop(T **data)
	{
		return RemNext(NULL,data);
	};
	T* Peek()
	{
		if (!m_head)
			return NULL;
		return m_head->m_data;
	};
};

template <class T> class DList_t
{
public:
	/***********************************
	ListElmt class.
	***********************************/
	class DListElmt_t
	{
	public:
		DListElmt_t()
		{
			m_data = NULL;
			m_next = NULL;
		};
		~DListElmt_t() {};

		T*			m_data;
		DListElmt_t* m_prev;
		DListElmt_t* m_next;
	};

	DListElmt_t*	m_head;
	DListElmt_t*	m_tail;
	unsigned	m_size;
	bool		m_isOwner; // if true list class will takeover destruction of objects 

	/***********************************
	List Constructor.
	***********************************/
	DList_t()
	{
		// nullify data
		m_head=m_tail=NULL;
		m_size=0;
		m_isOwner = true;
	};

	/***********************************
	List Constructor.
	***********************************/
	DList_t(bool isOwner)
	{
		// nullify data
		m_head=m_tail=NULL;
		m_size=0;
		m_isOwner = isOwner;
	};

	/***********************************
	List Destructor.
	***********************************/
	~DList_t()
	{
		T *data;
		// Remove each element.
		while (m_size)
			if (Remove(m_tail,&data)&&m_isOwner)
				delete data;
		// nullify data
		m_head=m_tail=NULL;
	};

	/***********************************
	Inserts data after given elmt.
	Returns false on failure.
	***********************************/
	bool InsNext(DListElmt_t *element,const T *data)
	{
		DListElmt_t          *newElmt;
		// Don't allow null element.
		if (element == NULL && m_size != 0)
			return false;
		// Allocate storage for the element.
		if ((newElmt = new DListElmt_t) == NULL)
			return false;
		// Insert the element into the list.
		newElmt->m_data = (T*)data;
		// Handle insertion into empty list.
		if (m_size == 0)
		{
			m_head = newElmt;
			m_head->m_prev = NULL;
			m_head->m_next = NULL;
			m_tail = newElmt;
		}
		else // Handle insertion when the list is not empty.
		{
			newElmt->m_next = element->m_next;
			newElmt->m_prev = element;
			if (element->m_next == NULL)
				m_tail = newElmt;
			else
				element->m_next->m_prev = newElmt;
			element->m_next = newElmt;
		}
		// Adjust the size of the list to account for the inserted element.
		m_size++;
		return true;
	};

	/***********************************
	Inserts data before given elmt.
	Returns false on failure.
	***********************************/
	bool InsPrev(DListElmt_t *element,const T *data)
	{
		DListElmt_t          *newElmt;
		// Do not allow a NULL element unless the list is empty.
		if (element == NULL && m_size != 0)
			return false;
		// Allocate storage to be managed by the abstract data type.
		if ((newElmt = new DListElmt_t) == NULL)
			return false;
		// Insert the new element into the list.
		newElmt->m_data = (T*)data;
		// Handle insertion when the list is empty.
		if (m_size == 0)
		{
			m_head = newElmt;
			m_head->m_prev = NULL;
			m_head->m_next = NULL;
			m_tail = newElmt;
		}
		else // Handle insertion when the list is not empty.
		{
			newElmt->m_next = element;
			newElmt->m_prev = element->m_prev;
			if (element->m_prev == NULL)
				m_head = newElmt;
			else
				element->m_prev->m_next = newElmt;
			element->m_prev = newElmt;
		}
		// Adjust the size of the list to account for the new element.
		m_size++;
		return true;
	};

	/***********************************
	Inserts list after given elmt.
	***********************************/
	void MergeNext(DListElmt_t *element,DList_t *list)
	{
		// reject self copy
		if (list==this)
			return;
		// reject empty lists
		if (!list)
			return;
		if (!list->m_size)
			return;
		if (!element)
		{ // Handle insertion into the head of the list
			if (!m_size)
				m_tail=list->m_tail;
			list->m_tail->m_next=m_head;
			if (m_head)
				m_head->m_prev=list->m_tail;
			m_head = list->m_head;
		}
		else
		{ // Handle insertion somewhere other than at the head.
			if (element->m_next == NULL)
				m_tail=list->m_tail;
			else
				element->m_next->m_prev = list->m_tail;
			list->m_tail->m_next=element->m_next;
			element->m_next=list->m_head;
			list->m_head->m_prev=element;
		}
		// Adjust the size of the list to account for the inserted elements
		m_size+=list->m_size;
		// Reset the inserted list so that it will not modify its elements
		list->m_head=list->m_tail=NULL;
		list->m_size=0;
	};

	/***********************************
	Inserts list before given elmt.
	***********************************/
	void MergePrev(DListElmt_t *element,DList_t *list)
	{
		// reject self copy
		if (list==this)
			return;
		// reject empty lists
		if (!list)
			return;
		if (!list->m_size)
			return;
		if (!element)
		{ // Handle insertion into the head of the list
			if (!m_size)
				m_tail=list->m_tail;
			list->m_tail->m_next=m_head;
			if (m_head)
				m_head->m_prev=list->m_tail;
			m_head = list->m_head;
		}
		else
		{ // Handle insertion somewhere other than at the head.
			if (element->m_prev == NULL)
				m_head=list->m_head;
			else
				element->m_prev->m_next = list->m_head;
			list->m_head->m_prev=element->m_prev;
			element->m_prev=list->m_tail;
			list->m_tail->m_next=element;
		}
		// Adjust the size of the list to account for the inserted elements
		m_size+=list->m_size;
		// Reset the inserted list so that it will not modify its elements
		list->m_head=list->m_tail=NULL;
		list->m_size=0;
	};

	/***********************************
	Removes given elmt.
	On Success data is set to the
	data of the removed elmt.
	Returns false on failure.
	***********************************/
	bool Remove(DListElmt_t *element,T **data)
	{
		// Do not allow a NULL element or removal from an empty list.
		if (element == NULL || m_size == 0)
			return false;
		// Remove the element from the list.
		*data = element->m_data;
		if (element == m_head)
		{
			// Handle removal from the head of the list.
			m_head = element->m_next;
			if (m_head == NULL)
				m_tail = NULL;
			else
				element->m_next->m_prev = NULL;
		}
		else
		{
			// Handle removal from other than the head of the list.
			element->m_prev->m_next = element->m_next;
			if (element->m_next == NULL)
				m_tail = element->m_prev;
			else
				element->m_next->m_prev = element->m_prev;
		}
		// Free the storage allocated by the abstract data type.
		delete element;
		// Adjust the size of the list to account for the removed element.
		m_size--;
		return true;
	};

	/***********************************
	Inserts data in sorted order.
	Returns false on failure.
	***********************************/
	bool SortedInsert(const T *data)
	{
		if (!m_size)
			return InsNext(NULL,data);
		DListElmt_t *prevElmt=m_head;
		int comp = m_head->m_data->CompareWith(data);
		// if < head insert before head
		if (NULL<comp)
			return InsPrev(m_head,data);
		// if = to head insert after
		if (comp==NULL)
			return InsNext(prevElmt,data);
		while (prevElmt->m_next)
		{
			if (prevElmt->m_next) // if next elmt is greater insert before it
				if (NULL<prevElmt->m_next->m_data->CompareWith(data))
					break;
			prevElmt = prevElmt->m_next;
		}
		return InsNext(prevElmt,data);
	};

	/***********************************
	Sorts the elements in the list.
	***********************************/
	void Sort()
	{
		if (m_size<2)
			return;
		bool progress = false;
		DListElmt_t *key1=m_head, *key2=m_head->m_next;
		bool done = false;
		unsigned long int count = 0, limit = m_size*m_size/2;
		while (!done)
		{
			int comp = key1->m_data->CompareWith(key2->m_data);
			// make swap if key1 is greater that key2
			if (comp>NULL)
			{
				progress=true;
				if (key1->m_prev==NULL)
					m_head = key2;
				else
					key1->m_prev->m_next=key2;
				key2->m_prev=key1->m_prev;
				key1->m_prev=key2;
				if (key2->m_next==NULL)
					m_tail=key1;
				else
					key2->m_next->m_prev=key1;
				key1->m_next=key2->m_next;			
				key2->m_next=key1;
			}
			// advance
			key1=key2;
			key2=key2->m_next;
			if (key1->m_next==NULL)
			{			
				if (count++>limit)// prevent infinite loop
					done = true;
				if (!progress)
					done=true; // end if last cycle thru showed nothing out of sequential order
				else
				{ // restart at begining reset progress to check status of new pass
					key1=m_head;
					key2=m_head->m_next;
					progress=false;
				}
			}
		}
	};

	/***********************************
	Sets result to the matching elmt.
	Or to NULL on failure
	***********************************/
	void Search(T *data,DListElmt_t **result)
	{
		*result=NULL;
		if (m_size==0)
			return;
		DListElmt_t *curElmt = m_head;
		do
		{
			if (curElmt->m_data->CompareWith(data)==NULL)
			{
				*result=curElmt;
				return;
			}
			curElmt = curElmt->m_next;
		} while (curElmt!=NULL);
	};
};


template <class T> class BiTree_t
{
public:
	/***********************************
	Node class.
	***********************************/
	class Node_t
	{
	public:
		Node_t()
		{
			m_data=NULL;
			m_right=m_left=NULL;
		};
		Node_t(T* data)
		{
			m_data=data;
			m_right=m_left=NULL;
		};
		~Node_t()
		{
			m_data=NULL;
			m_right=m_left=NULL;
		};
		T*		m_data; // pointer to data
		Node_t*	m_left; // pointer to left node
		Node_t*	m_right;// pointer to right node
	};
	unsigned m_size; // number of nodes in tree
	Node_t*	 m_root; // pointer to root node
	bool	 m_isOwner; // true if Bitree is the owner of T data, delete will be used for object destruction

	/***********************************
	BiTree Class Constructor.
	***********************************/
	BiTree_t()
	{
		m_size = 0;
		m_root = NULL;
		m_isOwner = true;
	};

	/***********************************
	BiTree Class Constructor.
	Sets isOwner flag.
	***********************************/
	BiTree_t(bool isOwner)
	{
		m_size = 0;
		m_root = NULL;
		m_isOwner = isOwner;
	};

	/***********************************
	BiTree Class Destructor.
	***********************************/
	~BiTree_t()
	{
		Reset(false);
	};

	/***********************************
	Destroys BiTree and resets it.
	***********************************/
	void Reset(bool isOwner)
	{
		RemLeft(NULL);
		int i=9;
		m_size = 0;
		m_root = NULL;
		m_isOwner = isOwner;
	};

	/***********************************
	Inserts an object to the left of
	The given Node.
	Returns false on failure.
	***********************************/
	bool InsLeft(Node_t *node,const T* data)
	{
		Node_t *newNode,
			   **position;
		// Determine where to insert the node
		if (!node)
		{ // Allow insertion at the root only in an empty tree
			if (m_size>0)
				return false;
			position = &m_root;
		}
		else
		{ // normally allow insertion only at the end of a branch
			if (node->m_left)
				return false;
			position = &node->m_left;
		}
		// Allocate storage for node
		if ( !(newNode = new Node_t((T*)data)) )
			return false;
		// Insert new Node
		*position = newNode;
		// Increment size counter
		m_size++;
		return true;
	};

	/***********************************
	Inserts an object to the right of
	The given Node.
	Returns false on failure.
	***********************************/
	bool InsRight(Node_t *node,const T* data)
	{
		Node_t *newNode,
			   **position;
		// Determine where to insert the node
		if (!node)
		{ // Allow insertion at the root only in an empty tree
			if (m_size>0)
				return false;
			position = &m_root;
		}
		else
		{ // normally allow insertion only at the end of a branch
			if (node->m_right)
				return false;
			position = &node->m_right;
		}
		// Allocate storage for node
		if ( !(newNode = new Node_t((T*)data)) )
			return false;
		// Insert new Node
		*position = newNode;
		// Increment size counter
		m_size++;
		return true;
	};

	/***********************************
	Removes objects from the left branch
	of the given Node.
	***********************************/
	void RemLeft(Node_t *node)
	{
		Node_t **position;
		// Deny removal from empty trees
		if (m_size==0)
			return;
		// Determine wher to remove nodes
		if (!node)
			position = &m_root;
		else
			position = &node->m_left;
		// Stop at NULL nodes
		if (!(*position))
			return;
		// Remove the Nodes
		RemLeft(*position);
		RemRight(*position);
		// Destroy T objects if isOwner
		if (m_isOwner)
			delete (*position)->m_data;
		delete (*position);
		*position = NULL;
		// Decrement size counter
		m_size--;
	};

	/***********************************
	Removes objects from the right branch
	of the given Node.
	***********************************/
	void RemRight(Node_t *node)
	{
		Node_t **position;
		// Deny removal from empty trees
		if (m_size==0)
			return;
		// Determine wher to remove nodes
		if (!node)
			position = &m_root;
		else
			position = &node->m_right;
		// Stop at NULL nodes
		if (!(*position))
			return;
		// Remove the Nodes
		RemLeft(*position);
		RemRight(*position);
		// Destroy T objects if isOwner
		if (m_isOwner)
			delete (*position)->m_data;
		delete (*position);
		*position = NULL;
		// Decrement size counter
		m_size--;
	};

	/***********************************
	Resets this tree and remakes it
	as the merged result of left and
	right.
	Returns false on failure.
	***********************************/
	bool Merge(BiTree_t *right,BiTree_t *left,const T* data)
	{
		// left and right must not be null
		if (!left||!right)
			return false;
		// left and right must not be this tree
		if (right==this||left==this)
			return false;
		// NOTE: this is the merged tree
		// Reset the merged tree
		Reset(m_isOwner);
		// Insert the data for the root node of the merged tree.
		if (InsLeft(NULL,data))
		{
			Reset(false); // destroy merged tree
			return false
		}
		// Merge the trees
		m_root->m_left = left->m_root;
		m_root->m_right= right->m_root;
		// Do not let the original trees access the merged nodes.
		left->m_root=NULL;
		left->m_size=0;
		left->m_isOwner=false;
		right->m_root=NULL;
		right->m_size=0;
		right->m_isOwner=false;
		return true;
	};
};

template <class T> class BisTree_t
{
public:
	/***********************************
	AVL balance enumeration.
	***********************************/
	enum AVLBAL_t { AVL_LFT_HEAVY=1, AVL_BALANCED=0, AVL_RGT_HEAVY=-1 };
	/***********************************
	AVLNode class.
	***********************************/
	class AVLNode_t
	{
	public:
		AVLNode_t()
		{
			m_data = NULL;
			m_isHidden = false;
			m_factor = AVL_BALANCED;
		};

		AVLNode_t(T* data)
		{
			m_data = data;
			m_isHidden = false;
			m_factor = AVL_BALANCED;
		};

		~AVLNode_t()
		{
			m_data = NULL;
			m_isHidden = false;
			m_factor = AVL_BALANCED;
		};
		T*		 m_data; // pointer to node data
		bool	 m_isHidden; // true if node is hidden
		AVLBAL_t m_factor; // balance factor of node
	};

	/***********************************
	BisTree Class Constructor.
	***********************************/
	BisTree_t()
	{
		pBiTree=new BiTree_t<AVLNode_t>();
	};

	/***********************************
	BisTree Class Constructor.
	Sets isOwner flag
	***********************************/
	BisTree_t(bool isOwner)
	{
		pBiTree=new BiTree_t<AVLNode_t>(isOwner);
	};

	/***********************************
	BisTree Class Destructor.
	***********************************/
	~BisTree_t()
	{
		delete pBiTree;
		pBiTree = NULL;
	};

	/***********************************
	Sets isOwner flag
	***********************************/
	void SetIsOwner(bool isOwner)
	{
		if (pBiTree)
			pBiTree->m_isOwner=isOwner;
	};

protected:
	BiTree_t<AVLNode_t> *pBiTree; // pointer to Bit Tree

public:
	/***********************************
	Inserts new data into the tree.
	Returns false on failure.
	***********************************/
	bool Insert(const T* data)
	{
		if (!pBiTree) // must be initialized
			return false;
		bool isBalanced = false;
		return NInsert(&pBiTree->m_root,data,&isBalanced);
	};

	/***********************************
	Inserts new data into the tree.
	Or returns it if it already exists.
	Returns false on failure.
	***********************************/
	bool InsertFind(const T* data,T** foundData)
	{
		if (!pBiTree) // must be initialized
			return false;
		bool isBalanced = false;
		return NInsert(&pBiTree->m_root,data,&isBalanced,foundData);
	};

	/***********************************
	Removes matching data from the tree.
	Returns false on failure.
	***********************************/
	bool Remove(const T* data)
	{
		if (!pBiTree) // must be initialized
			return false;
		return Hide(pBiTree->m_root,data);
	}

	/***********************************
	Returns the matching data found in
	the	tree.
	Returns false on failure.
	***********************************/
	bool LookUp(T** data)
	{
		if (!pBiTree) // must be initialized
			return false;
		return NLookUp(pBiTree->m_root,data);
	}

	/***********************************
	Returns the true if the tree is
	set as the owner of its data.
	Returns false on failure.
	***********************************/
	bool GetIsOwner()
	{
		if (!pBiTree) // must be initialized
			return false;
		return pBiTree->m_isOwner;
	}

	/***********************************
	Returns the number of nodes in the
	tree.
	Returns 0 on failure.
	***********************************/
	unsigned GetSize()
	{
		if (!pBiTree) // must be initialized
			return 0;
		return pBiTree->m_size;
	}

protected:
	/***********************************
	Performs left rotation on specified
	node.
	***********************************/
	void RotateLeft(BiTree_t<AVLNode_t>::Node_t **node)
	{
		BiTree_t<AVLNode_t>::Node_t *left,
									*grandchild;
		left = (*node)->m_left;
		if ( (*node)->m_data->m_factor == AVL_LFT_HEAVY )
		{
			// Perform an LL rotation
			(*node)->m_left = left->m_right;
			left->m_right = *node;
			(*node)->m_data->m_factor = AVL_BALANCED;
			left->m_data->m_factor = AVL_BALANCED;
			*node = left;
		}
		else
		{
			// Perform an LR rotation
			grandchild = left->m_right;
			left->m_right = grandchild->m_left;
			grandchild->m_left = left;
			(*node)->m_left = grandchild->m_right;
			grandchild->m_right = *node;

			switch (grandchild->m_data->m_factor)
			{
			case AVL_LFT_HEAVY:
				(*node)->m_data->m_factor = AVL_RGT_HEAVY;
				left->m_data->m_factor = AVL_BALANCED;
				break;

			case AVL_BALANCED:
				(*node)->m_data->m_factor = AVL_BALANCED;
				left->m_data->m_factor = AVL_BALANCED;
				break;

			case AVL_RGT_HEAVY:
				(*node)->m_data->m_factor = AVL_BALANCED;
				left->m_data->m_factor = AVL_LFT_HEAVY;
				break;

			default:
				break;
			}

			grandchild->m_data->m_factor = AVL_BALANCED;
			*node = grandchild;
		}
	};

	/***********************************
	Performs right rotation on
	specified node.
	***********************************/
	void RotateRight(BiTree_t<AVLNode_t>::Node_t **node)
	{
		BiTree_t<AVLNode_t>::Node_t *right,
									*grandchild;
		right = (*node)->m_right;
		if (right->m_data->m_factor == AVL_RGT_HEAVY)
		{
			// Perform an RR rotation.
			(*node)->m_right = right->m_left;
			right->m_left = *node;
			(*node)->m_data->m_factor = AVL_BALANCED;
			right->m_data->m_factor = AVL_BALANCED;
			*node = right;
		}
		else
		{
			// Perform an RL rotation.
			grandchild = right->m_left;
			right->m_left = grandchild->m_right;
			grandchild->m_right = right;
			(*node)->m_right = grandchild->m_left;
			grandchild->m_left = *node;

			switch (grandchild->m_data->m_factor)
			{
			case AVL_LFT_HEAVY:
				(*node)->m_data->m_factor = AVL_BALANCED;
				right->m_data->m_factor = AVL_RGT_HEAVY;
				break;

			case AVL_BALANCED:
				(*node)->m_data->m_factor = AVL_BALANCED;
				right->m_data->m_factor = AVL_BALANCED;
				break;

			case AVL_RGT_HEAVY:
				(*node)->m_data->m_factor = AVL_LFT_HEAVY;
				right->m_data->m_factor = AVL_BALANCED;
				break;

			default:
				break;
			}

			grandchild->m_data->m_factor = AVL_BALANCED;
			*node = grandchild;
		}
	};

	/***********************************
	Removes objects from the left branch
	of the given Node.
	***********************************/
	void DestroyLeft(BiTree_t<AVLNode_t>::Node_t *node)
	{
		// Must be initialized first
		if (!pBiTree)
			return;
		BiTree_t<AVLNode_t>::Node_t **position;
		// Do not allow destruction of an empty tree
		if (!pBiTree->m_size)
			return;
		// Determine where to destroy nodes.
		if (node==NULL)
			position = &pBiTree->m_root;
		else
			position = &node->m_left;
		// Stop at NULL nodes
		if (!(*position))
			return;
		// Destroy the nodes
		DestroyLeft(*position);
		DestroyRight(*position);
		if (pBiTree->m_isOwner)
		{
			delete (*position)->m_data->m_data;
		}
		// Free data in AVL node then free the node itself
		delete (*position)->m_data;
		delete (*position);
		// Decrement size counter
		pBiTree->size--;
	};

	/***********************************
	Removes objects from the right branch
	of the given Node.
	***********************************/
	void DestroyRight(BiTree_t<AVLNode_t>::Node_t *node)
	{
		// Must be initialized first
		if (!pBiTree)
			return;
		BiTree_t<AVLNode_t>::Node_t **position;
		// Do not allow destruction of an empty tree
		if (!pBiTree->m_size)
			return;
		// Determine where to destroy nodes.
		if (node==NULL)
			position = &pBiTree->m_root;
		else
			position = &node->m_right;
		// Stop at NULL nodes
		if (!(*position))
			return;
		// Destroy the nodes
		DestroyLeft(*position);
		DestroyRight(*position);
		if (pBiTree->m_isOwner)
		{
			delete (*position)->m_data->m_data;
		}
		// Free data in AVL node then free the node itself
		delete (*position)->m_data;
		delete (*position);
		// Decrement size counter
		pBiTree->size--;
	};

	/***********************************
	Inserts a new AVL node into the
	tree. If duplicate found foundData
	is pointed to it.
	Returns false on failure.
	***********************************/
	bool NInsert(BiTree_t<AVLNode_t>::Node_t **node,
		const T* data, bool *isBalanced,T** foundData=NULL)
	{
		AVLNode_t *avlData;
		int cmpval;
		// NInsert the data into the tree
		if (!(*node))
		{
			// Handle insertion into an empty tree
			if (!(avlData=new AVLNode_t((T*)data)))
				return false; // new failed
			return pBiTree->InsLeft(*node,avlData);
		}
		else
		{
			// Handle insertion into a tree that is not empty
			cmpval = (data)->CompareWith((*node)->m_data->m_data);
			if (cmpval<NULL)
			{
				// **** Move to the left. ****
				if (!(*node)->m_left)
				{
					if (!(avlData=new AVLNode_t((T*)data)))
						return false; // new failed
					if (!pBiTree->InsLeft((*node),avlData))
						return false; // insert failed
					*isBalanced = false;
				}
				else
					if (!NInsert(&(*node)->m_left,data,isBalanced,foundData))
						return false;
				// Ensure that the tree remains balanced.
				if (!(*isBalanced))
				{
					switch ((*node)->m_data->m_factor)
					{
					case AVL_LFT_HEAVY:
						RotateLeft(node);
						*isBalanced=true;
						break;

					case AVL_BALANCED:
						(*node)->m_data->m_factor = AVL_LFT_HEAVY;
						break;

					case AVL_RGT_HEAVY:
						(*node)->m_data->m_factor = AVL_BALANCED;
						*isBalanced=true;
						break;

					default:
						break;
					}
				}
			}
			else if (cmpval>NULL)
			{
				// **** Move to the right. ****
				if (!(*node)->m_right)
				{
					if (!(avlData=new AVLNode_t((T*)data)))
						return false; // new failed
					if (!pBiTree->InsRight((*node),avlData))
						return false; // insert failed
					*isBalanced = false;
				}
				else
					if (!NInsert(&(*node)->m_right,data,isBalanced,foundData))
						return false;
				// Ensure that the tree remains balanced.
				if (!(*isBalanced))
				{
					switch ((*node)->m_data->m_factor)
					{
					case AVL_LFT_HEAVY:
						(*node)->m_data->m_factor = AVL_BALANCED;
						*isBalanced=true;
						break;

					case AVL_BALANCED:
						(*node)->m_data->m_factor = AVL_RGT_HEAVY;
						break;

					case AVL_RGT_HEAVY:
						RotateRight(node);
						*isBalanced=true;
						break;

					default:
						break;
					}
				}
			}
			else
			{
				// Handle duplicate copy of data
				if (!(*node)->m_data->m_isHidden)
				{
					// data is present and not hidden
					if (foundData)
						*foundData=(*node)->m_data->m_data;
				}
				else
				{
					// replace old data with new and mark it as not hidden
					if (pBiTree->m_isOwner)
						delete (*node)->m_data->m_data;
					(*node)->m_data->m_data = (T*)data;
					(*node)->m_data->m_isHidden = false;
					// Do not rebalance since the tree structure is unchanged
					*isBalanced;
				} // end if
			} // end if
		} // end if
		return true;
	};

	/***********************************
	Searches for and hides the
	specified data, searching from
	the specified node.
	Returns false on failure.
	***********************************/
	bool Hide(BiTree_t<AVLNode_t>::Node_t *node,const T* data)
	{
		int cmpval;
		if (!node) // Data not found
			return false;
		cmpval = data->CompareWith(node->m_data->m_data);
		if (cmpval<NULL) // Move to the left.
			return Hide(node->m_left,data);
		else if (cmpval>NULL) // Move to the right
			return Hide(node->m_right,data);
		// else
		// Mark the node as hidden
		node->m_data->m_isHidden = true;
		return true;
	};

	/***********************************
	Finds data in tree searching from
	given node.
	Returns false on failure.
	***********************************/
	bool NLookUp(BiTree_t<AVLNode_t>::Node_t *node,T **data)
	{
		int cmpval;
		if (!node) // returns that the data was not found
			return false;
		cmpval = (*data)->CompareWith(node->m_data->m_data);
		if (cmpval<NULL) // Move to the left
			return NLookUp(node->m_left,data);
		else if (cmpval>NULL) // Move to the right
			return NLookUp(node->m_right,data);
		// else (cmpval==NULL)
		if (!node->m_data->m_isHidden)
		{
			// Pass back data from tree
			*data = node->m_data->m_data;
			return true; // data was found
		}
		// else (data was hidden)
		return false; // hidden data = not found
	};
};

template <class T> class NTree_t
{
public:
	/***********************************
	NTree Node class.
	***********************************/
	class Node_t
	{
	public:
		Node_t()
		{
			m_data=NULL;
			m_branches.m_isOwner=true;
		};

		Node_t(T* data)
		{
			m_data=data;
			m_branches.m_isOwner=true;
		};

		~Node_t()
		{
			m_data=NULL;
		};

		int CompareWith(Node_t *node) const
		{
			if (node)
				return m_data->CompareWith(node->m_data);
			return NULL;
		}

		T* m_data; // pointer to data
		DList_t<Node_t> m_branches; // list of branches
	};

	typedef DList_t<Node_t>::DListElmt_t Branch_t;

	unsigned m_size; // number of nodes in tree
	Node_t*	 m_root; // pointer to root

	bool	 m_isOwner; // true if owner of the data

public:
	/***********************************
	NTree class constructor.
	***********************************/
	NTree_t()
	{
		m_size=0;
		m_root=NULL;
		m_isOwner=false;
	};

	/***********************************
	NTree class constructor.
	Set the isOwner flag.
	***********************************/
	NTree_t(bool isOwner)
	{
		m_size=0;
		m_root=NULL;
		m_isOwner=isOwner;
	};

	/***********************************
	NTree class destructor.
	***********************************/
	~NTree_t()
	{
		NRemove(NULL);
	};
	
	/***********************************
	Inserts a new branch into the given
	node.
	Returns false on failure.
	***********************************/
	bool NInsert(Node_t *node,const T *data)
	{
		if (node==NULL)
		{ // handle insertion at the root of the tree
			if (m_size) // deny null if size!=0
				return false;
			m_root = new Node_t((T*)data);
			if (!m_root)
				return false; // new failed
			m_size++; // increment size counter
			return true; // return success
		}
		// handle insertion elsewhere
		// create new node
		Node_t *newNode = new Node_t((T*)data);
		if (!newNode)
			return false; // new failed
		// add as a new branch
		if (!node->m_branches.InsNext(node->m_branches.m_tail,newNode))
			return false; // insert failed
		m_size++; // increment size counter
		return true; // return success
	};

	/***********************************
	Removes all the branches
	attached to the given node
	***********************************/
	void NRemove(Node_t *node)
	{
		if (!node)
		{ // handle destruction of the tree
			// stop if the tree is already destroyed
			if (!m_root)
				return;
			// remove all branches from the root
			NRemove(m_root);
			// remove the data from the root
			if (m_isOwner)
				delete m_root->m_data;
			// remove the root
			delete m_root;
			m_size=0;
			m_root=NULL;
			return; // finished
		}
		// loop through all branches
		Branch_t *branch=node->m_branches.m_head;
		Branch_t *branchNext=NULL;
		while (branch)
		{
			// copy pointer to next branch
			branchNext=branch->m_next;
			// remove the children of the branch
			NRemove(branch->m_data);
			// remove the branch node from the branches list
			Node_t *deadNode=NULL;
			node->m_branches.Remove(branch,&deadNode);			
			// Remove the data of the branch node
			if (m_isOwner)
				delete deadNode->m_data;
			// destroy the branch node
			delete deadNode;
			deadNode=NULL;
			// decrement size counter
			m_size--;
			// advance to next list item
			branch = branchNext;
		}
	};

	/***********************************
	Removes the given branch of the
	given node.
	***********************************/
	void NRemoveB(Node_t *node,Branch_t *branch)
	{
		if (!node||!branch)
			return; // reject null values
		// remove the children of the branch
		NRemove(branch->m_data);
		// remove the branch node from the branches list
		Node_t *deadNode=NULL;
		node->m_branches.Remove(branch,&deadNode);			
		// Remove the data of the branch node
		if (m_isOwner)
			delete deadNode->m_data;
		// destroy the branch node
		delete deadNode;
		deadNode=NULL;
		// decrement size counter
		m_size--;
	}

	/***********************************
	Searches for a branch in the
	branches of the given node and
	then removes it.
	***********************************/
	void NFindRem(Node_t *node,const T *data)
	{
		Branch_t *result=NULL;
		NLookUpB(node,data,&result);
		NRemoveB(node,result);
	};

	/***********************************
	Counts the number of nodes from
	the given node in the tree.
	***********************************/
	void NCount(Node_t *node,unsigned *count)
	{
		// get the node to count from
		if (!node)
		{
			node=m_root;
			if (!node)
				return;
		}
		*count = (*count) + 1;
		// count other nodes
		Branch_t *branch=node->m_branches.m_head;
		// loop through all branches
		while (branch)
		{
			// check node
			NCount(branch->m_data,count);
			// advance to next list item
			branch = branch->m_next;
		}
	};

	/***********************************
	Searches for a node in the tree.
	From the given node.
	***********************************/
	void NLookUp(Node_t *node,const T *data,Node_t **result)
	{
		// get the node to search for
		if (!node)
		{
			node=m_root;
			if (!node)
				return;
		}
		*result=NULL;
		// check this node for equality
		if (node->m_data->CompareWith(data)==NULL)
		{ // this node is the matching node
			*result=node;
			return;
		}
		// search other nodes
		Branch_t *branch=node->m_branches.m_head;
		// loop through all branches
		while (branch)
		{
			// check node
			NLookUp(branch->m_data,data,result);
			if (*result)
				return; // if found then return result
			// advance to next list item
			branch = branch->m_next;
		}
	};

	/***********************************
	Searches for a branch in the
	branches of the given node.
	***********************************/
	void NLookUpB(Node_t *node,const T *data,Branch_t **result)
	{
		if (!node)
			return; // reject null nodes
		// check branches for equality
		Node_t sNode((T*)data);
		node->m_branches.Search(&sNode,result);
	};

	/***********************************
	Searches for a node in the branches
	of the given node.
	***********************************/
	void NBLookUp(Node_t *node,const T *data,Node_t **result)
	{
		// get the node to search for
		if (!node)
		{
			node=m_root;
			if (!node)
				return;
		}
		*result=NULL;
		// check this node for equality
		if (node->m_data->CompareWith(data)==NULL)
		{ // this node is the matching node
			*result=node;
			return;
		}
		// check branches for equality
		Node_t sNode((T*)data);
		Branch_t *branch=NULL;
		node->m_branches.Search(&sNode,&branch);
		if (branch)
			*result=branch->m_data;
	};

	/***********************************
	Attaches the given tree as a
	branch of the given node.
	Returns false on failure.
	***********************************/
	bool NMerge(Node_t *node,NTree_t *tree)
	{
		// reject self copy
		if (tree==this)
			return false;
		if (!node)
		{
			// handle replacement of root
			if (m_size)
				return false; // deny null nodes if the tree is not empty
			// copy tree data
			m_root=tree->m_root;
			m_size=tree->m_size;
		}
		else
		{
			// handle insertion elsewhere
			// add as tree root a new branch
			if (!node->m_branches.InsNext(node->m_branches.m_tail,tree->m_root))
				return false; // insert failed
			m_size+=tree->m_size; // increment size counter
		}
		// reset user tree so that it will not modify its nodes
		tree->m_root=NULL;
		tree->m_size=NULL;
		return true;
	};

	/***********************************
	Returns the number of branches
	on a given node.
	***********************************/
	int GetNumOfBranches(Node_t *node)
	{
		if (node)
			return node->m_branches->m_size;
		return 0;
	};
};

// X tree
template <class T,const int nBranches> class XTree_t
{
public:
	class Node_t
	{
	public:
		Node_t()
		{
			m_data=NULL;
			int i=nBranches;
			while (i--)
				m_branches[i]=NULL;
		};

		Node_t(T* data)
		{
			m_data=data;
			int i=nBranches;
			while (i--)
				m_branches[i]=NULL;
		};

		~Node_t()
		{
			m_data=NULL;
			int i=nBranches;
			while (i--)
			{
				delete m_branches[i];
				m_branches[i]=NULL;
			}
		};

		T*		m_data; // pointer to data
		Node_t*	m_branches[nBranches]; // array of pointers to branches
	};

	/***********************************
	XTree_t class constructor.
	***********************************/
	XTree_t()
	{
		m_size=0;
		m_root=NULL;
		m_isOwner=false;
	};

	/***********************************
	XTree_t class constructor.
	Set the isOwner flag.
	***********************************/
	XTree_t(bool isOwner)
	{
		m_size=0;
		m_root=NULL;
		m_isOwner=isOwner;
	};

	/***********************************
	XTree_t class destructor.
	***********************************/
	~XTree_t()
	{
		Destroy();
	};

	unsigned m_size; // number of nodes in tree
	Node_t*	 m_root; // pointer to root node

	bool	 m_isOwner; // true if XTree_t is the owner of T data, delete will be used for object destruction

public:
	/***********************************
	Inserts a node into the given branch
	of the given node.
	Returns false on failure.
	***********************************/
	bool NInsert(Node_t *node,int branch,const T *data)
	{
		if (node==NULL)
		{ // handle insertion at the root of the tree
			if (m_size) // deny null if size!=0
				return false;
			m_root = new Node_t((T*)data);
			if (!m_root)
				return false; // new failed
			m_size++; // increment size counter
			return true; // return success
		}
		// handle insertion elsewhere
		// normally allow insertion only at the end of a branch
		if (node->m_branches[branch])
			return false;
		// create new node
		Node_t *newNode = new Node_t((T*)data);
		if (!newNode)
			return false; // new failed
		// insert at desired branch
		node->m_branches[branch] = newNode;
		m_size++; // increment size counter
		return true; // return success
	};

	/***********************************
	Counts the number of nodes from
	the given node in the tree.
	***********************************/
	void NCount(Node_t *node,unsigned *count)
	{
		// Reject null nodes
		if (!node)
			return;
		*count = (*count) + 1;		
		// count other nodes
		// loop through all branches
		int i=nBranches;
		while (i--)
			if (node->m_branches[i])
				NCount(node->m_branches[i],count);
	};

	/***********************************
	Destroys the tree.
	***********************************/
	void Destroy()
	{
		// stop if the tree is already destroyed
		if (!m_root)
			return;
		// remove all branches from the root
		NRemove(m_root);
		// remove the data from the root
		if (m_isOwner)
			delete m_root->m_data;
		// remove the root
		delete m_root;
		m_size=0;
		m_root=NULL;
	};

	/***********************************
	Removes all branches of the
	given node.
	***********************************/
	void NRemove(Node_t *node)
	{
		// Reject null nodes
		if (!node)
			return;		
		// loop through all branches
		int i=nBranches;
		while (i--)
		{
			// remove the children of the branch
			if (node->m_branches[i])
			{
				NRemove(node->m_branches[i]);
				// remove the data of the branch node
				if (m_isOwner)
					delete node->m_branches[i]->m_data;
				// destroy the branch node
				delete node->m_branches[i];
				node->m_branches[i]=NULL;
				// decrement size counter
				m_size--;
			}
		}
	};

	/***********************************
	Removes the given branch of the
	given node.
	***********************************/
	void NRemoveB(Node_t *node,int branch)
	{
		// Reject null nodes
		if (!node)
			return;
		if (!node->m_branches[branch])
			return; // reject null branches
		// Remove all children of the branch
		NRemove(node->m_branches[branch]);
		// Remove the branch	
		// remove the data of the branch node
		if (m_isOwner)
			delete node->m_branches[branch]->m_data;
		// destroy the branch node
		delete node->m_branches[branch];
		node->m_branches[branch]=NULL;
		// decrement size counter
		m_size--;
	};

	/***********************************
	Searches for a node in the tree.
	From the given node.
	***********************************/
	void NLookUp(Node_t *node,const T *data,Node_t **result)
	{
		// Reject null nodes
		if (!node)
			return;
		*result=NULL;
		// check this node for equality
		if (node->m_data->CompareWith(data)==NULL)
		{ // this node is the matching node
			*result=node;
			return;
		}
		// search other nodes
		// loop through all branches		
		int i=nBranches;
		while (i--)
		{
			if (node->m_branches[i])
				NLookUp(node->m_branches[i],data,result);
			if (*result)
				return; // if found then return result
		}
	};

	/***********************************
	Attaches the given tree as a
	branch of the given node.
	Returns false on failure
	***********************************/
	bool NMerge(Node_t *node,int branch,XTree_t *tree)
	{
		// reject self copy
		if (tree==this)
			return false;
		if (!node)
		{
			// handle replacement of root
			if (m_size)
				return false; // deny null nodes if the tree is not empty
			// copy tree data
			m_root=tree->m_root;
			m_size=tree->m_size;
		}
		else
		{
			// handle insertion elsewhere
			// normally allow insertion only at the end of a branch
			if (node->m_branches[branch])
				return false;
			// add as tree root a new branch
			node->m_branches[branch]=tree->m_root;
			m_size+=tree->m_size; // increment size counter
		}
		// reset user tree so that it will not modify its nodes
		tree->m_root=NULL;
		tree->m_size=NULL;
		return true;
	};
};

/*********************************************************************
 
 Indexed X tree (an XTree that uses array indexes to point to nodes)
 Data type T must have a copy constructor.
 T m_data=NULL; should nullify a T object

*********************************************************************/
template <class T,const int nBranches> class IXTree_t
{
public:
	class Node_t
	{
	public:
		Node_t()
		{
			int i=nBranches;
			while (i--)
				m_branches[i]=NULL;			
		};
		Node_t(T* data)
		{
			if (data)
				m_data=*(data);;
			int i=nBranches;
			while (i--)
				m_branches[i]=NULL;			
		};		
		Node_t(T* data,int arIdx,int relIdx)
		{
			if (data)
				m_data=*(relIdx+data);
			int i=nBranches;
			while (i--)
				m_branches[i]=NULL;			
		};
		~Node_t()
		{
			int i=nBranches;
			while (i--)
				m_branches[i]=NULL;			
		};

		int CompareWith(T* data) const
		{
			return m_data.CompareWith(data);
		};
		T			m_data; // data
		unsigned	m_branches[nBranches]; // array of indexes of branches
	};
	unsigned m_root; // index of root node

	CDSArray_t<Node_t,T> m_nodes; // arrary of nodes


	IXTree_t()
	{
		m_root=0;
		m_nodes.Init(IXTreeNodeCOMPFUNC<T,nBranches>,IXTreeNodeINITFUNC<T,nBranches>,true,10);
	};

	~IXTree_t()
	{
		Destroy();
	};

	/***********************************
	Returns the number of nodes in the
	tree.
	Returns false on failure.
	***********************************/
	unsigned GetSize()
	{
		return m_nodes.m_nElmts;
	};

	/***********************************
	Returns a pointer to the node
	at the given id.
	Returns NULL on failure.
	***********************************/
	Node_t *GetNode(unsigned nodeID)
	{
		if (!nodeID)
			return NULL;
		return (Node_t *)m_nodes.GetElmt(nodeID);
	};

	/***********************************
	Returns a pointer to the node data
	at the given id.
	Returns NULL on failure.
	***********************************/
	T *GetData(unsigned nodeID)
	{
		if (!nodeID)
			return NULL;
		return &((Node_t *)m_nodes.GetElmt(nodeID))->m_data;
	};

	/***********************************
	Inserts a node into the given branch
	of the given node.
	Returns the id of the new node or
	0 on failure.
	***********************************/
	unsigned NInsert(unsigned nodeID,int branch,const T *data)
	{
		if (nodeID==NULL)
		{ // handle insertion at the root of the tree
			if (GetSize()) // deny null if size!=0
				return NULL;
			// create new node
			if ( (m_root=m_nodes.AddElmts(1,(T*)data)) == NULL )
				return NULL; // AddElmts failed
			return m_root; // return ID
		}
		// handle insertion elsewhere
		// get node
		Node_t *node = (Node_t *)m_nodes.GetElmt(nodeID);
		// normally allow insertion only at the end of a branch
		if (node->m_branches[branch])
			return NULL;
		// create new node
		if ( (node->m_branches[branch]=m_nodes.AddElmts(1,(T*)data)) == NULL )
			return NULL; // AddElmts failed
		return node->m_branches[branch]; // return ID
	};

	/***********************************
	Counts the number of nodes from
	the given node in the tree.
	***********************************/
	void NCount(unsigned nodeID,unsigned *count)
	{
		// Reject null nodes
		if (!nodeID)
			return;
		// get node
		Node_t *node = (Node_t *)m_nodes.GetElmt(nodeID);
		if (!node)
			return; // node doesn't exist
		*count = (*count) + 1;		
		// count other nodes
		// loop through all branches
		int i=nBranches;
		while (i--)
			if (node->m_branches[i])
				NCount(node->m_branches[i],count);
	};

	/***********************************
	Destroys the tree.
	***********************************/
	void Destroy()
	{
		m_nodes.Destroy();
		m_nodes.Init(IXTreeNodeCOMPFUNC<T,nBranches>,IXTreeNodeINITFUNC<T,nBranches>,true,10);
		m_root=0;
	};

	/***********************************
	Copies another tree over this tree.
	***********************************/
	bool Copy(IXTree_t *pTree)
	{
		if (pTree==this)
			return true;
		// verify parameters
		if (!pTree)
			return false;
		if (!pTree->GetSize())
			return false;
		// destroy this tree
		Destroy();
		// get size
		unsigned i=pTree->GetSize();
		// copy root
		m_root=pTree->m_root;
		// copy isOwner flag
		m_nodes.m_isOwner = pTree->m_nodes.m_isOwner;
		// allocate space
		if (!m_nodes.IncrSize(i))
			return false;
		// copy nodes one by one using their copy constructors
		Node_t *src,*dest;
		while (i--)
		{ // copy node
			dest=m_nodes.m_pArray+i;
			src=pTree->m_nodes.m_pArray+i;
			// copy data
			if (pTree->m_nodes.m_pInitFunc)
			{
				if (!pTree->m_nodes.m_pInitFunc(dest,&src->m_data,
					i,0))
					return false;
				m_nodes.m_nElmts++; // increment number of elements
			}
			else
				dest->m_data = src->m_data;
			// copy branches
			int inner=nBranches;
			while (inner--)
				dest->m_branches[inner]=src->m_branches[inner];
		}
		return true;
	};
	
	// You cannot remove nodes from this tree
	
	/***********************************
	Searches the entire tree for a node
	with matching data.
	***********************************/
	void LookUp(const T *data,unsigned *resultNodeID)
	{
		Node_t searchNode((T*)data);
		*resultNodeID=m_nodes.FindElmt(&searchNode);
	};

	/***********************************
	Searches for a node in the tree.
	From the given node.
	***********************************/
	void NLookUp(unsigned nodeID,const T *data,unsigned *resultNodeID)
	{
		// Reject null nodes
		if (!nodeID)
			return;
		// get node
		Node_t *node = (Node_t *)m_nodes.GetElmt(nodeID);
		if (!node)
			return; // node doesn't exist
		*resultNodeID=NULL;
		// check this node for equality
		if (node->m_data.CompareWith(data)==NULL)
		{ // this node is the matching node
			*resultNodeID=nodeID;
			return;
		}
		// search other nodes
		// loop through all branches		
		int i=nBranches;
		while (i--)
		{
			if (node->m_branches[i])
				NLookUp(node->m_branches[i],data,resultNodeID);
			if (*resultNodeID)
				return; // if found then return result
		}
	};
};

//template <class T,const int nBranches> class IXTree_t
template <class T,const int nBranches>
int IXTreeNodeCOMPFUNC(const IXTree_t<T,nBranches>::Node_t *key1,const IXTree_t<T,nBranches>::Node_t *key2)
{
	return key1->m_data.CompareWith(&key2->m_data);
}

template <class T,const int nBranches>
bool IXTreeNodeINITFUNC(IXTree_t<T,nBranches>::Node_t *obj,T *params,int arIdx,int relIdx)
{
	obj->m_data=NULL; // nullify object
	if (params)
		obj->m_data=*(relIdx+params);
	int i=nBranches;
		while (i--)
			obj->m_branches[i]=NULL;
	return true;
}

#endif//__INCLUDED_ADVDTYPES_H__