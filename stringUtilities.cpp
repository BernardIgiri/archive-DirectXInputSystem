#include "stringUtilities.h"
#include "stdmacros.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define ROUNDFRAC(x,y) ((x-1)/y + 1)

bool STRUTILInsertChar(char **pStr,char c,int idx,int maxSize)
{
	if (!pStr||!c)
		return false; // invalid input
	// get pointer
	char *str = *pStr;
	// get length
	int len = strlen(str);
	if (idx>len) // you cannot insert past end of string
		return false;
	if ((len+1)>=maxSize) // space for NULL padding must remain
		return false; // no more space
	// move data 1 space to the right
	memcpy(str+idx+1,str+idx,len-idx+1);
	str[idx]=c; // insert new char
	return true;
}


bool STRUTILAppendChar(char **pStr,char c,int maxSize)
{
	if (!pStr||!c)
		return false; // invalid input
	// get pointer
	char *str = *pStr;
	// get length
	int len = strlen(str);
	if ((len+1)>=maxSize) // space for NULL padding must remain
		return false; // no more space
	str[len]=c; // insert new char
	str[len+1]=NULL; // pad string with NULL
	return true;
}

bool STRUTILCopyStr(char **dest, const char *src)
{
	if (!src)
		return false;
	int size = strlen(src)+1;
	(*dest) = NULL;
	(*dest) = (char *)malloc(size);
	if (!(*dest))
		return false;
	memcpy((*dest),src,size);
	return true;
}

int STRUTILEstWordWrapSpace(int cols,int length)
{
	return length+(length*2)/cols+2;
}

STRING_t CreateString(const char *pStr)
{
	if (!pStr)
		return NULL;
	int length = strlen(pStr);
	if (!length)
		return NULL;
	STRING_t string = new char[length+1];
	if (string)
		memcpy(string,pStr,length+1);
	return string;
}

STRING_t CreateString(unsigned maxLen)
{
	if (maxLen)
		return new char[maxLen+1];
	else
		return NULL;
}

void DestroyString(STRING_t string)
{
	delete [] string;
}

STRING_t STRUTILEReadTxtFile(const char *path,unsigned maxLen)
{
	// check params
	if (!path||(maxLen<1))
		return NULL;
	// Open File
	FILE* fptr=fopen(path,"rb");
	// Check file pointer
	if(!fptr)
		return NULL;
	// obtain file size.
	fseek (fptr,NULL,SEEK_END);
	unsigned fSize = ftell(fptr);
	rewind(fptr);
	if (fSize>maxLen)
		fSize=maxLen;
	// Allocate buffer space
	STRING_t text=CreateString(fSize);
	// check buffer space pointer
	if (!text)
		return NULL;
	// Read text
	text[fread(text,sizeof(char),fSize,fptr)]=NULL; // append NULL
	// Close File
	fclose(fptr);
	// Return text
	return text;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************

  Initialize BITSTRING_t with the number of bits for it to contain.
  BITSTRING_t is a class for the use of bytes as an array of bits.
  Individual bits may be accessed by index just as in an array.
  Bits are stored to max compression with 8 bits to a bytes.
  All bits are off on initialization.

*****************************************************************************/
BITSTRING_t::BITSTRING_t(unsigned nBits)
{
	Clear();
	Init(nBits);
}
/*****************************************************************************

  Creates an uninitialized instance of  BITSTRING_t.
  BITSTRING_t is a class for the use of bytes as an array of bits.
  Individual bits may be accessed by index just as in an array.
  Bits are stored to max compression with 8 bits to a bytes.
  All bits are off on initialization.

*****************************************************************************/
BITSTRING_t::BITSTRING_t()
{
	Clear();
}

/*****************************************************************************

  BITSTRING_t class deconstructor

*****************************************************************************/
BITSTRING_t::~BITSTRING_t()
{
	Kill();
}

/*****************************************************************************

  Clears all data within BITSTRING_t (Use Only just before intialization)

*****************************************************************************/
void BITSTRING_t::Clear()
{
	bytes = NULL;
}

/*****************************************************************************

  Frees Up Memory Allocated by BITSTRING_t

*****************************************************************************/
void BITSTRING_t::Kill()
{
	if (bytes)
		free(bytes);
	Clear();
}

/*****************************************************************************

  Initializes or reinitializes BITSTRING_t to hold given number of bits.
  WARNING If class constructor is skipped you must call clear first!
  Failing to do so will result in a crash.

*****************************************************************************/
bool BITSTRING_t::Init(unsigned nBits)
{
	Kill();
	unsigned size = GetSize(nBits);
	bytes = (unsigned char*)malloc(size);
	if (bytes)
	{
		memset(bytes,NULL,size); // initializes bytes as NULL
		return true; // success
	}
	return false; // failure
}

/*****************************************************************************

  Returns the number of bytes needed to store the specified number
  of bits, this function does not access BITSTRING_t data.

*****************************************************************************/
unsigned BITSTRING_t::GetSize(unsigned nBits)
{
	return ROUNDFRAC(nBits,8); // round up
}

////////////////////////////////BITARRAY_t////////////////////////////////////

/*****************************************************************************

  Initialize BITARRAY_t with the number of bits for it to contain.
  BITARRAY_t is a class for the use of bytes as an array of bits.
  Individual bits may be accessed by index just as in an array.
  Bits are stored to max compression with 8 bits to a bytes.
  All bits are off on initialization.

*****************************************************************************/
BITARRAY_t::BITARRAY_t(unsigned nBits)
{
	Clear();
	Init(nBits);
}
/*****************************************************************************

  Creates an uninitialized instance of  BITARRAY_t.
  BITARRAY_t is a class for the use of bytes as an array of bits.
  Individual bits may be accessed by index just as in an array.
  Bits are stored to max compression with 8 bits to a bytes.
  All bits are off on initialization.

*****************************************************************************/
BITARRAY_t::BITARRAY_t()
{
	Clear();
}

/*****************************************************************************

  BITSTRING_t class deconstructor

*****************************************************************************/
BITARRAY_t::~BITARRAY_t()
{
	Kill();
}

/*****************************************************************************

  Frees Up Memory Allocated by BITARRAY_t

*****************************************************************************/
void BITARRAY_t::Kill()
{
	if (bytes)
		free(bytes);
	Clear();
}

/*****************************************************************************

  Clears all data within BITARRAY_t (Use Only just before intialization)

*****************************************************************************/
void BITARRAY_t::Clear()
{
	bytes = NULL;
	m_nBytes=m_nBits=NULL;
}

/*****************************************************************************

  Initializes or reinitializes BITARRAY_t to hold given number of bits.
  WARNING If class constructor is skipped you must call clear first!
  Failing to do so will result in a crash.

*****************************************************************************/
bool BITARRAY_t::Init(unsigned nBits)
{
	Kill();
	m_nBytes = GetSize(nBits);
	m_nBits = nBits;
	bytes = (unsigned char*)malloc(m_nBytes);
	if (bytes)
	{
		memset(bytes,NULL,m_nBytes); // initializes bytes as NULL
		return true; // success
	}
	return false; // failure
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************

  CAUTOSTR class constructor.

*****************************************************************************/
CAUTOSTR::CAUTOSTR()
{
	Clear();
}

/*****************************************************************************

  CAUTOSTR class deconstructor.

*****************************************************************************/
CAUTOSTR::~CAUTOSTR()
{
	Kill();
}

/*****************************************************************************

  Set all CAUTOSTR class variables to zero. USE WITH CAUTION!!!

*****************************************************************************/
void CAUTOSTR::Clear()
{
	memset(this,NULL,sizeof(CAUTOSTR));
}

/*****************************************************************************

  Frees all ram allocated by the CAUTOSTR class.

*****************************************************************************/
void CAUTOSTR::Kill()
{
	if (m_pStr)
		free(m_pStr);
	Clear();
}

/*****************************************************************************

  Copies the contents of const char *str to member string char *m_str.
  Returns false on failure.
  NOTE: This operation is not very fast for reseting the string's
  contents, though it will work in that operation.

*****************************************************************************/
bool CAUTOSTR::Set(const char *str)
{
	if (!str) // invalid input
		return false;
	Kill(); // nullify all class data
	m_size = strlen(str) + 1;
	m_pStr = (char *)malloc(m_size);
	if (!m_pStr)
	{ // malloc failed
		Clear(); // nullify all class data
		return false;
	}
	memcpy(m_pStr,str,m_size);
	return true;
}

/*****************************************************************************

  Copies the contents of member string char *m_pStr over the constents of
  the string pointed to by char **dest.
  NOTE: This methods assumes that char **dest points to a NULL char
  pointer.
  IMPORTANT: malloc is used on dest, remember to call free to free the
  allocated RAM.
  Returns false on failure.

*****************************************************************************/
bool CAUTOSTR::CopyToStr(char **dest)
{
	if (!m_pStr)
		return false;
	(*dest) = NULL;
	(*dest) = (char *)malloc(m_size);
	if (!(*dest))
		return false;
	memcpy((*dest),m_pStr,m_size);
	return true;
}

/*****************************************************************************

  Copies the contents of this CAUTOSTR over the contents of
  CAUTOSTR *dest.
  Returns false on failure.

*****************************************************************************/
bool CAUTOSTR::CopyToAutoStr(CAUTOSTR *dest)
{
	if (!dest) // invalid input
		return false;
	dest->Kill();
	dest->m_pStr = (char *)malloc(m_size);
	if (!dest->m_pStr)
		return false;
	memcpy(dest->m_pStr,m_pStr,m_size);
	dest->m_size = m_size;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************

  CTXTBUFF class constructor.

*****************************************************************************/
CTXTBUFF::CTXTBUFF()
{
	Clear();
}

CTXTBUFF::CTXTBUFF(int maxSize, bool isScrolling)
{
	Clear();
	Initialize(maxSize,isScrolling);
}

/*****************************************************************************

  CTXTBUFF class deconstructor.

*****************************************************************************/
CTXTBUFF::~CTXTBUFF()
{
	Kill();
}

/*****************************************************************************

  Set all CTXTBUFF class variables to zero. USE WITH CAUTION!!!

*****************************************************************************/
void CTXTBUFF::Clear()
{
	memset(this,NULL,sizeof(CTXTBUFF));
}

/*****************************************************************************

  Frees all ram allocated by the CTXTBUFF class.

*****************************************************************************/
void CTXTBUFF::Kill()
{
	if (m_pBuffer)
		free(m_pBuffer);
	Clear();
}

/*****************************************************************************

  Initializes CTXTBUFF to store int maxSize bytes for use in char storage.
  Returns false on failure.

  NOTE: If int maxSize is less than 3 then Initialize will return false.
  WARNING: Initialize should only be called once unless it has failed.
		   Using any other methods on this class without a successful
		   initialization may result in errors.

*****************************************************************************/
bool CTXTBUFF::Initialize(int maxSize,bool isScrolling)
{
	if (maxSize<3) // buffer is too small
		return false;
	Clear(); // nullify all class data
	m_maxSize = maxSize;
	if (isScrolling)
		m_halfSize = m_maxSize/2;
	// create buffer
	m_pBuffer = (char *)malloc(m_maxSize);
	if (!m_pBuffer)
	{ // malloc failed
		Kill();
		return false;
	}
	ClearBuffer(); // clear buffer
	return true;
}

/*****************************************************************************

  Resizes the buffer to fit the new specified maxSize.
  Returns false on failure.

  NOTE: If int maxSize is less than 3 then Initialize will return false.
  NOTE: If Resize fails to allocate memory, m_pBuffer will be set to NULL.

*****************************************************************************/
bool CTXTBUFF::Resize(int maxSize)
{
	if (maxSize<3) // buffer is too small
		return false;
	if (!m_pBuffer)
		return false; // buffer must be initialized first
	m_maxSize = maxSize; // copy new maxSize
	if (m_halfSize) // recalc halfSize if necessary
		m_halfSize = m_maxSize/2;
	if (m_maxSize<(m_strLength+1)) // if the current string length will not fit
	{
		m_strLength = m_maxSize-1; // clip string
		m_pBuffer[m_strLength]=NULL; // pad string with NULL
	}
	m_pBuffer = (char *)realloc(m_pBuffer,m_maxSize);
	if (!m_pBuffer)
	{ // realloc failed
		Kill();
		return false; // system must now be reinitialized
	}
	return true;
}

/*****************************************************************************

  Switches the buffer between scrolling and non-scrolling modes.
  In Scrolling mode when the buffer is overrun the data in the first half
  of the buffer is removed from the buffer and all data is shifted
  over by half m_maxSize.

*****************************************************************************/
void CTXTBUFF::SetBuffMode(bool isScrolling)
{
	if (isScrolling)
		m_halfSize = m_maxSize/2;
	else
		m_halfSize = 0;
}

/*****************************************************************************

  Appends char c to the end of the buffer.  Returns false on failure.

*****************************************************************************/
bool CTXTBUFF::PutChar(char c)
{
	if (!m_pBuffer||!c)
		return false; // invalid input
	if ((m_strLength+1)>=m_maxSize) // space for NULL padding must remain
	{ // no more space
		if (m_halfSize==0)
			return false; // if not dynamic do no insert new char, return false
		CutBuffer(); // make more space in the buffer
		if ((m_strLength+1)>=m_maxSize)
			return false; // still not enough space
	}
	m_pBuffer[m_strLength]=c; // insert new char
	m_pBuffer[m_strLength+1]=NULL; // pad string with NULL
	m_strLength++; // add to str length counter
	return true;
}

/*****************************************************************************

  Appends string str to the end of the buffer.  Returns false on failure.

*****************************************************************************/
bool CTXTBUFF::PutStr(const char *str)
{
	if (!m_pBuffer||!str)
		return false; // invalid input
	int spaceLeft = m_maxSize - (m_strLength +1);
	int length = strlen(str);
	if (!m_halfSize) // if !isScrolling then..
	{
		if (!spaceLeft)
			return false; // no space left copy not possible
		if (length>spaceLeft)
			length = spaceLeft; // clip string if necessary
	}
	else
	{ // if isScrolling then
		if ((length+1)>m_halfSize) // str is too long
			return false; // reject it
		if (length>spaceLeft) // str is longer than spaceLeft
			CutBuffer(); // make more space in the buffer
	}
	memcpy(m_pBuffer+m_strLength,str,length); // copy string
	m_strLength+=length;  // add to str length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Appends string str to the end of the buffer.  Returns false on failure.
  NOTE: This method is faster since strlen is not called, set int length
  to the length of the string in chars.

*****************************************************************************/
bool CTXTBUFF::PutStr(const char *str,int length)
{
	if (!m_pBuffer||!str||!length)
		return false; // invalid input
	int spaceLeft = m_maxSize - (m_strLength +1);
	if (!m_halfSize) // if !isScrolling then..
	{
		if (!spaceLeft)
			return false; // no space left copy not possible
		if (length>spaceLeft)
			length = spaceLeft; // clip string if necessary
	}
	else
	{ // if isScrolling then
		if ((length+1)>m_halfSize) // str is too long
			return false; // reject it
		if (length>spaceLeft) // str is longer than spaceLeft
			CutBuffer(); // make more space in the buffer
	}
	memcpy(m_pBuffer+m_strLength,str,length); // copy string
	m_strLength+=length;  // add to str length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Overwrites the buffer string with the contents of string str.
  Returns false on failure.
  NOTE: The string will be clipped to fit the buffer if necessary.

*****************************************************************************/
bool CTXTBUFF::SetStr(const char *str)
{
	if (!m_pBuffer||!str)
		return false; // invalid input
	int length = strlen(str);
	if ((length+1)>m_maxSize)
		length = m_maxSize - 1; // clip string to fit buffer
	memcpy(m_pBuffer,str,length); // copy string
	m_strLength=length; // reset string length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Overwrites the buffer string with the contents of string str.
  Returns false on failure.
  NOTE: The string will be clipped to fit the buffer if necessary.
  NOTE: This method is faster since strlen is not called, set int length
  to the length of the string in chars.

*****************************************************************************/
bool CTXTBUFF::SetStr(const char *str,int length)
{
	if (!m_pBuffer||!str||!length)
		return false; // invalid input
	if ((length+1)>m_maxSize)
		length = m_maxSize - 1; // clip string to fit buffer
	memcpy(m_pBuffer,str,length); // copy string
	m_strLength=length; // reset string length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}
/*****************************************************************************

  Works like sprintf, this method overwrites the buffer string with the
  resulting string.
  Returns false on failure.

  WARNING: It buffer overflow occurs, this may corrupt memory, ensure
  that the buffer is large enough for the resultant string.

*****************************************************************************/
bool CTXTBUFF::SPrintF(const char *format,...)
{
	if (!m_pBuffer)
		return false;
	va_list arglist;
	va_start(arglist,format);
	vsprintf(m_pBuffer,format,arglist);
	va_end(arglist);
	m_strLength = strlen(m_pBuffer);
	return true;
}

/*****************************************************************************

  Inserts a char into the buffer at the specified index into the buffer
  string.
  Returns false on failure.

*****************************************************************************/
bool CTXTBUFF::InsertChar(int idx,char c)
{
	if (!m_pBuffer||!c||!((idx>-1)&&(idx<=m_strLength)))
		return false; // invalid input
	// **** Test For Available Space ****
	if ((m_strLength+1)>=m_maxSize) // space for NULL padding must remain
	{ // no more space
		if (m_halfSize==0)
			return false; // if not dynamic do no insert new char, return false
		CutBuffer(); // make more space in the buffer
		if ((m_strLength+1)>=m_maxSize)
			return false; // still not enough space
	}
	// **** Insert New Char ****
	memcpy(m_pBuffer+idx+1,m_pBuffer+idx,m_strLength-idx); // shift data over by 1
	m_pBuffer[idx]=c; // insert new char
	m_strLength++; // add to str length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Inserts a string into the buffer at the specified index into the buffer
  string.  If the entered string is too long then it is clipped.
  Returns false on failure.

*****************************************************************************/
bool CTXTBUFF::InsertStr(int idx,const char *str)
{
	if (!m_pBuffer||!str||!((idx>-1)&&(idx<=m_strLength)))
		return false; // invalid input
	int spaceLeft = m_maxSize - (m_strLength +1);
	int length = strlen(str);
	// **** Test For Available Space ****
	if (!m_halfSize) // if !isScrolling then..
	{
		if (!spaceLeft)
			return false; // no space left, copy not possible
		if (length>spaceLeft)
			length = spaceLeft; // clip string if necessary
	}
	else
	{ // if isScrolling then
		if ((length+1)>m_halfSize) // str is too long
			return false; // reject it
		if (length>spaceLeft) // str is longer than spaceLeft
			CutBuffer(); // make more space in the buffer
	}
	// **** Insert New String ****
	memcpy(m_pBuffer+idx+length,m_pBuffer+idx,m_strLength-idx); // shift data over by length
	memcpy(m_pBuffer+idx,str,length); // insert string (without the NULL char)
	m_strLength+=length; // update length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Inserts a string into the buffer at the specified index into the buffer
  string.  If the entered string is too long then it is clipped.
  Returns false on failure.

  NOTE: This method is faster since strlen is not called, set int length
  to the length of the string in chars.

*****************************************************************************/
bool CTXTBUFF::InsertStr(int idx,const char *str,int length)
{
	if (!m_pBuffer||!str||!((idx>-1)&&(idx<=m_strLength)))
		return false; // invalid input
	int spaceLeft = m_maxSize - (m_strLength +1);
	// **** Test For Available Space ****
	if (!m_halfSize) // if !isScrolling then..
	{
		if (!spaceLeft)
			return false; // no space left, copy not possible
		if (length>spaceLeft)
			length = spaceLeft; // clip string if necessary
	}
	else
	{ // if isScrolling then
		if ((length+1)>m_halfSize) // str is too long
			return false; // reject it
		if (length>spaceLeft) // str is longer than spaceLeft
			CutBuffer(); // make more space in the buffer
	}
	// **** Insert New String ****
	memcpy(m_pBuffer+idx+length,m_pBuffer+idx,m_strLength-idx); // shift data over by length
	memcpy(m_pBuffer+idx,str,length); // insert string (without the NULL char)
	m_strLength+=length; // update length counter
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Removes a segement of text from the buffer. The segement within the range
  of the indexes specified by start and end are removed..
  Returns false on failure.

*****************************************************************************/
bool CTXTBUFF::DeleteSec(int start,int end)
{
	if (!m_pBuffer||!ISIDXVALID(start,m_strLength)||!((end>-1)&&(end<=m_strLength))||
		(start>end))
		return false; // invalid input
	memcpy(m_pBuffer+start,m_pBuffer+end,m_strLength-end); // cut out segment
	m_strLength-=(end-start); // update string length
	m_pBuffer[m_strLength] = NULL; // pad string with NULL
	return true;
}

/*****************************************************************************

  Word wraps text to specified number of columns.  The characters specified
  by pBrkChars are used as potential line breaking characters.
  If pBrkChars is NULL, then the default break chars are used.
  Returns false on failure.

  If bool isForced is set to true, Resize will be called whenever the buffer
  is filled. This will add a minimal ammount of overhead. However, with
  isForced set to true, this process will not stop until the operation is
  complete.

  NOTE: If word wrap fails mid way, the buffer text may only be partially
  word wrapped.
  
  ADDED FEATURE: Set index to the index within the string on which you want
  word wrapping to begin on.

*****************************************************************************/
bool CTXTBUFF::WordWrap(int cols,const char *pBrkChars,int index,bool isForced)
{
	if (!m_pBuffer||(cols<1)||(index<0))
		return false;
	if (!pBrkChars)
		pBrkChars = STRUTIL_DEFBREAKCHARS;
	int nBrkChars = strlen(pBrkChars);
	if (!nBrkChars)
		return false;
	int lineLen=0,
		lastBrkChar=-1; // -1 = none found
	while (index<m_strLength)
	{
		lineLen++;
		if (STRUTILCmpAgnstStr(m_pBuffer[index],pBrkChars,nBrkChars))
			lastBrkChar = index;
		if (m_pBuffer[index]=='\n') // if line break was found
		{
			lastBrkChar=-1; // reset last breaking char
			lineLen=0; // reset line length
		}
		else if (lineLen>=cols)
		{
			if (m_pBuffer[index+1]!='\n')
			{ // if the line is not already broken then break it
				if (lastBrkChar==-1) // if there is no breaking char
					lastBrkChar=index; // use current char as break point
				lastBrkChar++; // get postion after last breaking char
				if (!InsertChar(lastBrkChar,'\n')) // insert new line char
				{ //if insert failed
					if (!isForced) // and word wrap is not forced then return false
						return false;
					// else resize and try again
					Resize(m_maxSize+cols);
					if (!InsertChar(lastBrkChar,'\n')) // out of memory
						return false;
				}
			}
			else
			{
				lastBrkChar = index+1;
			}
			index=lastBrkChar; // reset index
			lastBrkChar=-1; // reset last breaking char
			lineLen=0; // reset line length
		}
		index++;
	}
	return true;
}

/*****************************************************************************

  Empties all string data held in the text buffer.

*****************************************************************************/
void CTXTBUFF::ClearBuffer()
{
	m_pBuffer[0]=NULL; // clear buffer
	m_strLength =0; // reset string size
}

/*****************************************************************************

  Cuts the text buffer in half, moving that last half to the start of the
  buffer and leaving the rest open for new string data.

*****************************************************************************/
void CTXTBUFF::CutBuffer()
{
	m_wasCut = true;
	m_strLength = m_maxSize-m_halfSize -1; // get new string length
	memcpy(m_pBuffer,m_pBuffer+m_halfSize,m_strLength); // copy end half over begining half
	m_pBuffer[m_strLength] = NULL; // pad new string with NULL
	m_strLength = strlen(m_pBuffer); // get actual string length
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
