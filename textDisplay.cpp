#include "textDisplay.h"
#include <string.h>
#include <memory.h>
#include "stdmacros.h"

/*****************************************************************************

  HTXTVISCALC class constructor.

*****************************************************************************/
HTXTVISCALC::HTXTVISCALC()
{
	memset(this,NULL,sizeof(HTXTVISCALC));
}

/*****************************************************************************

  HTXTVISCALC class constructor.

*****************************************************************************/
HTXTVISCALC::HTXTVISCALC(char *pStr)
{
	memset(this,NULL,sizeof(HTXTVISCALC));
	SetStr(pStr);
}


/*****************************************************************************

  HTXTVISCALC class deconstructor.

*****************************************************************************/
HTXTVISCALC::~HTXTVISCALC()
{
	memset(this,NULL,sizeof(HTXTVISCALC));
}

/*****************************************************************************

  Copies the reference to char *pStr for visibility calculations

*****************************************************************************/
void HTXTVISCALC::SetStr(char *pStr)
{
	if (pStr)
	{
		m_pStr = pStr;
		m_strLen = strlen(m_pStr);
		m_startIdx = 0;
	}
}

/*****************************************************************************

  Updates the length of the string.
  NOTE: Use this method every time the string is modified.

*****************************************************************************/
void HTXTVISCALC::SetStrLen(int length)
{
	if (length>=0)
		m_strLen=length;
	if (m_startIdx>=length)
		m_startIdx=length-1;
}

/*****************************************************************************

  Sets the number of viewable chars horzontally.

*****************************************************************************/
void HTXTVISCALC::SetWidth(int width)
{
	m_width=width;
}

/*****************************************************************************

  Sets the start index of the viewable area.

*****************************************************************************/
void HTXTVISCALC::SetStartIdx(int idx)
{
	CLAMP(0,idx,m_strLen-1);
	m_startIdx = idx;
}

/*****************************************************************************

  Adjusts the start index so that the specified index may be visible.

*****************************************************************************/
void HTXTVISCALC::ShowIdx(int idx)
{
	int diff = idx-m_startIdx;
	if (diff>=m_width)
		m_startIdx += diff-m_width+1;
	else if (diff<0)
		m_startIdx +=diff;
	CLAMP(0,m_startIdx,(m_strLen-1));
}

/*****************************************************************************

  Returns the visible portion of the string after modifying the string
  contents pointed to by m_pStr.

  Call RestoreStr() to return the string to it's original contents.

  Returns NULL on failure.

*****************************************************************************/
char *HTXTVISCALC::GetVisibleStr()
{
	if (!m_pStr)
		return NULL;
	m_chBuff = *(m_pStr+(m_startIdx+m_width));
	*(m_pStr+(m_startIdx+m_width)) = NULL;
	return m_pStr+m_startIdx;
}

/*****************************************************************************

  Restores the contents of the string after GetVisibleStr has modified it.

  NOTE: Calling this repeatedly will not have compound effects, however
  calling this between calls to SetWidth may result in errors.

*****************************************************************************/
void HTXTVISCALC::RestoreStr()
{
	if (!m_pStr)
		return;
	*(m_pStr+(m_startIdx+m_width)) = m_chBuff;
}

/*****************************************************************************

  VTXTVISCALC class constructor.

*****************************************************************************/
VTXTVISCALC::VTXTVISCALC()
{
	memset(this,NULL,sizeof(VTXTVISCALC));
}

/*****************************************************************************

  VTXTVISCALC class destructor.

*****************************************************************************/
VTXTVISCALC::~VTXTVISCALC()
{
	memset(this,NULL,sizeof(VTXTVISCALC));
}

/*****************************************************************************

  Copies the reference to char *pStr for visibility calculations

*****************************************************************************/
void VTXTVISCALC::SetStr(char *pStr)
{
	if (!pStr)
		return;
	SetStr(pStr,strlen(pStr));
}

/*****************************************************************************

  Copies the reference to char *pStr for visibility calculations

*****************************************************************************/
void VTXTVISCALC::SetStr(char *pStr,int length)
{
	if (pStr&&(length>0))
	{
		m_pStr=pStr;
		m_strLen=length;
	}
}

/*****************************************************************************

  Scrolls down from the top until the specified line number is visible.

*****************************************************************************/
void VTXTVISCALC::ShowLine(int lineNum)
{
	BOUNDLWR(lineNum,0);
	m_startIdx=0;
	while (lineNum&&m_startIdx<m_strLen)
	{
		if (*(m_pStr+m_startIdx)=='\n')
			lineNum--;
		m_startIdx++;
	}
	// found startIdx
	if (*(m_pStr+m_startIdx)=='\n')
		m_startIdx++;// push passed newline char
	CLAMP(0,m_startIdx,m_strLen-1);
	int height = m_viewHeight;
	m_endIdx=m_startIdx;
	while (height&&m_endIdx<m_strLen)
	{
		m_endIdx++;
		if (*(m_pStr+m_endIdx)=='\n')
			height--;
	}
	// found endIdx
	CLAMP(0,m_endIdx,m_strLen-1);
}

/*****************************************************************************

  Scrolls up from the bottom until only the end is visible.

*****************************************************************************/
void VTXTVISCALC::ShowEnd()
{
	m_startIdx=m_strLen;
	int height = m_viewHeight+1;
	while (height&&m_startIdx--)
	{
		if (*(m_pStr+m_startIdx)=='\n')
			height--;
	}
	if (m_startIdx<0)
		m_startIdx=0;
	// found startIdx
	if (*(m_pStr+m_startIdx)=='\n')
		m_startIdx++;// push passed newline char
	CLAMP(0,m_startIdx,m_strLen-1);
	m_endIdx=m_strLen-1;
}


/*****************************************************************************

  Scrolls up or down the given number of lines.

*****************************************************************************/
void VTXTVISCALC::Scroll(int nLines)
{
	if (!nLines)
		return;
	int step=1,isReverse=0;
	if (nLines<0)
	{
		isReverse=1; // going backward startcount has 1 more to go
		step=-1;
		nLines*=-1;
	}
	int startCount=nLines+isReverse,
		endCount=nLines;
	while (startCount||endCount)
	{
		if (startCount)
			m_startIdx+=step;
		if (endCount)
			m_endIdx+=step;
		if ((m_startIdx<0)||(m_endIdx<0))
		{ // either is less than zero
			m_startIdx++;
			m_endIdx++;
			break;
		}
		else if (m_startIdx>=m_strLen)
		{ // start greater than or equal to m_strLen
			break;
		}

		if (m_endIdx>=m_strLen)
		{ // end greater than or equal to m_strLen
			m_endIdx=m_strLen-1;
			endCount=0;
			// stop moving the end
		}

		// count number of lines each must still step though
		if (startCount&&(*(m_pStr+m_startIdx)=='\n'))
			startCount--;
		if (endCount&&(*(m_pStr+m_endIdx)=='\n'))
			endCount--;
	}
	if (*(m_pStr+m_startIdx)=='\n')
		m_startIdx++; // push passed newline char
	CLAMP(0,m_startIdx,m_strLen-1);
	CLAMP(0,m_endIdx,m_strLen-1);
	if (m_startIdx==m_strLen-1)
	{ // if start index at end
		while (m_startIdx--)
		{ // scroll back one line
			if (*(m_pStr+m_startIdx)=='\n')
				break;
		}
		m_startIdx++;
		CLAMP(0,m_endIdx,m_strLen-1);
	}
}

/*****************************************************************************

  Sets the max number of lines visible at any one time

*****************************************************************************/
void VTXTVISCALC::SetViewHeight(int nLines)
{
	BOUNDLWR(nLines,0);
	m_viewHeight = nLines;
}

/*****************************************************************************

  Retuns the current view height

*****************************************************************************/
int VTXTVISCALC::GetViewHeight()
{
	return m_viewHeight;
}

/*****************************************************************************

  Returns the visible portion of the string after modifying the string
  contents pointed to by m_pStr.

  Call RestoreStr() to return the string to it's original contents.

  Returns NULL on failure.

*****************************************************************************/
char *VTXTVISCALC::GetVisibleStr()
{
	if (!m_pStr)
		return NULL;
	m_chBuff = *(m_pStr+m_endIdx);
	*(m_pStr+m_endIdx) = NULL;
	return m_pStr+m_startIdx;
}

/*****************************************************************************

  Restores the contents of the string after GetVisibleStr has modified it.

  NOTE: Calling this repeatedly will not have compound effects, however
  calling this between calls to SetWidth may result in errors.

*****************************************************************************/
void VTXTVISCALC::RestoreStr()
{
	if (!m_pStr)
		return;
	*(m_pStr+m_endIdx) = m_chBuff;
}


/*****************************************************************************

  STRSEGMENT class constructor.

*****************************************************************************/
STRSEGMENT::STRSEGMENT()
{
	memset(this,NULL,sizeof(STRSEGMENT));
}

/*****************************************************************************

  STRSEGMENT class deconstructor.

*****************************************************************************/
STRSEGMENT::~STRSEGMENT()
{
	memset(this,NULL,sizeof(STRSEGMENT));
}

/*****************************************************************************

  Counts down the number of lines sent via lineNum, and then returns the
  segment of string for that line, via a call to GetSegment, use RestorStr
  after this process.

  Returns NULL on failure.

*****************************************************************************/
char *STRSEGMENT::GetLine(char *pStr,int lineNum)
{
	if (!pStr||(lineNum<0))
		return NULL;
	int i=0, // current index
		startOfLastLine=0; // start of last line read
	m_pStr=pStr;
	lineNum++; // find end of line
	while (*(m_pStr+i)&&lineNum)
	{
		if (*(m_pStr+i)=='\n')
		{
			lineNum--;
			if (lineNum) // only record if lines are left
				startOfLastLine = i+1;
		}
		if (lineNum)
			i++;
	}
	m_startIdx=startOfLastLine;
	m_endIdx=i;
	return GetSegment();
}

/*****************************************************************************

  Returns the segemented portion of the string after modifying the string
  contents pointed to by m_pStr.

  Call RestoreStr() to return the string to it's original contents.

  Returns NULL on failure.

*****************************************************************************/
char *STRSEGMENT::GetSegment()
{
	if (!m_pStr)
		return NULL;
	m_chBuff = *(m_pStr+m_endIdx);
	*(m_pStr+m_endIdx) = NULL;
	return m_pStr+m_startIdx;
}

/*****************************************************************************

  Restores the contents of the string after GetSegment has modified it.

  NOTE: Calling this repeatedly will not have compound effects, however
  modifying other data within this class between calls to GetSegment
  and RestoreStr, may result in errors.

*****************************************************************************/
void STRSEGMENT::RestoreStr()
{
	if (!m_pStr)
		return;
	*(m_pStr+m_endIdx) = m_chBuff;
}