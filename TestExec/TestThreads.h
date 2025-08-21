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

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

// Start threads function
int StartThreads (void);

// Thread functions
DWORD WINAPI Thread_Test (LPVOID lpParam);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestThreads_H__ */
