/*****************************************************************************
//////////////////////////////////////////////////////////////////////////////

  DATE	:	12/13/2003
  AUTHOR:	Bernard Igiri

  "mtutilities.h"

  This is a collection of classes and structures for use in multithreaded
  applications.

  This code has the following dependancies:
  <windows.h>,<malloc.h>,"log.h","stdmacros.h", & "stringUtilities.h"

///////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifndef __INCLUDED_MTUTILITIES_H__
#define __INCLUDED_MTUTILITIES_H__

#include "log.h" // for logger

// maximum allowed usage rate before buffer auto scales up
#define MTU_BUFFERTOLERANCE 0.90f
#define MTU_SUCCESS		0
#define MTU_NULLDATA	1
#define MTU_BUFFERFULL	2


/*****************************************************************************

  To create or destroy any instance of CACCESSHANDLE use the functions
  below.

*****************************************************************************/
class CACCESSHANDLE
{
public:
	CACCESSHANDLE();
	~CACCESSHANDLE();
	void Access();
	void Release();
protected:
	void* hHandle;
};
CACCESSHANDLE *CreateAccessHandle();
void DestroyAccessHandle(CACCESSHANDLE *pHandle);

class CCOMBUFFER
{
public:
	CCOMBUFFER();
	~CCOMBUFFER();
	bool Initialize(const char *pName,CLOG *pLog,int expectedNUsers,
		unsigned maxNBlocks,unsigned nBlocks,unsigned segmentSize);
	void Kill();
	void GetNUsers(int &nUsers);
	void GetPerformance(float &usageRate);
	void Send(const void *data, int &error);
	void Recieve(int userID,void *data);
	void RegisterUser(int &userID);
	void Finalize(int userID);
protected:
	void Clear();
	bool AddUserPMs(int nUsers);
	void LogMsg(const char *pMsg,int isError=1);
	void ScaleBuffer();
	char*		m_pName; // name of CCOMBUFFER instance
	CLOG*		m_pLog; // pointer to logger

	float			m_usagePeak; // record higest m_usageRate since last call to ScaleBuffer
	float			m_usageRate; // measures the current usage rate of the buffer
	unsigned char*	m_pBuffer; // allocated space for buffer
	unsigned		m_segmentSize; // size of the blocks within the buffer in bytes
	unsigned		m_nBlocks; // number of blocks within the cache
	unsigned		m_maxNBlocks; // max number of blocks for buffer to resize to
	int				m_writePos; // write position in buffer
	int*			m_pUserPMs; // list of user position markers, -1 = dead
	int				m_nUserPMs; // number of user positions stored in list
	int				m_nUsers; // number of users
	CACCESSHANDLE*	m_pHandle; // handle for shared access
};

#endif//__INCLUDED_MTUTILITIES_H__