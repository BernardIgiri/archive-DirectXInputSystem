/*****************************************************************************

  Input Output Utilities
  ioUtilities.h

  DATE	:	12/29/2003
  AUTHOR:	Bernard Igiri

  A set of functions and classes to assist in file loading and saving
  binary data.

  Use

  This code has the following dependancies:
  <stdio.h> <malloc.h> <memory.h> <string.h>

*****************************************************************************/
#ifndef __INIOULUDED_IOUTILITIES_H__
#define __INIOULUDED_IOUTILITIES_H__

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

/*****************************************************************************

  IOUBData concept explained-----------------------------

  Purpose:
	The purpose of IOUBData is to simplify the loading and saving of binary
	data stored in structures and classes in memory. IOUBData oraginizes
	the binary info so that it may be easily saved or loaded using the
	Save and Load member functions. IOUBData does not open or close the file
	it operates on the file stream sent to it in the save and load functions.

	IOUBData objects are made up of three major parts
	1) IOUBDataItm - Data Item
		Each data item is a data type in a structure or class
		this data type may be an array, but it must not contain any pointers.
		Data Items that are arrays have elements (all data items should have
		at least one element), the number of "elements" in a data item is
		"variable" and is thus "saved to and loaded from" the binary file.
	2) IOUBDataUnit - Data Unit
		Each data unit is a structure or a class, and they should contain
		a "fixed number" of "Data Items", the "number of data items" is "NOT
		saved to or loaded from the binary file."
	3) IOUBData - Data
		This is the full collection of "data units", the number of data units
		is "variable" and is "saved to and loaded from" the binary file.

  Memory Management Note:
	All data recieved by the IOUBData object is dynamically copied and
	managed internally.
	All data sent out from the IOUBData object must be copied and
	managed by the user. IOUBData does not allocate ram for the copies of
	the data sent out, it only stores and maintains its own copies.
  File Format Note:
	"Data Units" are saved in reverse order.

*****************************************************************************/

class IOUBDataItm
{
public:
	IOUBDataItm();
	~IOUBDataItm();

	// saves data
	bool Save(FILE *pFilePtr);
	// loads data
	bool Load(FILE *pFilePtr);

	// sets the size of individual elements
	inline void SetElmtSize(unsigned n)
	{
		m_elmtSize = n;
	};
	// sets item data
	inline bool SetData(unsigned nElmts,unsigned elmtSize,void *pElmts)
	{
		m_elmtSize = elmtSize;
		m_nElmts   = nElmts;
		m_pElmts   = malloc(m_elmtSize*m_nElmts); // allocate space for data
		if (!m_pElmts)
			return false;
		memcpy(m_pElmts,pElmts,m_elmtSize*m_nElmts); // copy data
		return true; // return success
	};
	// gets item data
	inline void GetData(unsigned &nElmts,unsigned &elmtSize,void **pElmts)
	{
		elmtSize = m_elmtSize;
		nElmts   = m_nElmts;
		*pElmts  = m_pElmts;
	};
private:
	unsigned m_nElmts; // number of elements (saved to the file)
	unsigned m_elmtSize; // size of individual elements
	void*	 m_pElmts; // pointer to elements in continuous memory alignment
};

class IOUBDataUnit
{
public:
	IOUBDataUnit();
	IOUBDataUnit(unsigned nItms);
	~IOUBDataUnit();
	// saves data
	bool Save(FILE *pFilePtr);
	// loads data
	bool Load(FILE *pFilePtr);

	// sets the number of items in this data unit
	bool SetNItms(unsigned n);

	// sets the format of the data unit
	inline void SetFormat(unsigned *pElmtSizes)
	{
		unsigned i=m_nItms;
		while (i--)
			(m_pItms+i)->SetElmtSize(*(pElmtSizes+i));
	};
	// Sets data for item at specified index
	inline bool SetItms(unsigned idx,unsigned nElmts,unsigned elmtSize,void *pElmts)
	{
		return (m_pItms+idx)->SetData(nElmts,elmtSize,pElmts);
	};
	// Gets data from item at specified index
	inline void GetItms(unsigned idx,unsigned &nElmts,unsigned &elmtSize,void **pElmts)
	{
		(m_pItms+idx)->GetData(nElmts,elmtSize,pElmts);
	};
private:
	unsigned	 m_nItms; // number of items (supplied by user)
	IOUBDataItm* m_pItms; // pointer to items
};

class IOUBData
{
public:
	IOUBData();
	IOUBData(unsigned nItmsPUnit);
	IOUBData(unsigned nItmsPUnit,unsigned nUnits);
	~IOUBData();

	// saves data
	bool Save(FILE *pFilePtr);
	// loads data
	bool Load(FILE *pFilePtr);

	// Sets size format of units items - pElmtSizes is a ptr to an array of the unit item sizes
	inline void SetItmFormat(const unsigned *pElmtSizes)
	{
		if (m_state==SETTING_SAVE_FORMAT)
			m_state=SETTING_SAVE_DATA;
		else if (m_state==SETTING_LOAD_FORMAT)
			m_state=LOADING_DATA;
		else
			return; // operation done in wrong order
		memcpy(m_pElmtSizes,pElmtSizes,sizeof(unsigned)*m_nItmsPUnit);
	};
	// Selects the current unit for updating
	void SelUnit(unsigned idx);
	// Sets the parameters for loading binary data
	bool SetLoadParams(unsigned nItmsPUnit); // SetNItmsPUnit
	// Sets the parameters for saving binary data
	bool SetSaveParams(unsigned nItmsPUnit,unsigned nUnits);

	// Sets data for items within current unit - idx is the item index
	inline bool SetUnitItms(unsigned idx,unsigned nElmts,void *pElmts)
	{
		if (m_state==SETTING_SAVE_DATA)
			return (m_pUnits+m_currUnit)->SetItms(idx,nElmts,*(m_pElmtSizes+idx),pElmts);
		else
			return false; // operation done in wrong order
	};
	// Gets data from items within current unit - idx is the item index
	inline void GetUnitItms(unsigned idx,unsigned &nElmts,unsigned &elmtSize,void **pElmts)
	{
		if (m_state==OUTPUTTING_LOADED_DATA)
			(m_pUnits+m_currUnit)->GetItms(idx,nElmts,elmtSize,pElmts);
		else
			return; // operation done in wrong order
	};
	// Gets the number of units read from the binary file
	inline int GetNUnits()
	{
		return m_nUnits;
	}
	inline bool GetIsLoading()
	{
		return (m_state==SETTING_LOAD_FORMAT||
				m_state==LOADING_DATA||
				m_state==OUTPUTTING_LOADED_DATA);
	}
	// state constants, shows the next action to be performed
	enum STATE_t { INITIALIZING,
		SETTING_SAVE_FORMAT, SETTING_SAVE_DATA, SAVING_DATA, // save states
		SETTING_LOAD_FORMAT, LOADING_DATA, OUTPUTTING_LOADED_DATA // load states
	};
	inline STATE_t GetState()
	{
		return m_state;
	}
	// error constants, shows the current error status of the system
	enum ERROR_t { ISOK, OUTOFMEMORY, FILENOTFOUND, FILEISPROTECTED, INVALIDPARAMS,
		OPOUTOFORDER, // operation done out of order or invalid operation
		WRITEFAILED, READFAILED
	};
	inline ERROR_t GetError()
	{
		return m_error;
	}
private:
	// set the number of items per unit and allocates ram for m_pElmtSizes
	bool SetNItmsPUnit(unsigned nItmsPUnit);
	// Sets the parameters for saving binary data
	bool SetParams(unsigned nItmsPUnit,unsigned nUnits);

	ERROR_t		  m_error; // shows the current error status of the system
	STATE_t		  m_state; // shows the next action to be performed by IOUBData
	unsigned	  m_currUnit; // currently selected unit
	unsigned*	  m_pElmtSizes; // size values of elements of unit items
	unsigned	  m_nItmsPUnit; // number of items per unit
	unsigned	  m_nUnits; // number of units
	IOUBDataUnit* m_pUnits; // pointer to units
};

// Returns true if the file exists
bool IOUFileExist(const char *pPath);

// Encrypts string using the specified key, returns modified string, NOTE: Results may contain all 256 chars
char *EncryptStr(char *str,const char *key);
// Decrypts string using the specified key, returns modified string
char *DecryptStr(char *str,const char *key);

#endif//__INIOULUDED_IOUTILITIES_H__