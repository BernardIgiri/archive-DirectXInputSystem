#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#include "mtutilities.h"
#include "stdmacros.h"
#include "stringUtilities.h"

/*****************************************************************************

  Creates a new CACCESSHANDLE, returns NULL on failure.

*****************************************************************************/
CACCESSHANDLE *CreateAccessHandle()
{
	return new CACCESSHANDLE;
}

/*****************************************************************************

  Destroys an instance of CACCESSHANDLE.

*****************************************************************************/
void DestroyAccessHandle(CACCESSHANDLE *pHandle)
{
	if (pHandle)
		delete pHandle;
}

/*****************************************************************************

  CACCESSHANDLE class constructor.

*****************************************************************************/
CACCESSHANDLE::CACCESSHANDLE()
{
	hHandle = new HANDLE;
	(*((HANDLE *)hHandle))=CreateMutex(NULL, FALSE, NULL);
}

/*****************************************************************************

  CACCESSHANDLE class deconstructor.

*****************************************************************************/
CACCESSHANDLE::~CACCESSHANDLE()
{
	if ((*((HANDLE *)hHandle)))
		CloseHandle((*((HANDLE *)hHandle)));
	if (hHandle)
		delete ((HANDLE *)hHandle);
}

/*****************************************************************************

  Request's access to handle

*****************************************************************************/
void CACCESSHANDLE::Access()
{
	WaitForSingleObject((*((HANDLE *)hHandle)),INFINITE);
}

/*****************************************************************************

  Releases access to handle.

*****************************************************************************/
void CACCESSHANDLE::Release()
{
	ReleaseMutex((*((HANDLE *)hHandle)));
}

/*****************************************************************************

  CCOMBUFFER class constructor.

*****************************************************************************/
CCOMBUFFER::CCOMBUFFER()
{
	Clear();
}

/*****************************************************************************

  CCOMBUFFER class deconstructor.

*****************************************************************************/
CCOMBUFFER::~CCOMBUFFER()
{
	Kill();
}

/*****************************************************************************

  Set all CCOMBUFFER class variables to zero. USE WITH CAUTION!!!

*****************************************************************************/
void CCOMBUFFER::Clear()
{
	memset(this,NULL,sizeof(CCOMBUFFER));
}

/*****************************************************************************

  Frees all ram allocated by the CCOMBUFFER class.

*****************************************************************************/
void CCOMBUFFER::Kill()
{
	if (m_nUsers>0)
	{ // killed without being released by all users
		LogMsg("System was terminated with users still connected.");
	}
	if (m_pBuffer)
		free(m_pBuffer);
	if (m_pUserPMs)
		free(m_pUserPMs);
	DestroyAccessHandle(m_pHandle);
	Clear();
}

/*****************************************************************************

  Initializes CCOMBUFFER.
  pName - (optional) is used in error reporting
  pLog  - (optional) is used to  log errors STONGLY RECOMMENDED
  expectedNUsers - (optional) estimate of the number of users
				   setting this above zero should reduce the number of
				   calls to realloc.  Setting it too high will waste
				   RAM.
  maxNBlocks - (optional) max number of blocks, set above zero to allow the
				   the buffer to automatically scale in size up to the
				   specified maximum number of blocks.
  nBlocks - (required) is the number of segments to hold in the buffer.
  segmentSize - (required) is the size in bytes of the data packets to be
				sent through the system.

  Returns false upon failure.

  NOTE: This procedure must be called once, before usage of CCOMBUFFER

*****************************************************************************/
bool CCOMBUFFER::Initialize(const char *pName,CLOG *pLog,int expectedNUsers,
		unsigned maxNBlocks,unsigned nBlocks,unsigned segmentSize)
{
	if (segmentSize==0||nBlocks<1)
	{ // invalid input
		m_pLog = pLog;
		LogMsg("Initialization failed invalid parameters!"); // log error
		return false;
	}
	Clear();
	// **** Copy Data ****
	STRUTILCopyStr(&m_pName,pName);
	m_pLog			= pLog;
	m_segmentSize	= segmentSize;
	m_nBlocks		= nBlocks;
	m_maxNBlocks	= maxNBlocks;
	LogMsg("Initializing...",0); // log message initializing
	// **** Allocate needed RAM ****
	// create access handle
	m_pHandle = CreateAccessHandle();
	if (m_pHandle==NULL)
	{ // failed
		LogMsg("Failed to create access handle!"); // log error
		Kill(); // kill system
		return false;
	}
	// create buffer
	m_pBuffer = (unsigned char *)malloc(m_segmentSize*m_nBlocks);
	if (!m_pBuffer)
	{ // malloc failed
		LogMsg("Memory allocation failed!"); // log error
		Kill(); // kill system
		return false;
	}
	// create user positions if valid expectedNUsers is given
	if (expectedNUsers>0)
		if (!AddUserPMs(expectedNUsers))
		{ // ram allocation failed
			LogMsg("Memory allocation failed!"); // log error
			Kill(); // kill system
			return false;
		}
	LogMsg("Initialization Successful!",0); // log message initialization successful
	return true;
}

/*****************************************************************************

  Logs a message to the logger if logger is present.

*****************************************************************************/
void CCOMBUFFER::LogMsg(const char *pMsg,int isError)
{
	if (m_pLog)
	{
		char *report[2]={"message","error"}; // if isError = 1 report error
		if (m_pName)
			m_pLog->PrintF("%s CCOMBUFFER %s: %s",m_pName,report[isError],pMsg);
		else
			m_pLog->PrintF("CCOMBUFFER %s: %s",report[isError],pMsg);
	}
}

/*****************************************************************************

  Returns the number of users still connected to the system.

*****************************************************************************/
void CCOMBUFFER::GetNUsers(int &nUsers)
{
	m_pHandle->Access();
		nUsers = m_nUsers;
	m_pHandle->Release();
}

/*****************************************************************************

  Allocates new space for specified number of users.

*****************************************************************************/
bool CCOMBUFFER::AddUserPMs(int nUsers)
{
	int oldCount = m_nUserPMs; // store old count
	m_nUserPMs+=nUsers; // increment userPM count
	// allocate ram
	if (!m_pUserPMs) // first time initialization use malloc
		m_pUserPMs = (int*)malloc(sizeof(int)*m_nUserPMs);
	else
	{
		m_pUserPMs = (int*)realloc(m_pUserPMs,sizeof(int)*m_nUserPMs);
	}
	if (!m_pUserPMs) // memory allocation failed
		return false; // allow user to log error
	// set new user positions as dead
	int count = m_nUserPMs;
	while (count-->oldCount)
		*(m_pUserPMs + count) = -1;
	return true;
}

/*****************************************************************************

  Increases size of buffer if possible.

*****************************************************************************/
void CCOMBUFFER::ScaleBuffer()
{
	if (!m_pBuffer)
		return; // resize impossible if m_pBuffer is NULL
	m_usagePeak = 0.0f; // reset usage peak
	if (m_maxNBlocks>m_nBlocks) // if permitted to use more space
	{
		// resize in inverse ratio to the tolerance
		unsigned newSize = (int)((2.0f - MTU_BUFFERTOLERANCE)*float(m_nBlocks));
		// clip newSize within limits of 1 to m_maxNBlocks
		if (newSize<1)
			newSize = 1;
		else if (newSize>m_maxNBlocks)
			newSize = m_maxNBlocks;
		m_nBlocks = newSize; // copy newSize to m_nBlocks
		m_pBuffer = (unsigned char *)realloc(m_pBuffer,m_segmentSize*m_nBlocks);
		if (!m_pBuffer)
		{ // realloc failed
			LogMsg("Fatal error, memory allocation failed while resizing buffer!"); // log error
			Kill(); // kill system
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////N O T E ! ! !/////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// All CCOMBUFFER member functions beyond this point must use m_pHandle for //
// all operations. ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************

  Returns current percent usage of the allocated buffer space.

*****************************************************************************/
void CCOMBUFFER::GetPerformance(float &usageRate)
{
	m_pHandle->Access();
		usageRate = m_usageRate;;
	m_pHandle->Release();
}

/*****************************************************************************

  Adds a new user to CCOMBUFFER. And returns the userID, or -1 on failure.

*****************************************************************************/
void CCOMBUFFER::RegisterUser(int &userID)
{
	m_pHandle->Access();
		userID = -1;
		m_nUsers++; // increment user counter
		// **** Get new userID  ****
		if (m_nUsers>m_nUserPMs) // if more users than UserPMs then correct this
		{
			if (AddUserPMs(m_nUsers-m_nUserPMs))
				userID = m_nUsers-1; // userID = last new userID
			else
			{ // memory allocation failed
				LogMsg("Failed to register new user."); // log error
				userID = -1; // report failure
				m_pHandle->Release(); // release handle
				Kill(); // kill system
				return; // return
			}
		}
		else
		{ // find lowest available userID
			int count = m_nUsers;
			while (count--)
				if (*(m_pUserPMs + count)== -1) // if user position marker is dead
					userID = count; // set userID to position marker index
		}
		// **** Set user pm and return id  ****
		*(m_pUserPMs + userID) = 0; // set new user's postion marker to zero
	m_pHandle->Release();
}

/*****************************************************************************

  Removes specified user from CCOMBUFFER.

*****************************************************************************/
void CCOMBUFFER::Finalize(int userID)
{
	m_pHandle->Access();
		if (ISIDXVALID(userID,m_nUsers)) // if valid userID
			if (*(m_pUserPMs + userID) != -1) // and user is not already dead
			{
				(*(m_pUserPMs + userID)) = -1; // Mark user as dead
				m_nUsers--; // decrement user counter
			}
	m_pHandle->Release();
}

/*****************************************************************************

  Sends new data through CCOMBUFFER.
  Sets error to - MTU_SUCCESS on success
				  MTU_NULLDATA if data==NULL
				  MTU_BUFFERFULL if the buffer is full

*****************************************************************************/
void CCOMBUFFER::Send(const void *data, int &error)
{
	m_pHandle->Access();
		// **** Verify Input ****
		if (data==NULL) // invalid data
		{			
			error = MTU_NULLDATA; // report error
			m_pHandle->Release(); // release handle
			return; // cannot write NULL data
		}
		// **** Check if buffer is full ****
		// get next lowest position marker, and check all positon markers
		register int i = m_nUsers;
		register int nextLowestPM = m_nBlocks;
		while (i--)
		{
			if (*(m_pUserPMs + i)==m_writePos) // if pm equals writePos
			{
				nextLowestPM = m_writePos; // record it as the next lowest postion marker
				break; // break loop, lowest possible nextLowesPM found
			}
			else if ( (*(m_pUserPMs + i)<nextLowestPM)&& // if pm lower than current next lowest
				(*(m_pUserPMs + i)>m_writePos) ) // and greater than the writePos
				nextLowestPM = m_writePos; // record it as the next lowest postion marker
		}
		if (m_writePos==nextLowestPM) // buffer is full if writePos = nextLowestPM
		{
			m_usagePeak = m_usageRate = 1.0f; // usage is 100%
			if (m_writePos==int(m_nBlocks-1)) // if the write positon is at the end of the buffer
				ScaleBuffer(); // scale buffer
			error = MTU_BUFFERFULL; // report error
			m_pHandle->Release(); // release handle
			return; // cannot write to a full buffer
		}
		// **** Write to buffer at m_writePos ****
		memcpy(m_pBuffer+(m_writePos*int(m_segmentSize)),data,m_segmentSize);
		// **** Update write postion ****
		m_writePos++; // increment write postion counter
		if (m_writePos>=int(m_nBlocks)) // if passed end of buffer
			m_writePos = 0; // reset write position
		// **** Calculate current usageRate ****
		m_usageRate = float(m_nBlocks-(nextLowestPM-m_writePos)) / // # blocks filled
					  float(m_nBlocks); // divided by total # of blocks
		// **** Scale buffer if necessary ****
		if (m_usageRate>m_usagePeak) // record usage peak
			m_usagePeak = m_usageRate;
		if ((m_usagePeak>MTU_BUFFERTOLERANCE)&& // if peak is passed tolerance and
			(m_writePos==int(m_nBlocks-1)) ) // the write positon is at the end of the buffer
				ScaleBuffer(); // scale buffer
		error = MTU_SUCCESS; // report success
	m_pHandle->Release();
}

/*****************************************************************************

  Polls for new data from CCOMBUFFER.

*****************************************************************************/
void CCOMBUFFER::Recieve(int userID,void *data)
{
	m_pHandle->Access();
		if (ISIDXVALID(userID,m_nUsers)&&(data!=NULL)) // if valid userID and data!=NULL
			if ((*(m_pUserPMs + userID) != -1)&& // and user is not dead
				(*(m_pUserPMs + userID) != m_writePos)) // and new data is available for user
			{
				// **** Copy data from buffer ****
				memcpy(data,m_pBuffer+(*(m_pUserPMs + userID)*int(m_segmentSize)),m_segmentSize);
				// **** Update user position marker ****
				(*(m_pUserPMs + userID))++; // increment user position marker
				if (m_writePos>=int(m_nBlocks)) // if at end of buffer
					m_writePos = 0; // reset user position marker
			}
	m_pHandle->Release();
}
