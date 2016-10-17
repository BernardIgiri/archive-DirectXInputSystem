/*****************************************************************************

  Text Input System 2.0
  inputman.h

  DATE		:	1/1/2004
  AUTHOR	:	Bernard Igiri

  System for supporting text input.

  This code has the followind dependancies:
  <memory.h> <stdio.h> "stringUtilities.h" inputman.h" "textInput.h"
  "stdmacros.h" "textDisplay.h"

*****************************************************************************/
#ifndef __INCLUDED_TEXTINPUT_H__
#define __INCLUDED_TEXTINPUT_H__

// Includes
#include "stringUtilities.h"
#include "inputman.h" // for key codes
#include <stdio.h> // for NULL

// Defines
#define TEXIN_CLIPBOARD_BUFFERSIZE 1024
// SKey Defines
#define TEXIN_SKEY_ISSHIFT		0
#define TEXIN_SKEY_ISCTRL		1
#define TEXIN_SKEY_ISALT		2
// Special Key Code Defines
#define TEXIN_SKCODE_BKSPC		INPUTMAN_TEXT_CODE_BACKSPACE
#define TEXIN_SKCODE_DELETE		INPUTMAN_TEXT_CODE_DELETE
#define TEXIN_SKCODE_INSERT		INPUTMAN_TEXT_CODE_INSERT
#define TEXIN_SKCODE_HOME		INPUTMAN_TEXT_CODE_HOME
#define TEXIN_SKCODE_END		INPUTMAN_TEXT_CODE_END
#define TEXIN_SKCODE_UP			INPUTMAN_TEXT_CODE_UP
#define TEXIN_SKCODE_DOWN		INPUTMAN_TEXT_CODE_DOWN
#define TEXIN_SKCODE_RIGHT		INPUTMAN_TEXT_CODE_RIGHT
#define TEXIN_SKCODE_LEFT		INPUTMAN_TEXT_CODE_LEFT
// Option Defines
#define TEXIN_OPT_ISMULTILINE	0
#define TEXIN_OPT_ISOVERWRITE	1

// text input object
class CTXTIN
{
public:
	CTXTIN();
	CTXTIN(int maxLen,bool isMultiline);
	~CTXTIN();
	bool Initialize(int maxLen,bool isMultiline);
	bool Update(char c,unsigned char sKeys=NULL);
	bool SetText(const char *pStr);
	void SetCursorPos(int position);
	void SelectTxt(int start,int end);
	void GetSelectedTxt(int &start,int &end) const;
	char *GetText() const;
	inline int GetOptions() const { return m_options; };
	inline int GetStrLen() const { if (m_pText) return m_pText->m_strLength; else return NULL; };
	void ClearTxt();
private:
	void DelSelTxt();
	bool DragStart(bool isHighLighted,unsigned char sKeys);
	void DragEnd(bool isDeselected);
	unsigned char	m_options; // (0-isMultiline,1-isOverWriteOn)
	int				m_cursor; // index of cursor postion within string.
	int				m_selcursor; // (end/start) index of selected text
	CTXTBUFF*		m_pText; // unformatted text buffer
	static CTXTBUFF*m_pClipBoard; // global clipboard buffer
	static int		m_nTxtIns; // number of text input objects
};

#endif//__INCLUDED_TEXTINPUT_H__