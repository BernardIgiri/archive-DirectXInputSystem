/*****************************************************************************

  Pallete Manager
  font.h

  DATE	:	12/25/2003
  AUTHOR:	Bernard Igiri

  A pallete manager system.

  Use this to define rgb colors by name.

  This code has the following dependancies:
  <stdlib.h> <memory.h> <string.h> "palman.h" "stringUtilities.h"
  "ioUtilities.h" "arrays.h" "stdmacros.h"

*****************************************************************************/
#ifndef __INCLUDED_FONT_H__
#define __INCLUDED_FONT_H__

#include <stdlib.h>
#include "arrays.h"

class CPALCOL // pallete color
{
public:
	CPALCOL();
	~CPALCOL();
	friend class CSclArray_t<CPALCOL,CPALCOL>;
	char*		  m_pName;
	unsigned char m_r;
	unsigned char m_g;
	unsigned char m_b;
private:
	void CreateMe(CPALCOL* pInit,int idx,int relIdx);
};

class CPALMAN
{
public:
	CPALMAN(const char *pSaveFile);
	~CPALMAN();
	bool LoadColors(const char *pPath=NULL);
	bool SaveColors(const char *pPath=NULL);
	void ResetColors();
	int FindColor(const char *pName) const;
	bool AddColor(const char *pName,unsigned char r,unsigned char g,unsigned char b);
	bool AddColor(const char *pName,float r,float g,float b);
	void GetColor(int id,unsigned char &r,unsigned char &g,unsigned char &b) const;
	void GetColor(int id,float &r,float &g,float &b) const;
	const CPALCOL* GetColorPtr(int id) const;
private:
	char*	m_pSaveFile;
	CSclArray_t<CPALCOL,CPALCOL> *m_pColorList;
};

#endif//__INCLUDED_FONT_H__