/*****************************************************************************

  Input Manager 2.0
  inputman.h

  DATE		:	11/28/2003
  AUTHOR	:	Bernard Igiri

  System for managing input.
  Provides:
  1) Support for device independant buttons for use in input
  2) A layer of complete abstraction from the method of recieving input
	  be it Direct Input 8 or any other means.
  3) A means for recieving text input.

*****************************************************************************/
#ifndef __INCLUDED_INPUTMAN_H__
#define __INCLUDED_INPUTMAN_H__

#include "stringUtilities.h"

// Key Buffer Size
#define INPUTMAN_BUFFERSIZE			20
// Text System Defines
#define INPUTMAN_TEXT_REPEATE_DELAY	0.5f
#define INPUTMAN_TEXT_REPEATE_RATE	0.1f
#define INPUTMAN_TEXT_MAXTIME		100.0f
// Button Option Defines
#define INPUTMAN_BOPT_ISKEY			0
#define INPUTMAN_BOPT_TGLSTATE		1
#define INPUTMAN_BOPT_TERMNTSTATE	2
// Text Option Defines
#define INPUTMAN_TOPT_ISREPEAT		0
#define INPUTMAN_TOPT_ISCAPS		1
// Special Keys Defines
#define INPUTMAN_TSKEY_ISSHIFT		0
#define INPUTMAN_TSKEY_ISCTRL		1
#define INPUTMAN_TSKEY_ISALT		2

// SPECIAL KEY DEFINES
#define INPUTMAN_TEXT_CODE_BACKSPACE 8
#define INPUTMAN_TEXT_CODE_TAB		9
#define INPUTMAN_TEXT_CODE_SHIFTTAB	7
#define INPUTMAN_TEXT_CODE_ENTER	10
#define INPUTMAN_TEXT_CODE_ESCAPE	27
#define INPUTMAN_TEXT_CODE_INSERT	1
#define INPUTMAN_TEXT_CODE_HOME		2
#define INPUTMAN_TEXT_CODE_PAGEUP	3
#define INPUTMAN_TEXT_CODE_DELETE	4
#define INPUTMAN_TEXT_CODE_END		5
#define INPUTMAN_TEXT_CODE_PAGEDOWN	6

#define INPUTMAN_TEXT_CODE_UP		24
#define INPUTMAN_TEXT_CODE_DOWN		25
#define INPUTMAN_TEXT_CODE_RIGHT	26
#define INPUTMAN_TEXT_CODE_LEFT		28

#define INPUTMAN_TEXT_CODE_F1		128
#define INPUTMAN_TEXT_CODE_F2		129
#define INPUTMAN_TEXT_CODE_F3		130
#define INPUTMAN_TEXT_CODE_F4		131
#define INPUTMAN_TEXT_CODE_F5		132
#define INPUTMAN_TEXT_CODE_F6		133
#define INPUTMAN_TEXT_CODE_F7		134
#define INPUTMAN_TEXT_CODE_F8		135
#define INPUTMAN_TEXT_CODE_F9		136
#define INPUTMAN_TEXT_CODE_F10		137
#define INPUTMAN_TEXT_CODE_F11		138
#define INPUTMAN_TEXT_CODE_F12		139

#define INPUTMAN_TEXT_CODE_CAPSLOCK		140
#define INPUTMAN_TEXT_CODE_SCROLLLOCK	141
#define INPUTMAN_TEXT_CODE_NUMLOCK		142
#define INPUTMAN_TEXT_CODE_SYSRQ		143
#define INPUTMAN_TEXT_CODE_PAUSE		144

#define INPUTMAN_TEXT_CODE_LSHIFT		145
#define INPUTMAN_TEXT_CODE_RSHIFT		146
#define INPUTMAN_TEXT_CODE_LALT			147
#define INPUTMAN_TEXT_CODE_RALT			148
#define INPUTMAN_TEXT_CODE_LCTRL		149
#define INPUTMAN_TEXT_CODE_RCTRL		150

class CASCIICONTBLE
{
public:
	CASCIICONTBLE();
	~CASCIICONTBLE();
	unsigned char   table[256][2];
	void Clear();
	unsigned char Convert(unsigned char c,bool capsLock,bool shiftKey);
	void CopyTable(const CASCIICONTBLE *pConTbl);
};

class CINPUTVAR
{
public:
	CINPUTVAR();
	~CINPUTVAR();

	void Init(int min,int max,int value);
	int m_min; // min input bound
	int m_max; // max input bound
	int m_value; // input value
};

class CINPUTDEVICE
{
public:
	CINPUTDEVICE();
	~CINPUTDEVICE();

	void Update();

	char*		m_pName; // device name lable
	int			m_nKeys; // number of keys
	BITSTRING_t m_onKeys; // list of key states
	int			m_nInputs; // number of inputs
	CINPUTVAR*	m_pInputStates; // list of input states
	void*		m_pData; // pointer to data for device operation
	void		(*m_pGetInput)(CINPUTDEVICE *pDevIn,void *pData); // pointer to input updating function
	void		(*m_pKill)(void *pData); // pointer to cleanup function

	void Kill();
	void Clear();
};

class CBUTTON
{
public:
	CBUTTON();
	~CBUTTON();

	char*			m_pName; // Button Name
	CINPUTDEVICE*	m_pDevice; // pointer to device for button
	int				m_id; // index to button data within device class
	int				m_value; // cache for value since last Update(float) call
	int				m_oldValue; // cache for value since last GetTravel() call
	unsigned char	m_options; // {0-isKey,1-toggleState,2-teminateState}
	float			m_tActive; // # seconds this button has been active
	float			m_tInactive; // # seconds since last activity

	void Update(float elapsedTime);
	float GetTravel();
	float GetAnalog();
	bool GetInstant();
	bool GetToggle();
	bool GetTerminate();
	void ClearTerminate();
	void ClearToggle();
	void Kill();
	void Clear();
};

class CINPUTMAN
{
public:
	CINPUTMAN();
	~CINPUTMAN();

	CBUTTON* GetButton(int buttonID);
	char PeekKey();
	char GetKey();
	void PutKey(char c);
	unsigned char GetSKeys();
	void SetAsSysKeyBoard(int deviceID,int caps,int lshift,int rshift,
		int lctrl,int rctrl,int lalt,int ralt);
	int RegisterDevice(const char *name, int nKeys, int nInputs, CINPUTVAR *pInputs,
						void *pData,void(*pGetInput)(CINPUTDEVICE *pDevIn,void *pData),
						void(*pKill)(void *pData));
	int FindDevice(const char *name);
	int AddButton(const char *name);
	bool SetButton(int buttonID, int deviceID, int inputID, bool isKey);
	int FindButton(const char *name);
	void CalibrateDevice(int deviceID,int inputID,int min,int max);
	void Update();
	void Kill();
	bool Initialize();
	void SetAsciiConverion(const CASCIICONTBLE *pConTbl);

	float GetTmInactive();
private:
	void Clear();
	int				m_keyboard; // index of system keyboard in device list
	char			m_keyBuffer[INPUTMAN_BUFFERSIZE]; // text input buffer
	unsigned char	m_lastKey; // last key recieved
	float			m_keyDwnTm; // time that last key has been held
	unsigned char	m_textSKeys[7]; // special key codes - caps & l&r shift,ctrl,&alt
	unsigned char	m_textOpt; // 0-isRepeate,1-isCaps
	unsigned char	m_textSKeysOn; // 0-Shift,2-Ctrl,3-Alt
	CASCIICONTBLE   m_asciiConTbl; // char conversion table

	int				m_nDevices; // number of devices
	CINPUTDEVICE*	m_pDevices; // list of devices
	int				m_nButtons; // number of buttons
	CBUTTON*		m_pButtons; // list of buttons
	float			m_elapsedTime; // time elapsed at point of last update
	void*			m_pTimer; // pointer to timing function
};

#endif//__INCLUDED_INPUTMAN_H__