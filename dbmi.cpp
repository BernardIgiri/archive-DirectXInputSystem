#include "dbmi.h"
#include <il/ilu.h>
#include <malloc.h>
#include <memory.h>

void DevilInit()
{
	static bool isIntialized=false;
	if (!isIntialized)
	{
		ilInit();
		iluInit();
		isIntialized=true;
	}
}

/*****************************************************************************

  Creates a new CDBMI object.

*****************************************************************************/
CDBMI* CreateCDBMI()
{
	return new CDBMI;
}

/*****************************************************************************

  Destroys a given CDBMI object.

*****************************************************************************/
void DestroyCDBMI(CDBMI* pImage)
{
	if (pImage)
		pImage->Destroy();
}

/*****************************************************************************

  CDBMI class constructor.

*****************************************************************************/
CDBMI::CDBMI()
{
	// Initialize DevIL for all instances of CDBMI
	DevilInit();
	Clear();
}

/*****************************************************************************

  CDBMI class deconstructor.

*****************************************************************************/
CDBMI::~CDBMI()
{
	Destroy();
}

/*****************************************************************************

  Frees all ram allocated by the CDBMI class.

*****************************************************************************/
void CDBMI::Destroy()
{
	if (!m_isProtected&&m_pData) // if not protected and there is data
		delete [] m_pData; // delete the data
	Clear();
}

/*****************************************************************************

  Destroys only the bitmap image data stored in CDBMI.

*****************************************************************************/
void CDBMI::DelData()
{
	if (!m_isProtected&&m_pData) // if not protected and there is data
		delete [] m_pData; // delete the data
}

/*****************************************************************************

  Set all CDBMI class variables to default values. USE WITH CAUTION!!!

*****************************************************************************/
void CDBMI::Clear()
{
	memset(this,NULL,sizeof(CDBMI));
	m_isProtected = true; // data is protected by default
}

/*****************************************************************************

  Sets protection to state, if protection is on CDBMI will not try to
  deallocate the memory pointed to by m_pData.

*****************************************************************************/
void CDBMI::SetProtection(bool state)
{
	m_isProtected = state;
}

/*****************************************************************************

  Does a complete copy of the data within pImage over the contents of this
  CDBMI object.
  Returns false on failure.

*****************************************************************************/
bool CDBMI::Copy(CDBMI *pImage)
{
	if (!pImage)
		return false; // invalid input
	Destroy(); // Destroy old data if there is any
	m_isProtected = false; // data is not protected
	// copy all vars
	memcpy(this,pImage,sizeof(CDBMI));
	if (m_size<2)
		m_size=2; // ensure allocation of array object
	// allocate space
	m_pData = NULL;
	m_pData = new unsigned char[m_size];
	if (!m_pData)
		return false; // new failed
	// copy image data
	memcpy(m_pData,pImage->m_pData,pImage->m_size);
	return true;
}

/*****************************************************************************

  Loads an image into CDBMI through DevIL.
  Returns false on failure.

*****************************************************************************/
bool CDBMI::Load(const char *fileName)
{
	Destroy(); // Destroy old data if there is any
	m_isProtected = false; // data is not protected
	if (ilLoadImage((char *)fileName)==IL_FALSE)
		return NULL;
	// read data
	ILubyte *data = NULL;
	data = ilGetData();
	if (data==NULL) // get data failed
		return NULL;
	// get image info
	ILinfo imageInfo;
	iluGetImageInfo(&imageInfo);
	if (imageInfo.Data==NULL)
		return NULL;
	m_width	=(int)imageInfo.Width;
	m_height=(int)imageInfo.Height;
	m_bpp	=(int)imageInfo.Bpp*8;
	m_size	=(unsigned)imageInfo.SizeOfData;
	if (m_size<2)
		m_size=2; // ensure allocation of array object
	m_pData = NULL;
	m_pData = new unsigned char[m_size];
	if (m_pData!=NULL)
		memcpy(m_pData,imageInfo.Data,imageInfo.SizeOfData);
	return (m_pData!=NULL);
}

/*****************************************************************************

  Reads a single pixel from the bitmap image.

*****************************************************************************/
unsigned char CDBMI::ReadPx(int x,int y)
{
	int Bpp=m_bpp/8; // Bpp is in bytes/pixel
	return *(m_pData+(x*Bpp)+(y*Bpp)*m_width);
}

/*****************************************************************************

  Converts an image into a luminance alpha image.
  Returns false on failure.

*****************************************************************************/
bool CDBMI::ConvertToLumAlpha(unsigned char minVal)
{
	if (m_bpp==16)
		return false; // 16 bit color is not supported
	m_bpp/=8; // bpp is now bytes/pixel
	unsigned outDataSize = 2*(m_size/m_bpp);
	unsigned char *output = (unsigned char *)malloc(outDataSize);
	if (output==NULL)
	{
		m_bpp*=8; // fix bpp
		return false; // malloc failed
	}
	int inDataIdx = m_size,
		outDataIdx = outDataSize;
	while (inDataIdx)
	{
		// update data indexes
		inDataIdx-=m_bpp;
		outDataIdx-=2;
		// get brightness data
		unsigned char brightness=0;
		switch(m_bpp)
		{
		case 1: // brighteness = alpha
			*(output+outDataIdx) = 
				*(output+outDataIdx+1) = *(m_pData+inDataIdx);
			break;
		case 2:
			// not supported
			break;
		case 3: // brighteness = alpha
			*(output+outDataIdx) = 
				*(output+outDataIdx+1) = (unsigned char)
										 (((unsigned)*(m_pData+inDataIdx)+
										 (unsigned)*(m_pData+inDataIdx+1)+
										 (unsigned)*(m_pData+inDataIdx+2))/3); // average all three colors
			break;
		case 4:
			*(output+outDataIdx) = (unsigned char)
								   (((unsigned)*(m_pData+inDataIdx)+
								   (unsigned)*(m_pData+inDataIdx+1)+
								   (unsigned)*(m_pData+inDataIdx+2))/3); // average all three colors
			*(output+outDataIdx+1) = *(m_pData+inDataIdx+3); // copy alpha value
			break;
		default:
			// error
			break;
		};
		if (minVal)
		{
			if (*(output+outDataIdx)<minVal)
				*(output+outDataIdx)=0;
			if (*(output+outDataIdx+1)<minVal)
				*(output+outDataIdx+1)=0;
		}
	}
	DelData(); // delete bitmap data
	m_pData = output;
	m_size = outDataSize;
	m_bpp = 16; // now 16 bits/pixel
	return true;
}