#include "di8input.h"	// direct input 8 wrapper includes inputman & glwinapi
#include "strinproc.h"	// stringed input processor
#include <stdio.h>		// OpenGL utilty library
#include "hud.h"		// head up display
#include "log.h"
#include "consol.h"


BOOL    InitializeScene();
BOOL    DisplayScene(CCONSOL *pConsol,CINPUTMAN *pInput,CSTRINPROC *pStrInProc,CTFMAN *pTFMAN);
BOOL    Cleanup();

/*****************************************************************************
 WinMain()

 Windows entry point
*****************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	CLOG cLog("log.txt");
	GLWINAPI* pGLWINAPI = GetGLWINAPI(); // Get the First Instance of GLWINAPI
	CINPUTMAN cInput;		// input manager
	MSG   msg;				// message
	BOOL  isDone = true;    // flag indicating the app is done
	CSTRINPROC cStrInProc;	// Stringed Input Processor

	CTFMAN cTFMAN("fonts/",&cLog); // font manager

	

	cLog.PrintF("Intializing Input 2003 Demo Window.");
	// if the window is set up correctly, we can proceed with the message loop
	// 1280,1024
	// init glwinapi
	isDone = !pGLWINAPI->Initialize("Input 2003",1280,1024,32,true);
	// create consol
	CCONSOL cConsol(&cLog,pGLWINAPI,&cTFMAN);
	// init input
	if (!isDone)
	{
		cLog.PrintF("Intializing Input 2003 Demo Scene.");
		InitializeScene();
		cLog.PrintF("Intializing Input 2003 Demo Input.");
		void *pDI8 = DI8Initialize(&cInput,pGLWINAPI);
		cLog.PrintF("Installing Keyboard...");
		isDone=!DI8AddKeyBoard(pDI8)||!DI8AddMouse(pDI8);
		cLog.PrintF("Creating buttons...");
		if (DI8AddKeyBoard(pDI8))
		{
			int up		= cInput.AddButton("Up"),
				down	= cInput.AddButton("Down"),
				left	= cInput.AddButton("Left"),
				right	= cInput.AddButton("Right"),
				p2up	= cInput.AddButton("P2 Up"),
				p2down	= cInput.AddButton("P2 Down"),
				p2left	= cInput.AddButton("P2 Left"),
				p2right	= cInput.AddButton("P2 Right"),
				keyboard= cInput.FindDevice(DI8INPUT_KEYBOARD),
				mousex	= cInput.AddButton("Mouse X"),
				mousey	= cInput.AddButton("Mouse Y"),
				mouse	= cInput.FindDevice(DI8INPUT_MOUSE);

			cInput.SetButton(mousex,mouse,DI8INPUT_MOUSEX,false);
			cInput.SetButton(mousey,mouse,DI8INPUT_MOUSEY,false);

			cInput.SetButton(up,keyboard,200,true);
			cInput.SetButton(down,keyboard,208,true);
			cInput.SetButton(left,keyboard,203,true);
			cInput.SetButton(right,keyboard,205,true);
			
			cInput.SetButton(cInput.AddButton("Tab"),keyboard,15,true);

			cInput.SetButton(p2up,keyboard,0x11,true);
			cInput.SetButton(p2down,keyboard,0x1F,true);
			cInput.SetButton(p2left,keyboard,0x1E,true);
			cInput.SetButton(p2right,keyboard,0x20,true);

			cStrInProc.Initialize('+',STRINPROC_DEFAULTTIMEOUT,10);

			CSTRBUTTON cSIPButtons[4];
			cSIPButtons[0].Set(cInput.GetButton(up),'^');
			cSIPButtons[1].Set(cInput.GetButton(down),'V');
			cSIPButtons[2].Set(cInput.GetButton(left),'<');
			cSIPButtons[3].Set(cInput.GetButton(right),'>');

			cStrInProc.AddChannel("Player 1 Input",4,cSIPButtons);

			CSTRBUTTON cSIPButtons2[4];
			cSIPButtons2[0].Set(cInput.GetButton(p2up),'^');
			cSIPButtons2[1].Set(cInput.GetButton(p2down),'V');
			cSIPButtons2[2].Set(cInput.GetButton(p2left),'<');
			cSIPButtons2[3].Set(cInput.GetButton(p2right),'>');

			cStrInProc.AddChannel("Player 2 Input",4,cSIPButtons2);
		}
	}
	cLog.PrintF("Intialization of Demo Input Complete.");

	// main message loop
	
	CGWAWIN windowDataCpy; // stores local copy of window data
	BITSTRING_t keysOnCpy; // stores local copy of keysOn
	keysOnCpy.Init(GLWINAPI_NKEYS);
	bool oldState=false;
	bool sState=false;
	while (!isDone)
	{		
		pGLWINAPI->GetWindow(&windowDataCpy); // updates local copy of window data
		if(PeekMessage(&msg, windowDataCpy.m_hwnd, NULL, NULL, PM_REMOVE))
		{
			pGLWINAPI->GetKeysOn(&keysOnCpy);
			if (keysOnCpy.Check(VK_ESCAPE)) // Post Quit if Esc key is hit
			{
				PostQuitMessage(0);
				isDone = TRUE;              // if so, time to quit the application
			}

			else if (keysOnCpy.Check(VK_SPACE)!=oldState)
			{
				oldState=keysOnCpy.Check(VK_SPACE);
				if (oldState)
				{
					bool success;
					sState=!sState;
					if (sState)
						pGLWINAPI->SetDisplay(640,480,success);
					else
						pGLWINAPI->SetDisplay(1024,768,success);
				}
			}

			if (msg.message == WM_QUIT)   // do we receive a WM_QUIT message?
			{
				isDone = TRUE;              // if so, time to quit the application
			}
			else
			{
				TranslateMessage(&msg);     // translate and dispatch to event queue
				DispatchMessage(&msg);
			}
		}


		// don't update the scene if the app is minimized
		if (windowDataCpy.m_isActive)
		{
			// update the scene every time through the loop
			cInput.Update();
			cStrInProc.Update();
			DisplayScene(&cConsol,&cInput,&cStrInProc,&cTFMAN);
		}
	}

	cLog.PrintF("Shutting Down Input 2003 Demo.");

	cStrInProc.Kill();

	Cleanup(); // Call CleanUp Procedures

	pGLWINAPI->Kill();
	cLog.PrintF("Exiting Input 2003 Demo Window.");

	return msg.wParam;
} // end WinMain()


/*****************************************************************************
 InitializeScene()

 Performs one-time application-specific setup. Returns FALSE on any failure.
*****************************************************************************/
BOOL InitializeScene()
{
	GLWINAPI* pGLWINAPI = GetGLWINAPI();
	glClearDepth (1.0f);

	float fogColor[] = {0.5f, 0.8f, 0.8f, 1.0f};
	
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.001f);
	
	glFogf(GL_FOG_START, 100.0f);
	glFogf(GL_FOG_END, 400.0f);
	
	float diffuseLight[] = {0.9f,0.9f,0.9f,1.0f};
	float matDiff[] = {0.9f,0.9f,0.9f,1.0f};
	float lightPosition[] = {0.0f,200.0f,-500.0f,1.0f};
  	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,matDiff);
	glMaterialfv(GL_FRONT,GL_AMBIENT,matDiff);
	glMaterialf(GL_FRONT,GL_SHININESS,10.0f);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_AMBIENT,diffuseLight);

	lightPosition[0]=float(32*1)/2.0f;
	lightPosition[2]=float(32*1)/2.0f;
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosition);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(1.0f);
	pGLWINAPI->Sel2DProj();
	
	return TRUE;
} // end InitializeScene()

/*****************************************************************************
 DisplayScene()

 The work of the application is done here. This is called every frame, and
 handles the actual rendering of the scene.
*****************************************************************************/
BOOL DisplayScene(CCONSOL *pConsol,CINPUTMAN *pInput,CSTRINPROC *pStrInProc,CTFMAN *pTFMAN)
{
	static GLWINAPI* pGLWINAPI = GetGLWINAPI();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color/depth buffer
	glLoadIdentity(); // reset modelview matrix
	static HUDdata_t info(pTFMAN);
	info.count = 1;
	
	static CGWASCRN scrnCpy;
	static CGWAVIDEO videoCpy;
	pGLWINAPI->GetGraphics(&videoCpy,&scrnCpy);

	static bool isFirstTime=true;
	if (isFirstTime)
	{
		isFirstTime=false;
		info.m_pConsol = pConsol;
	}

	char gotKey=NULL;
	unsigned char flags=NULL;

	if (pInput!=NULL)
	{
		static int keyboard	= pInput->FindDevice(DI8INPUT_KEYBOARD);
		gotKey=pInput->GetKey();
		flags=pInput->GetSKeys();
	}
	if (pStrInProc)
	{
		static int plyr1in = pStrInProc->FindChannel("Player 1 Input"),
				   plyr2in = pStrInProc->FindChannel("Player 2 Input");
		static CBUTTON* pTabKey = pInput->GetButton(pInput->FindButton("Tab")),
						*pMousex = pInput->GetButton(pInput->FindButton("Mouse X")),
						*pMousey = pInput->GetButton(pInput->FindButton("Mouse Y"));
		if (pTabKey)
		{
			if (pTabKey->GetToggle())
				info.pStr = pStrInProc->GetInStr(plyr1in);
			else
				info.pStr = pStrInProc->GetInStr(plyr2in);;
		}
		if (pMousex&&pMousey)
		{
			info.x = (int)(float(scrnCpy.m_width-1)*pMousex->GetAnalog());
			info.y = (int)(float(scrnCpy.m_height-1)*pMousey->GetAnalog());
			info.y++;
		}
	}
	HUDRender(&info, scrnCpy.m_width, scrnCpy.m_height,gotKey,flags);
	glFlush();
	SwapBuffers(videoCpy.m_hdc);
	return TRUE;
} // end DisplayScene()


/*****************************************************************************
 Cleanup()

 Called at the end of successful program execution.
*****************************************************************************/
BOOL Cleanup()
{
	return TRUE;
} // end Cleanup()