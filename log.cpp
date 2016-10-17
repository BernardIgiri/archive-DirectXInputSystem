#include "log.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#include "stringUtilities.h"

CLOG::CLOG(const char *path)
{
	memset(this,NULL,sizeof(CLOG)); // clear all vars
	// copy path
	if (!STRUTILCopyStr(&m_pPath,path)) // copy error
		MessageBox(NULL,"Log Error, string copy failed!","ERROR",MB_OK);
	else
	{
		m_fptr=fopen(m_pPath,"w"); // open file and clear contents.
		if(!m_fptr)	// fopen error
			MessageBox(NULL,"Log Error, invalid path","ERROR",MB_OK);
	}
	Close();
	// create handle
	m_pHandle = new HANDLE;
	if (!m_pHandle) // new failed
		MessageBox(NULL,"Log Error, ram allocation failed.","ERROR",MB_OK);
	(*((HANDLE *)m_pHandle))=CreateMutex(NULL, FALSE, NULL);
	if ((*((HANDLE *)m_pHandle))==NULL)
		MessageBox(NULL,"Log Error, CreateMutex failed.","ERROR",MB_OK);
}

CLOG::~CLOG()
{
	// free ram
	if (m_pPath)
		free(m_pPath);
	if (m_pHandle)
		delete ((HANDLE *)m_pHandle);
	// close file
	Close();
}

void CLOG::Access()
{
	WaitForSingleObject((*((HANDLE *)m_pHandle)),INFINITE);
}

void CLOG::Release()
{
	ReleaseMutex((*((HANDLE *)m_pHandle)));
}

void CLOG::Open()
{
	if(!m_fptr)
		m_fptr = fopen(m_pPath,"a");
}

void CLOG::Close()
{
	if(m_fptr!=NULL)
	{
		fclose(m_fptr);
		m_fptr = NULL;
	}
}

void CLOG:: SetEcho(CECHOOBJ *pEcho)
{
	Access();
		m_pEcho = pEcho;
	Release();
}

void CLOG::PrintF(const char *text,...)
{
	Access();
		Open();
			if (!m_fptr)
				return; // If there was an Error, Stop
			m_buff[0]=NULL; // clear buffer
			va_list arglist;
			va_start(arglist,text);
			vsprintf(m_buff,text,arglist);
			va_end(arglist);
			strcat(m_buff,"\n"); // append newline char
			fputs(m_buff,m_fptr); // Write To The File
		Close(); // Close The Log
		if (m_pEcho) // echo ouput
			m_pEcho->Echo(m_buff);
	Release();
}
