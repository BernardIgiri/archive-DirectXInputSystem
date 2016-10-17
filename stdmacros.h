/*****************************************************************************
//////////////////////////////////////////////////////////////////////////////

  DATE	:	12/13/2003
  AUTHOR:	Bernard Igiri

  "stdmacros.h"

  This is a collection of macros for use C++ coding.

///////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifndef __INCLUDED_STDMACROS_H__
#define __INCLUDED_STDMACROS_H__

// true if index is valid
#define ISIDXVALID(idx,max) ((idx>-1)&&(idx<max))
#define CLAMP(min,value,max) if (value<min) value=min;else if (value>max) value=max
#define BOUNDLWR(value,bound) if (value<bound) value=bound
#define BOUNDUPR(value,bound) if (value>bound) value=bound

#endif//__INCLUDED_STDMACROS_H__