#include "textInput.h"
#include "stdmacros.h"
#include "textDisplay.h"
#include <memory.h>

CTXTBUFF* CTXTIN::m_pClipBoard = NULL;
int CTXTIN::m_nTxtIns = 0;

/*****************************************************************************

  CTXTIN class constructor.

*****************************************************************************/
CTXTIN::CTXTIN()
{
	// handle static vars
	if (!m_pClipBoard)
		m_pClipBoard = new CTXTBUFF(TEXIN_CLIPBOARD_BUFFERSIZE);
	m_nTxtIns++;
	// initiliaze non-static vars
	m_options	= NULL;
	m_cursor	= NULL;
	m_selcursor	= NULL;
	m_pText		= NULL;
}

/*****************************************************************************

  CTXTIN class constructor.
  Sets the max length and the isMultiline flag for text input.

*****************************************************************************/
CTXTIN::CTXTIN(int maxLen,bool isMultiline)
{
	// handle static vars
	if (!m_pClipBoard)
		m_pClipBoard = new CTXTBUFF(TEXIN_CLIPBOARD_BUFFERSIZE);
	m_nTxtIns++;
	// do dynamic initialization
	Initialize(maxLen,isMultiline);
}

/*****************************************************************************

  CTXTIN class destructor.

*****************************************************************************/
CTXTIN::~CTXTIN()
{
	// handle static vars
	m_nTxtIns--;
	if (m_nTxtIns<1)
	{
		delete m_pClipBoard;
		m_pClipBoard=NULL;
	}
	// handle non-static vars
	// deallocate ram
	delete m_pText;
	m_options	= NULL;
	m_cursor	= NULL;
	m_selcursor	= NULL;
	m_pText		= NULL;
}

/*****************************************************************************

  Initializes or reinitializes the CTXTIN class.

*****************************************************************************/
bool CTXTIN::Initialize(int maxLen,bool isMultiline)
{
	// deallocate ram if necessary
	delete m_pText;
	// initialize non-static vars
	m_options	= NULL;
	m_cursor	= NULL;
	m_selcursor	= NULL;
	m_pText		= NULL;
	// set flags
	m_options = STRUTILSetBit(TEXIN_OPT_ISMULTILINE,m_options,isMultiline);
	// allocate ram
	m_pText = new CTXTBUFF(maxLen+1);
	if (!m_pText)
		return false;
	return (m_pText->m_pBuffer!=NULL); // return initialization success
}

/*****************************************************************************

  Deletes the selected text.

*****************************************************************************/
void CTXTIN::DelSelTxt()
{
	int start=m_cursor,end=m_selcursor;
	if (start>end)
	{
		end=m_cursor;
		start=m_selcursor;
	}
	m_pText->DeleteSec(start,end);
	m_selcursor=m_cursor=start;
}

/*****************************************************************************

  Performs the initial step in moving the cursor when handeling selected
  text.  Call before moving the cursor.
  The result should be be copied and passed on to DragEnd.

  Returns true if the text should be deselected

*****************************************************************************/
bool CTXTIN::DragStart(bool isHighLighted,unsigned char sKeys)
{
	if (STRUTILCheckBit(TEXIN_SKEY_ISSHIFT,sKeys))
	{// shift key is down
		if (!isHighLighted) // if not hightlighted then do so
			m_selcursor=m_cursor;
	}
	else if (isHighLighted) // highlighted but shift key is not down, deselect
		return true;
	else // not highlighted and shift key is not down, deselect
		return true;
	return false;
}

/*****************************************************************************

  Performs the final step in moving the cursor when handeling selected
  text.  Call before after movie the cursor.
  Should be passed the returned value of the previous DragStart call.

*****************************************************************************/
void CTXTIN::DragEnd(bool isDeselected)
{
	if (isDeselected)
		m_selcursor=m_cursor;
}

/*****************************************************************************

  Updates the text input object with the next entered character.
  Returns true if the the string contents were changed.
  NOTE: sKeys is a set of key flags defined the same way as CINPUTMAN's
  SKey flags are defined.  The first bit shows the status of the shift keys,
  the next for the control, and then the alt keys. 1 bit for each.

*****************************************************************************/
bool CTXTIN::Update(char c,unsigned char sKeys)
{
	if (!m_pText) // must be initialized first
		return false;
	// clamp cursor positions
	CLAMP(0,m_cursor,m_pText->m_strLength);
	CLAMP(0,m_selcursor,m_pText->m_strLength);
	bool isHighLighted=(m_cursor!=m_selcursor);
	// Filter Text Input
	if (!(c>31&&c<128))
	{
		switch ((unsigned char)c)
		{
		case '\n': // newline char
		case '\f': // form feed char
		case '\r': // carraige return char
			if (STRUTILCheckBit(TEXIN_OPT_ISMULTILINE,m_options))
				c='\n';
			else
				return false; // reject invalid chars
			break;
		case '\t': // tab char
			break;
		case TEXIN_SKCODE_BKSPC:
			if (!isHighLighted)
			{
				m_cursor--; // move back 1
				m_selcursor=m_cursor; // deselect
			}
			CLAMP(0,m_cursor,m_pText->m_strLength);
		case TEXIN_SKCODE_DELETE:
			// delete character
			if (!isHighLighted)
				m_pText->DeleteSec(m_cursor,m_cursor+1);
			else
				DelSelTxt();
			return true; // report change to string
			break;
		case TEXIN_SKCODE_INSERT:
			m_options = STRUTILSetBit(TEXIN_OPT_ISOVERWRITE,
				m_options,!STRUTILCheckBit(TEXIN_OPT_ISOVERWRITE,m_options));
			return false; // finished
			break;
		case TEXIN_SKCODE_HOME:
			{//DelSelTxt();
				bool isDeselected=DragStart(isHighLighted,sKeys); // drag start

					if (STRUTILCheckBit(TEXIN_OPT_ISMULTILINE,m_options)&&
						!STRUTILCheckBit(TEXIN_SKEY_ISCTRL,sKeys))
					{ // find start of line
						m_cursor--;
						if (m_cursor<0)
							m_cursor=0;
						while (m_cursor&&(*(m_pText->m_pBuffer+m_cursor)!='\n'))
							m_cursor--;
						m_cursor++;
						if (m_cursor>m_pText->m_strLength)
							m_cursor=m_pText->m_strLength;
					}
					else
						m_cursor=0;

				DragEnd(isDeselected); // drag end
				return false; // finished
			}
			break;
		case TEXIN_SKCODE_END:
			{
				bool isDeselected=DragStart(isHighLighted,sKeys); // drag start

					if (STRUTILCheckBit(TEXIN_OPT_ISMULTILINE,m_options)&&
						!STRUTILCheckBit(TEXIN_SKEY_ISCTRL,sKeys))
					{ // find end of line
						while ((m_cursor<m_pText->m_strLength)&&(*(m_pText->m_pBuffer+m_cursor)!='\n'))
							m_cursor++;
					}
					else
						m_cursor=m_pText->m_strLength;

				DragEnd(isDeselected); // drag end
				return false; // finished
			}
			break;
		case TEXIN_SKCODE_RIGHT:
			{
				bool isDeselected=DragStart(isHighLighted,sKeys); // drag start

					m_cursor++;
					if (STRUTILCheckBit(TEXIN_SKEY_ISCTRL,sKeys))
					{ // ctr+right
						CLAMP(0,m_cursor,m_pText->m_strLength);
						// scroll right to first space char or newline char
						while ((m_cursor<m_pText->m_strLength)&&
							   *(m_pText->m_pBuffer+m_cursor)!=' '&&
							   *(m_pText->m_pBuffer+m_cursor)!='\n')
							   m_cursor++;
						// scroll right passed all space chars
						while ((m_cursor<m_pText->m_strLength)&&
								*(m_pText->m_pBuffer+m_cursor)==' ')
								m_cursor++;
					}

				DragEnd(isDeselected); // drag end
				return false; // finished
			}
			break;
		case TEXIN_SKCODE_LEFT:
			{
				bool isDeselected=DragStart(isHighLighted,sKeys); // drag start

					m_cursor--;
					if (STRUTILCheckBit(TEXIN_SKEY_ISCTRL,sKeys))
					{ // ctr+left
						CLAMP(0,m_cursor,m_pText->m_strLength);
						// scroll left passed all space chars
						while ((m_cursor>=0)&&
								*(m_pText->m_pBuffer+m_cursor)==' ')
								m_cursor--;
						// scroll left to first space char
						while ((m_cursor>=0)&&
								*(m_pText->m_pBuffer+m_cursor)!=' ')
								   m_cursor--;
						if (*(m_pText->m_pBuffer+m_cursor)==' ')
						{ // scroll to right of space char
							m_cursor++;
							BOUNDUPR(m_cursor,m_pText->m_strLength);
						}
					}

				DragEnd(isDeselected); // drag end
				return false; // finished
			}
			break;
		case TEXIN_SKCODE_UP:
			{
				bool isDeselected=DragStart(isHighLighted,sKeys); // drag start

					if (STRUTILCheckBit(TEXIN_OPT_ISMULTILINE,m_options))
					{
						int newLineIdx1=0,newLineIdx2=0,i=m_cursor;
						// correct for cursor on newline char
						if (*(m_pText->m_pBuffer+m_cursor)=='\n')
							if ((i--)<0)
								i=0;
						while (i&&(!newLineIdx2))
						{
							if (*(m_pText->m_pBuffer+i)=='\n')
							{
								if (!newLineIdx1)
									newLineIdx1=i;
								else
									newLineIdx2=i;
							}
							i--;
						}
						int xPos = m_cursor-newLineIdx1;
						if (!newLineIdx2)
							xPos--; // correct for lack of \n char
						m_cursor = newLineIdx2+xPos;
						if (m_cursor>newLineIdx1)
							m_cursor=newLineIdx1;
					}
					else
						m_cursor=0; // find start of line

				DragEnd(isDeselected); // drag end
				return false; // finished
			}
			break;
		case TEXIN_SKCODE_DOWN:
			{
				bool isDeselected=DragStart(isHighLighted,sKeys); // drag start

					if (STRUTILCheckBit(TEXIN_OPT_ISMULTILINE,m_options))
					{
						int newLineIdx1=0,newLineIdx2=0,newLineIdx3=0,i=m_cursor-1;
						if (i<0)
							i=0;
						while (i&&(!newLineIdx1))
						{
							if (*(m_pText->m_pBuffer+i)=='\n')
							{
								newLineIdx1=i;
								break;
							}
							i--;
						}
						i=m_cursor;
						while ((i<m_pText->m_strLength)&&(!newLineIdx3))
						{
							if (*(m_pText->m_pBuffer+i)=='\n')
							{
								if (!newLineIdx2)
									newLineIdx2=i;
								else
									newLineIdx3=i;
							}
							i++;
						}
						if (!newLineIdx2)
							newLineIdx2=m_pText->m_strLength;
						int xPos = m_cursor-newLineIdx1;
						if (!newLineIdx1)
							xPos++; // correct for lack of \n char
						m_cursor = newLineIdx2+xPos;
						// clamp m_cursor
						if (newLineIdx3)
							if (m_cursor>newLineIdx3)
								m_cursor=newLineIdx3;
						else if (m_cursor>m_pText->m_strLength)
							m_cursor=m_pText->m_strLength;
					}
					else
						m_cursor=m_pText->m_strLength; // find start of line

				DragEnd(isDeselected); // drag end
				return false; // finished
			}
			break;
		default:
			return false; // reject invalid chars
			break;
		}
	}
	// **** Proceed to Recording Valid Char ****
	// if text is selected
	// handle special sequences such as ctrl+c
	bool success=false; // true if the string was successfully modified
	if (STRUTILCheckBit(TEXIN_SKEY_ISCTRL,sKeys))
	{
		switch ((unsigned char)c)
		{
		case 'v': // Paste Command
			{
				if (isHighLighted)
					DelSelTxt();
				success=m_pText->InsertStr(m_cursor,m_pClipBoard->m_pBuffer);
				if (success)
					m_cursor+=m_pClipBoard->m_strLength;
				m_selcursor=m_cursor; // deselect
				return success;
			}
			break;
		case 'a': // Select All Command
			m_selcursor=0;
			m_cursor=m_pText->m_strLength;
			return false;
			break;
		default:
			if (isHighLighted)
			{
				switch ((unsigned char)c)
				{
				case 'c': // Copy Command
					{
						// get selected segment
						STRSEGMENT strSeg;
						// get string
						strSeg.m_pStr=m_pText->m_pBuffer;
						// get start and end indexes
						strSeg.m_startIdx=m_cursor;
						strSeg.m_endIdx=m_selcursor;
						if (strSeg.m_startIdx>strSeg.m_endIdx)
						{ // swap
							strSeg.m_startIdx=m_selcursor;
							strSeg.m_endIdx=m_cursor;
						}
						// copy
						m_pClipBoard->SetStr(strSeg.GetSegment());
						// restore string
						strSeg.RestoreStr();
						// return
						return false;
					}
					break;
				case 'x': // Cut Command
					{
						// get selected segment
						STRSEGMENT strSeg;
						// get string
						strSeg.m_pStr=m_pText->m_pBuffer;
						// get start and end indexes
						strSeg.m_startIdx=m_cursor;
						strSeg.m_endIdx=m_selcursor;
						if (strSeg.m_startIdx>strSeg.m_endIdx)
						{ // swap
							strSeg.m_startIdx=m_selcursor;
							strSeg.m_endIdx=m_cursor;
						}
						// copy
						m_pClipBoard->SetStr(strSeg.GetSegment());
						// restore string
						strSeg.RestoreStr();
						// delete cut text
						DelSelTxt();
						// return
						return true;
					}
					break;
				default:					
					break;
				}
			}
			break;
		}
	}

	if (isHighLighted) // highlighted and not a special key sequence
		DelSelTxt();
	// record new char
	if (STRUTILCheckBit(TEXIN_OPT_ISOVERWRITE,m_options)) // if overwrite is on
	{ // overwrite
		if (m_cursor<m_pText->m_strLength)
		{
			*(m_pText->m_pBuffer+m_cursor)=c; // overwrite
			success=true;
		}
		else // unless! the cursor is at the end of the string
			success=m_pText->InsertChar(m_cursor,c); // insert
	}
	else
		success=m_pText->InsertChar(m_cursor,c); // insert
	if (success)
		m_cursor++; // move cursor foward
	m_selcursor=m_cursor; // copy to sel cursor
	return success;
}

/*****************************************************************************

  Overwrites the string contents with the specified text.
  Returns false on error.

*****************************************************************************/
bool CTXTIN::SetText(const char *pStr)
{
	if (!pStr)
		return false; // reject null strings
	if (!*pStr)
		return false; // reject empty strings
	// clear contents
	ClearTxt();
	return m_pText->SetStr(pStr);
}

/*****************************************************************************

  Sets the current cursor position.

*****************************************************************************/
void CTXTIN::SetCursorPos(int position)
{
	m_cursor=position;
	// clamp cursor positions
	CLAMP(0,m_cursor,m_pText->m_strLength);
}

/*****************************************************************************

  Selects the text between the given range, int end should be set to which
  ever position the cursor is currently at.

*****************************************************************************/
void CTXTIN::SelectTxt(int start,int end)
{
	m_cursor=end;
	m_selcursor=start;
	// clamp cursor positions
	CLAMP(0,m_cursor,m_pText->m_strLength);
	CLAMP(0,m_selcursor,m_pText->m_strLength);
}

/*****************************************************************************

  Returns the positions of the cursor and the "select cursor".
  start will be set to the position of the "select cursor", and end
  will be set to the position of the cursor

*****************************************************************************/
void CTXTIN::GetSelectedTxt(int &start,int &end) const
{
	end=m_cursor;
	start=m_selcursor;
}

/*****************************************************************************

  Returns a pointer to the text string.
  Returns NULL on failure.

*****************************************************************************/
char *CTXTIN::GetText() const
{
	if (m_pText)
		return m_pText->m_pBuffer;
	return NULL;
}

/*****************************************************************************

  Erases the string contents of the text obj.

*****************************************************************************/
void CTXTIN::ClearTxt()
{
	m_cursor=m_selcursor=0;
	if (m_pText)
		m_pText->ClearBuffer();
}