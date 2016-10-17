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

*****************************************************************************/
#ifndef __INCLUDED_GLWINAPI_H__
#define __INCLUDED_GLWINAPI_H__

#define WIN32_LEAN_AND_MEAN   // get rid of Windows things we don't need

#include <windows.h>          // included in all Windows apps
#include <gl/gl.h>            // OpenGL include
#include <gl/glu.h>           // OpenGL utilty library
#include "stringUtilities.h"

struct CGWASCRN
{
	HDC		m_hdc;
	bool	m_isFullScreen;
	bool	m_is3D;
	int		m_width;
	int		m_height;
	int		m_bits;
};

struct CGWAPROJ
{
	CPROJECTION() : m_pov(45.0f), m_near (1.0f), m_far(400.0f) {};
	~CPROJECTION() {};
	void Set(float spov, float snear, float sfar);
	float m_pov;
	float m_near;
	float m_far;
};

struct CGWAWIN
{
	bool		m_isActive;
	HWND		m_hwnd;
	HGLRC		m_hrc;
	HINSTANCE	m_hInstance;
	char*		m_pTitle;
}


class GLWINAPI
{
public:
	GLWINAPI();
	~GLWINAPI();
	bool Initialize(const char* appTitle, int width, int height, int bits, bool isFullScreen);
	void Resize(int width, int height);
	void Sel3DProj();
	void Sel2DProj();
	void Kill();


protected:
	CGWASCRN	m_screen;
	CGWAPROJ	m_projection;
	CGWAWIN		m_window;
	BITSTRING_t m_keysOn; // BITSTRING_t list of Virtual Key states
	void*		m_handle; // access handle
};

GLWINAPI* GetGLWINAPI();

#endif//__INCLUDED_GLWINAPI_H__