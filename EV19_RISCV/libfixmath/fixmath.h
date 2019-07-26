#ifndef __libfixmath_fixmath_h__
#define __libfixmath_fixmath_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
	\file fixmath.h
	\brief Functions to perform fast accurate fixed-point math operations.
*/


#define FIXMATH_NO_CACHE	1
#define FIXMATH_FAST_SIN 1


#include "uint32.h"
#include "int64.h"
#include "fract32.h"
#include "fix16.h"

#ifdef __cplusplus
}
#endif

#endif
