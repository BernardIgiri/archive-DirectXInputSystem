#include "ioUtilities.h"
#include <string.h>

IOUBDataItm::IOUBDataItm()
{
	memset(this,NULL,sizeof(IOUBDataItm));
}

IOUBDataItm::~IOUBDataItm()
{
	if (m_pElmts)
		free(m_pElmts);
	memset(this,NULL,sizeof(IOUBDataItm));
}

bool IOUBDataItm::Save(FILE *pFilePtr)
{
	// write number of elements
	if (fwrite(&m_nElmts,sizeof(unsigned),1,pFilePtr)==NULL)
		return false;
	// write elements
	return (fwrite(m_pElmts,m_elmtSize,m_nElmts,pFilePtr)!=NULL);
}

bool IOUBDataItm::Load(FILE *pFilePtr)
{
	// read number of elements
	if (fread(&m_nElmts,sizeof(unsigned),1,pFilePtr)==NULL)
		return false;
	// malloc space for data
	m_pElmts = malloc(m_elmtSize*m_nElmts);
	if (!m_pElmts)
		return false;
	// read elements
	return (fread(m_pElmts,m_elmtSize,m_nElmts,pFilePtr)!=NULL);
}

IOUBDataUnit::IOUBDataUnit()
{
	memset(this,NULL,sizeof(IOUBDataUnit));
}

IOUBDataUnit::IOUBDataUnit(unsigned nItms)
{
	memset(this,NULL,sizeof(IOUBDataUnit));
	SetNItms(nItms);
}

IOUBDataUnit::~IOUBDataUnit()
{
	if (m_nItms<2) // if not plural
		delete m_pItms;
	else // else
		delete [] m_pItms;
	memset(this,NULL,sizeof(IOUBDataUnit));
}

bool IOUBDataUnit::Save(FILE *pFilePtr)
{
	unsigned count=m_nItms;
	// save all objs, return false if one fails
	while (count--)
		if (!(m_pItms+count)->Save(pFilePtr))
			return false;
	return true;
}

bool IOUBDataUnit::Load(FILE *pFilePtr)
{
	unsigned count=m_nItms;
	// load all objs, return false if one fails
	while (count--)
		if (!(m_pItms+count)->Load(pFilePtr))
			return false;
	return true;
}

bool IOUBDataUnit::SetNItms(unsigned n)
{
	if (m_pItms) // cannot initialize twice
		return false;
	// set vars
	m_nItms = n;
	// allocate ram
	if (m_nItms<2) // if not plural
		m_pItms = new IOUBDataItm;
	else // else
		m_pItms = new IOUBDataItm[m_nItms];
	return (m_pItms!=NULL); // return success
}

IOUBData::IOUBData()
{
	memset(this,NULL,sizeof(IOUBData));
	m_state = INITIALIZING;
	m_error = ISOK;
}

IOUBData::IOUBData(unsigned nItmsPUnit)
{
	memset(this,NULL,sizeof(IOUBData));
	SetNItmsPUnit(nItmsPUnit);
	m_state = SETTING_LOAD_FORMAT;
	m_error = ISOK;
}

IOUBData::IOUBData(unsigned nItmsPUnit,unsigned nUnits)
{
	memset(this,NULL,sizeof(IOUBData));
	SetParams(nItmsPUnit,nUnits);
	m_state = SETTING_SAVE_FORMAT;
	m_error = ISOK;
}

IOUBData::~IOUBData()
{
	// deallocate ram
	// units
	if (m_nUnits<2) // if not plural
		delete m_pUnits;
	else // else
		delete [] m_pUnits;
	// element sizes
	if (m_nItmsPUnit<2) // if not plural
		delete m_pElmtSizes;
	else // else
		delete [] m_pElmtSizes;
	// clear ram
	memset(this,NULL,sizeof(IOUBData));
	m_state = INITIALIZING;
	m_error = ISOK;
}

bool IOUBData::Save(FILE *pFilePtr)
{
	if (m_state!=SETTING_SAVE_DATA&&m_state!=SAVING_DATA)
	{
		m_error = OPOUTOFORDER;
		return false; // operation done in wrong order
	}
	// write number of units
	if (fwrite(&m_nUnits,sizeof(unsigned),1,pFilePtr)==NULL)
	{
		m_error = WRITEFAILED;
		return false;
	}
	unsigned count=m_nUnits;
	// save all units, return false if one fails
	while (count--)
		if (!(m_pUnits+count)->Save(pFilePtr))
		{
			m_error = WRITEFAILED;
			return false;
		}
	m_state=SAVING_DATA;
	return true;
}

bool IOUBData::Load(FILE *pFilePtr)
{
	if (m_state!=LOADING_DATA)
	{
		m_error = OPOUTOFORDER;
		return false; // operation done in wrong order
	}
	// read number of units
	if (fread(&m_nUnits,sizeof(unsigned),1,pFilePtr)==NULL)
	{
		m_error = READFAILED;
		return false;
	}
	// set params
	if (!SetParams(m_nItmsPUnit,m_nUnits))
		return false;
	unsigned count=m_nUnits;
	// save all units, return false if one fails
	while (count--)
		if (!(m_pUnits+count)->Load(pFilePtr))
		{
			m_error = READFAILED;
			return false;
		}
	m_state=OUTPUTTING_LOADED_DATA;
	return true;
}

bool IOUBData::SetLoadParams(unsigned nItmsPUnit)
{
	if (m_state!=INITIALIZING)
		return false; // operation done in wrong order
	m_state = SETTING_LOAD_FORMAT;
	return SetNItmsPUnit(nItmsPUnit);
}

bool IOUBData::SetSaveParams(unsigned nItmsPUnit,unsigned nUnits)
{
	if (m_state!=INITIALIZING)
	{
		m_error = OPOUTOFORDER;
		return false; // operation done in wrong order
	}
	m_state = SETTING_SAVE_FORMAT;
	return SetParams(nItmsPUnit,nUnits);
}

void IOUBData::SelUnit(unsigned idx)
{
	m_currUnit = idx;
}

bool IOUBData::SetNItmsPUnit(unsigned n)
{
	m_nItmsPUnit = n;
	if (!m_pElmtSizes)
	{ // initialize only once
		// allocate ram for item format
		if (m_nItmsPUnit<2) // if not plural
			m_pElmtSizes = new unsigned;
		else // else
			m_pElmtSizes = new unsigned[m_nItmsPUnit];
		if (!m_pElmtSizes)
		{
			m_error = OUTOFMEMORY;
			return false;
		}
		// zero out memory
		memset(m_pElmtSizes,NULL,sizeof(unsigned)*m_nItmsPUnit);
	}
	return true;
}

bool IOUBData::SetParams(unsigned nItmsPUnit,unsigned nUnits)
{
	if (m_pUnits) // cannot initialize twice
		return false;
	// set vars
	m_nUnits = nUnits;
	if (!SetNItmsPUnit(nItmsPUnit))
		return false;
	// allocate ram
	if (m_nUnits<2) // if not plural
		m_pUnits = new IOUBDataUnit;
	else // else
		m_pUnits = new IOUBDataUnit[m_nUnits];
	// check pointer
	if (!m_pUnits)
	{
		m_error = OUTOFMEMORY;
		return false;
	}
	// initialize objects
	unsigned count = m_nUnits;
	while (count--)
	{
		// set number of items
		if (!(m_pUnits+count)->SetNItms(m_nItmsPUnit))
		{
			m_error = OUTOFMEMORY;
			return false;
		}
		// set item format
		(m_pUnits+count)->SetFormat(m_pElmtSizes);
	}
	return true; // return success
}

bool IOUFileExist(const char *pPath)
{
	FILE *test = fopen(pPath,"rb");
	if (test)
		fclose(test);
	return (test!=NULL);
}

char *EncryptStr(char *str,const char *key)
{
	unsigned k,s,f;
	unsigned sLen = strlen(str);
	unsigned kLen = strlen(key);
	while (sLen--)
	{
		s = (unsigned char)(*(str+sLen));
		k = (unsigned char)(*(key+(sLen%kLen)));
		f = s+k;
		if (f>255)
			f-=255;
		*(str+sLen) = (char)f;
	}
	return str;
}

char *DecryptStr(char *str,const char *key)
{
	unsigned k,s,f;
	unsigned sLen = strlen(str);
	unsigned kLen = strlen(key);
	while (sLen--)
	{
		s = (unsigned char)(*(str+sLen));
		k = (unsigned char)(*(key+(sLen%kLen)));
		f = (s+255)-k;
		if (f>255)
			f-=255;
		*(str+sLen) = (char)f;
	}
	return str;
}