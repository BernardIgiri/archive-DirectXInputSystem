#include "strinproc.h"
#include "qpctimer.h"
#include <malloc.h>

#define ISCHANNELVALID(x) ((x)->m_pName!=NULL)
#define ISIDXVALID(idx,max) ((idx>-1)&&(idx<max))

/*****************************************************************************

  Sets button data for use in stringed input.

*****************************************************************************/
void CSTRBUTTON::Set(CBUTTON *pButton,char symbol)
{
	if (!pButton||!symbol) /// no values may be NULL
		return;
	m_pButton = pButton;
	m_symbol  = symbol;
}

/*****************************************************************************

  CSTRINCHANNEL class constructor.

  NOTE: CSTRINCHANNEL is only valid if m_pName != NULL

*****************************************************************************/
CSTRINCHANNEL::CSTRINCHANNEL()
{
	Clear();
}

/*****************************************************************************

  CSTRINCHANNEL class deconstructor.

*****************************************************************************/
CSTRINCHANNEL::~CSTRINCHANNEL()
{
	Kill();
}

/*****************************************************************************

  Clears CSTRINCHANNEL class data.  USE WITH CAUTION.

*****************************************************************************/
void CSTRINCHANNEL::Clear()
{
	memset(this,NULL,sizeof(CSTRINCHANNEL));
}

/*****************************************************************************

  Frees RAM allocated by the CSTRINCHANNEL class.

*****************************************************************************/
void CSTRINCHANNEL::Kill()
{
	if (m_pButtons)
		free(m_pButtons);
	if (m_pName)
		free(m_pName);
	if (m_pInputStr)
		free(m_pInputStr);
	Clear();
}

/*****************************************************************************

  Initializes stringed input channel.  Return true on success.

*****************************************************************************/
bool CSTRINCHANNEL::Initialize(CSTRINPROC *pStrInProc,const char *name,
							   int nButtons,const CSTRBUTTON *pButtons)
{
	Clear(); // clear all data
	// verify input validity
	if ((!pStrInProc)||(!name)||(!nButtons)||(!pButtons))
		return false;
	// copy data	
	m_nButtons = nButtons;
	// malloc input string space
	m_pInputStr = (char *)malloc(pStrInProc->m_maxLen+1);
	if (!m_pInputStr)
	{
		Kill();
		return false;
	}
	memset(m_pInputStr,NULL,2); // clear input string
	// malloc button space
	int size = sizeof(CSTRBUTTON)*nButtons;
	m_pButtons = (CSTRBUTTON*)malloc(size);
	if (!m_pButtons)
	{
		Kill();
		return false;
	}
	memcpy(m_pButtons,pButtons,size);
	m_pStrInProc = pStrInProc;
	return STRUTILCopyStr(&m_pName,name); // if successful then system is now valid
}


/*****************************************************************************

  Updates stringed input channel with current button state info.

*****************************************************************************/
void CSTRINCHANNEL::Update(float elapsedTime)
{
	if (!m_pStrInProc)
		return; // CSTRINCHANNEL not valid
	if (m_isDone) // if done then
	{
		memset(m_pInputStr,NULL,2); // clear input string
		m_isDone = false;
	}
	register int i = m_nButtons;
	register int linkCount = 0;
	while (i--) // loop through buttons
	{
		if ((m_pButtons+i)->m_pButton->GetInstant()) // button pressed
		{
			linkCount++; // if 2 or more then link buttons

			if (!(m_pButtons+i)->m_oldState) // if button was just pressed then add button symbol to string
			{
				if (linkCount>1) // if linked then first add link symbol to string
					STRUTILAppendChar(&m_pInputStr,m_pStrInProc->m_linkSymbol,m_pStrInProc->m_maxLen);
				STRUTILAppendChar(&m_pInputStr,(m_pButtons+i)->m_symbol,m_pStrInProc->m_maxLen);
			}

			(m_pButtons+i)->m_oldState = true; // record state
		}
		else
			(m_pButtons+i)->m_oldState = false; // button wasn't pressed
	}
	if (!linkCount) // if no button was pressed
	{
		m_tInactive+=elapsedTime; // count time as inactive
		if (m_tInactive>m_pStrInProc->m_timeOut)  // if timed out
			m_isDone = true; // input string is complete
	}
	else
		m_tInactive = 0.0f;
}

/*****************************************************************************

  CSTRINPROC class constructor.

*****************************************************************************/
CSTRINPROC::CSTRINPROC()
{
	Clear();
}

/*****************************************************************************

  CSTRINPROC class deconstructor.

*****************************************************************************/
CSTRINPROC::~CSTRINPROC()
{
	Kill();
}

/*****************************************************************************

  Frees RAM allocated by the CSTRINPROC class.

*****************************************************************************/
void CSTRINPROC::Kill()
{
	if (m_pChannels)
		free(m_pChannels);
	if (m_pTimer)
		delete ((CQPCTimer*)m_pTimer);
	Clear();
}

/*****************************************************************************

  Clears CSTRINPROC class data.  USE WITH CAUTION.

*****************************************************************************/
void CSTRINPROC::Clear()
{
	memset(this,NULL,sizeof(CSTRINPROC));
}

/*****************************************************************************

  Initializes CSTRINPROC so that it may be used, returns true on success.

*****************************************************************************/
bool CSTRINPROC::Initialize(char linkSymbol,float timeOut,int maxLen)
{
	Clear();
	// verify valid input
	if ((linkSymbol==NULL)||(timeOut==0.0f)||(maxLen==0))
		return false;
	m_linkSymbol = linkSymbol;
	m_timeOut	 = timeOut;
	m_maxLen	 = maxLen;
	// create timer
	m_pTimer = (void *)(new CQPCTimer);
	if (m_pTimer)
		if (!((CQPCTimer *)m_pTimer)->Init()) // initialize timer
		{ // Timer initialization failed
			delete ((CQPCTimer*)m_pTimer);
			m_pTimer = NULL; // on failure remove timer
			return false;
		}
	return true;
}

/*****************************************************************************

  Adds a new channel to CSTRINPROC. Returns the channel's index, or -1 on
  failure.

*****************************************************************************/
int CSTRINPROC::AddChannel(const char *name,int nButtons,const CSTRBUTTON *pButtons)
{
	if (!m_pTimer)
		return -1; // the system is not initialized
	// verify valid input
	if ((!name)||(!nButtons)||(!pButtons))
		return -1;
	// allocate space for channels
	m_nChannels++; // incr number of channels
	if (!m_pChannels)
	{ // malloc
		m_pChannels = (CSTRINCHANNEL*)malloc(sizeof(CSTRINCHANNEL));
		if (!m_pChannels)
		{
			m_nChannels = 0;
			return -1;
		}
	}
	else
	{ // realloc
		m_pChannels = (CSTRINCHANNEL*)realloc(m_pChannels,sizeof(CSTRINCHANNEL)*m_nChannels);
		if (!m_pChannels)
		{
			m_nChannels = 0;
			return -1;
		}
	}
	CSTRINCHANNEL* newChannel = m_pChannels+m_nChannels-1;
	if (newChannel->Initialize(this,name,nButtons,pButtons))
		return m_nChannels - 1; // return index of new channel
	else
	{
		m_nChannels = 0;
		return -1;
	}
}

/*****************************************************************************

  Returns index of channel or -1 on failure.

*****************************************************************************/
int CSTRINPROC::FindChannel(const char *name)
{
	if (name==NULL)
		return -1; // invalid input
	if (m_pChannels&&m_nChannels)
		for (int i=0; i<m_nChannels; i++)
			if (ISCHANNELVALID(m_pChannels + i)) // skip all invalid channels
				if(strcmp(name,(m_pChannels + i)->m_pName)==NULL)
					return i;
	return -1;
}

/*****************************************************************************

  Returns input string from channel. Returns NULL if it fails or if the
  channel's string is blank. This method only returns completed strings.

*****************************************************************************/
const char *CSTRINPROC::GetInStr(int channel)
{
	if (ISIDXVALID(channel,m_nChannels))
		if ((m_pChannels + channel)->m_isDone) // if input string is done then return it
			return (m_pChannels + channel)->m_pInputStr;
	return NULL;
}

/*****************************************************************************

  Returns input string from channel. Returns NULL if it fails or if the
  channel's string is blank. This method only returns the string even if it
  is not completed.

*****************************************************************************/
const char *CSTRINPROC::PeekInStr(int channel)
{
	if (ISIDXVALID(channel,m_nChannels))
		return (m_pChannels + channel)->m_pInputStr;
	return NULL;
}

/*****************************************************************************

  Updates all input channels within CSTRINPROC.

*****************************************************************************/
void CSTRINPROC::Update()
{
	register float eSecs = 0.0f;
	if (m_pTimer)
		eSecs=((CQPCTimer*)m_pTimer)->GetElapsedSeconds();
	else
		return; // system invalid
	register int i = m_nChannels;
	while (i--)
		if (ISCHANNELVALID(m_pChannels + i))
			(m_pChannels + i)->Update(eSecs);
}