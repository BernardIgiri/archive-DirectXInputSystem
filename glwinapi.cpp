#include "glwinapi.h"
#include <winuser.h>		// Windows constants
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "mtutilities.h"	// for handle
#include "stdmacros.h"

#define WND_CLASS_NAME  "OpenGL Window Class"

LRESULT CALLBACK GLWINAPIWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

/*****************************************************************************

  Sets the projections setting variables.

*****************************************************************************/
void CGWAPROJ::Set(float spov, float snear, float sfar)
{
	m_pov=spov;
	m_near=snear;
	m_far=sfar;
}

/*****************************************************************************

  Sets CGWAUSER variables to their default settings.

*****************************************************************************/
void CGWAUSER::Reset()
{
	memset(this,NULL,sizeof(CGWAUSER));
	m_id = -1;
}

/*****************************************************************************

  Creates an instance of GLWINAPI.

*****************************************************************************/
GLWINAPI::GLWINAPI()
{
	Clear();
	// create handle
	m_handle = CreateAccessHandle();
	if (!m_handle)
	    MessageBox(NULL,"Handle creation failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
}

/*****************************************************************************

  Destroys an instance of GLWINAPI.

*****************************************************************************/
GLWINAPI::~GLWINAPI()
{
	Kill();	
	// destroy handle
	DestroyAccessHandle((CACCESSHANDLE *)m_handle);
	m_handle = NULL;
	Clear();
}

/*****************************************************************************

  Set all GLWINAPI class variables to zero. USE WITH CAUTION!!!

*****************************************************************************/
void GLWINAPI::Clear()
{
	memset(this,NULL,sizeof(GLWINAPI));
}

/*****************************************************************************

  Creates an application window and initializes GLWINAPI.

*****************************************************************************/
bool GLWINAPI::Initialize(const char *appTitle, int width, int height, int bits, bool isFullScreen)
{
	// COPY SETTINGS TO MEMBER VARS
	m_video.m_isFullScreen	= isFullScreen;
	m_screen.m_width		= width;
	m_screen.m_height		= height;
	m_screen.m_bpp			= bits;
	if (!STRUTILCopyStr(&m_pTitle,appTitle))
	{
		m_pTitle=NULL;
	    MessageBox(NULL,"Invalid input or memory error.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	m_keysOn.Init(GLWINAPI_NKEYS);

	// get our instance handle
	m_window.m_hInstance = GetModuleHandle(NULL);
	
	WNDCLASSEX  wc;    // window class
	
	// fill out the window class structure
	memset(&wc,0,sizeof(wc));
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc    = GLWINAPIWndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = m_window.m_hInstance;
	wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);  // default icon
	wc.hIconSm        = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);      // default arrow
	wc.hbrBackground  = NULL;     // no background needed
	wc.lpszMenuName   = NULL;     // no menu
	wc.lpszClassName  = WND_CLASS_NAME;


	// register the windows class
	if (!RegisterClassEx(&wc))
	{
	    MessageBox(NULL,"Unable to register the window class", "Error", MB_OK | MB_ICONEXCLAMATION);

		// exit and return FALSE
		return false;       
	}

	// if we're in fullscreen mode, set the display up for it
	if (m_video.m_isFullScreen)
	{
	    // set up the device mode structure
	    DEVMODE screenSettings;
	    memset(&screenSettings,0,sizeof(screenSettings));

		screenSettings.dmSize       = sizeof(screenSettings);
		screenSettings.dmPelsWidth  = width;    // screen width
		screenSettings.dmPelsHeight = height;   // screen height
		screenSettings.dmBitsPerPel = bits;     // bits per pixel
		screenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// attempt to switch to the resolution and bit depth we've selected
		if (ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// if we can't get fullscreen, let them choose to quit or try windowed mode
			if (MessageBox(NULL, "Cannot run in the fullscreen mode at the selected resolution\n"
								 "on your video card. Try windowed mode instead?",
								 appTitle,
								 MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				m_video.m_isFullScreen = false;
			}
			else
			{
				return false;
			}
		}
	}
	
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // default for non-fullscreen
	DWORD dwStyle = WS_OVERLAPPEDWINDOW; // default for non-fullscreen

	// set the window style appropriately, depending on whether we're in fullscreen mode
	if (m_video.m_isFullScreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;           // simple window with no borders or title bar
		ShowCursor(FALSE);            // hide the cursor for now
	}

	// set up the window we're rendering to so that the top left corner is at (0,0)
	// and the bottom right corner is (height,width)
	RECT  windowRect;
	windowRect.left = 0;
	windowRect.right = (LONG) width;
	windowRect.top = 0;
	windowRect.bottom = (LONG) height;

	// change the size of the rect to account for borders, etc. set by the style
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	// class registered, so now create our window OpenGL Window Class
	m_window.m_hwnd = CreateWindowEx(dwExStyle,        // extended style
                          WND_CLASS_NAME,     // class name
                          appTitle,           // app name
                          dwStyle |           // window style
                          WS_CLIPCHILDREN |   // required for
                          WS_CLIPSIBLINGS,    // using OpenGL //
                          0, 0,               // x,y coordinate
                          windowRect.right - windowRect.left, // width
                          windowRect.bottom - windowRect.top, // height
                          NULL,               // handle to parent
                          NULL,               // handle to menu
                          m_window.m_hInstance,        // application instance
                          NULL);              // no extra params

	// see if our window handle is valid
	if (!m_window.m_hwnd)
	{
		MessageBox(NULL, "Unable to create window", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// get a device context
	if (!(m_video.m_hdc = GetDC(m_window.m_hwnd)))
	{
		MessageBox(NULL,"Unable to create device context", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// set the pixel format we want
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
		1,                              // default version
		PFD_DRAW_TO_WINDOW |            // window drawing support
		PFD_SUPPORT_OPENGL |            // OpenGL support
		PFD_DOUBLEBUFFER,               // double buffering support
		PFD_TYPE_RGBA,                  // RGBA color mode
		bits,                           // 32 bit color mode
		0, 0, 0, 0, 0, 0,               // ignore color bits, non-palettized mode
		0,                              // no alpha buffer
		0,                              // ignore shift bit
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // ignore accumulation bits
		16,                             // 16 bit z-buffer size
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main drawing plane
		0,                              // reserved
		0, 0, 0 };                      // layer masks ignored
      
	GLuint  pixelFormat;

	// choose best matching pixel format
	if (!(pixelFormat = ChoosePixelFormat(m_video.m_hdc, &pfd)))
	{
		MessageBox(NULL, "Can't find an appropriate pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// set pixel format to device context
	if(!SetPixelFormat(m_video.m_hdc, pixelFormat,&pfd))
	{
		MessageBox(NULL, "Unable to set pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	  // create the OpenGL rendering context
	if (!(m_video.m_hrc = wglCreateContext(m_video.m_hdc)))
	{
		MessageBox(NULL, "Unable to create OpenGL rendering context", "Error",MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// now make the rendering context the active one
	if(!wglMakeCurrent(m_video.m_hdc, m_video.m_hrc))
	{
		MessageBox(NULL,"Unable to activate OpenGL rendering context", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// show the window in the forground, and set the keyboard focus to it
	ShowWindow(m_window.m_hwnd, SW_SHOW);
	SetForegroundWindow(m_window.m_hwnd);
	SetFocus(m_window.m_hwnd);

	// set up the perspective for the current screen size
	Resize(width, height);

	return true;
}

/*****************************************************************************

  Frees up the memory allocated by GLWINAPI.

*****************************************************************************/
void GLWINAPI::Kill()
{
	if (m_pTitle)
	{
		free(m_pTitle);
		m_pTitle=NULL;
	}
	else
		return; // if NULL then it must already be dead
	// kill users
	if (m_pUsers)
		free(m_pUsers);
	m_pUsers = NULL;
	m_nUsers = 0;

	// restore the original display if we're in fullscreen mode
	if (m_video.m_isFullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	// if we have an RC, release it
	if (m_video.m_hrc)
	{
		// release the RC
		if (!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL, "Unable to release rendering context", "Error", MB_OK | MB_ICONINFORMATION);
		}

		// delete the RC
		if (!wglDeleteContext(m_video.m_hrc))
		{
			MessageBox(NULL, "Unable to delete rendering context", "Error", MB_OK | MB_ICONINFORMATION);
		}

		m_video.m_hrc = NULL;
	}

	// release the DC if we have one
	if (m_video.m_hdc && !ReleaseDC(m_window.m_hwnd, m_video.m_hdc))
	{
		MessageBox(NULL, "Unable to release device context", "Error", MB_OK | MB_ICONINFORMATION);
		m_video.m_hdc = NULL;
	}

	// destroy the window if we have a valid handle
	if (m_window.m_hwnd && !DestroyWindow(m_window.m_hwnd))
	{
		MessageBox(NULL, "Unable to destroy window", "Error", MB_OK | MB_ICONINFORMATION);
		m_window.m_hwnd = NULL;
	}

	// unregister our class so we can create a new one if we need to
	if (!UnregisterClass(WND_CLASS_NAME, m_window.m_hInstance))
	{
		MessageBox(NULL, "Unable to unregister window class", "Error", MB_OK | MB_ICONINFORMATION);
		m_window.m_hInstance = NULL;
	}
}

/*****************************************************************************

  Sets OpenGL projection mode.

*****************************************************************************/
void GLWINAPI::SelProj(bool is3D)
{
	if (is3D)
	{ // Set OpenGL to the 3D projection mode described by m_projection.
		m_video.m_is3D = true;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(m_projection.m_pov,(GLfloat)m_screen.m_width/(GLfloat)m_screen.m_height,m_projection.m_near,m_projection.m_far);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_DEPTH_TEST);
	}
	else
	{ // Sets OpenGL to a 2D projection mode.
		m_video.m_is3D = false;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, m_screen.m_width, 0, m_screen.m_height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
	}
}

/*****************************************************************************

  Calls the m_pAcquire function for all users.

*****************************************************************************/
void GLWINAPI::Acquire()
{
	register int count = m_nUsers;
	while (count--)
		if ((m_pUsers + count)->m_pAcquire) // if an acquire function is found
		{
			CGWAUSER *user = (m_pUsers + count);
			(m_pUsers + count)->m_pAcquire((m_pUsers + count)->m_pData); // call it
		}

}

/*****************************************************************************

  Calls the m_pUnAcquire function for all users.

*****************************************************************************/
void GLWINAPI::UnAcquire()
{
	register int count = m_nUsers;
	while (count--)
		if ((m_pUsers + count)->m_pUnAcquire) // if an unacquire function is found
			(m_pUsers + count)->m_pUnAcquire((m_pUsers + count)->m_pData); // call it
}

/*****************************************************************************

  Updates all users with the newly changed flag.

*****************************************************************************/
void GLWINAPI::UpdateUserFlags(unsigned char flag)
{
	register int count = m_nUsers;
	while (count--) // update all user, yes even dead ones;
		(m_pUsers + count)->m_flags = (m_pUsers + count)->m_flags | flag;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////N O T E ! ! !/////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/// All GLWINAPI member functions beyond this point must use m_handle for ////
/// all operations. //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************

  Sets the status of m_window.m_isActive.

*****************************************************************************/
void GLWINAPI::SetIsActive(bool state)
{
	((CACCESSHANDLE *)m_handle)->Access();
		m_window.m_isActive = state;
		if (state)
			Acquire(); // Instruct all users to acquire application focus
		else
			UnAcquire(); // Instruct all users to unacquire application focus
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Sets GLWINAPI key values to the value of state.

*****************************************************************************/
void GLWINAPI::SetKeysOn(int key,bool state)
{
	((CACCESSHANDLE *)m_handle)->Access();
		m_keysOn.Set(key,state);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Resizes the application GLWINAPI window.
  NOTE: This does not alter the screen resolution.

*****************************************************************************/
void GLWINAPI::Resize(int width, int height)
{
	((CACCESSHANDLE *)m_handle)->Access();
		UpdateUserFlags(GLWINAPI_RESCHANGE); // report change in resolution to all users
		// avoid divide by zero
		if (height==0)
			height=1;

		m_screen.m_width = width;
		m_screen.m_height = height;

		// set the viewport to the new dimensions
		glViewport(0, 0, width, height);

		SelProj(m_video.m_is3D);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Changes the current screen resolution.
  IMPORTANT: In Full 

*****************************************************************************/
void GLWINAPI::SetDisplay(int width,int height,bool &success)
{
	((CACCESSHANDLE *)m_handle)->Access();
		success=true;
		// avoid divide by zero
		if (height==0)
			height=1;
		// COPY SETTINGS TO MEMBER VARS
		m_screen.m_width		= width;
		m_screen.m_height		= height;
		// Adjust screen resolution if in full screen mode
		if (m_video.m_isFullScreen)
		{
			// set up the device mode structure
			DEVMODE screenSettings;
			memset(&screenSettings,0,sizeof(screenSettings));

			screenSettings.dmSize       = sizeof(screenSettings);
			screenSettings.dmPelsWidth  = m_screen.m_width;    // screen width
			screenSettings.dmPelsHeight = m_screen.m_height;   // screen height
			screenSettings.dmBitsPerPel = m_screen.m_bpp;     // bits per pixel
			screenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// attempt to switch to the resolution and bit depth we've selected
			if (ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// report failure
				success=false;
				((CACCESSHANDLE *)m_handle)->Release();
				return;
			}
		}
		// Adjust window size
		if (!SetWindowPos(m_window.m_hwnd,
		  HWND_TOP,0,0,m_screen.m_width,m_screen.m_height,
		  SWP_NOZORDER | SWP_SHOWWINDOW ))
		{
			// report failure
			success=false;
			((CACCESSHANDLE *)m_handle)->Release();
			return;
		}
			// Update Viewport size
			UpdateUserFlags(GLWINAPI_RESCHANGE); // report change in resolution to all users
			// set the viewport to the new dimensions
			glViewport(0, 0, width, height);
			// Select current projection mode
			SelProj(m_video.m_is3D);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Sets OpenGL to a 2D projection mode.

*****************************************************************************/
void GLWINAPI::Sel2DProj()
{
	((CACCESSHANDLE *)m_handle)->Access();
		SelProj(false);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Sets OpenGL to the 3D projection mode described by m_projection.

*****************************************************************************/
void GLWINAPI::Sel3DProj()
{
	((CACCESSHANDLE *)m_handle)->Access();
		SelProj(true);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Sets the projections settings for the 3D projection mode.

*****************************************************************************/
void GLWINAPI::Set3DProj(float spov,float snear,float sfar)
{
	((CACCESSHANDLE *)m_handle)->Access();
		m_projection.Set(spov,snear,sfar);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies the CGWAVIDEO struct over pVideo.

*****************************************************************************/
void GLWINAPI::GetVideo(CGWAVIDEO *pVideo)
{
	((CACCESSHANDLE *)m_handle)->Access();
		memcpy(pVideo,&m_video,sizeof(CGWAVIDEO));
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies the CGWASCRN struct over pScreen.

*****************************************************************************/
void GLWINAPI::GetScreen(CGWASCRN *pScreen)
{
	((CACCESSHANDLE *)m_handle)->Access();
		memcpy(pScreen,&m_screen,sizeof(CGWASCRN));
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies the CGWAVIDEO struct over pVideo.
  And copies the CGWASCRN struct over pScreen.

*****************************************************************************/
void GLWINAPI::GetGraphics(CGWAVIDEO *pVideo,CGWASCRN *pScreen)
{
	((CACCESSHANDLE *)m_handle)->Access();
		memcpy(pVideo,&m_video,sizeof(CGWAVIDEO));
		memcpy(pScreen,&m_screen,sizeof(CGWASCRN));
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies the CGWAPROJ struct over pProjection.

*****************************************************************************/
void GLWINAPI::GetProjection(CGWAPROJ *pProjection)
{
	((CACCESSHANDLE *)m_handle)->Access();
		memcpy(pProjection,&m_projection,sizeof(CGWAPROJ));
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies the CGWAWIN struct over pWindow.

*****************************************************************************/
void GLWINAPI::GetWindow(CGWAWIN *pWindow)
{
	((CACCESSHANDLE *)m_handle)->Access();
		memcpy(pWindow,&m_window,sizeof(CGWAWIN));
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies m_pTitle over the contents of pTitle.

*****************************************************************************/
void GLWINAPI::GetTitle(char **pTitle)
{
	((CACCESSHANDLE *)m_handle)->Access();
		STRUTILCopyStr(pTitle,m_pTitle);
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Copies m_keysOn over keysOn.
  IMPORTANT: Initialize keysOn to GLWINAPI_NKEYS bits first.

*****************************************************************************/
void GLWINAPI::GetKeysOn(BITSTRING_t *pKeysOn)
{
	((CACCESSHANDLE *)m_handle)->Access();		
		memcpy(pKeysOn->bytes,m_keysOn.bytes,m_keysOn.GetSize(GLWINAPI_NKEYS));
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Creates a new GLWINAPI user and set int &id to the id of the new user.
  An id of -1 indicates failure.

*****************************************************************************/
void GLWINAPI::CreateUser(int &id,GWA_PACQUIRE pAcquire,GWA_PUNACQUIRE pUnAcquire,void *pData)
{
	((CACCESSHANDLE *)m_handle)->Access();
		id = -1; // -1 indicates failure
		// increment number of users
		m_nUsers++;
		// allocate space for users
		if (!m_pUsers)
			m_pUsers = (CGWAUSER*)malloc(sizeof(CGWAUSER));
		else
			m_pUsers = (CGWAUSER*)realloc(m_pUsers,sizeof(CGWAUSER)*m_nUsers);
		// check pointer
		if (!m_pUsers)
		{
			m_nUsers=0;
			return;
		}
		// record user id
		id = m_nUsers-1;
		// initialize new user
		(m_pUsers+id)->Reset();
		(m_pUsers+id)->m_pAcquire = pAcquire;
		(m_pUsers+id)->m_pUnAcquire = pUnAcquire;
		(m_pUsers+id)->m_pData = pData;

	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Finalizes a GLWINAPI user so that it will no longer be updated.

*****************************************************************************/
void GLWINAPI::FinalizeUser(int id)
{
	((CACCESSHANDLE *)m_handle)->Access();
		if (ISIDXVALID(id,m_nUsers))
			(m_pUsers+id)->Reset();
	((CACCESSHANDLE *)m_handle)->Release();
}

/*****************************************************************************

  Retrieves the user flags and resets them to NULL.
  flags is not modified if this method fails.

*****************************************************************************/
void GLWINAPI::UpdateUser(int id,unsigned char &flags)
{
	((CACCESSHANDLE *)m_handle)->Access();
		if (ISIDXVALID(id,m_nUsers))
		{
			flags = (m_pUsers+id)->m_flags; // copy flags
			(m_pUsers+id)->m_flags = NULL; // reset flags
		}
	((CACCESSHANDLE *)m_handle)->Release();
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////// End of GLWINAPI class.////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************

  Returns a pointer to an instance of GLWINAPI

*****************************************************************************/
GLWINAPI* GetGLWINAPI()
{
	static GLWINAPI window;
	return &window;
}

/*****************************************************************************

  GLWINAPIWndProc()

  Windows Message Handler.

*****************************************************************************/
LRESULT CALLBACK GLWINAPIWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static GLWINAPI* pWindow = GetGLWINAPI();
	switch(message)
	{
	case WM_ACTIVATE:  // watch for the window being minimized and restored
	{
		if (!HIWORD(wParam))
		{
			// program was restored or maximized
			pWindow->SetIsActive(true);
			ShowCursor(FALSE);
		}
		else
		{
			// program was minimized
			pWindow->SetIsActive(false);
			ShowCursor(TRUE);
		}

		return 0;
	}

	case WM_SYSCOMMAND:  // look for screensavers and powersave mode
	{
		switch (wParam)
		{
			case SC_SCREENSAVE:     // screensaver trying to start
			case SC_MONITORPOWER:   // monitor going to powersave mode
				// returning 0 prevents either from happening
				return 0;
			default:
				break;
		}
	} break;

	case WM_CLOSE:    // window is being closed
	{
		// send WM_QUIT to message queue
		PostQuitMessage(0);

		return 0;
	}

	case WM_SIZE:
	{
		// update perspective with new width and height
		pWindow->Resize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	case WM_KEYDOWN:
	{
		pWindow->SetKeysOn(wParam,true);
		return 0;
		break;
	}
	case WM_KEYUP:

		pWindow->SetKeysOn(wParam,false);
		return 0;
		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
} // end GLWINAPIWndProc()
