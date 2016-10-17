#ifndef __INCLUDED_HUD_H__
#define __INCLUDED_HUD_H__
#include "font.h"
#include "consol.h"

struct HUDdata_t
{
	HUDdata_t(CTFMAN *pTFMAN);
	~HUDdata_t();
	int		count;
	int		x,y;
	const char	*pStr;
	char	string2[100];
	CTFMAN *m_pTFMAN;
	CCONSOL *m_pConsol;
};

void HUDRender(HUDdata_t *data, int screenWidth,int screenHeight,char gotKey,unsigned char flags);

#endif//__INCLUDED_HUD_H__