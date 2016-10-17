/*****************************************************************************

  Text Display
  textDisplay.h

  DATE	:	01/05/2004
  AUTHOR:	Bernard Igiri

  Assists in graphical display of text.

  A library of classes and functions to assist in graphical display of
  text.

  This code has the following dependancies:
  <string.h> <memory.h> "stdmacros.h"

*****************************************************************************/
#ifndef __INCLUDED_TEXTDISPLAY_H__
#define __INCLUDED_TEXTDISPLAY_H__

/*****************************************************************************

  Use this class to generate the visible portion of horizontally scrolling
  text.

  Horizontal Text Visibility Calculator

*****************************************************************************/
class HTXTVISCALC
{
public:
	HTXTVISCALC();
	HTXTVISCALC(char *pStr);
	~HTXTVISCALC();
	void SetStr(char *pStr);
	void SetStrLen(int length);
	void SetWidth(int width);
	void SetStartIdx(int idx);
	void ShowIdx(int idx);
	inline int GetRelIdx(int idx) const { return idx-m_startIdx; }; // calculates the relative index
	inline char *GetStrPtr() { return m_pStr; };
	char *GetVisibleStr();
	void RestoreStr();
	inline int GetWidth() const { return m_width; }; // returns the view width
protected:
	char*	m_pStr; // string being modified
	int		m_strLen; // length of string
	int		m_startIdx; // start index of viewable area
	int		m_width; // width of viewable area in chars
	char	m_chBuff; // buffer for replaced char
};

/*****************************************************************************

  Use this class to generate the visible portion of vertically scrolling
  text.

  Vertical Text Visibility Calculator

*****************************************************************************/
class VTXTVISCALC
{
public:
	VTXTVISCALC();
	~VTXTVISCALC();
	void SetStr(char *pStr);
	void SetStr(char *pStr,int length);
	void ShowLine(int lineNum);
	void ShowEnd();
	void Scroll(int nLines);
	void FindEnd();
	void SetViewHeight(int nLines);	

	inline char *GetStrPtr() { return m_pStr; };
	int	GetViewHeight();
	char *GetVisibleStr();
	void RestoreStr();
	int		m_nLines; // number of lines
	int		m_selLine; // selected line
	int		m_startLine; // starting line
protected:
	char*	m_pStr; // pointer to string data
	int		m_strLen; // string length
	int		m_startIdx; // start index of visible text
	int		m_endIdx; // end index of visible text
	int		m_viewHeight; // number of lines of text visible at any given time
	char	m_chBuff; // buffer for replaced char
};

/*****************************************************************************

  Use this class to generate a segment of a string created by modifying the
  string data, and then undoing the modification.
  Set startIdx and endIdx to the points for the string segment.

  String Segmenter

*****************************************************************************/
class STRSEGMENT
{
public:
	STRSEGMENT();
	~STRSEGMENT();
	char *GetLine(char *pStr,int lineNum);
	char *GetSegment();
	void RestoreStr();
	char* m_pStr; // pointer to string data
	int	  m_startIdx; // start index of string segment
	int	  m_endIdx; // end index of string segment
	char  m_chBuff; // buffer for replaced char
};

#endif//__INCLUDED_TEXTDISPLAY_H__