/*****************************************************************************

  Thread Safe Logger
  log.h

  DATE	:	12/13/2003
  AUTHOR:	Bernard Igiri

  A simple thread safe logger.

  This code has the following dependancies:
  <windows.h>,<malloc.h>,<stdio.h>, & "stringUtilities.h"

*****************************************************************************/
#ifndef __INCLUDED_LOG_H__
#define __INCLUDED_LOG_H__
#include <stdio.h>

class CECHOOBJ
{
public:
	virtual void Echo(const char *pStr) = 0; // function to echo string message
};

class CLOG
{
public:
	CLOG(const char *path);
	~CLOG();
	void SetEcho(CECHOOBJ *pEcho); // Sets an object to echo log output, NULL values are allowed
	void PrintF(const char *text,...); // Prints to log (1024 char max)
private:
	void Open();	// Opens Log
	void Close();	// Closes Log
	void Access();	// Accesses m_pHandle
	void Release(); // Releases m_pHandle
	char *m_pPath;	// Log Filepath
	FILE *m_fptr;	// Log File Handle
	void *m_pHandle;// Log Access Handle
	char m_buff[1024];// Buffer for Log Input

	CECHOOBJ *m_pEcho; // pointer to echo object
};

#endif//__INCLUDED_LOG_H__
