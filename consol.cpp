#include "consol.h"
#include "stdmacros.h"

/*****************************************************************************

  CCONSOL class constructor.

*****************************************************************************/
CCONSOL::CCONSOL()
{
	Clear();
	// create the scrolling buffer
	m_cBuffer.Initialize(CONSOL_BUFFERSIZE,true);
	// create the text input object
	m_cTextIn.Initialize(CONSOL_MAXTEXTINPUT,false);
	// initialize timer
	m_cTimer.Init();
}

/*****************************************************************************

  CCONSOL class constructor.

*****************************************************************************/
CCONSOL::CCONSOL(CLOG *pLog,GLWINAPI *pGLWINAPI,CTFMAN *pTFMAN)
{
	Clear();
	// create the scrolling buffer
	m_cBuffer.Initialize(CONSOL_BUFFERSIZE,true);
	// create the text input object
	m_cTextIn.Initialize(CONSOL_MAXTEXTINPUT,false);
	// initialize timer
	m_cTimer.Init();
	// initialize with supplied variables
	Initialize(pLog,pGLWINAPI,pTFMAN);
}

/*****************************************************************************

  CCONSOL class destructor.

*****************************************************************************/
CCONSOL::~CCONSOL()
{
	// finalize glwinapi user
	if (m_pGLWINAPI)
		m_pGLWINAPI->FinalizeUser(m_gwaUserId);
	Clear();
}

/*****************************************************************************

  Set all CClassName class variables to zero. USE WITH CAUTION!!!

*****************************************************************************/
void CCONSOL::Clear()
{
	m_left=m_top=m_right=m_bottom=0;
	m_scrnWidth=m_scrnHeight=0;
	m_minHeight=m_maxHeight=0;
	m_gwaUserId=NULL;
	m_pGLWINAPI=NULL;
	m_pLog=NULL;
	m_pTFMAN=NULL;
	m_pTF=NULL;
	m_eSecs=0.0f;
	m_options=NULL;
}

/*****************************************************************************

  Initializes the consol with a pointer to the logger and GLWINAPI.
  Returns false on failure.

  NOTE: May also be used to reset the consol.

*****************************************************************************/
bool CCONSOL::Initialize(CLOG *pLog,GLWINAPI *pGLWINAPI,CTFMAN *pTFMAN)
{
	m_handle.Access();
		if (!pLog||!pGLWINAPI||!pTFMAN)
		{
			m_handle.Release();
			return false;
		}
		// copy vars
		m_pLog=pLog;
		m_pGLWINAPI=pGLWINAPI;
		m_pTFMAN=pTFMAN;
		m_pLog->PrintF("Initializing Consol...");
		// copy defaults
		m_minHeight = m_maxHeight = CONSOL_MINLINES; 
		// initialize text input object
		if (!m_cTextIn.Initialize(CONSOL_MAXTEXTINPUT,false))
		{
			m_pLog->PrintF("Consol error: Failed to initializes text input object.");
			m_handle.Release();
			return false;
		}
		m_cBuffer.ClearBuffer();
		// register as user of glwinapi
		m_pGLWINAPI->CreateUser(m_gwaUserId,NULL,NULL,NULL);
		if (m_gwaUserId<0)
		{
			m_pLog->PrintF("Consol error: Failed to register as user of glwinapi.");
			m_handle.Release();
			return false;
		}
		// Continue with Resolution dependant set up
		CGWASCRN cScreen; // screen data object
		m_pGLWINAPI->GetScreen(&cScreen);
		SetRes(cScreen.m_width,cScreen.m_height);
		// Set as the echo function for the logger
		m_pLog->SetEcho(this);
		//m_options = STRUTILSetBit(CONSOL_OPT_ISMINIMIZED,m_options,true);
		// set string for m_selectList
		m_selectList.SetStr(m_cBuffer.m_pBuffer,m_cBuffer.m_strLength);
	m_handle.Release();
	return true;
}

/*****************************************************************************

  Adjusts the consol to the new resolution settings.

*****************************************************************************/
void CCONSOL::SetRes(int width,int height)
{ // IS PROTECTED DOSEN'T NEED THE HANDLE
	m_pLog->PrintF("Consol: Adjusting to screen resolution.");
	m_scrnWidth=width;
	m_scrnHeight=height;
	// **** Get Font ****
	int whSum=width+height,
		fontSz = CONSOL_VGAFONTSZ;
	if (whSum>=CONSOL_UXGA)
		fontSz = CONSOL_UXGAFONTSZ;
	else if (whSum>=CONSOL_SXGA)
		fontSz = CONSOL_SXGAFONTSZ;
	else if (whSum>=CONSOL_XGA)
		fontSz = CONSOL_XGAFONTSZ;
	else if (whSum>=CONSOL_SVGA)
		fontSz = CONSOL_SVGAFONTSZ;
	// optimal font size calculated, get font
	m_pTF = m_pTFMAN->GetFont(m_pTFMAN->FindFont(CONSOL_FONT,fontSz,false,false));
	// Calc max number of visible lines
	m_maxHeight=(height-3)/m_pTF->m_typeHeight-1;
	// **** Set Visibility ****
	// set text input box
	m_inputBox.SetWidth(width/m_pTF->m_typeWidth);
	m_inputBox.SetStr(m_cTextIn.GetText());
	// init select list
	SetHeight();
}

/*****************************************************************************

  Adjusts the consol to the new height settings.

*****************************************************************************/
void CCONSOL::SetHeight()
{ // IS PROTECTED DOSEN'T NEED THE HANDLE
	int curViewHeight = m_maxHeight;
	if (STRUTILCheckBit(CONSOL_OPT_ISMINIMIZED,m_options))
		curViewHeight = m_minHeight; // if minimized use minimum view height
	m_selectList.SetViewHeight(curViewHeight);
	// **** Set Consol Draw Coordinates ****
	m_left=0;
	m_top=m_scrnHeight-1;
	m_right=m_scrnWidth-1;
	m_bottom=m_top-((curViewHeight+1)*m_pTF->m_typeHeight + 3);
	m_selectList.m_startLine=m_selectList.m_selLine-(m_selectList.GetViewHeight()-1);
	BOUNDLWR(m_selectList.m_startLine,0);

	// scroll to end
	if (m_cBuffer.m_pBuffer)
		if (*m_cBuffer.m_pBuffer)
			m_selectList.ShowEnd();
}


/*****************************************************************************

  Echos text on the consol.

*****************************************************************************/
void CCONSOL::Echo(const char *pStr)
{
	m_handle.Access();
		SEcho(pStr);
	m_handle.Release();
}

/*****************************************************************************

  Echos text on the consol, after the access handle has been accessed.

*****************************************************************************/
void CCONSOL::SEcho(const char *pStr)
{
	if (!pStr)
		return; // reject null strings
	if (!*(pStr))
		return; // reject empty strings
	// select last line
	m_selectList.m_selLine=m_selectList.m_nLines-1;
	// Get data on string
	int length = strlen(pStr);
	// add new string into buffer
	m_cBuffer.PutStr(pStr,length);
	// append new line char
	if (*(pStr+length-1)!='\n')
		m_cBuffer.PutChar('\n');
	else if (m_selectList.m_nLines)// if new line already appended adjust line count for word wrap
		m_selectList.m_nLines--;
	// Word Wrap new text
	int wrapIndex = m_cBuffer.m_strLength-(length+1);
	if (wrapIndex<0)
		wrapIndex=0;
	// handle cuts to the buffer
	if (m_cBuffer.m_wasCut)
	{
		// if first char is newline char, delete it
		if (*(m_cBuffer.m_pBuffer)=='\n')
			m_cBuffer.DeleteSec(0,1);
		m_selectList.SetStr(m_cBuffer.m_pBuffer,m_cBuffer.m_strLength);
		m_selectList.m_nLines=0;
		m_selectList.m_selLine=0;
		m_selectList.m_startLine=0;
		wrapIndex=0;
	}
	int oldSelLine = m_selectList.m_selLine; // copy old selected line
	m_selectList.m_nLines += m_pTF->WordWrap(&m_cBuffer,
		m_scrnWidth,NULL,wrapIndex)-1;
	m_selectList.m_selLine=m_selectList.m_nLines-1; // select last line
	int viewHeight=m_selectList.GetViewHeight();// get view height
	m_selectList.m_startLine=m_selectList.m_selLine-(viewHeight-1); // get starting line
	// update string data
	m_selectList.SetStr(m_cBuffer.m_pBuffer,m_cBuffer.m_strLength);
	if (m_cBuffer.m_wasCut)  // handle cuts to the buffer
		m_selectList.ShowEnd();
	else if (m_selectList.m_nLines>viewHeight) // if not enough screen space left
		m_selectList.Scroll(m_selectList.m_selLine-oldSelLine); // scroll by change in selected line
	else
	{
		m_selectList.m_startLine=0;
		m_selectList.ShowEnd();
	}
	m_cBuffer.m_wasCut=false;
}

/*****************************************************************************

  Draws the consol on screen using OpenGL.
  eSecs should be the seconds elapsed since the last update.

*****************************************************************************/
void CCONSOL::Draw()
{
	m_handle.Access();
		m_eSecs+=m_cTimer.GetElapsedSeconds();
		// **** Set Up Colors ****
		const float color[] = {0.0f,0.0f,0.0f,1.0f,
							   1.0f,1.0f,1.0f,1.0f,
							   0.5f,0.5f,0.5f,1.0f};
		// get input text box cursor positions
		int selcur,cursor;
		m_cTextIn.GetSelectedTxt(selcur,cursor);
		const int inTxtY = m_bottom+m_pTF->m_typeHeight+1; // y coordinate of input text box
		// get input text box visible string
		const char *visStr=m_inputBox.GetVisibleStr();
		// **** Draw 2D Polygons ****
		// draw quads
		glBegin(GL_QUADS);
			// black background
			glColor4fv(color);
			glVertex2i(m_left,m_top);
			glVertex2i(m_left,m_bottom);
			glVertex2i(m_right,m_bottom);
			glVertex2i(m_right,m_top);
			// cursor for buffer
			if (STRUTILCheckBit(CONSOL_OPT_ISBUFFINFOC,m_options))
			{
				// **** Get relative index *****
				int relIdx=m_selectList.m_selLine-m_selectList.m_startLine;
				// get view height
				int viewHeight=m_selectList.GetViewHeight();
				if (relIdx<0)
				{ // scroll up if passed upper bound
					m_selectList.Scroll(relIdx);
					m_selectList.m_startLine+=relIdx;
					relIdx=0;
				}
				else if (relIdx>(viewHeight-1))
				{ // scroll down if passed lower bound
					m_selectList.Scroll(relIdx-(viewHeight-1));
					m_selectList.m_startLine+=(relIdx-(viewHeight-1));
					relIdx=viewHeight-1;
				}
				// **** Translate relative index into relative screen position ****
				// adjust if there are no enough lines on screen to fill the screen
				if (viewHeight>m_selectList.m_nLines)
					relIdx+=viewHeight-m_selectList.m_nLines;
				// flip relIdx to match the inverted y-axis in OpenGL
				relIdx = viewHeight-relIdx+1;
				// **** Draw the Cursor ****
				glColor4fv(color+8);
				glVertex2i(m_left,m_bottom+2+relIdx*m_pTF->m_typeHeight);
				glVertex2i(m_left,m_bottom+2+(relIdx-1)*m_pTF->m_typeHeight);
				glVertex2i(m_right,m_bottom+2+(relIdx-1)*m_pTF->m_typeHeight);
				glVertex2i(m_right,m_bottom+2+relIdx*m_pTF->m_typeHeight);
			}
			// highlight for text box
			if (selcur!=cursor)
			{
				int hL,hT,hR,hB,temp1,temp2,
					width=m_inputBox.GetWidth(),
					selCurCpy=selcur,curCpy=cursor;
				// get relative indexes
				selCurCpy=m_inputBox.GetRelIdx(selCurCpy);
				curCpy=m_inputBox.GetRelIdx(curCpy);
				// clamp values to a relavant range
				CLAMP(0,selCurCpy,width);
				CLAMP(0,curCpy,width);
				// sort values
				if (curCpy<selCurCpy)
				{
					temp1=selCurCpy; // temp
					selCurCpy=curCpy;
					curCpy=temp1;
				}
				if (curCpy)
					curCpy--;
				m_pTF->CalcCharCoords(hL,hT,temp1,temp2,selCurCpy,visStr);
				m_pTF->CalcCharCoords(temp1,temp2,hR,hB,curCpy,visStr);

				glColor4fv(color+8);
				glVertex2i(m_left+hL,inTxtY+hT);
				glVertex2i(m_left+hL,inTxtY+hB);
				glVertex2i(m_left+hR,inTxtY+hB);
				glVertex2i(m_left+hR,inTxtY+hT);
			}
		glEnd();//GL_QUADS
		// draw text input outline
		glBegin(GL_LINES);
			glColor4fv(color+4);
			// bottom line
			glVertex2i(m_left,m_bottom);
			glVertex2i(m_right,m_bottom);
			// top line
			glVertex2i(m_left,m_bottom+m_pTF->m_typeHeight+2);
			glVertex2i(m_right,m_bottom+m_pTF->m_typeHeight+2);
		glEnd();//GL_LINES
		// **** Draw text ****
		int bfTxtY = m_top; // y coordinate of buffer text
		// ensure buffer text is bottom aligned
		int selLstVwHgt= m_selectList.GetViewHeight();
		if (m_selectList.m_nLines<selLstVwHgt)
			bfTxtY+=(m_selectList.m_nLines-selLstVwHgt)*m_pTF->m_typeHeight;
		glEnable(GL_TEXTURE_2D);
			glColor4f(1.0f,0.0f,0.0f,1.0f);
			// buffer text
			m_pTF->PrintF(m_left,bfTxtY,m_selectList.GetVisibleStr(),true);
			m_selectList.RestoreStr();	
			// input object text
			m_pTF->PrintF(m_left,inTxtY,visStr);
		glDisable(GL_TEXTURE_2D);
		// **** Draw cursor ****
		cursor=m_inputBox.GetRelIdx(cursor);
		int left,top,right,bottom;
		bool isOverWriteOn = STRUTILCheckBit(TEXIN_OPT_ISOVERWRITE,m_cTextIn.GetOptions());
		if ((cursor == m_cTextIn.GetStrLen()))
			isOverWriteOn=false;
		m_pTF->CalcCharCoords(left,top,right,bottom,cursor-(isOverWriteOn ? 0 : 1),visStr);
		if (m_eSecs>CONSOL_CURSORBLINKRATE)
		{
			m_eSecs=0.0f;
			m_options = STRUTILSetBit(CONSOL_OPT_ISCURSORON,m_options,
				!STRUTILCheckBit(CONSOL_OPT_ISCURSORON,m_options));
			// blink cursor
		}
		if (STRUTILCheckBit(CONSOL_OPT_ISCURSORON,m_options)&&!STRUTILCheckBit(CONSOL_OPT_ISBUFFINFOC,m_options))
		{// if the cursor is on
			if (isOverWriteOn)
			{ // draw overwrite cursor
				glBegin(GL_QUADS);
					glColor4fv(color+4);
					glVertex2i(m_left+right,inTxtY+top);
					glVertex2i(m_left+left,inTxtY+top);
					glVertex2i(m_left+left,inTxtY+bottom);
					glVertex2i(m_left+right,inTxtY+bottom);
				glEnd();
			}
			else
			{ // draw normal cursor
				glBegin(GL_LINES);
					glColor4fv(color+4);
					glVertex2i(m_left+right,inTxtY+top);
					glVertex2i(m_left+right,inTxtY+bottom);
				glEnd();
			}
		}
		// restore text input string
		m_inputBox.RestoreStr();
	m_handle.Release();
}

/*****************************************************************************

  Updates the consol with current text input.

*****************************************************************************/
void CCONSOL::Update(char c,unsigned char sKeys)
{
	m_handle.Access();
		int selCur,cursor;
		
		switch ((unsigned char)c)
		{
		case NULL:
			break;
		case CONSOL_MINTGLKEY:
			m_options = STRUTILSetBit(CONSOL_OPT_ISMINIMIZED,m_options,
				!STRUTILCheckBit(CONSOL_OPT_ISMINIMIZED,m_options));
			SetHeight();
			break;
		case CONSOL_TABKEY:
			m_options = STRUTILSetBit(CONSOL_OPT_ISBUFFINFOC,m_options,
				!STRUTILCheckBit(CONSOL_OPT_ISBUFFINFOC,m_options));
			break;
		default:
			// if consol buffer is in focus
			if (STRUTILCheckBit(CONSOL_OPT_ISBUFFINFOC,m_options))
			{
				switch ((unsigned char)c)
				{
				case CONSOL_RETURNKEY: // the return key was recieved
					{
						// get line string
						STRSEGMENT strSeg;
						// overwrite text in text input object
						m_cTextIn.SetText(strSeg.GetLine(m_selectList.GetStrPtr(),m_selectList.m_selLine));
						strSeg.RestoreStr();
						// update text in object
						m_inputBox.SetStr(m_cTextIn.GetText());
					}
					break;
				case CONSOL_UPKEY:
					m_selectList.m_selLine--;
					BOUNDLWR(m_selectList.m_selLine,0);
					break;
				case CONSOL_DOWNKEY:
					m_selectList.m_selLine++;
					if (m_selectList.m_selLine>=m_selectList.m_nLines) // passed limit
						m_options=STRUTILSetBit(CONSOL_OPT_ISBUFFINFOC,m_options,false); // switch focus
					BOUNDUPR(m_selectList.m_selLine,m_selectList.m_nLines-1);
					break;
				case CONSOL_HOMEKEY:
					m_selectList.m_selLine=0;
					break;
				case CONSOL_ENDKEY:
					m_selectList.m_selLine=m_selectList.m_nLines-1;
					break;
				case CONSOL_PAGEUPKEY:
					m_selectList.m_selLine-=m_selectList.GetViewHeight()-1;
					BOUNDLWR(m_selectList.m_selLine,0);
					break;
				case CONSOL_PAGEDOWNKEY:
					m_selectList.m_selLine+=m_selectList.GetViewHeight()-1;
					BOUNDUPR(m_selectList.m_selLine,m_selectList.m_nLines-1);
					break;
				default:
					break;
				}
			}
			else // if textinput object is in focus
			{
				switch ((unsigned char)c)
				{
				case CONSOL_UPKEY:
					m_options=STRUTILSetBit(CONSOL_OPT_ISBUFFINFOC,m_options,true); // switch focus
					break;
				case CONSOL_RETURNKEY: // the return key was recieved
					// echo text in text input object
					SEcho(m_cTextIn.GetText());
					// erase text in text input object
					m_cTextIn.ClearTxt();
					// update text in object
					m_inputBox.SetStr(m_cTextIn.GetText());
					break;
				case NULL:
					break;
				default:
					// show cursor
					m_options = STRUTILSetBit(CONSOL_OPT_ISCURSORON,m_options,true);
					// stall blink
					m_eSecs=0.0f;
					// update text in obj
					m_cTextIn.Update(c,sKeys);
					// get cursor
					m_cTextIn.GetSelectedTxt(selCur,cursor);
					// make sure the cursor is visible
					m_inputBox.SetStrLen(m_cTextIn.GetStrLen());
					m_inputBox.ShowIdx(cursor);
					break;
				}
			}
			break;
		}
	m_handle.Release();
}