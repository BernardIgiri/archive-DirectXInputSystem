/*****************************************************************************

  CINPUTMAN OPERATION NOTES:

  1) Invalid devices are marked with m_pGetInput set to NULL.
  2) Invalid buttons are maked with m_pDevice set to NULL.
  3) Devices and buttons are marked invalid if their initialization fails.
  4) Setting m_nButtons to zero should prevent m_pButtons from being accessed
  5) Ditto for m_nDevices and m_pDevices.

*****************************************************************************/
// Includes
#include "inputman.h"
#include "stdlib.h"
#include "qpctimer.h"
// Defines
#define ISDEVICEVALID(x) ((x)->m_pGetInput!=NULL)
#define ISBUTTONVALID(x) ((x)->m_pDevice!=NULL)
#define ISIDXVALID(idx,max) ((idx>-1)&&(idx<max))

/*****************************************************************************

  CINPUTVAR class constructor

*****************************************************************************/
CINPUTVAR::CINPUTVAR()
{
	m_min=m_max=m_value=0;
}

/*****************************************************************************

  CINPUTVAR class destructor.

*****************************************************************************/
CINPUTVAR::~CINPUTVAR()
{
}

/*****************************************************************************

  Sets the initial values for an input var.

*****************************************************************************/
void CINPUTVAR::Init(int min,int max,int value)
{
	if (min>max) // deny invalid input, min & max are zero
		return;
	m_min	= min;
	m_max	= max;
	m_value = value;
}

/*****************************************************************************

  CINPUTDEVICE class constructor.

*****************************************************************************/
CINPUTDEVICE::CINPUTDEVICE()
{
	Clear();
}

/*****************************************************************************

  CINPUTDEVICE class deconstructor.

*****************************************************************************/
CINPUTDEVICE::~CINPUTDEVICE()
{
	Kill();
}

/*****************************************************************************

  Clears all data in CINPUTDEVICE. USE ONLY BEFORE INITIALIZATION.

*****************************************************************************/
void CINPUTDEVICE::Clear()
{
	m_onKeys.Clear();
	m_pName			= NULL;
	m_nKeys			= NULL;
	m_nInputs		= NULL;
	m_pInputStates	= NULL;
	m_pData			= NULL;
	m_pGetInput		= NULL;
	m_pKill			= NULL;
}

/*****************************************************************************

  Frees all ram allocated to CINPUTDEVICE.

*****************************************************************************/
void CINPUTDEVICE::Kill()
{
	// free ram
	m_onKeys.Kill();
	if (m_pInputStates)
		free(m_pInputStates);
	if (m_pName)
		free(m_pName);
	if (m_pKill) // call device kill function if present
		m_pKill(m_pData);
	// reset vars
	Clear();
}

/*****************************************************************************

  Updates CINPUTDEVICE data with the current input states.

*****************************************************************************/
void CINPUTDEVICE::Update()
{
	if (m_pGetInput)
		m_pGetInput(this,m_pData);
}

/*****************************************************************************

  CBUTTON class constructor.

*****************************************************************************/
CBUTTON::CBUTTON()
{
	Clear();
}

/*****************************************************************************

  CBUTTON class deconstructor.

*****************************************************************************/
CBUTTON::~CBUTTON()
{
	Kill();
}

/*****************************************************************************

  Clears all data in CBUTTON. USE ONLY BEFORE INITIALIZATION.

*****************************************************************************/
void CBUTTON::Clear()
{
	m_pName		= NULL;
	m_pDevice	= NULL;
	m_id		= NULL;
	m_value		= NULL;
	m_oldValue	= NULL;
	m_options	= NULL;
	m_tActive	= NULL;
	m_tInactive	= NULL;
}

/*****************************************************************************

  Frees all ram allocated to CBUTTON.

*****************************************************************************/
void CBUTTON::Kill()
{
	// free ram
	if (m_pName)
		free(m_pName);
	// reset vars
	Clear();
}

/*****************************************************************************

  Updates button status based on input device status.

*****************************************************************************/
void CBUTTON::Update(float elapsedTime)
{
	if (!m_pDevice) // if there is no device do nothing.
		return;
	// ****Get input, handle button toggle****
	bool isMoved = false;
	bool isOn = false;
	if (STRUTILCheckBit(INPUTMAN_BOPT_ISKEY,m_options)) // if this button is a key then
	{
		m_value = 0; // keep track of value for keys
		isOn = m_pDevice->m_onKeys.Check(m_id); // get key state Off/On
		if (isOn) // if on check if this is new
		{
			m_value = 1; // key value is 1 if on.
			isMoved = (!m_tActive); // then check isMoved and toggle, only keys can toggle
			m_options = STRUTILSetBit(INPUTMAN_BOPT_TGLSTATE,m_options,
					!STRUTILCheckBit(INPUTMAN_BOPT_TGLSTATE,m_options)); // toggle the toggle
		}
	}
	else // treat as scalar input
	{
		int newValue = (m_pDevice->m_pInputStates+m_id)->m_value; // get input state
		isOn=isMoved= (newValue != m_value); // if it moved, then it is on and it moved
		m_value = newValue;
	}
	// ****Update status according to input****
	// Movement
	if (isMoved) // if it moved then check terminate
		m_options = STRUTILSetBit(INPUTMAN_BOPT_TERMNTSTATE,m_options,true);
	// Activity
	if (isOn) // if on count active time and reset inactive time
	{
		m_tActive += elapsedTime;
		m_tInactive = 0.0f;
	}
	else // do opposite
	{
		m_tActive = 0.0f;
		m_tInactive += elapsedTime;
	}
}

/*****************************************************************************

  Returns a scaler of button travel since the last call to this function.
  The value is between -1.0f and 1.0f.  Positive numbers indicate forward
  motion, negative numbers indicate backwards motion.

*****************************************************************************/
float CBUTTON::GetTravel()
{
	if (!m_pDevice)
		return 0.0f; // if no device return 0
	float result = 0.0f;
	if (m_value != m_oldValue) // check if it moved
	{ // It Moved!
		if (STRUTILCheckBit(INPUTMAN_BOPT_ISKEY,m_options)) // if key treat as digital
			result = 1.0f; // moving keys result in 1.0f
		else // treat as scalar
		{
			if ((m_pDevice->m_pInputStates+m_id)->m_max==
				(m_pDevice->m_pInputStates+m_id)->m_min)
				result = 0.0f; // if min=max then result can only be 0.0f
			else
			{
				// if new > old, motion is positive, elsewise motion is negative
				result = ((float)(m_value - m_oldValue)) // distance moved, divided by
						 /((float)((m_pDevice->m_pInputStates+m_id)->m_max // maximum difference
						 - (m_pDevice->m_pInputStates+m_id)->m_min));
			}
		}
	}
	m_oldValue = m_value; // update m_oldValue to the new old value
	return result;
}

/*****************************************************************************

  Returns a normalized scalar value of the current button position.
  The result is between 0.0f and 1.0f.

*****************************************************************************/
float CBUTTON::GetAnalog()
{
	if (!m_pDevice)
		return 0.0f; // if no device return 0
	if (STRUTILCheckBit(INPUTMAN_BOPT_ISKEY,m_options)) // if key treat as digital
	{
		if (m_pDevice->m_onKeys.Check(m_id)) // if key is on
			return 1.0f;
		else
			return 0.0f;
	}
	// It's not a key! Treat as scalar.
	if ((m_pDevice->m_pInputStates+m_id)->m_max==
		(m_pDevice->m_pInputStates+m_id)->m_min)
		return 0.0f; // if min=max then result can only be 0.0f
	return ((float)m_value) // divide value by
			/((float)((m_pDevice->m_pInputStates+m_id)->m_max
			 - (m_pDevice->m_pInputStates+m_id)->m_min)); // maximum difference
}

/*****************************************************************************

  Returns instant state of button. Returns true if button has a value.

*****************************************************************************/
bool CBUTTON::GetInstant()
{
	if (!m_pDevice)
		return 0.0f; // if no device return 0
	if (STRUTILCheckBit(INPUTMAN_BOPT_ISKEY,m_options)) // if key treat as digital
		return m_pDevice->m_onKeys.Check(m_id); // return key state
	// It's not a key! Treat as scalar.
	return (m_value > (m_pDevice->m_pInputStates+m_id)->m_min); // return true if value > min
}

/*****************************************************************************

  Returns toggle status of button.

*****************************************************************************/
bool CBUTTON::GetToggle()
{
	return STRUTILCheckBit(INPUTMAN_BOPT_TGLSTATE,m_options);
}

/*****************************************************************************

  Returns terminate status of button.

*****************************************************************************/
bool CBUTTON::GetTerminate()
{
	return STRUTILCheckBit(INPUTMAN_BOPT_TERMNTSTATE,m_options);
}

/*****************************************************************************

  Sets terminate state to false.

*****************************************************************************/
void CBUTTON::ClearTerminate()
{
	m_options = STRUTILSetBit(INPUTMAN_BOPT_TERMNTSTATE,m_options,false);
}

/*****************************************************************************

  Sets toggle state to false.

*****************************************************************************/
void CBUTTON::ClearToggle()
{
	m_options = STRUTILSetBit(INPUTMAN_BOPT_TGLSTATE,m_options,false);
}

/*****************************************************************************

  CASCIICONTBLE class constructor.

*****************************************************************************/
CASCIICONTBLE::CASCIICONTBLE()
{
	Clear();
}

/*****************************************************************************

  CASCIICONTBLE class deconstructor.

*****************************************************************************/
CASCIICONTBLE::~CASCIICONTBLE()
{
}

/*****************************************************************************

  Clears all data within CASCIICONTBLE.

*****************************************************************************/
void CASCIICONTBLE::Clear()
{
	memset(this,NULL,sizeof(CASCIICONTBLE));
}

/*****************************************************************************

  Converts unsigned char c, using ascii conversion table.

*****************************************************************************/
unsigned char CASCIICONTBLE::Convert(unsigned char c,bool capsLock,bool shiftKey)
{
	if (shiftKey)
		shiftKey = true;
	unsigned char outC=table[c][0];
	int version=0;
	if ((capsLock||shiftKey) &&(outC<='z'&&outC>='a') && (!(capsLock&&shiftKey)))
		version=1;
	if ((!(outC<='z'&&outC>='a'))&&shiftKey)
		version=1;
	if (version&&(c!='*'))
		version=1;
	outC=table[c][version];
	if (outC==INPUTMAN_TEXT_CODE_F4)
		int hu=0;
	return outC;
}

/*****************************************************************************

  Copys pConTbl onto this table.

*****************************************************************************/
void CASCIICONTBLE::CopyTable(const CASCIICONTBLE *pConTbl)
{
	if (pConTbl)
		memcpy(this,pConTbl,sizeof(CASCIICONTBLE));
}

/*****************************************************************************

  CINPUTMAN class constructor.

*****************************************************************************/
CINPUTMAN::CINPUTMAN()
{
	Initialize();
}

/*****************************************************************************

  CINPUTMAN class deconstructor.

*****************************************************************************/
CINPUTMAN::~CINPUTMAN()
{
	Kill();
}

/*****************************************************************************

  Clears all data in CINPUTMAN. USE ONLY BEFORE INITIALIZATION.

*****************************************************************************/
void CINPUTMAN::Clear()
{
	m_keyboard		= -1; // invalid if less than 0
	memset(m_keyBuffer,NULL,INPUTMAN_BUFFERSIZE);
	m_asciiConTbl.Clear();
	m_lastKey		= NULL;
	m_keyDwnTm		= NULL;
	m_textOpt		= NULL;
	m_textSKeys[0]		= NULL;
	m_textSKeys[1]	= NULL;
	m_textSKeys[2]	= NULL;
	m_nDevices		= NULL;
	m_pDevices		= NULL;
	m_nButtons		= NULL;
	m_pButtons		= NULL;
	m_elapsedTime	= NULL;
	m_pTimer		= NULL;
}

/*****************************************************************************

  Allocates RAM for CINPUTMAN operation, this method is only necessary
  if the CINPUTMAN class constructor has been skipped, such as in cases
  where malloc is used to return a pointer.

*****************************************************************************/
bool CINPUTMAN::Initialize()
{
	Clear();
	m_pTimer = (void *)(new CQPCTimer); // create timer
	if (m_pTimer)
		if (!((CQPCTimer *)m_pTimer)->Init()) // initialize timer
		{ // Timer initialization failed
			delete ((CQPCTimer*)m_pTimer);
			m_pTimer = NULL; // on failure remove timer
			return false;
		}
	else // new failed
		return false;
	return true;
}

/*****************************************************************************

  Frees all ram allocated to CINPUTMAN.

*****************************************************************************/
void CINPUTMAN::Kill()
{
	// free ram
	if (m_pTimer)
		delete ((CQPCTimer*)m_pTimer);
	if (m_pDevices)
	{
		while(m_nDevices--)
			(m_pDevices + m_nDevices)->Kill(); // hit all kill switches
		free(m_pDevices);
	}
	if (m_pButtons)
	{
		while (m_nButtons--)
			(m_pButtons + m_nButtons)->Kill(); // hit all kill switches
		free(m_pButtons);
	}
	// reset vars
	Clear();
}

/*****************************************************************************

  Returns a pointer to the requested button, returns NULL on failure.

*****************************************************************************/
CBUTTON* CINPUTMAN::GetButton(int buttonID)
{
	if (ISIDXVALID(buttonID,m_nButtons)&&m_pButtons)
		return (m_pButtons + buttonID);
	return NULL;
}

/*****************************************************************************

  Returns the next key on the key buffer stack without removing it.

*****************************************************************************/
char CINPUTMAN::PeekKey()
{
	return m_keyBuffer[0];
}

/*****************************************************************************

  Returns next key in the key buffer stack.

*****************************************************************************/
char CINPUTMAN::GetKey()
{

	char key = m_keyBuffer[0]; // copy top key
	memcpy(m_keyBuffer,m_keyBuffer+1,INPUTMAN_BUFFERSIZE-1); // delete top key
	return key; // return copy
}

/*****************************************************************************

  Adds key to the end of the key buffer stack if there is room.

*****************************************************************************/
void CINPUTMAN::PutKey(char c)
{
	int len = strlen(m_keyBuffer);
	if ((len+1)>INPUTMAN_BUFFERSIZE)
		return;
	m_keyBuffer[len]=c;
	m_keyBuffer[len+1]=NULL;
}

/*****************************************************************************

  Returns the current on status of the shift, ctrl, and alt keys.

*****************************************************************************/
unsigned char CINPUTMAN::GetSKeys()
{
	return m_textSKeysOn;
}

/*****************************************************************************

  Sets device as the SysKeyBoard for use in text input.
  Also records the key codes of special keys for text input.

*****************************************************************************/
void CINPUTMAN::SetAsSysKeyBoard(int deviceID,int caps,int lshift,int rshift,
								 int lctrl,int rctrl,int lalt,int ralt)
{
	if (ISIDXVALID(deviceID,m_nDevices)&&m_pDevices) // if id is valid and there are devices
		if (ISDEVICEVALID(m_pDevices + deviceID)) // and the device is valid
			if (((m_pDevices + deviceID)->m_nKeys>255)&& // and the device has enough keys
				ISIDXVALID(lshift,256)&&ISIDXVALID(rshift,256)&&
				ISIDXVALID(caps,256)) // and caps, lshift, and rshift are valid
			{
				// then the device may be set as the SysKeyBoard
				m_keyboard = deviceID; // copy deviceID
				// copy key codes of caplock, lshift, and rshift
				m_textSKeys[0]	 = (unsigned char)caps;
				m_textSKeys[1] = (unsigned char)lshift;
				m_textSKeys[2] = (unsigned char)rshift;
				m_textSKeys[3] = (unsigned char)lctrl;
				m_textSKeys[4] = (unsigned char)rctrl;
				m_textSKeys[5] = (unsigned char)lalt;
				m_textSKeys[6] = (unsigned char)ralt;
			}
}

/*****************************************************************************

  Adds a new device to the device list, returns index of new device, or
  -1 on failure.

  Notes.
  name		- name lable for device
  nKeys		- # of physical keys on device
  nInputs	- # of physical scalar input sources on device
  pInputs	- pointer to CINPUTVAR default data for scalar input source.
			  This should be NULL if there are no scalar input sources.
  pData		- pointer to data for device operation, should not be NULL
  pGetInput	- pointer to function to collect input data from device.
			  This value must be defined!
  pKill		- pointer to kill function for device, this may be NULL

*****************************************************************************/
int CINPUTMAN::RegisterDevice(const char *name,int nKeys,int nInputs,CINPUTVAR *pInputs,
							   void *pData,void(*pGetInput)(CINPUTDEVICE *pDevIn,void *pData),
							   void(*pKill)(void *pData))
{
	if ( (name==NULL)||(nInputs&&(!pInputs))||((!nKeys)&&(!nInputs))||(pGetInput==NULL) )
		return -1; // invalid input
	// ****Allocate space for new device****
	if (m_nDevices==0)
	{ // if this is the first device then malloc space for new device
		m_pDevices = (CINPUTDEVICE *)malloc(sizeof(CINPUTDEVICE));
		if (m_pDevices==NULL)
		{
			m_nDevices=0; // m_pDevices no longer valid
			return -1; // malloc failure
		}
		m_nDevices++; // incr device counter
	}
	else
	{ // else realloc space for device
		m_nDevices++; // incr device counter
		m_pDevices = (CINPUTDEVICE *)realloc(m_pDevices,sizeof(CINPUTDEVICE)*m_nDevices);
		if (m_pDevices==NULL)
		{
			m_nDevices=0; // m_pDevices no longer valid
			return -1; // realloc failure
		}
	}
	// ****Enter data into new device****
	CINPUTDEVICE *newDevice = (m_pDevices + (m_nDevices-1));
	newDevice->Clear(); // clear all data in new device
	// NOTE: newDevice is currently invalid since newDevice.m_pGetInput = NULL
	// initialize space for onKeys if there are keys
	newDevice->m_pKill		= pKill; // must be set if present
	newDevice->m_pData		= pData;
	newDevice->m_nKeys		= nKeys;
	if (nKeys)
		if (!newDevice->m_onKeys.Init(nKeys))
			return -1; // onKeys.Init failed
	// malloc space for InputStates data if there are inputs
	newDevice->m_nInputs	= nInputs;
	newDevice->m_pInputStates= NULL;
	if (nInputs)
	{
		unsigned size = sizeof(CINPUTVAR)*nInputs;
		newDevice->m_pInputStates = (CINPUTVAR*)malloc(size);
		if (newDevice->m_pInputStates==NULL)
			return -1; // malloc failed
		memcpy(newDevice->m_pInputStates,pInputs,size); // copy user set defaults
	}
	// Copy name of device
	if (!STRUTILCopyStr(&newDevice->m_pName,name))
		return -1; // STRUTILCopStr failed  device has no name
	// This last step marks the device as valid
	newDevice->m_pGetInput	= pGetInput;
	return m_nDevices-1; // success
}

/*****************************************************************************

  Returns index of Device with given name. Returns -1 if not found.

*****************************************************************************/
int CINPUTMAN::FindDevice(const char *name)
{
	if (name==NULL)
		return -1; // invalid input
	if (m_pDevices&&m_nDevices)
		for (int i=0; i<m_nDevices; i++)
			if ( ISDEVICEVALID(m_pDevices + i) ) // skip all invalid devices
				if(strcmp(name,(m_pDevices + i)->m_pName)==NULL)
					return i;
	return -1;
}

/*****************************************************************************

  Adds a new button to CINPUTMAN, returns true index of new button, or
  returns -1 on failure.

*****************************************************************************/
int CINPUTMAN::AddButton(const char *name)
{
	if (name==NULL)
		return -1; // invalid input
	// ****Allocate space for new button****
	if (m_nButtons==0)
	{ // if this is the first button then malloc space for new button
		m_pButtons = (CBUTTON *)malloc(sizeof(CBUTTON));
		if (m_pButtons==NULL)
		{
			m_nButtons=0; // m_pButtons no longer valid
			return -1; // malloc failure
		}
		m_nButtons++; // incr button counter
	}
	else
	{ // else realloc space for buttons
		m_nButtons++; // incr button counter
		m_pButtons = (CBUTTON *)realloc(m_pButtons,sizeof(CBUTTON)*m_nButtons);
		if (m_pButtons==NULL)
		{
			m_nButtons=0; // m_pButtons no longer valid
			return -1; // realloc failure
		}
	}
	// ****Enter data into new button****
	CBUTTON *newButton = (m_pButtons + (m_nButtons-1));
	newButton->Clear(); // clear data in new button
	if (!STRUTILCopyStr(&newButton->m_pName,name))
		return -1;
	return m_nButtons -1;
}

/*****************************************************************************

  Sets up button to read input from device.  Returns true on success.

  buttonID - index of button, deviceID - index of device, inputID
  id of control on the input device, isKey - set to true if this button
  is being set to a non-scalar control on the device such as a
  keyboard key.

*****************************************************************************/
bool CINPUTMAN::SetButton(int buttonID,int deviceID,int inputID,bool isKey)
{
	// ****Verify valid input****
	if (!ISIDXVALID(buttonID,m_nButtons)||!ISIDXVALID(deviceID,m_nDevices))
		return false; // invalid buttonID or deviceID
	if (!ISDEVICEVALID(m_pDevices + deviceID))
		return false; // invalid device	
	// ****Type specific verify and set isKey option****
	if (isKey)
	{ // if key treat as key
		if (!ISIDXVALID(inputID,(m_pDevices + deviceID)->m_nKeys))
			return false; // invalid inputID
		(m_pButtons + buttonID)->m_options = STRUTILSetBit(INPUTMAN_BOPT_ISKEY,
			(m_pButtons + buttonID)->m_options,true); // if key, set option isKey to true
	}
	else
	{ // else treat as scalar
		if (!ISIDXVALID(inputID,(m_pDevices + deviceID)->m_nInputs))
			return false; // invalid inputID
	}	
	// ****Set button values****
	(m_pButtons + buttonID)->m_id=inputID;
	(m_pButtons + buttonID)->m_pDevice=(m_pDevices + deviceID);
	return true; // success!
}

/*****************************************************************************

  Returns index of Button with given name. Returns -1 if not found.

*****************************************************************************/
int CINPUTMAN::FindButton(const char *name)
{
	if (name==NULL)
		return -1; // invalid input
	if (m_pButtons&&m_nButtons)
		for (int i=0; i<m_nButtons; i++)
			if ( ISBUTTONVALID(m_pButtons + i) ) // skip all invalid buttons
				if(strcmp(name,(m_pButtons + i)->m_pName)==NULL)
					return i;
	return -1;
}

/*****************************************************************************

  Resets the min and max values for a scalar input from a device.

*****************************************************************************/
void CINPUTMAN::CalibrateDevice(int deviceID,int inputID,int min,int max)
{
	if (ISIDXVALID(deviceID,m_nDevices)) // is deviceID valid?
		if (ISDEVICEVALID(m_pDevices + deviceID)) // is device valid?
			if (ISIDXVALID(inputID,(m_pDevices + deviceID)->m_nInputs)) // is inputID valid
			{ // set new min and max values
				((m_pDevices + deviceID)->m_pInputStates+inputID)->m_min = min;
				((m_pDevices + deviceID)->m_pInputStates+inputID)->m_max = max;
			}
}

/*****************************************************************************

  Sets the Ascii Conversion Table For Input Manager.

*****************************************************************************/
void CINPUTMAN::SetAsciiConverion(const CASCIICONTBLE *pConTbl)
{
	m_asciiConTbl.CopyTable(pConTbl);
}

/*****************************************************************************

  Gets the time of inactivity from the most recently used button.

*****************************************************************************/
float CINPUTMAN::GetTmInactive()
{
	if (!m_nButtons) // if no buttons return zero
		return 0.0f;
	register int i = m_nButtons-1;// index of last button
	register float time = (m_pButtons + i)->m_tInactive; // get time of last button
	while (i--)
		if ((m_pButtons + i)->m_tInactive<time) // get time of least inactive button
			time = (m_pButtons + i)->m_tInactive;
	return time;
}

/*****************************************************************************

  Updates all data within CINPUTMAN with current input states.

*****************************************************************************/
void CINPUTMAN::Update()
{
	// ****Capture elapsed time****
	register float elapsedTime = ((CQPCTimer *)m_pTimer)->GetElapsedSeconds();
	register int i = m_nDevices;
	// ****Update all devices****
	while (i--)
		(m_pDevices + i)->Update();
	// ****Update all buttons****
	i = m_nButtons;
	while (i--)
		(m_pButtons + i)->Update(elapsedTime);
	// ****Update text input****
	if (m_keyboard>-1) // if there is a SysKeyBoard
	{ // note: device validity already confirmed
		register int len = strlen(m_keyBuffer);
		if ((len+1)<INPUTMAN_BUFFERSIZE) // if there is buffer space then accept input
		{
			register unsigned char c = NULL;
			i = 256;
			m_textSKeysOn = NULL;
			register unsigned char isSpcKey=false;
			while (i--)
				if ((m_pDevices+m_keyboard)->m_onKeys.Check(i))
				{
					isSpcKey=false;
					if ((((unsigned char)i==m_textSKeys[1])||((unsigned char)i==m_textSKeys[2]))) // shift key
					{
						m_textSKeysOn = STRUTILSetBit(INPUTMAN_TSKEY_ISSHIFT,m_textSKeysOn,true);
						isSpcKey=true;
					}
					if ((((unsigned char)i==m_textSKeys[3])||((unsigned char)i==m_textSKeys[4]))) // ctrl key
					{
						m_textSKeysOn = STRUTILSetBit(INPUTMAN_TSKEY_ISCTRL,m_textSKeysOn,true);
						isSpcKey=true;
					}
					if ((((unsigned char)i==m_textSKeys[5])||((unsigned char)i==m_textSKeys[6]))) // alt key
					{
						m_textSKeysOn = STRUTILSetBit(INPUTMAN_TSKEY_ISALT,m_textSKeysOn,true);
						isSpcKey=true;
					}
					if (!isSpcKey) // not a special key
						c = (unsigned char)i; // capture key
				}
			bool isRecordOk = false; // true if it is ok to record the key
			if (c==m_lastKey) // if same key as last time key has been held
			{
				m_keyDwnTm+=elapsedTime; // record time held
				if (m_keyDwnTm>INPUTMAN_TEXT_MAXTIME) // keep counted time within resonable bounds
					m_keyDwnTm=INPUTMAN_TEXT_MAXTIME;
				//INPUTMAN_TEXT_REPEATE_DELAY
				if ((!STRUTILCheckBit(INPUTMAN_TOPT_ISREPEAT,m_textOpt))&&
					(m_keyDwnTm>INPUTMAN_TEXT_REPEATE_DELAY)) // repeat off and initial delay passed
				{
					m_textOpt = STRUTILSetBit(INPUTMAN_TOPT_ISREPEAT,m_textOpt,true); // turn repeate on
					isRecordOk = true; // record key
				}
				else if (STRUTILCheckBit(INPUTMAN_TOPT_ISREPEAT,m_textOpt)&&
					(m_keyDwnTm>INPUTMAN_TEXT_REPEATE_RATE)) // repeat on and repeat delay passed
					isRecordOk = true; // record key
			}
			else
			{ // if key is has not been held turn off repeat and reset held time
				m_keyDwnTm = 0.0f;
				m_textOpt = STRUTILSetBit(INPUTMAN_TOPT_ISREPEAT,m_textOpt,false);
				isRecordOk = true; // record new key
				// check if capslock key, if so toggle caps
				if (c==m_textSKeys[0])
					m_textOpt = STRUTILSetBit(INPUTMAN_TOPT_ISCAPS,m_textOpt,
								  !STRUTILCheckBit(INPUTMAN_TOPT_ISCAPS,m_textOpt));
			}
			m_lastKey=c; // store last key
			if (isRecordOk) // if ok then record key in buffer
			{
				// convert character
				m_keyBuffer[len]=(char)m_asciiConTbl.Convert(c,
					STRUTILCheckBit(INPUTMAN_TOPT_ISCAPS,m_textOpt),
					(m_pDevices+m_keyboard)->m_onKeys.Check(m_textSKeys[1])||
					(m_pDevices+m_keyboard)->m_onKeys.Check(m_textSKeys[2]));
				// pad buffer with NULL
				m_keyBuffer[len+1]=NULL;
				m_keyDwnTm = 0.0f; // reset held time
			}
		}
	}
}