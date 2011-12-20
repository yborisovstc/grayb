#ifndef _GRAYB_PLAT_DEFS_
#define _GRAYB_PLAT_DEFS_

// Platform specific definition 

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FAPWS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FAPWS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.


#if defined (_PLAT_WIN32_)  // Windows platform

//#include "stdafx.h"	// Precompiled header
#include <afxcoll.h>
#include <windows.h>

#ifdef FAPWS_EXPORTS
#define FAPWS_API __declspec(dllexport)
#else
#define FAPWS_API __declspec(dllimport)
#endif


typedef int TInt; 
typedef DWORD TUint32; 
typedef BYTE TUint8; 
typedef bool TBool; 
#define ETrue TRUE
#define EFalse FALSE

#define _TEMPLATE_

// Panic indication 
#define _IND_PANIC(KFapPanic, aRes) _RPT1(_CRT_ERROR, KFapPanic, aRes);

// Assertion 
#define __ASSERT_H_ crtdbg.h 
#include <crtdbg.h>
#define __ASSERT _ASSERT

#endif // _PLAT_WIN32_ 



#if defined (_PLAT_LINUX_)  // Linux  platform

#ifdef FAPWS_EXPORTS
#define FAPWS_API 
#else
//#define FAPWS_API extern 
#define FAPWS_API 
#endif

typedef int TInt; 
typedef long int TUint32; 
typedef unsigned char TUint8; 
typedef bool TBool; 
#define ETrue 1
#define EFalse 0

#define _TEMPLATE_ template<> 

// Panic indication 
#define _IND_PANIC(KFapPanic, aRes) ;

// Assertion 
#define __ASSERT_H_ assert.h 
#include <assert.h>
#define __ASSERT assert

#endif  // Linux



#endif // _FAP_PLAT_DEFS_

