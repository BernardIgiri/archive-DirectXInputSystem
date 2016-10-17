#include <memory.h>
#include <string.h>
#include "palman.h"
#include "stringUtilities.h"
#include "ioUtilities.h"
#include "stdmacros.h"

/*****************************************************************************

  CPALCOL class constructor.

*****************************************************************************/
CPALCOL::CPALCOL()
{
	memset(this,NULL,sizeof(CPALCOL));
}

/*****************************************************************************

  CPALCOL class destructor.

*****************************************************************************/
CPALCOL::~CPALCOL()
{
	DestroyString(m_pName);
}

/*****************************************************************************

  CPALCOL class manual constructor.

*****************************************************************************/
void CPALCOL::CreateMe(CPALCOL *pInit,int idx,int relIdx)
{
	if (!pInit)
		return; // initialized as null
	CPALCOL *pCurInit = pInit+relIdx; // get current initialization data
	if (pCurInit->m_pName)
		m_pName = CreateString(pCurInit->m_pName);
	m_r = pCurInit->m_r;
	m_g = pCurInit->m_g;
	m_b = pCurInit->m_b;
}

/*****************************************************************************

  CPALMAN class constructor.

*****************************************************************************/
CPALMAN::CPALMAN(const char *pSaveFile)
{
	memset(this,NULL,sizeof(CPALMAN));
	m_pSaveFile = CreateString(pSaveFile);
	m_pColorList = new CSclArray_t<CPALCOL,CPALCOL>;
}

/*****************************************************************************

  CPALMAN class destructor.

*****************************************************************************/
CPALMAN::~CPALMAN()
{
	DestroyString(m_pSaveFile);
	delete m_pColorList;
	memset(this,NULL,sizeof(CPALMAN));
}

/*****************************************************************************

  Destroys list of colors and remakes it as an empty list.

*****************************************************************************/
void CPALMAN::ResetColors()
{
	delete m_pColorList;
	m_pColorList = new CSclArray_t<CPALCOL,CPALCOL>;
}

/*****************************************************************************

  Returns index of found color, or returns -1 on failure.

*****************************************************************************/
int CPALMAN::FindColor(const char *pName) const
{
	if (!m_pColorList||!pName)
		return -1; // invalid data
	if (!m_pColorList->m_ptr)
		return -1; // list is empty
	int i=m_pColorList->m_counter;
	while (i--)
		if ((m_pColorList->m_ptr+i)->m_pName)
			if (strcmp((m_pColorList->m_ptr+i)->m_pName,pName)==NULL)
				return i;
	return -1; // not found
}

/*****************************************************************************

  Adds a new RGB color to the system.
  returns false on failure.

*****************************************************************************/
bool CPALMAN::AddColor(const char *pName,unsigned char r,unsigned char g,unsigned char b)
{
	if (!m_pColorList||!pName)
		return false; // invalid data
	CPALCOL newColor;
	newColor.m_pName = (char *)pName;
	newColor.m_r = r;
	newColor.m_g = g;
	newColor.m_b = b;
	return m_pColorList->AddElmts(1,&newColor);
}

/*****************************************************************************

  Adds a new RGB color to the system.
  returns false on failure.

*****************************************************************************/
bool CPALMAN::AddColor(const char *pName,float r,float g,float b)
{
	if (!m_pColorList||!pName)
		return false; // invalid data
	if (r>1.0f)
		r=1.0f;
	else if (r<0.0f)
		r=0.0f;
	if (g>1.0f)
		g=1.0f;
	else if (g<0.0f)
		g=0.0f;
	if (b>1.0f)
		b=1.0f;
	else if (b<0.0f)
		b=0.0f;
	CPALCOL newColor;
	newColor.m_pName = (char *)pName;
	newColor.m_r = (unsigned char)(r*255.0f);
	newColor.m_g = (unsigned char)(g*255.0f);
	newColor.m_b = (unsigned char)(b*255.0f);
	return m_pColorList->AddElmts(1,&newColor);
}

/*****************************************************************************

  Gets the RGB color at the specified index.

*****************************************************************************/
void CPALMAN::GetColor(int id,unsigned char &r,unsigned char &g,unsigned char &b) const
{
	if (id<0)
		return;
	unsigned idx=(unsigned)id;
	if (m_pColorList)
		if (m_pColorList->m_ptr)
			if (ISIDXVALID(idx,m_pColorList->m_counter))
			{
				r = (m_pColorList->m_ptr+id)->m_r;
				g = (m_pColorList->m_ptr+id)->m_g;
				b = (m_pColorList->m_ptr+id)->m_b;
			}
}

/*****************************************************************************

  Gets the RGB color at the specified index.

*****************************************************************************/
void CPALMAN::GetColor(int id,float &r,float &g,float &b) const
{
	if (id<0)
		return;
	unsigned idx=(unsigned)id;
	if (m_pColorList)
		if (m_pColorList->m_ptr)
			if (ISIDXVALID(idx,m_pColorList->m_counter))
			{
				r = ((float)(m_pColorList->m_ptr+id)->m_r)/255.0f;
				g = ((float)(m_pColorList->m_ptr+id)->m_g)/255.0f;
				b = ((float)(m_pColorList->m_ptr+id)->m_b)/255.0f;
			}
}

/*****************************************************************************

  Returns a const pointer the RGB color at the specified index.
  Returns NULL on failure.

*****************************************************************************/
const CPALCOL* CPALMAN::GetColorPtr(int id) const
{
	if (id<0)
		return NULL;
	unsigned idx=(unsigned)id;
	if (m_pColorList)
		if (m_pColorList->m_ptr)
			if (ISIDXVALID(idx,m_pColorList->m_counter))
				return m_pColorList->m_ptr+id;
	return NULL; // failed
}

/*****************************************************************************

  Saves the pallete colors at the specified save file.
  Or m_pSaveFile if pPath is NULL
  Returns false on failure.

*****************************************************************************/
bool CPALMAN::SaveColors(const char *pPath)
{
	// **** Get file path ****
	if (!pPath)
		pPath=(const char *)m_pSaveFile;
	if (!pPath)
		return false;
	// **** Check if list exist and has data ****
	if (!m_pColorList)
		return false; // there is no list
	if (!m_pColorList->m_ptr)
		return false; // list is empty
	// **** Create IOUBData object ****
	IOUBData cBData;
	// **** Set parameters ****
	cBData.SetSaveParams(4,m_pColorList->m_counter);
	// **** Set data format ****
	unsigned elmtSizes[4] = {sizeof(char),
		sizeof(unsigned char),sizeof(unsigned char),sizeof(unsigned char)};
	cBData.SetItmFormat(elmtSizes);
	// **** Transfere data to IOUBData object ****
	unsigned i=m_pColorList->m_counter;
	CPALCOL *pCurColor = NULL; // output buffer
	while (i--)
	{
		pCurColor = m_pColorList->m_ptr+i; // get current color
		// select current unit
		cBData.SelUnit(i);
		// set unit items
		cBData.SetUnitItms(0,strlen(pCurColor->m_pName)+1,pCurColor->m_pName); // item 0 is m_pName
		cBData.SetUnitItms(1,1,&pCurColor->m_r); // item 1 is m_r
		cBData.SetUnitItms(2,1,&pCurColor->m_g); // item 2 is m_g
		cBData.SetUnitItms(3,1,&pCurColor->m_b); // item 3 is m_b
	}
	// **** Save data ****
	FILE *fPtr = fopen(pPath,"wb");
	if (!fPtr)
		return false; // fopen failed
	if (!cBData.Save(fPtr))
		return false; // save data failed
	fclose(fPtr);
	return true; // return success
}

/*****************************************************************************

  Loads the pallete colors from the specified save file.
  Or m_pSaveFile if pPath is NULL
  Returns false on failure.

  NOTE: Loaded colors will be appended to the end of the color list

*****************************************************************************/
bool CPALMAN::LoadColors(const char *pPath)
{
	// **** Get file path ****
	if (!pPath)
		pPath=m_pSaveFile;
	if (!pPath)
		return false;
	// **** Check if list exist and has data ****
	IOUBData cBData;
	// **** Set parameters ****
	cBData.SetLoadParams(4);
	// **** Set data format ****
	unsigned elmtSizes[4] = {sizeof(char),
		sizeof(unsigned char),sizeof(unsigned char),sizeof(unsigned char)};
	cBData.SetItmFormat(elmtSizes);
	// **** Read data ****
	FILE *fPtr = fopen(pPath,"rb");
	if (!fPtr)
		return false; // fopen failed
	if (!cBData.Load(fPtr))
		return false; // load data failed
	fclose(fPtr);
	// **** Copy data ****
	unsigned i=cBData.GetNUnits();
	CPALCOL cCurColor; // input buffer
	unsigned ui1,ui2; // buffer for uneeded data
	unsigned char *pR,*pG,*pB; // color component input buffers
	while (i--)
	{
		// select current unit
		cBData.SelUnit(i);
		// get unit items
		cBData.GetUnitItms(0,ui1,ui2,(void **)&cCurColor.m_pName); // get name
		cBData.GetUnitItms(1,ui1,ui2,(void **)&pR); // get red value
		cBData.GetUnitItms(2,ui1,ui2,(void **)&pG); // get green value
		cBData.GetUnitItms(3,ui1,ui2,(void **)&pB); // get blue value
		// copy color components to cCurColor
		cCurColor.m_r=*pR;
		cCurColor.m_g=*pG;
		cCurColor.m_b=*pB;
		// insert cCurColor into color list
		if (!m_pColorList->AddElmts(1,&cCurColor))
			return false;
	}
	return true; // return success
}