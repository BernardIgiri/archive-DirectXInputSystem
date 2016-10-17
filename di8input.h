/*****************************************************************************

  Direct Input 8 Input implimentation.
  di8input.h

  DATE	:	11/27/2003
  AUTHOR:	Bernard Igiri

  Allows for direct input 8 to be used as the input source for
  CINPUTMAN.

*****************************************************************************/
#ifndef __INCLUDED_DI8INPUT_H__
#define __INCLUDED_DI8INPUT_H__

#include "inputman.h" // for CINPUTMAN class
#include "glwinapi.h" // for GLWINAPI class needed for window data

#define DI8INPUT_KEYBOARD	"Keyboard"
#define DI8INPUT_MOUSE		"Mouse"
#define DI8INPUT_JOYSTICK	"Joystick"
#define DI8INPUT_MOUSEX		0
#define DI8INPUT_MOUSEY		1
#define DI8INPUT_NMOUSEKEYS	8
#define DI8INPUT_SCRLWHLUP	(DI8INPUT_NMOUSEKEYS+0)
#define DI8INPUT_SCRLWHLDWN	(DI8INPUT_NMOUSEKEYS+1)
#define DI8INPUT_MOUSEUP	(DI8INPUT_NMOUSEKEYS+2)
#define DI8INPUT_MOUSEDOWN	(DI8INPUT_NMOUSEKEYS+3)
#define DI8INPUT_MOUSELEFT	(DI8INPUT_NMOUSEKEYS+4)
#define DI8INPUT_MOUSERIGHT	(DI8INPUT_NMOUSEKEYS+5)
// number used to initilize mouse
#define DI8INPUT_INITMSKEYS	(DI8INPUT_NMOUSEKEYS+6)



void *DI8Initialize(CINPUTMAN *pInput,GLWINAPI *pWindow);
bool DI8AddKeyBoard(void *data);
bool DI8AddMouse(void *data,bool isExclusive=true);
//bool DI8AddJoystick(CINPUTMAN *pInput,void *data,int xLimit,int yLimit,int nButtons);

#endif//__INCLUDED_DI8INPUT_H__