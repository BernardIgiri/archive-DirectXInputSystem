/*****************************************************************************

  OpenGL Graphical User Interface 2.0
  glgui.h

  DATE	:	1/1/2004
  AUTHOR:	Bernard Igiri

  A graphical user interface for OpenGL.

  This code has the following dependancies:

*****************************************************************************/
#ifndef __INCLUDED_GLGUI_H__
#define __INCLUDED_GLGUI_H__


// Basic GLGUI element
class CGLGUI_ELMT
{
public:
	virtual void Draw() = 0;
	virtual void Update(char c) = 0;
	virtual void MUpdate(char c,int cX,int cY,bool isMBDown);
	inline void GetCoords(int &top,int &left,int &bottom,int &right)
	{
		top = m_yPos;
		left= m_xPos;
		bottom = m_yPos-m_height; // y axis is flipped in opengl
		right = m_xPos+m_width;
	};
	bool m_usesMouse; // true if this class uses the mouse
private:
	int m_width;
	int m_height;
	int m_xPos;
	int m_yPos;
};

#endif//__INCLUDED_GLGUI_H__