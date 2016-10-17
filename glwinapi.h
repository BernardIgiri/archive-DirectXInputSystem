/****************************************************************************

  OpenGL Windows API wrapper
  glwinapi.h

  DATE		:	11/26/2003
  AUTHOR	:	Bernard Igiri

  Includes the GLWINAPI System which serves the purpose of creating
  and managing the application window for Win32 apps.

  To Get an instance of the system call
	GLWINAPI* GetGLWINAPI();
  Use this for the first and all other instances.
  Instances of GLWINAPI are not counted or tracked. Therefore when
  the program ends is the only time the system should be killed.
  WARNING!!!
  Terminate all dependants manually before calling Kill()!
  Create and initialize before use by any dependants!
  -This should most likely be the first system initialized.

  This code has the following dependancies.
  <windows.h> <gl/gl.h> <gl/glu.h> <winuser.h> <stdio.h> <math.h> <malloc.h>
  "mtutilities.h" "stdmacros.h" "stringUtilities.h"


*****************************************************************************/
#ifndef __INCLUDED_GLWINAPI_H__
#define __INCLUDED_GLWINAPI_H__

#define WIN32_LEAN_AND_MEAN   // get rid of Windows things we don't need
#include <windows.h>	// included in all Windows apps
#include <gl/gl.h>		// OpenGL include
#include <gl/glu.h>		// OpenGL utilty library
#include "stringUtilities.h"

// **** DEFINES ****
// number of onKeys
#define GLWINAPI_NKEYS 256
// flags
#define GLWINAPI_RESCHANGE 1

struct CGWASCRN
{
	int		m_width;
	int		m_height;
	int		m_bpp;
};

struct CGWAVIDEO
{
	bool	m_isFullScreen;
	bool	m_is3D;
	HDC		m_hdc;
	HGLRC	m_hrc;

};

struct CGWAPROJ
{
	CGWAPROJ() : m_pov(45.0f), m_near (1.0f), m_far(400.0f) {};
	~CGWAPROJ() {};
	void Set(float spov, float snear, float sfar);
	float m_pov;
	float m_near;
	float m_far;
};

struct CGWAWIN
{
	bool		m_isActive;
	HWND		m_hwnd;
	HINSTANCE	m_hInstance;
};

typedef void (*GWA_PACQUIRE)(void *data);
typedef void (*GWA_PUNACQUIRE)(void *data);

struct CGWAUSER
{
	void Reset();
	int				m_id; // id of dependant
	unsigned char	m_flags; // bit is on if changed since last update
	GWA_PACQUIRE	m_pAcquire; // function called when focus is returned to the app, may be NULL
	GWA_PUNACQUIRE	m_pUnAcquire; // function called when focus is taken from the app, may be NULL
	void*			m_pData; // pointer to data passed to the Acquire and UnAcquire functions
};

class GLWINAPI
{
public:
	GLWINAPI();
	~GLWINAPI();
	bool Initialize(const char* appTitle, int width, int height, int bits, bool isFullScreen);
	void Kill();

	void SetIsActive(bool state);
	void SetKeysOn(int key,bool state);
	void Resize(int width, int height);

	void SetDisplay(int width, int height,bool &success);

	void Set3DProj(float spov, float snear, float sfar);
	void Sel3DProj();
	void Sel2DProj();

	void GetVideo(CGWAVIDEO *pVideo);
	void GetScreen(CGWASCRN *pScreen);
	void GetGraphics(CGWAVIDEO *pVideo,CGWASCRN *pScreen);
	void GetProjection(CGWAPROJ *pProjection);
	void GetWindow(CGWAWIN *pWindow);
	void GetTitle(char **pTitle);
	void GetKeysOn(BITSTRING_t *pKeysOn);

	void CreateUser(int &id,GWA_PACQUIRE pAcquire,GWA_PUNACQUIRE pUnAcquire,void *pData);
	void FinalizeUser(int id);
	void UpdateUser(int id,unsigned char &flags);
protected:
	void SelProj(bool is3D);
	void Clear();
	void Acquire();
	void UnAcquire();
	void UpdateUserFlags(unsigned char flag);

	CGWAVIDEO	m_video; // video data struct
	CGWASCRN	m_screen; // screen data struct
	CGWAPROJ	m_projection; // projection data struct
	CGWAWIN		m_window; // window data struct
	char*		m_pTitle; // window title
	BITSTRING_t m_keysOn; // BITSTRING_t list of Virtual Key states
	void*		m_handle; // access handle

	CGWAUSER*	m_pUsers; // pointer to users
	int			m_nUsers; // number of users
};

GLWINAPI* GetGLWINAPI();

#endif//__INCLUDED_GLWINAPI_H__