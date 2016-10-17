#define WIN32_LEAN_AND_MEAN   // get rid of Windows things we don't need

#include <windows.h>          // included in all Windows apps
#include <winuser.h>          // Windows constants
#include <stdio.h>
#include <gl/gl.h>            // OpenGL include
#include <gl/glu.h>           // OpenGL utilty library
#include <string.h>
#include "hud.h"
#include "stringUtilities.h"
#include "textInput.h"
#include "qpctimer.h"

HUDdata_t::HUDdata_t(CTFMAN *pTFMAN)
{
	memset(this,NULL,sizeof(HUDdata_t));
	m_pTFMAN = pTFMAN;
}

HUDdata_t::~HUDdata_t()
{
	memset(this,NULL,sizeof(HUDdata_t));
}

void HUDRender(HUDdata_t *data, int screenWidth,int screenHeight,char gotKey,unsigned char flags)
{
	data->m_pConsol->Update(gotKey,flags);
	data->m_pConsol->Draw();



/////////////////////
	/*glEnable(GL_TEXTURE_2D);
	static font = data->m_pTFMAN->FindFont("Perpetua",18,true,false);
	data->m_pTFMAN->SelectFont(font);
	static CTXTBUFF cTxtBuff;
	const bool isMultilineTin=true;
	static CTXTIN cTxtIn(200,isMultilineTin);
	static const CTYPEFACE *pTF = data->m_pTFMAN->GetFont(font);
	static CQPCTimer cTimer;
	static bool isFirstTime=true;
	if (isFirstTime)
	{
		cTimer.Init();
		isFirstTime=false;
		cTxtBuff.Initialize(1024,false);
	}
	//static char buff[400]={0};
	glColor3f(1.0f,1.0f,1.0f);
	cTxtBuff.SPrintF("X:%05i Y:%05i Count:%i\nStr:\"%s\" pStr:\"%s\" Last pStr:\"%s\""
		"Loaded 'ntdll.dll', no matching symbolic information found. Loaded 'C:\\WINDOWS\\SYSTEM32\\kernel32.dll', no matching symbolic information found. Loaded 'dC:\\WINDOWS\\SYSTEM32\\user32.dll', no matching symbolic information found.",
		data->x,data->y,
		data->count,data->pStr,data->string2);
	//cTxtBuff.SetStr(buff);

	data->m_pTFMAN->WordWrap(&cTxtBuff,screenWidth,NULL);

	cTxtIn.Update(gotKey,flags);
	const char *inTxt = cTxtIn.GetText();
	int left,right,top,bottom,scur,cur;
	cTxtIn.GetSelectedTxt(scur,cur);
	pTF->CalcCharCoords(left,top,right,bottom,cur-1,inTxt,isMultilineTin);
	data->m_pTFMAN->PrintF(0,screenHeight-80,cTxtIn.GetText(),isMultilineTin);

	static float elTm = 0;
	elTm+=cTimer.GetElapsedSeconds();
	static bool isCursorOn=false;
	if (elTm>0.25f)
	{
		elTm=0.0f;
		isCursorOn=!isCursorOn;
	}
	
	
	data->m_pTFMAN->PrintF(0,screenHeight-1,cTxtBuff.m_pBuffer,true);
	if (data->pStr)
		if (strlen(data->pStr)>1)
			strcpy(data->string2,data->pStr);
	glDisable(GL_TEXTURE_2D);

	if (isCursorOn)
	{
		glBegin(GL_LINES);
			glVertex2i(right,top+screenHeight-80);
			glVertex2i(right,bottom+screenHeight-80);
		glEnd();
	}
	/**/
	glBegin(GL_POINTS);
		glVertex2i(data->x,screenHeight-data->y);
	glEnd(); // */
}