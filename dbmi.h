/*****************************************************************************

  DevIL Bitmap Image
  font.h

  DATE	:	12/23/2003
  AUTHOR:	Bernard Igiri

  A library containing a class for bitmap images that supports various
  methods of manipulating bitmaps. Including method from the DevIL image
  library..

  NOTE:  Image data type is assumed as follows for all CDBMI operations
  8bit  - 8bit grayscale
  16bit - 16bit luminance alpha pair
  24bit - 24bit RGB image
  32bit - 32bit RGBA image
  --	- N/A

  This code has the following dependancies:
  <il/ilu.h> <malloc.h> <memory.h>

*****************************************************************************/
#ifndef __INCLUDED_DBMI_H__
#define __INCLUDED_DBMI_H__

// Initilized DevIL
void DevilInit(); // all classes derived from DBMI need to call this upon construction

class CDBMI;

CDBMI* CreateCDBMI();
void DestroyCDBMI(CDBMI* pImage);

class CDBMI
{
public:
	CDBMI();
	~CDBMI();
	void Clear();
	void Destroy();

	void SetProtection(bool state);
	bool Copy(CDBMI *pImage);
	bool Load(const char *fileName);

	unsigned char ReadPx(int x,int y);

	bool ConvertToLumAlpha(unsigned char minVal=0);

	unsigned char*	m_pData; // pixel data
	int				m_width; // image width
	int				m_height; // image height
	int				m_bpp; // bits per pixel
	unsigned		m_size; // size of pixel data
protected:
	void DelData();
private:
	bool			m_isProtected; // true if data is protected from deletion
};

#endif//__INCLUDED_DBMI_H__

