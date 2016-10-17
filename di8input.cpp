// includes
#include <dinput.h>
#include "di8input.h"
#include "stringUtilities.h"

/*****************************************************************************

  IMPORTANT NOTE.
  The direct input 8 mouse is the only device that uses the
  "DI8InputData_t" object's "gwaUserID" member.

*****************************************************************************/

// data types
struct DI8IDevice_t; // device data struct

struct DI8IDevLElmt_t // device list element type
{
	DI8IDevice_t* pDevice; // pointer to device
};

class DI8InputData_t // di8input object
{
public:
	CINPUTMAN*		pInput; // pointer to CINPUTMAN
	GLWINAPI*		pWindow; // pointer to GLWINAPI
	int				gwaUserID; // GLWINAPI user id
	int				nDevices; // number of nDevices
	DI8IDevLElmt_t*	pDevices; // list of devices
	LPDIRECTINPUT8	pDI; // pointer to direct input object
	int AddDevice(DI8IDevice_t* pDevice);
	void RemoveDevice(int id);
private:
	int				nDependants; // number of dependants
};

struct DI8IDevice_t
{
	DI8InputData_t*		 m_pInput;
	LPDIRECTINPUTDEVICE8 m_pDIDev;
	int					 m_di8inputID;
};

// prototypes
void DI8Kill(void *data);
void DI8Acquire(void *data);
void DI8UnAcquire(void *data);

void DI8GetKeyBoardInput(CINPUTDEVICE *pDevIn,void *data);
void DI8RemoveKeyBoard(void *data);
void DI8GetMouseInput(CINPUTDEVICE *pDevIn,void *data);
void DI8RemoveMouse(void *data);

/*****************************************************************************

  Adds a direct input 8 device to the device list.
  Returns the id of the new device.

*****************************************************************************/
int DI8InputData_t::AddDevice(DI8IDevice_t* pDevice)
{
	int id = -1; // return of -1 indicates failure
	// inc device counter
	nDevices++;
	// allocate memory
	if (!pDevices)
		pDevices=(DI8IDevLElmt_t*)malloc(sizeof(DI8IDevLElmt_t));
	else
		pDevices=(DI8IDevLElmt_t*)realloc(pDevices,sizeof(DI8IDevLElmt_t)*nDevices);
	// check pointer
	if (!pDevices)
	{ // allocation failed
		nDevices = 0;
		return -1;
	}
	id = nDevices-1;
	// initialize new element
	(pDevices+id)->pDevice = pDevice;
	// increment number of dependants
	nDependants++;
	return id; // return id
}

/*****************************************************************************

  Removes a direct input 8 device from the device list.
  Destroys the DI8InputData_t when all devices are removed.

*****************************************************************************/
void DI8InputData_t::RemoveDevice(int id)
{
	if ((id>-1)&&(id<nDevices))
	{ // only accept valid ids
		(pDevices+id)->pDevice = NULL; // remove device from list
		nDependants--; // reduce number of dependants
		if (nDependants<1)
			DI8Kill(this); // kill if no more dependants are left
	}
}

/*****************************************************************************

  Initializes direct input to ascii conversion table.

*****************************************************************************/
void DI8GetAsciiConTable(CASCIICONTBLE* pTable)
{
	/// CHARECTERS
	// formatting charecters
	pTable->table[DIK_BACK][0]=INPUTMAN_TEXT_CODE_BACKSPACE;	pTable->table[DIK_BACK][1]=INPUTMAN_TEXT_CODE_BACKSPACE;
	pTable->table[DIK_SPACE][0]=' ';							pTable->table[DIK_SPACE][1]=' ';
	pTable->table[DIK_TAB][0]=INPUTMAN_TEXT_CODE_TAB;			pTable->table[DIK_TAB][1]=INPUTMAN_TEXT_CODE_SHIFTTAB;
	pTable->table[DIK_RETURN][0]=INPUTMAN_TEXT_CODE_ENTER;		pTable->table[DIK_RETURN][1]=INPUTMAN_TEXT_CODE_ENTER;

	// alphabet
	pTable->table[DIK_A][0]='a';		pTable->table[DIK_A][1]='A';
	pTable->table[DIK_B][0]='b';		pTable->table[DIK_B][1]='B';
	pTable->table[DIK_C][0]='c';		pTable->table[DIK_C][1]='C';
	pTable->table[DIK_D][0]='d';		pTable->table[DIK_D][1]='D';
	pTable->table[DIK_E][0]='e';		pTable->table[DIK_E][1]='E';
	pTable->table[DIK_F][0]='f';		pTable->table[DIK_F][1]='F';
	pTable->table[DIK_G][0]='g';		pTable->table[DIK_G][1]='G';
	pTable->table[DIK_H][0]='h';		pTable->table[DIK_H][1]='H';
	pTable->table[DIK_I][0]='i';		pTable->table[DIK_I][1]='I';
	pTable->table[DIK_J][0]='j';		pTable->table[DIK_J][1]='J';
	pTable->table[DIK_K][0]='k';		pTable->table[DIK_K][1]='K';
	pTable->table[DIK_L][0]='l';		pTable->table[DIK_L][1]='L';
	pTable->table[DIK_M][0]='m';		pTable->table[DIK_M][1]='M';
	pTable->table[DIK_N][0]='n';		pTable->table[DIK_N][1]='N';
	pTable->table[DIK_O][0]='o';		pTable->table[DIK_O][1]='O';
	pTable->table[DIK_P][0]='p';		pTable->table[DIK_P][1]='P';
	pTable->table[DIK_Q][0]='q';		pTable->table[DIK_Q][1]='Q';
	pTable->table[DIK_R][0]='r';		pTable->table[DIK_R][1]='R';
	pTable->table[DIK_S][0]='s';		pTable->table[DIK_S][1]='S';
	pTable->table[DIK_T][0]='t';		pTable->table[DIK_T][1]='T';
	pTable->table[DIK_U][0]='u';		pTable->table[DIK_U][1]='U';
	pTable->table[DIK_V][0]='v';		pTable->table[DIK_V][1]='V';
	pTable->table[DIK_W][0]='w';		pTable->table[DIK_W][1]='W';
	pTable->table[DIK_X][0]='x';		pTable->table[DIK_X][1]='X';
	pTable->table[DIK_Y][0]='y';		pTable->table[DIK_Y][1]='Y';
	pTable->table[DIK_Z][0]='z';		pTable->table[DIK_Z][1]='Z';

	//numbers
	pTable->table[DIK_0][0]='0';		pTable->table[DIK_0][1]=')';
	pTable->table[DIK_1][0]='1';		pTable->table[DIK_1][1]='!';
	pTable->table[DIK_2][0]='2';		pTable->table[DIK_2][1]='@';
	pTable->table[DIK_3][0]='3';		pTable->table[DIK_3][1]='#';
	pTable->table[DIK_4][0]='4';		pTable->table[DIK_4][1]='$';
	pTable->table[DIK_5][0]='5';		pTable->table[DIK_5][1]='%';
	pTable->table[DIK_6][0]='6';		pTable->table[DIK_6][1]='^';
	pTable->table[DIK_7][0]='7';		pTable->table[DIK_7][1]='&';
	pTable->table[DIK_8][0]='8';		pTable->table[DIK_8][1]='*';
	pTable->table[DIK_9][0]='9';		pTable->table[DIK_9][1]='(';
	pTable->table[DIK_NUMPAD0][0]='0';		pTable->table[DIK_NUMPAD0][1]=INPUTMAN_TEXT_CODE_INSERT;
	pTable->table[DIK_NUMPAD1][0]='1';		pTable->table[DIK_NUMPAD1][1]=INPUTMAN_TEXT_CODE_END;
	pTable->table[DIK_NUMPAD2][0]='2';		pTable->table[DIK_NUMPAD2][1]=INPUTMAN_TEXT_CODE_DOWN;
	pTable->table[DIK_NUMPAD3][0]='3';		pTable->table[DIK_NUMPAD3][1]=INPUTMAN_TEXT_CODE_PAGEDOWN;
	pTable->table[DIK_NUMPAD4][0]='4';		pTable->table[DIK_NUMPAD4][1]=INPUTMAN_TEXT_CODE_LEFT;
	pTable->table[DIK_NUMPAD5][0]='5';		pTable->table[DIK_NUMPAD5][1]='5';
	pTable->table[DIK_NUMPAD6][0]='6';		pTable->table[DIK_NUMPAD6][1]=INPUTMAN_TEXT_CODE_RIGHT;
	pTable->table[DIK_NUMPAD7][0]='7';		pTable->table[DIK_NUMPAD7][1]=INPUTMAN_TEXT_CODE_HOME;
	pTable->table[DIK_NUMPAD8][0]='8';		pTable->table[DIK_NUMPAD8][1]=INPUTMAN_TEXT_CODE_UP;
	pTable->table[DIK_NUMPAD9][0]='9';		pTable->table[DIK_NUMPAD9][1]=INPUTMAN_TEXT_CODE_PAGEUP;

	//symbols
	pTable->table[DIK_GRAVE][0]='`';		pTable->table[DIK_GRAVE][1]='~';
	pTable->table[DIK_MINUS][0]='-';		pTable->table[DIK_MINUS][1]='_';
	pTable->table[DIK_EQUALS][0]='=';	pTable->table[DIK_EQUALS][1]='+';
	pTable->table[DIK_LBRACKET][0]='[';	pTable->table[DIK_LBRACKET][1]='{';
	pTable->table[DIK_RBRACKET][0]=']';	pTable->table[DIK_RBRACKET][1]='}';
	pTable->table[DIK_BACKSLASH][0]='\\';pTable->table[DIK_BACKSLASH][1]='|';
	pTable->table[DIK_SEMICOLON][0]=';';	pTable->table[DIK_SEMICOLON][1]=':';
	pTable->table[DIK_APOSTROPHE][0]='\'';pTable->table[DIK_APOSTROPHE][1]='"';
	pTable->table[DIK_COMMA][0]=',';		pTable->table[DIK_COMMA][1]='<';
	pTable->table[DIK_PERIOD][0]='.';	pTable->table[DIK_PERIOD][1]='>';
	pTable->table[DIK_SLASH][0]='/';		pTable->table[DIK_SLASH][1]='?';

	pTable->table[DIK_DIVIDE][0]='/';	pTable->table[DIK_DIVIDE][1]='/';
	pTable->table[DIK_MULTIPLY][0]='*';	pTable->table[DIK_MULTIPLY][1]='*';
	pTable->table[DIK_SUBTRACT][0]='-';	pTable->table[DIK_SUBTRACT][1]='-';
	pTable->table[DIK_ADD][0]='+';		pTable->table[DIK_ADD][1]='+';
	pTable->table[DIK_DECIMAL][0]='.';	pTable->table[DIK_DECIMAL][1]=INPUTMAN_TEXT_CODE_DELETE;
	pTable->table[DIK_NUMPADENTER][0] =	pTable->table[DIK_NUMPADENTER][1]=INPUTMAN_TEXT_CODE_ENTER;

	/// COMMANDS
	pTable->table[DIK_INSERT][0]=INPUTMAN_TEXT_CODE_INSERT;	pTable->table[DIK_INSERT][1]=INPUTMAN_TEXT_CODE_INSERT;
	pTable->table[DIK_HOME][0]=INPUTMAN_TEXT_CODE_HOME;		pTable->table[DIK_HOME][1]=INPUTMAN_TEXT_CODE_HOME;
	pTable->table[DIK_PRIOR][0]=INPUTMAN_TEXT_CODE_PAGEUP;	pTable->table[DIK_PRIOR][1]=INPUTMAN_TEXT_CODE_PAGEUP;
	pTable->table[DIK_DELETE][0]=INPUTMAN_TEXT_CODE_DELETE;	pTable->table[DIK_DELETE][1]=INPUTMAN_TEXT_CODE_DELETE;
	pTable->table[DIK_END][0]=INPUTMAN_TEXT_CODE_END;		pTable->table[DIK_END][1]=INPUTMAN_TEXT_CODE_END;
	pTable->table[DIK_NEXT][0]=INPUTMAN_TEXT_CODE_PAGEDOWN;	pTable->table[DIK_NEXT][1]=INPUTMAN_TEXT_CODE_PAGEDOWN;
	pTable->table[DIK_UP][0]=INPUTMAN_TEXT_CODE_UP;			pTable->table[DIK_UP][1]=INPUTMAN_TEXT_CODE_UP;
	pTable->table[DIK_DOWN][0]=INPUTMAN_TEXT_CODE_DOWN;		pTable->table[DIK_DOWN][1]=INPUTMAN_TEXT_CODE_DOWN;
	pTable->table[DIK_RIGHT][0]=INPUTMAN_TEXT_CODE_RIGHT;	pTable->table[DIK_RIGHT][1]=INPUTMAN_TEXT_CODE_RIGHT;
	pTable->table[DIK_LEFT][0]=INPUTMAN_TEXT_CODE_LEFT;		pTable->table[DIK_LEFT][1]=INPUTMAN_TEXT_CODE_LEFT;
	pTable->table[DIK_ESCAPE][0]=INPUTMAN_TEXT_CODE_ESCAPE;	pTable->table[DIK_ESCAPE][1]=INPUTMAN_TEXT_CODE_ESCAPE;
	// LOCKS ANS SYSTEM COMMANDS
	pTable->table[DIK_CAPSLOCK][0]=INPUTMAN_TEXT_CODE_CAPSLOCK;	pTable->table[DIK_CAPSLOCK][1]=INPUTMAN_TEXT_CODE_CAPSLOCK;
	pTable->table[DIK_SCROLL][0]=INPUTMAN_TEXT_CODE_SCROLLLOCK;	pTable->table[DIK_SCROLL][1]=INPUTMAN_TEXT_CODE_SCROLLLOCK;
	pTable->table[DIK_NUMLOCK][0]=INPUTMAN_TEXT_CODE_NUMLOCK;	pTable->table[DIK_NUMLOCK][1]=INPUTMAN_TEXT_CODE_NUMLOCK;
	pTable->table[DIK_SYSRQ][0]=INPUTMAN_TEXT_CODE_SYSRQ;		pTable->table[DIK_SYSRQ][1]=INPUTMAN_TEXT_CODE_SYSRQ;
	pTable->table[DIK_PAUSE][0]=INPUTMAN_TEXT_CODE_PAUSE;		pTable->table[DIK_PAUSE][1]=INPUTMAN_TEXT_CODE_PAUSE;
	// FUNCTION KEYS
	pTable->table[DIK_F1][0]=INPUTMAN_TEXT_CODE_F1;		pTable->table[DIK_F1][1]=INPUTMAN_TEXT_CODE_F1;
	pTable->table[DIK_F2][0]=INPUTMAN_TEXT_CODE_F2;		pTable->table[DIK_F2][1]=INPUTMAN_TEXT_CODE_F2;
	pTable->table[DIK_F3][0]=INPUTMAN_TEXT_CODE_F3;		pTable->table[DIK_F3][1]=INPUTMAN_TEXT_CODE_F3;
	pTable->table[DIK_F4][0]=INPUTMAN_TEXT_CODE_F4;		pTable->table[DIK_F4][1]=INPUTMAN_TEXT_CODE_F4;
	pTable->table[DIK_F5][0]=INPUTMAN_TEXT_CODE_F5;		pTable->table[DIK_F5][1]=INPUTMAN_TEXT_CODE_F5;
	pTable->table[DIK_F6][0]=INPUTMAN_TEXT_CODE_F6;		pTable->table[DIK_F6][1]=INPUTMAN_TEXT_CODE_F6;
	pTable->table[DIK_F7][0]=INPUTMAN_TEXT_CODE_F7;		pTable->table[DIK_F7][1]=INPUTMAN_TEXT_CODE_F7;
	pTable->table[DIK_F8][0]=INPUTMAN_TEXT_CODE_F8;		pTable->table[DIK_F8][1]=INPUTMAN_TEXT_CODE_F8;
	pTable->table[DIK_F9][0]=INPUTMAN_TEXT_CODE_F9;		pTable->table[DIK_F9][1]=INPUTMAN_TEXT_CODE_F9;
	pTable->table[DIK_F10][0]=INPUTMAN_TEXT_CODE_F10;	pTable->table[DIK_F10][1]=INPUTMAN_TEXT_CODE_F10;
	pTable->table[DIK_F11][0]=INPUTMAN_TEXT_CODE_F11;	pTable->table[DIK_F11][1]=INPUTMAN_TEXT_CODE_F11;
	pTable->table[DIK_F12][0]=INPUTMAN_TEXT_CODE_F12;	pTable->table[DIK_F12][1]=INPUTMAN_TEXT_CODE_F12;
	// SHIFT ALT CTRL KEYS
	pTable->table[DIK_LSHIFT][0]=INPUTMAN_TEXT_CODE_LSHIFT;		pTable->table[DIK_LSHIFT][1]=INPUTMAN_TEXT_CODE_LSHIFT;
	pTable->table[DIK_RSHIFT][0]=INPUTMAN_TEXT_CODE_RSHIFT;		pTable->table[DIK_RSHIFT][1]=INPUTMAN_TEXT_CODE_RSHIFT;
	pTable->table[DIK_LALT][0]=INPUTMAN_TEXT_CODE_LALT;			pTable->table[DIK_LALT][1]=INPUTMAN_TEXT_CODE_LALT;
	pTable->table[DIK_RALT][0]=INPUTMAN_TEXT_CODE_RALT;			pTable->table[DIK_RALT][1]=INPUTMAN_TEXT_CODE_RALT;
	pTable->table[DIK_LCONTROL][0]=INPUTMAN_TEXT_CODE_LCTRL;	pTable->table[DIK_LCONTROL][1]=INPUTMAN_TEXT_CODE_LCTRL;
	pTable->table[DIK_RCONTROL][0]=INPUTMAN_TEXT_CODE_RCTRL;	pTable->table[DIK_RCONTROL][1]=INPUTMAN_TEXT_CODE_RCTRL;
}

/*****************************************************************************

  Initializes Direct Input 8 wrapper for use in CINPUTMAN.

*****************************************************************************/
void *DI8Initialize(CINPUTMAN *pInput,GLWINAPI *pWindow)
{
	// ****Verify Input****
	if ((!pWindow)||(!pInput))
		return NULL; // invalid input
	// ****Allocate Ram****
	DI8InputData_t *pDI8Input = NULL;
	pDI8Input = new DI8InputData_t;
	if (!pDI8Input)
		return NULL; // new failed
	// ****Set Vars****
	memset(pDI8Input,NULL,sizeof(DI8InputData_t)); // nullify everything first
	pDI8Input->pInput		= pInput;
	pDI8Input->pWindow		= pWindow;
	// ****Add as GLWINAPI user****
	pDI8Input->pWindow->CreateUser(pDI8Input->gwaUserID,DI8Acquire,DI8UnAcquire,pDI8Input);
	if (pDI8Input->gwaUserID==-1)
	{// failed to create CGLWINAPI user
		delete pDI8Input;
		return NULL;
	}
	// ****create the DI object****
	CGWAWIN windowDataCpy;
	pDI8Input->pWindow->GetWindow(&windowDataCpy);
	if (FAILED(DirectInput8Create(windowDataCpy.m_hInstance,
								DIRECTINPUT_VERSION,
								IID_IDirectInput8,
								(void **)&pDI8Input->pDI,
								NULL)))
	{// failed to create the DI object
		delete pDI8Input;
		return NULL;
	}
	return pDI8Input;
}

/*****************************************************************************

  Acquires all present devices.

*****************************************************************************/
void DI8Acquire(void *data)
{
	if (data==NULL)
		return; // reject null data
	// get pointer
	DI8InputData_t *pDI8Input = (DI8InputData_t *)data;
	// acquire all devices
	register int count = pDI8Input->nDevices;
	while (count--)
		if ((pDI8Input->pDevices+count)->pDevice) // only acquire present devices
			(pDI8Input->pDevices+count)->pDevice->m_pDIDev->Acquire();
}

/*****************************************************************************

  Unacquires all present devices.

*****************************************************************************/
void DI8UnAcquire(void *data)
{
	if (data==NULL)
		return; // reject null data
	// get pointer
	DI8InputData_t *pDI8Input = (DI8InputData_t *)data;
	// unacquire all devices
	register int count = pDI8Input->nDevices;
	while (count--)
		if ((pDI8Input->pDevices+count)->pDevice) // only unacquire present devices
			(pDI8Input->pDevices+count)->pDevice->m_pDIDev->Unacquire();
}

/*****************************************************************************

  Cleans up memory usage of Direct Input 8 wrapper.

*****************************************************************************/
void DI8Kill(void *data)
{
	if (data==NULL)
		return; // reject null data
	// get pointer
	DI8InputData_t *pDI8Input = (DI8InputData_t *)data;
	// finalize as a GLWINAPI user
	pDI8Input->pWindow->FinalizeUser(pDI8Input->gwaUserID);
	// free device list data
	if (pDI8Input->pDevices)
		free(pDI8Input->pDevices);
	// clear struct
	memset(pDI8Input,NULL,sizeof(DI8InputData_t));
	// delete object
	delete pDI8Input;
}

/*****************************************************************************

  K E Y B O A R D.

*****************************************************************************/

/*****************************************************************************

  Adds Key Board to Direct Input 8 wrapper implementation

*****************************************************************************/
bool DI8AddKeyBoard(void *data)
{
	if (!data)
		return false; // invalid input
	// ****Do direct input 8 keyboard initialization****
	// create keyboard struct
	DI8IDevice_t *pKeyBoard = NULL;
	pKeyBoard = new DI8IDevice_t;
	if (!pKeyBoard)
		return false; // new failed
	pKeyBoard->m_pInput = (DI8InputData_t *)data; // copy data
	// initialize direct input 8 keyboard
	if (FAILED(pKeyBoard->m_pInput->pDI->CreateDevice(GUID_SysKeyboard, &pKeyBoard->m_pDIDev, NULL)))
	{
		delete pKeyBoard; // release allocated ram
		return false; // return false
	}
	if (FAILED(pKeyBoard->m_pDIDev->SetDataFormat(&c_dfDIKeyboard)))
	{
		delete pKeyBoard; // release allocated ram
		return false; // return false
	}
	CGWAWIN windowDataCpy;
	pKeyBoard->m_pInput->pWindow->GetWindow(&windowDataCpy);
	if (FAILED(pKeyBoard->m_pDIDev->SetCooperativeLevel(windowDataCpy.m_hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		delete pKeyBoard; // release allocated ram
		return false; // return false
	}

	if (FAILED(pKeyBoard->m_pDIDev->Acquire()))
	{
		delete pKeyBoard; // release allocated ram
		return false; // return false
	}
	// ****Attempt to register device in InputMan****
	int deviceID = pKeyBoard->m_pInput->pInput->RegisterDevice(DI8INPUT_KEYBOARD,256,0,NULL,
		pKeyBoard,DI8GetKeyBoardInput,DI8RemoveKeyBoard);
	if (deviceID<0)
	{
		delete pKeyBoard; // release allocated ram
		return false; // return false
	}
	pKeyBoard->m_pInput->pInput->SetAsSysKeyBoard(deviceID,DIK_CAPSLOCK,DIK_LSHIFT,DIK_RSHIFT,
		DIK_LCONTROL,DIK_RCONTROL,DIK_LALT,DIK_RALT); // sets as SysKeyBoard
	CASCIICONTBLE cTable; // create table
	DI8GetAsciiConTable(&cTable); // set table
	pKeyBoard->m_pInput->pInput->SetAsciiConverion(&cTable); // send table to CINPUTMAN
	pKeyBoard->m_di8inputID = pKeyBoard->m_pInput->AddDevice(pKeyBoard); // add device to di8input's device list
	// check returned id
	if (pKeyBoard->m_di8inputID<0)
	{
		delete pKeyBoard; // release allocated ram
		return false; // return false
	}	
	return true; // Success!
}

/*****************************************************************************

  Grabs input from direct input and sends it to CINPUTDEVICE.

*****************************************************************************/
void DI8GetKeyBoardInput(CINPUTDEVICE *pDevIn,void *data)
{
	DI8IDevice_t *pKeyBoard = (DI8IDevice_t *)data; // convert data
	// ****Grab Key Data in char[256] format***
	char keys[256] = {0};
	if (FAILED(pKeyBoard->m_pDIDev->GetDeviceState(sizeof(keys), (LPVOID)keys)))
	{
	    if (FAILED(pKeyBoard->m_pDIDev->Acquire()))
			return;
		if (FAILED(pKeyBoard->m_pDIDev->GetDeviceState(sizeof(keys), (LPVOID)keys)))
			return;
	}
	// ****Convert Key Data***
	register int i = 256;
	while (i--)
		pDevIn->m_onKeys.Set(i,(keys[i] & 0x80) ? true : false);
}

/*****************************************************************************

  Removes Key Board from Direct Input 8 wrapper implementation

*****************************************************************************/
void DI8RemoveKeyBoard(void *data)
{
	if (data==NULL)
		return;
	DI8IDevice_t *pKeyBoard = (DI8IDevice_t *)data; // convert data
	// release direct input 8 device
	if (pKeyBoard->m_pDIDev)
	{
		pKeyBoard->m_pDIDev->Unacquire();
		pKeyBoard->m_pDIDev->Release();
	}
	// removed direct input 8 device from device list
	pKeyBoard->m_pInput->RemoveDevice(pKeyBoard->m_di8inputID);
	// free allocated ram
	delete pKeyBoard;
}

/*****************************************************************************

  M O U S E.

*****************************************************************************/

/*****************************************************************************

  Adds Mouse to Direct Input 8 wrapper implementation

*****************************************************************************/
bool DI8AddMouse(void *data,bool isExclusive)
{
	if (!data)
		return false; // invalid input
	// ****Do direct input 8 mouse initialization****
	// create mouse struct
	DI8IDevice_t *pMouse = NULL;
	pMouse = new DI8IDevice_t;
	if (!pMouse)
		return false; // new failed
	pMouse->m_pInput = (DI8InputData_t *)data; // copy data
	// initialize direct input 8 mouse
	if (FAILED(pMouse->m_pInput->pDI->CreateDevice(GUID_SysMouse, &pMouse->m_pDIDev, NULL)))
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	if (FAILED(pMouse->m_pDIDev->SetDataFormat(&c_dfDIMouse2)))
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	DWORD flags;
	if (isExclusive)
		flags = DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY;
	else
		flags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

	CGWAWIN windowDataCpy;
	pMouse->m_pInput->pWindow->GetWindow(&windowDataCpy);
	if (FAILED(pMouse->m_pDIDev->SetCooperativeLevel(windowDataCpy.m_hwnd, flags)))
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	if (FAILED(pMouse->m_pDIDev->Acquire()))
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	DIMOUSESTATE2 state;
	if (FAILED(pMouse->m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE2), &state)))
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	// ****Attempt to register device in InputMan****
	// x and y min are 0 and their maxes are the respective screen resolutions
	
	CGWASCRN scrnDataCpy;
	pMouse->m_pInput->pWindow->GetScreen(&scrnDataCpy);
	CINPUTVAR xyAxis[2]; // x,y
	xyAxis[DI8INPUT_MOUSEX].Init(0,scrnDataCpy.m_width-1,scrnDataCpy.m_width/2);
	xyAxis[DI8INPUT_MOUSEY].Init(0,scrnDataCpy.m_height-1,scrnDataCpy.m_height/2);
	// Register device in CINPUTMAN
	int deviceID = pMouse->m_pInput->pInput->RegisterDevice(DI8INPUT_MOUSE,DI8INPUT_INITMSKEYS,
		2,xyAxis,pMouse,DI8GetMouseInput,DI8RemoveMouse);
	if (deviceID<0)
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	pMouse->m_di8inputID = pMouse->m_pInput->AddDevice(pMouse); // add device to di8input's device list
	// check returned id
	if (pMouse->m_di8inputID<0)
	{
		delete pMouse; // release allocated ram
		return false; // return false
	}
	return true; // Success!
}

/*****************************************************************************

  Grabs input from direct input and sends it to CINPUTDEVICE.

*****************************************************************************/
void DI8GetMouseInput(CINPUTDEVICE *pDevIn,void *data)
{
	DI8IDevice_t *pMouse = (DI8IDevice_t *)data; // convert data
	// ****Grab Mouse Data in DIMOUSESTATE2 format***
	DIMOUSESTATE2 state;
	if (FAILED(pMouse->m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE2), &state)))
	{
		if (FAILED(pMouse->m_pDIDev->Acquire()))
			return;
		if (FAILED(pMouse->m_pDIDev->GetDeviceState(sizeof(DIMOUSESTATE2), &state)))
			return;
	}
	// ****Convert Mouse Data***
	// buttons
	register int i = DI8INPUT_NMOUSEKEYS;
	while (i--)
		pDevIn->m_onKeys.Set(i,(state.rgbButtons[i] & 0x80) ? true : false);
	// x y axises
	pDevIn->m_pInputStates[DI8INPUT_MOUSEX].m_value+=state.lX; // x axis
	pDevIn->m_pInputStates[DI8INPUT_MOUSEY].m_value+=state.lY; // y axis
	// ensure max x and y are set to current screen resolution
	unsigned char flags = NULL;
	pMouse->m_pInput->pWindow->UpdateUser(pMouse->m_pInput->gwaUserID,flags);
	if (flags)
	{ // only update the screen resolution if it was changed
		CGWASCRN scrnDataCpy;
		pMouse->m_pInput->pWindow->GetScreen(&scrnDataCpy);
		pDevIn->m_pInputStates[DI8INPUT_MOUSEX].m_max=scrnDataCpy.m_width-1;
		pDevIn->m_pInputStates[DI8INPUT_MOUSEY].m_max=scrnDataCpy.m_height-1;
	}
	// set extended buttons
	// scroll wheel
	pDevIn->m_onKeys.Set(DI8INPUT_SCRLWHLDWN,(state.lZ<0)); // down
	pDevIn->m_onKeys.Set(DI8INPUT_SCRLWHLUP,(state.lZ>0)); // up
	// mouse directional values
	pDevIn->m_onKeys.Set(DI8INPUT_MOUSEUP,(state.lY<0)); // up
	pDevIn->m_onKeys.Set(DI8INPUT_MOUSEDOWN,(state.lY>0)); // down
	pDevIn->m_onKeys.Set(DI8INPUT_MOUSELEFT,(state.lX<0)); // left
	pDevIn->m_onKeys.Set(DI8INPUT_MOUSERIGHT,(state.lX>0)); // right

	// set x and y values
	i = 2;
	while (i--)
	{ // clip x and y to respective limits
		if (pDevIn->m_pInputStates[i].m_value > pDevIn->m_pInputStates[i].m_max)
			pDevIn->m_pInputStates[i].m_value = pDevIn->m_pInputStates[i].m_max;
		else if (pDevIn->m_pInputStates[i].m_value < pDevIn->m_pInputStates[i].m_min)
			pDevIn->m_pInputStates[i].m_value = pDevIn->m_pInputStates[i].m_min;
	}
}

/*****************************************************************************

  Removes Mouse from Direct Input 8 wrapper implementation

*****************************************************************************/
void DI8RemoveMouse(void *data)
{
	if (data==NULL)
		return;
	DI8IDevice_t *pMouse = (DI8IDevice_t *)data; // convert data
	// release direct input 8 device
	if (pMouse->m_pDIDev)
	{
		pMouse->m_pDIDev->Unacquire();
		pMouse->m_pDIDev->Release();
	}
	// removed direct input 8 device from device list
	pMouse->m_pInput->RemoveDevice(pMouse->m_di8inputID);
	// free allocated ram
	delete pMouse;
}