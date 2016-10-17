/*****************************************************************************

  Graphical Text Display System
  font.h

  DATE	:	12/23/2003
  AUTHOR:	Bernard Igiri

  A library of classes for a text display system.

  This code is OpenGL specific, but it may easily be rewritten for another
  graphics library of comparable function.

  Use CTFMAN(const char *pPath,CLOG* pLog)
  to enable fonts in your application.

  This code has the following dependancies:
  <windows.h> <gl/gl.h> <stdio.h> <malloc.h> "log.h" "stringUtilities.h"
  "stdmacros.h" "dbmi.h" "stdmacros.h"

*****************************************************************************/
#ifndef __INCLUDED_FONT_H__
#define __INCLUDED_FONT_H__

#include "log.h"
#include "stringUtilities.h"

class CTYPEFACE;

class CCHARACTER
{
public:
	void Print(int &x,int y,const CTYPEFACE *pTypeFace) const;
	void SeqPrint(int &x,int y,const CTYPEFACE *pTypeFace) const;

	int m_width; // width of char
	float m_left, // left texture coordinate
		  m_right, // right texture coordinate
		  m_top, // top texture coordinate
		  m_bottom; // bottom texture coordinate
};

class CTYPEFACE
{
public:

	void Print(int x,int y,const char *string) const;
	void PrintF(int x,int y,const char *string,bool isMultiLine=false) const;
	void CalcPrintArea(int &width,int &height,const char *string,bool isMultiLine=false) const;
	void EstPrintArea(int &width,int &height,int rows,int cols) const;
	void EstCharSpace(int &rows,int &cols,int width,int height) const;
	void CalcCharCoords(int &left,int &top,int &right,int &bottom,int index,const char *string,bool isMultiLine=false) const;
	int GetSelectedChar(int cX,int cY,const char *string,bool isMultiLine=false) const;
	int WordWrap(CTXTBUFF *pTextBuffer,int width,const char *pBrkChars,int index=0,bool isForced=false) const;
	int WordWrapLN(int *pLnIdxs,CTXTBUFF *pTextBuffer,int width,const char *pBrkChars,int index=0,bool isForced=false) const;

	int m_texWidth; // width of font texture file
	int m_texHeight; // height of font texture file
	int m_width; // width of font bitmap data
	int m_height; // height of font bitmap data
	int m_typeWidth; // width of char bitmap data
	int m_typeHeight; // height of char bitmap data
	int m_lineHeight; // height of line for underlining text
	int m_charSpacing; // space between chars
	CCHARACTER m_chars[96]; // character data
};

class CTFDESC // type face description
{
public:
	CTFDESC(const char* pName,int fontSize,bool isBold,bool isItalic,bool isSymbol=false);
	~CTFDESC();
	STRING_t GenFontName() const;
private:
	// the contents of this are never edited after creation
	char*	m_pName; // font name
	int		m_fontSize; // font size
	bool	m_isBold; // true if the font is bold
	bool	m_isItalic; // true if the font is italic
	bool	m_isSymbol; // true if the font is a symbol font
};

class CTFLELMT // type face list element
{
public:
	CTFLELMT();
	~CTFLELMT();
	void Kill();
	void Clear();
	bool LoadFont(const char *pPath,STRING_t pName);
	void Select() const;
	STRING_t  m_pName; // name of font
	unsigned  m_texID; // texture id for font texture
	CTYPEFACE m_typeFace; // type face data
};

class CTFMAN // type face manager
{
public:
	CTFMAN(const char *pPath,CLOG* pLog);
	~CTFMAN();
	int	FindFont(const char *pFontName,int fontSize,bool isBold,bool isItalic,bool isSymbol=false);
	int FindFont(CTFDESC *pTFDesc);
	bool SelectFont(int fontID);
	void PrintF(int x,int y,const char *string,bool isMultiline = false,bool isFormatted=true) const;
	int WordWrap(CTXTBUFF *pTextBuffer,int width,const char *pBrkChars,int index=0,bool isForced=false) const;
	const CTYPEFACE *GetFont(int fontID) const;
private:
	void LogMsg(const char *pMsg,int isError=0);
	int AddFont(STRING_t pName);

	CLOG*		m_pLog; // pointer to logger
	STRING_t	m_pPath; // directory location of font data
	CTFLELMT*	m_pFonts; // list of fonts
	int			m_nFonts; // number of fonts
	int			m_curFont; // id of current font
};

#endif//__INCLUDED_FONT_H__