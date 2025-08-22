/***************************************************************************//*!
* \file TestThreads.h
* \author Michael Harhay
* \copyright . All Rights Reserved.
* \date 2025-08-18 11:27:12 AM
*******************************************************************************/

#ifndef __TestThreads_H__
#define __TestThreads_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include <utility.h>
#include <ansi_c.h>

//==============================================================================
// Constants

//==============================================================================
// Types
		
typedef struct
{
	LPSECURITY_ATTRIBUTES security;		// security options
	SIZE_T stackSize;					// initial stack size in bytes
	LPTHREAD_START_ROUTINE func;		// function to call when starting thread
	LPVOID param;						// param to pass into called function
	DWORD creationFlags;				// flags controlling creation of thread
	LPDWORD threadID;
} ThreadConfig;

//==============================================================================
// External variables

//==============================================================================
// Global functions

// Start threads function
int StartTestThreads (char errmsg[ERRLEN]);

// Thread functions
int CVICALLBACK Thread_EStop (void *functionData);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestThreads_H__ */
