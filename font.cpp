#include "font.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <malloc.h>
#include "dbmi.h"
#include "stdmacros.h"

inline void FONTSYTEM_PrintChar(int &x,const int &y,const CCHARACTER* pC,const CTYPEFACE *pTypeFace)
{
	glTexCoord2f(pC->m_left,pC->m_bottom);
	glVertex2i(x,y-pTypeFace->m_typeHeight);
	glTexCoord2f(pC->m_right,pC->m_bottom);
	glVertex2i(x+pC->m_width,y-pTypeFace->m_typeHeight);
	glTexCoord2f(pC->m_right,pC->m_top);
	glVertex2i(x+pC->m_width,y);
	glTexCoord2f(pC->m_left,pC->m_top);
	glVertex2i(x,y);
	x+=pC->m_width+pTypeFace->m_charSpacing;
}

/*****************************************************************************

  Prints the character on screen at the given coordinates,
  x and y specify the upper left corner.
  pTypeFace is a pointer to the type face data for the character.

*****************************************************************************/
void CCHARACTER::Print(int &x,int y,const CTYPEFACE *pTypeFace) const
{ // printed down from top left
	glBegin(GL_QUADS);
		glTexCoord2f(m_left,m_bottom);
		glVertex2i(x,y-pTypeFace->m_typeHeight);
		glTexCoord2f(m_right,m_bottom);
		glVertex2i(x+m_width,y-pTypeFace->m_typeHeight);
		glTexCoord2f(m_right,m_top);
		glVertex2i(x+m_width,y);
		glTexCoord2f(m_left,m_top);
		glVertex2i(x,y);
	glEnd();
	x+=m_width+pTypeFace->m_charSpacing;
}

/*****************************************************************************

  Prints the character on screen at the given coordinates,
  x and y specify the upper left corner.
  pTypeFace is a pointer to the type face data for the character.
  This method is marginally faster since it does not call
  glBegin(GL_QUADS) or glEnd()

*****************************************************************************/
void CCHARACTER::SeqPrint(int &x,int y,const CTYPEFACE *pTypeFace) const
{ // printed down from top left
	glTexCoord2f(m_left,m_bottom);
	glVertex2i(x,y-pTypeFace->m_typeHeight);
	glTexCoord2f(m_right,m_bottom);
	glVertex2i(x+m_width,y-pTypeFace->m_typeHeight);
	glTexCoord2f(m_right,m_top);
	glVertex2i(x+m_width,y);
	glTexCoord2f(m_left,m_top);
	glVertex2i(x,y);
	x+=m_width+pTypeFace->m_charSpacing;
}

/*****************************************************************************

  Prints the given string at the given coordinates,
  x and y specify the upper left corner.

*****************************************************************************/
void CTYPEFACE::Print(int x,int y,const char *string) const
{
	if (!string)
		return; // reject null strings
	if (!strlen(string))
		return; // reject empty strings
	glBegin(GL_QUADS);
		while (*string)
		{
			if (*string>31&&*string<128)
				FONTSYTEM_PrintChar(x,y,&m_chars[*string-32],this);
			else // use char 95 for invalid chars
				FONTSYTEM_PrintChar(x,y,&m_chars[95],this);
			string++;
		}
	glEnd();
}

/*****************************************************************************

  Prints the given formatted string at the given coordinates,
  x and y specify the upper left corner.
  Set isMultiLine to true to allow text to be printed on multiple lines.

*****************************************************************************/
void CTYPEFACE::PrintF(int x,int y,const char *string,bool isMultiLine) const
{
	if (!string)
		return; // reject null strings
	if (!strlen(string))
		return; // reject empty strings
	int xMargin = x; // copy x for carriage returns.
	glBegin(GL_QUADS);
		while (*string)
		{
			if (*string>31&&*string<128)
				FONTSYTEM_PrintChar(x,y,&m_chars[*string-32],this);
			else
			{
				switch (*string)
				{
				case '\n': // newline char
				case '\f': // form feed char
				case '\r': // carraige return char
					if (isMultiLine)
					{
						y-=m_typeHeight; // note y axis is inverted in opengl
						x=xMargin;
					}
					else
						FONTSYTEM_PrintChar(x,y,&m_chars[95],this); // use char 95 for invalid chars
					break;
				case '\t': // tab char
					x+=m_chars[0].m_width*8; // width of tab is 8 spaces
					break;
				default:
					FONTSYTEM_PrintChar(x,y,&m_chars[95],this); // use char 95 for invalid chars
					break;
				}
			}
			string++;
		}
	glEnd();
}

/*****************************************************************************

  Calculates the area needed to print the given string.
  Set isMultiLine to true for text printed on multiple lines.
  width and height are set to the calculated dimensions.

  NOTE: This process requires a full loop through all of the strings chars.

*****************************************************************************/
void CTYPEFACE::CalcPrintArea(int &width,int &height,const char *string,bool isMultiLine) const
{
	if (!string)
		return; // reject null strings
	if (!strlen(string))
		return; // reject empty strings
	width=0;
	int curLineWidth=0;
	height=m_typeHeight;
	while (*string)
	{
		if (*string>31&&*string<128)
			curLineWidth+=m_chars[*string-32].m_width;
		else
		{
			switch (*string)
			{
			case '\n': // newline char
			case '\f': // form feed char
			case '\r': // carraige return char
				if (isMultiLine)
				{
					if (curLineWidth>width)
						width=curLineWidth; // get width of widest line
					height+=m_typeHeight; // increase height for every new line
					curLineWidth= -m_charSpacing; // to nullify addition of m_charSpacing
				}
				else
					curLineWidth+=m_chars[95].m_width; // use char 95 for invalid chars
				break;
			case '\t': // tab char
				curLineWidth+=m_chars[0].m_width*8; // width of tab is 8 spaces
				break;
			default:
				curLineWidth+=m_chars[95].m_width; // use char 95 for invalid chars
				break;
			}
		}
		curLineWidth+=m_charSpacing;
		string++;
	}
	if (curLineWidth>width)
		width=curLineWidth; // get width of widest line
}

/*****************************************************************************

  Calculates the max area needed to print a string with the give
  number of rows and columns.
  width and height are set to the resulting dimensions.

  NOTE: This process does not require a loop.

*****************************************************************************/
void CTYPEFACE::EstPrintArea(int &width,int &height,int rows,int cols) const
{
	width = cols*(m_typeWidth+m_charSpacing);
	height = rows*m_typeHeight;
}

/*****************************************************************************

  Calculates the minimum number of rows and columns of text that can fit
  into the given width and height in pixels..
  width and height are set to the resulting dimensions.

  NOTE: This process does not require a loop.

*****************************************************************************/
void CTYPEFACE::EstCharSpace(int &rows,int &cols,int width,int height) const
{
	cols = width/(m_typeWidth+m_charSpacing);
	rows = height/m_typeHeight;
}

/*****************************************************************************

  Calculates the left, right,top, and bottom, coordinates of the char found
  at the given index within the string.
  Set isMultiLine to true for text printed on multiple lines.
  left,top,right, and bottom are set to there respective x or y coordinate
  values.

  NOTE: This process requires a loop through the chars in string.
  NOTE: All returned coordinates are relative to the original print
  coordinates of the string.

*****************************************************************************/
void CTYPEFACE::CalcCharCoords(int &left,int &top,int &right,int &bottom,int index,const char *string,bool isMultiLine) const
{
	if (!string)
		return; // reject null strings
	bool isModified=false; // true if the coordinates were modified in the while loop		
	top=left=right=0;
	bottom= -m_typeHeight;
	int i=0;
	isModified=((*string!=NULL)&&(i<=index));
	while ((*string!=NULL)&&(i<=index))
	{
		left=right;
		if (*string>31&&*string<128)
			right+=m_chars[*string-32].m_width;
		else
		{
			switch (*string)
			{
			case '\n': // newline char
			case '\f': // form feed char
			case '\r': // carraige return char
				if (isMultiLine)
				{
					top=bottom;
					bottom-=m_typeHeight; // note y axis is inverted in opengl
					left=0;
					right=-m_charSpacing;
					if (i==index)
					{
						right=0;
						return; // return with dimensions for newline char
					}
				}
				else
					right+=m_chars[95].m_width; // use char 95 for invalid chars
				break;
			case '\t': // tab char
				right+=m_chars[0].m_width*8-m_charSpacing;; // width of tab is 8 spaces
				break;
			default:
				right+=m_chars[95].m_width; // use char 95 for invalid chars
				break;
			}
		}
		right+=m_charSpacing;
		string++;
		i++;
	}
	if (isModified)
		right-=m_charSpacing;
}

/*****************************************************************************

  Calculates and returns the index of the char found at the specifed
  coordinates.  Set isMultiline to true for text printed on multiple lines.

  NOTE: This process requires a loop through the chars in string.
  NOTE: Coordinates cX and cY should be relative to the original print
  coordinates of the string.

*****************************************************************************/
int CTYPEFACE::GetSelectedChar(int cX,int cY,const char *string,bool isMultiLine) const
{
	if (!string)
		return -1; // reject null strings
	if (!strlen(string))
		return -1; // reject empty strings
	int top=0,left=0,right=0,
		bottom = -m_typeHeight;

	int i=0;	
	while (*string)
	{
		left=right;
		if (*string>31&&*string<128)
			right+=m_chars[*string-32].m_width;
		else
		{
			switch (*string)
			{
			case '\n': // newline char
			case '\f': // form feed char
			case '\r': // carraige return char
				if (isMultiLine)
				{
					top=bottom;
					bottom-=m_typeHeight; // note y axis is inverted in opengl
					left=0;
					right=-m_charSpacing;
				}
				else
					right+=m_chars[95].m_width; // use char 95 for invalid chars
				break;
			case '\t': // tab char
				right+=m_chars[0].m_width*8-m_charSpacing;; // width of tab is 8 spaces
				break;
			default:
				right+=m_chars[95].m_width; // use char 95 for invalid chars
				break;
			}
		}
		right+=m_charSpacing;
		if ((cX>=left&&cX<right)&&
			(cY>=bottom&&cY<=top))
			return i;
		string++;
		i++;
	}
	return -1;
}

/*****************************************************************************

  Using the supplied text buffer this function word wraps the text in the
  text buffer to the specified width in pixels. It then returns the resulting
  number of lines the text will occupy.
  NOTE: This works just like the CTXTBUFF word wrap member function.
  ADDED FEATURE: Set index to the index within the string on which you want
  word wrapping to begin on.
  Returns 0 on failure.

*****************************************************************************/
int CTYPEFACE::WordWrap(CTXTBUFF *pTextBuffer,int width,const char *pBrkChars,int index,bool isForced) const
{
	width-=m_typeWidth; // prevent drawing out of bounds
	if (!pTextBuffer||(index<0)||(width<1))
		return 0; // invalid input
	if (!pTextBuffer->m_pBuffer)
		return 0; // invalid input
	if (!pBrkChars)
		pBrkChars = STRUTIL_DEFBREAKCHARS;
	int nBrkChars = strlen(pBrkChars);
	if (!nBrkChars)
		return 0;
	int lineLen=0,
		lastBrkChar=-1, // -1 = none found
		nLines=1;
	bool isLineBroken = false;
	while (index<pTextBuffer->m_strLength)
	{
		isLineBroken = false;
		// calculate line length in pixels		
		if (pTextBuffer->m_pBuffer[index]>31&&pTextBuffer->m_pBuffer[index]<128)
			lineLen+=m_chars[pTextBuffer->m_pBuffer[index]-32].m_width+m_charSpacing;
		else
		{
			switch (pTextBuffer->m_pBuffer[index])
			{
			case '\n': // newline char
			case '\f': // form feed char
			case '\r': // carraige return char
				// if line break was found
				lastBrkChar=-1; // reset last breaking char
				lineLen=0; // reset line length
				isLineBroken = true; // report that line was broken
				nLines++; // increment line counter
				break;
			case '\t': // tab char
				lineLen+=m_chars[0].m_width*8; // width of tab is 8 spaces
				break;
			default:
				lineLen+=m_chars[95].m_width+m_charSpacing; // use char 95 for invalid chars
				break;
			}
		}
		if (STRUTILCmpAgnstStr(pTextBuffer->m_pBuffer[index],pBrkChars,nBrkChars))
			lastBrkChar = index;
		if (lineLen>=width)
		{
			if (!isLineBroken)
			{ // if the line is not already broken then break it
				if (lastBrkChar==-1) // if there is no breaking char
					lastBrkChar=index; // use current char as break point
				lastBrkChar++; // get postion after last breaking char
				if (!pTextBuffer->InsertChar(lastBrkChar,'\n')) // insert new line char
				{ //if insert failed
					if (!isForced) // and word wrap is not forced then return false
						return 0;
					// else resize and try again
					pTextBuffer->Resize(pTextBuffer->m_maxSize+(width/m_typeWidth));
					if (!pTextBuffer->InsertChar(lastBrkChar,'\n')) // out of memory
						return 0;
				}
				nLines++; // increment line counter
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
	return nLines;
}

/*****************************************************************************

  Performs word wrap will recording line indexes for use with the
  "textDisplay.h" library.
  WARNING: pLnIdxs must be preallocated to the necessary space or beyond it.
  Returns 0 on failure.

*****************************************************************************/
int CTYPEFACE::WordWrapLN(int *pLnIdxs,CTXTBUFF *pTextBuffer,int width,const char *pBrkChars,int index,bool isForced) const
{
	width-=m_typeWidth; // prevent drawing out of bounds
	if (!pTextBuffer||(index<0)||(width<1)||!pLnIdxs)
		return 0; // invalid input
	if (!pTextBuffer->m_pBuffer)
		return 0; // invalid input
	if (!pBrkChars)
		pBrkChars = STRUTIL_DEFBREAKCHARS;
	int nBrkChars = strlen(pBrkChars);
	if (!nBrkChars)
		return 0;
	int lineLen=0,
		lastBrkChar=-1, // -1 = none found
		nLines=1;
	bool isLineBroken = false;
	while (index<pTextBuffer->m_strLength)
	{
		isLineBroken = false;
		// calculate line length in pixels		
		if (pTextBuffer->m_pBuffer[index]>31&&pTextBuffer->m_pBuffer[index]<128)
			lineLen+=m_chars[pTextBuffer->m_pBuffer[index]-32].m_width+m_charSpacing;
		else
		{
			switch (pTextBuffer->m_pBuffer[index])
			{
			case '\n': // newline char
			case '\f': // form feed char
			case '\r': // carraige return char
				// if line break was found
				lastBrkChar=-1; // reset last breaking char
				lineLen=0; // reset line length
				isLineBroken = true; // report that line was broken
				nLines++; // increment line counter
				break;
			case '\t': // tab char
				lineLen+=m_chars[0].m_width*8; // width of tab is 8 spaces
				break;
			default:
				lineLen+=m_chars[95].m_width+m_charSpacing; // use char 95 for invalid chars
				break;
			}
		}
		if (STRUTILCmpAgnstStr(pTextBuffer->m_pBuffer[index],pBrkChars,nBrkChars))
			lastBrkChar = index;
		if (lineLen>=width)
		{
			if (!isLineBroken)
			{ // if the line is not already broken then break it
				if (lastBrkChar==-1) // if there is no breaking char
					lastBrkChar=index; // use current char as break point
				lastBrkChar++; // get postion after last breaking char
				if (!pTextBuffer->InsertChar(lastBrkChar,'\n')) // insert new line char
				{ //if insert failed
					if (!isForced) // and word wrap is not forced then return false
						return 0;
					// else resize and try again
					pTextBuffer->Resize(pTextBuffer->m_maxSize+(width/m_typeWidth));
					if (!pTextBuffer->InsertChar(lastBrkChar,'\n')) // out of memory
						return 0;
				}
				nLines++; // increment line counter
				// record line index
				pLnIdxs++; // advance to next, and skip line 0
				*pLnIdxs = lastBrkChar+1;
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
	// record ending line, line index
	pLnIdxs++; // advance to next, and skip line 0
	*pLnIdxs = pTextBuffer->m_strLength;
	return nLines;
}

/*****************************************************************************

  CTFDESC class constructor.

*****************************************************************************/
CTFDESC::CTFDESC(const char* pName,int fontSize,bool isBold,bool isItalic,bool isSymbol)
{
	m_pName = (char *)pName;
	m_fontSize = fontSize;
	m_isBold = isBold;
	m_isItalic = isItalic;
	m_isSymbol = isSymbol;
}

/*****************************************************************************

  CTFDESC class deconstructor.

*****************************************************************************/
CTFDESC::~CTFDESC()
{
}

/*****************************************************************************

  Generates a fontName string based on the type face description.
  The output is allocated using CreateString() call DestroyString()
  to deallocate it.

*****************************************************************************/
STRING_t CTFDESC::GenFontName() const
{
	if (!m_pName)
		return NULL;
	// get string version of font size
	char *fontSz[5]={0};
	sprintf((char *)fontSz,"%i",m_fontSize);
	int size = strlen(m_pName)+strlen((char *)fontSz)+
		(m_isBold ? 4 : 0)+(m_isItalic ? 6 : 0)+
		(m_isSymbol ? 6 : 0)+1;
	if (!size)
		return NULL;
	STRING_t result = CreateString(size);
	if (!result)
		return NULL;
	int i = strlen(result);
	strcpy(result,m_pName);
	strcat(result,(char *)fontSz);
	if (m_isBold)
		strcat(result,"Bold");
	if (m_isItalic)
		strcat(result,"Italic");
	if (m_isSymbol)
		strcat(result,"Symbol");
	return result;
}

/*****************************************************************************

  CTFLELMT class constructor.

*****************************************************************************/
CTFLELMT::CTFLELMT()
{
	Clear();
}

/*****************************************************************************

  CTFLELMT class deconstructor.

*****************************************************************************/
CTFLELMT::~CTFLELMT()
{
	Kill();
}

/*****************************************************************************

  Frees all ram allocated by the CTFLELMT class.

*****************************************************************************/
void CTFLELMT::Kill()
{
	DestroyString(m_pName);
	glDeleteTextures(1,&m_texID);
	Clear();
}

/*****************************************************************************

  Set all CTFLELMT class variables to zero. USE WITH CAUTION!!!.

*****************************************************************************/
void CTFLELMT::Clear()
{
	memset(this,NULL,sizeof(CTFLELMT));
}

/*****************************************************************************

  Loads the specified font into memory.
  IMPORTANT: pName should be created via CreateString before being
  passed to this function.
  Returns false on failure.

*****************************************************************************/
bool CTFLELMT::LoadFont(const char *pPath,STRING_t pName)
{
	// ****Load type face data****
	// copy name
	m_pName = pName;
	if (!m_pName)
		return false; // invalid input
	// get font file path
	char fPath[1024]={0};
	strcpy(fPath,pPath);
	strcat(fPath,m_pName);
	strcat(fPath,".font");
	FILE *fptr=fopen(fPath,"rb");
	if (!fptr)
	{
		return false; // failed to open file
	}
	bool result = fread(&m_typeFace,sizeof(CTYPEFACE),1,fptr)!=NULL;
	fclose(fptr);
	if (!result)
	{
		return false;
	}
	// ****Load font texture file****
	CDBMI cImage;
	strcpy(fPath,pPath);
	strcat(fPath,m_pName);
	strcat(fPath,".jpg");
	if (!cImage.Load(fPath))
	{
		return false; // image load failed
	}
	// ****Convert to Luminance Alpha****
	if (!cImage.ConvertToLumAlpha(128))
	{
		return false; // conversion failed
	}
	// ****Create OpenGL Texture****
	// enable OpenGL textures
	GLboolean isTex2DOn = glIsEnabled(GL_TEXTURE_2D);
	if (isTex2DOn==GL_FALSE)
		glEnable(GL_TEXTURE_2D);
	// gen tex name
	glGenTextures(1,&m_texID);
	glBindTexture(GL_TEXTURE_2D,m_texID);
	// set params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// load image
	glTexImage2D(GL_TEXTURE_2D,0,2,cImage.m_width,cImage.m_height,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,cImage.m_pData);
	// disable if necessary OpenGL textures	
	if (isTex2DOn==GL_FALSE)
		glDisable(GL_TEXTURE_2D);
	// ****Destroy Bitmap Data****
	cImage.Destroy();
	return true;
}

/*****************************************************************************

  Binds the texture used to display the font.

*****************************************************************************/
void CTFLELMT::Select() const
{
	glBindTexture(GL_TEXTURE_2D,m_texID);
}

/*****************************************************************************

  CTFMAN class constructor.

*****************************************************************************/
CTFMAN::CTFMAN(const char *pPath,CLOG* pLog)
{
	// initialize vars
	memset(this,NULL,sizeof(CTFMAN));
	m_pLog = pLog;
	// copy path
	m_pPath = CreateString(pPath);
	if (!m_pPath)
		LogMsg("Initialization Failed!",1);
	else
		LogMsg("Initialized Successfully.");
}

/*****************************************************************************

  CTFMAN class deconstructor.

*****************************************************************************/
CTFMAN::~CTFMAN()
{
	// free allocated ram
	DestroyString(m_pPath);
	free(m_pFonts);
}
/*****************************************************************************

  Logs a message to the logger if logger is present.

*****************************************************************************/
void CTFMAN::LogMsg(const char *pMsg,int isError)
{
	if (m_pLog)
	{
		char *report[2]={"message","error"}; // if isError = 1 report error
		m_pLog->PrintF("Type Face Manager %s: %s",report[isError],pMsg);
	}
}

/*****************************************************************************

  Returns the id of the requested font, or returns -1 on failure.

*****************************************************************************/
int CTFMAN::FindFont(const char *pFontName,int fontSize,bool isBold,bool isItalic,bool isSymbol)
{
	CTFDESC cTF(pFontName,fontSize,isBold,isItalic,isSymbol);
	return FindFont(&cTF);
}

/*****************************************************************************

  Returns the id of the requested font, or returns -1 on failure.

*****************************************************************************/
int CTFMAN::FindFont(CTFDESC *pTFDesc)
{
	STRING_t pFName = pTFDesc->GenFontName();
	if (!pFName)
	{ // name generation failed
		LogMsg("Invalid font description or memory error.",1);
		return -1;
	}
	if (!m_pFonts) // font list is empty
		return AddFont(pFName); // add the font and return it's id
	int id = -1, count = m_nFonts;
	while (count--)
		if (strcmp(pFName,(m_pFonts+count)->m_pName)==NULL)
			id = count;
	if (id>=0)
	{ // font was found
		DestroyString(pFName); // destroy string
		return id; // return id
	}
	return AddFont(pFName); // add the font and return it's id
}

/*****************************************************************************

  Adds the font to the system and returns the font's id.
  Returns -1 on failure.

*****************************************************************************/
int CTFMAN::AddFont(STRING_t pName)
{
	// increment font counter
	m_nFonts++;
	// allocate memory
	if (!m_pFonts)
		m_pFonts = (CTFLELMT*)malloc(sizeof(CTFLELMT));
	else
		m_pFonts = (CTFLELMT*)realloc(m_pFonts,sizeof(CTFLELMT)*m_nFonts);
	if (!m_pFonts)
	{ // memory allocation failed
		m_curFont = 0; // select font 0
		m_nFonts = 0; // there are no more fonts
		LogMsg("Memory allocation failed while creating a new font.",1); // log error
		// destroy font name
		DestroyString(pName);
		return -1; // report failure
	}
	// record id
	int id = m_nFonts-1;
	// intialize new font
	(m_pFonts+id)->Clear();
	// load font
	if ((m_pFonts+id)->LoadFont(m_pPath,pName))
		return id; // font loading successful
	else // **** FAILED TO LOAD FONT ****
	{
		// log failed to load error
		char buff[1024]={0};
		sprintf(buff,"Failed to load font: %s",pName);
		DestroyString(pName); // destroy font name
		LogMsg(buff,1);
		// **** Destroy New Font ****
		m_nFonts--; // decrement font counter
		if (!m_nFonts) // if there are no fonts left
		{
			free(m_pFonts); // destroy font list
			m_pFonts = NULL; // set to null
			m_curFont = 0; // select font 0
			return -1; // report failure
		}
		// Fonts are still left so...
		// realloc font list space
		m_pFonts = (CTFLELMT*)realloc(m_pFonts,sizeof(CTFLELMT)*m_nFonts);
		if (!m_pFonts)
		{ // memory reallocation failed
			m_curFont = 0; // select font 0
			m_nFonts = 0; // there are no more fonts
			LogMsg("Memory reallocation failed while removing a lost font.",1); // log error
			return -1; // report failure
		}
	}
	return -1; // report failure
}

/*****************************************************************************

  Selects requested font from the font list.
  Returns false on failure.

*****************************************************************************/
bool CTFMAN::SelectFont(int fontID)
{
	if (!ISIDXVALID(fontID,m_nFonts))
		return false; // invalid input
	(m_pFonts+fontID)->Select(); // bind font texture
	m_curFont = fontID; // record current font
	return true; // success!
}

/*****************************************************************************

  Prints using the current selected font. Be sure to call SelectFont first.
  Set isMultiline to true for printing across multiple lines and set
  isFormatted to false to disable formatted printing.

  NOTE: The x and y coordinates specify the top left corner of the text to
  be printed.

*****************************************************************************/
void CTFMAN::PrintF(int x,int y,const char *string,bool isMultiline,bool isFormatted) const
{
	if (!m_pFonts) // if there are no fonts
		return; // return
	if (isFormatted)
		(m_pFonts + m_curFont)->m_typeFace.PrintF(x,y,string,isMultiline);
	else
		(m_pFonts + m_curFont)->m_typeFace.Print(x,y,string);
}

/*****************************************************************************

  Using the supplied text buffer this function word wraps the text in the
  text buffer to the specified width in pixels. It then returns the resulting
  number of lines the text will occupy.
  NOTE: This works just like the CTXTBUFF word wrap member function.
  Returns 0 on failure.
  ADDED FEATURE: Set index to the index within the string on which you want
  word wrapping to begin on.

*****************************************************************************/
int CTFMAN::WordWrap(CTXTBUFF *pTextBuffer,int width,const char *pBrkChars,int index,bool isForced) const
{
	if (!m_pFonts) // if there are no fonts
		return 0; // return error
	return (m_pFonts + m_curFont)->m_typeFace.WordWrap(pTextBuffer,width,pBrkChars,index,isForced);
}

/*****************************************************************************

  Returns a pointer to type face data for the requested font.
  Returns NULL on failure.

*****************************************************************************/
const CTYPEFACE *CTFMAN::GetFont(int fontID) const
{
	if (!ISIDXVALID(fontID,m_nFonts))
		return NULL; // invalid input
	return &(m_pFonts + fontID)->m_typeFace;
}
