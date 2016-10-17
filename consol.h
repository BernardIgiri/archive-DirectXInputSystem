/*****************************************************************************

  Consol 3.0
  glgui.h

  DATE	:	1/1/2004
  AUTHOR:	Bernard Igiri

  A debugging consol for OpenGL applications

  This code has the following dependancies:
  "log.h" "stringUtilities.h" "textInput.h" "glwinapi.h"
  "textDisplay.h" "font.h" "qpctimer.h" "inputman.h"
  "mtutilities.h"

*****************************************************************************/
#ifndef __INCLUDED_CONSOL_H__
#define __INCLUDED_CONSOL_H__

#include "log.h"
#include "stringUtilities.h"
#include "textInput.h"
#include "glwinapi.h"
#include "textDisplay.h"
#include "font.h"
#include "qpctimer.h"
#include "inputman.h"
#include "mtutilities.h"

// Variable Defines
// key defines
#define CONSOL_MINTGLKEY	INPUTMAN_TEXT_CODE_F4
#define CONSOL_RETURNKEY	INPUTMAN_TEXT_CODE_ENTER
#define CONSOL_UPKEY		INPUTMAN_TEXT_CODE_UP
#define CONSOL_DOWNKEY		INPUTMAN_TEXT_CODE_DOWN
#define CONSOL_HOMEKEY		INPUTMAN_TEXT_CODE_HOME
#define CONSOL_ENDKEY		INPUTMAN_TEXT_CODE_END
#define CONSOL_TABKEY		INPUTMAN_TEXT_CODE_TAB
#define CONSOL_PAGEUPKEY	INPUTMAN_TEXT_CODE_PAGEUP
#define CONSOL_PAGEDOWNKEY	INPUTMAN_TEXT_CODE_PAGEDOWN
// default defines
#define CONSOL_CURSORBLINKRATE	0.5f
#define CONSOL_MAXTEXTINPUT	200
#define CONSOL_BUFFERSIZE	1024
#define CONSOL_MINLINES		5
#define CONSOL_FONT			"Times New Roman"
#define CONSOL_VGAFONTSZ	12
#define CONSOL_SVGAFONTSZ	12
#define CONSOL_XGAFONTSZ	14
#define CONSOL_SXGAFONTSZ	18
#define CONSOL_UXGAFONTSZ	24
// Fixed Defines
// option defines
#define CONSOL_OPT_ISMINIMIZED 0
#define CONSOL_OPT_ISCURSORON  1
#define CONSOL_OPT_ISBUFFINFOC 2
// resolution defines
#define CONSOL_VGA		1120
#define CONSOL_SVGA		1400
#define CONSOL_XGA		1792
#define CONSOL_SXGA		2304
#define CONSOL_UXGA		2800

class CCONSOL : public CECHOOBJ
{
public:
	CCONSOL();
	CCONSOL(CLOG *pLog,GLWINAPI *pGLWINAPI,CTFMAN *pTFMAN);
	~CCONSOL();
	bool Initialize(CLOG *pLog,GLWINAPI *pGLWINAPI,CTFMAN *pTFMAN);
	void Update(char c,unsigned char sKeys=NULL);
	void Echo(const char *pStr);
	void Draw();
private:
	void SEcho(const char *pStr);
	void Clear();
	void SetRes(int width,int height);
	void SetHeight();
	VTXTVISCALC			m_selectList; // vis calc for select list
	HTXTVISCALC			m_inputBox;  // vis calc for text input 

	int					m_left,m_top,m_right,m_bottom; // coordinates occupied by consol
	int					m_scrnWidth,m_scrnHeight; // screen resolution

	int					m_minHeight; // minum number of viewable lines
	int					m_maxHeight; // maximum number of viewable lines
	int					m_gwaUserId; // GLWINAPI user id
	GLWINAPI*			m_pGLWINAPI; // pointer to GLWINAPI
	CLOG*				m_pLog; // pointer to logger
	CTFMAN*				m_pTFMAN; // pointer to type face manager
	const CTYPEFACE*	m_pTF; // pointer to type face
	CTXTBUFF			m_cBuffer; // buffer for entered text
	CTXTIN				m_cTextIn; // text input object

	CQPCTimer			m_cTimer;
	float				m_eSecs; // used for making the cursor blink

	unsigned char		m_options; // (0-isMinimized,1-isCursorOn,2-isBufferInFocus)

	CACCESSHANDLE		m_handle; // access handle for multi-threaded use
};

#endif//__INCLUDED_CONSOL_H__