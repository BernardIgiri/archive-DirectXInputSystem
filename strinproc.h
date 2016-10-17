/*****************************************************************************

  Stringed Input Processor 2.0
  strinproc.h

  DATE	:	11/30/2003
  AUTHOR:	Bernard Igiri

  A more advanced string input processor for Input Manager 2.0
  Allows for stringed input to be read from CINPUTMAN

*****************************************************************************/
#ifndef __INCLUDED_STRINPROC_H__
#define __INCLUDED_STRINPROC_H__

#define STRINPROC_DEFAULTTIMEOUT 0.2f

#include "inputman.h"

struct CSTRBUTTON
{
	void Set(CBUTTON* pButton,char symbol);
	CBUTTON* m_pButton; // pointer to button
	char	 m_symbol; // symbol to represent button
	bool	 m_oldState; // cache for old status of button
};

class CSTRINPROC;

class CSTRINCHANNEL
{
public:
	CSTRINCHANNEL();
	~CSTRINCHANNEL();
	void Clear();
	bool Initialize(CSTRINPROC *pStrInProc,const char *name,int nButtons,const CSTRBUTTON *pButtons);
	void Kill();
	void Update(float elapsedTime);

	char*		m_pName; // name of channel
	char*		m_pInputStr; // input string buffer
	bool		m_isDone; // true if input string is complete
private:
	CSTRINPROC*	m_pStrInProc; // pointer to Stringed Input Processor
	int			m_nButtons; // number of symbols
	CSTRBUTTON*	m_pButtons; // pointer to symbols
	float		m_tInactive; // time since last input
};

class CSTRINPROC
{
public:
	CSTRINPROC();
	~CSTRINPROC();
	bool Initialize(char linkSymbol,float timeOut,int maxLen);
	void Kill();
	void Update();
	int AddChannel(const char *name,int nButtons,const CSTRBUTTON *pButtons);
	int FindChannel(const char *name);
	const char *GetInStr(int channel);
	const char *PeekInStr(int channel);

	char			m_linkSymbol; // symbol to show linked buttons
	int				m_maxLen; // max length of input string
	float			m_timeOut; // max time before reset
private:
	void Clear();
	int				m_nChannels; // number of channels
	CSTRINCHANNEL*	m_pChannels; // pointer to channels
	void*			m_pTimer; // pointer to timing function
};

#endif//__INCLUDED_STRINPROC_H__