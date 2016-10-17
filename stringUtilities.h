/*****************************************************************************
//////////////////////////////////////////////////////////////////////////////

  DATE	:	11/26/2003
  AUTHOR:	Bernard Igiri

  "stringUtilities.h"

  This is a collection of various functions, definitions, and types for
  use in operations with strings.

  This code has the following dependancies:
  "stdmacros.h" <string.h> <stdlib.h> <stdio.h> <stdarg.h>

///////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifndef __INCLUDED_STRINGUTILITIES_H__
#define __INCLUDED_STRINGUTILITIES_H__

#define STRUTIL_DEFBREAKCHARS " ,;'\"-+*/"

/*****************************************************************************

  This function returns true if char c exists in string str.

*****************************************************************************/
inline bool STRUTILCmpAgnstStr(char c,const char *str,int length)
{
	while (length--)
		if (c==str[length])
			return true;
	return false;
}

/*****************************************************************************

  Returns the last occurance of a given char within a string.
  Returns -1 on failure.

*****************************************************************************/
inline int STRUTILFindLast(char c,const char *str,int length)
{
	while (length--)
		if (c==*(str+length))
			return length;
	return -1;
}

/*****************************************************************************

  Returns the first occurance of a given char within a string.
  Returns -1 on failure.

*****************************************************************************/
inline int STRUTILFindFirst(char c,const char *str,int length)
{
	int i=length;
	length--; // we need length - 1
	while (i--)
		if (c==*(str+(length-i)))
			return length;
	return -1;
}

/*****************************************************************************

  This function sets *dest to null and then mallocs memory and copies *src
  to *dest.  **dest should be a pointer to a char pointer.  This funcion
  returns false on failure. \/

*****************************************************************************/
bool STRUTILCopyStr(char **dest, const char *src);

/*****************************************************************************

  This function takes the value of unsinged char byte and sets the bit,
  int bit (0-7) to bool value and returns the resulting unsigned char.

*****************************************************************************/
inline unsigned char STRUTILSetBit(int bit, unsigned char byte, bool value)
{
	unsigned char table[8] = {1,2,4,8,16,32,64,128};
	unsigned char check = table[bit]; // set check to blank byte with selected bit on
	if (value) // if on then...
		return byte | check; // turn bit on. Or turn bit off.
	else if (byte & check) // First, check if bit is on
		return byte^check; // if so then turn bit off
	return byte; // if not return byte since bit is already off
}

/*****************************************************************************

  This function takes the value of unsinged char byte and returns the
  true/false status of the bit int bit (0-7).

*****************************************************************************/
inline bool STRUTILCheckBit(int bit, unsigned char byte)
{
	unsigned char table[8] = {1,2,4,8,16,32,64,128};
	return (byte & table[bit])!=0;
}

/*****************************************************************************

  This function takes the string pointed to by char **pStr and inserts
  char c into the string at index int idx within the restraint of
  int maxSize.

*****************************************************************************/
bool STRUTILInsertChar(char **pStr,char c,int idx,int maxSize);

/*****************************************************************************

  This function takes the string pointed to by char **pStr and appends
  char c to the end of the string within the restraint of int maxSize.

*****************************************************************************/
bool STRUTILAppendChar(char **pStr,char c,int maxSize);

/*****************************************************************************

  This function estimates the max space needed to wrap a string of
  given length to the given number of columns.

  NOTE: The smaller cols is, the larger the return value.

*****************************************************************************/
int STRUTILEstWordWrapSpace(int cols,int length);

/*****************************************************************************

  A simple string type defined to impliment simple rules for memory
  allocation and deallocation.
  Use CreateString and DestroyString to create and destroy STRING_t
  objects.

*****************************************************************************/
typedef char* STRING_t;

/*****************************************************************************

  Creates a STRING_t object. And set its value to the contents
  of pStr.

  Also use this to create strings using the new operator.

*****************************************************************************/
STRING_t CreateString(const char *pStr);

/*****************************************************************************

  Creates a blank STRING_t object that can hold up to maxLen.

  Also use this to create strings using the new operator.

*****************************************************************************/
STRING_t CreateString(unsigned maxLen);

/*****************************************************************************

  Destroys a STRING_t object.

  Also use this to destroy strings using the delete operator.

  NOTE: This  function does it's own null check.

*****************************************************************************/
void DestroyString(STRING_t string);

/*****************************************************************************

  This function loads text from the given file up unsigned maxLen chars.
  The resulting string should be destroyed using DestroyString()

*****************************************************************************/
STRING_t STRUTILEReadTxtFile(const char *path,unsigned maxLen);

/*****************************************************************************

  This class allows for bits to be stored as 8 bit bytes and accessed
  individually by index.

  Clear before use if its class constructor is skipped.
  Also call Init before use, use Check and Set to access individual bits
  by index.

  GetSize returns the number of bytes needed to store the specified number
  of bits, this function does not access BITSTRING_t data.

  NOTE: This type may be safely casted as an unsigned char pointer.

*****************************************************************************/
class BITSTRING_t
{
public:
	BITSTRING_t(unsigned nBits);
	BITSTRING_t();
	~BITSTRING_t();
	void Clear();
	void Kill();
	bool Init(unsigned size);

	// Returns Off/On status of indexed bit. Works like an array.
	inline bool BITSTRING_t::Check(unsigned idx)
	{
		if (!bytes) // if not initialized return false
			return false;
		int byteIdx = idx/8;
		return STRUTILCheckBit(idx-byteIdx*8,*(bytes+byteIdx)); // calc indices and return bit
	};

	// Sets Off/On status of indexed bit. Works like an array. Returns the same value sent.
	inline bool BITSTRING_t::Set(unsigned idx, bool value)
	{
		if (!bytes) // if not initialized return false
			return false;
		int byteIdx = idx/8;
		*(bytes+byteIdx) = STRUTILSetBit(idx-byteIdx*8,*(bytes+byteIdx),value);	// calc indices and set value
		return value; // return value
	};

	unsigned GetSize(unsigned nBits);
	unsigned char *bytes;
};

// Same as BITSTRING_t with m_nBytes and m_nBits added

class BITARRAY_t : public BITSTRING_t
{
public:
	BITARRAY_t(unsigned nBits);
	BITARRAY_t();
	~BITARRAY_t();
	void Clear();
	void Kill();
	bool Init(unsigned size);
	unsigned m_nBytes; // number of bytes occupied by BITARRAY_t
	unsigned m_nBits; // number of bits in BITARRAY_t
};

/*****************************************************************************

  This class allows for managed storage of string data.
  Use bool Set(const char *str); to set the contents of the string.
  If the class constructor has been skipped call void Clear() first
  to initialize the class.

*****************************************************************************/
class CAUTOSTR
{
public:
	CAUTOSTR();
	~CAUTOSTR();
	void Clear();
	void Kill();
	bool Set(const char *str);
	bool CopyToStr(char **dest);
	bool CopyToAutoStr(CAUTOSTR *dest);
	char *m_pStr; // pointer to string
	int	  m_size; // current size of string (subtract 1 for the length)
};

/*****************************************************************************

  This class functions as a self maintaining text buffer.
  When new string (or char) input exceeds the buffer's size the text
  buffer may respond in one of two ways.
  1) Only the chars that go beyond the size limit are not copied.
  2) The contents of the buffer up one half it's size is removed
     and the new string is inserted at the end of the buffer.
	 With this method any strings longer than half the buffer size
	 are automatically rejected. This is occurs when the buffer is in
	 scrolling mode.

  For method 1 set bool isScrolling to false on initialization or
  by calling SetBuffMode.

*****************************************************************************/
class CTXTBUFF
{
public:
	CTXTBUFF();
	CTXTBUFF(int maxSize, bool isScrolling=false);
	~CTXTBUFF();
	void Kill();
	bool Initialize(int maxSize, bool isScrolling=false);
	bool Resize(int maxSize);
	void SetBuffMode(bool isScrolling);
	bool PutStr(const char *str);
	bool PutStr(const char *str,int length);
	bool PutChar(char c);
	bool SetStr(const char *str);
	bool SetStr(const char *str,int length);
	bool SPrintF(const char *format,...);

	bool InsertChar(int idx,char c);
	bool InsertStr(int idx,const char *str);
	bool InsertStr(int idx,const char *str,int length);
	bool DeleteSec(int start,int end);
	bool WordWrap(int cols,const char *pBrkChars,int index=0,bool isForced=false);

	void ClearBuffer();

	bool  m_wasCut;		// set to true anytime CutBuffer is called
	char *m_pBuffer;	// pointer to text buffer string
	int m_maxSize;		// maximum size of buffer
	int m_halfSize;		// half of maxSize or zero if not scrolling
	int m_strLength;	// current length of buffer string
private:
	void Clear();
	void CutBuffer();
};

#endif//__INCLUDED_STRINGUTILITIES_H__