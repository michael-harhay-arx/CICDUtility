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
#include "ArxtronToolslib.h"
		
#include "TestExec.h"

//==============================================================================
// Constants
		
#define MAX_BACKGROUND_THREADS		(20)
#define MAX_POPUP_MSGS				(100)

#define E_ESTOP						(99)

#define ARX_UI_CAPABILITY_SEQNAME	"Arxtron Sequencer"

#define CmtErrChk(fncall)\
	if ((error = fncall))\
	{\
		CmtGetErrorMessage(error, errmsg);\
		tsErrChk (error, errmsg);\
	}

#define SetTSSeqVarStruct(tsseqvar, seqname, lookupstring, vartype, varval)\
	sprintf(tsseqvar##.lookupString, "%s", #lookupstring);\
	strcpy(tsseqvar##.seqName, seqname);\
	tsseqvar##.varType = vartype;\
	sprintf(tsseqvar##.varVal, "%d", varval);

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

typedef void (*TS_ThreadLogicFn) (CAObjHandle execHandle, CAObjHandle seqFileHandle);

typedef struct 
{
    ApplicationWindow mainWindow;
    TS_ThreadLogicFn logicFn;
} ThreadParams;

//==============================================================================
// External variables

//==============================================================================
// Global functions

// General thread functions
int StartTestThreads (ApplicationWindow GMainWindow, char errmsg[ERRLEN]);
int CVICALLBACK GenericTestThread (void *functionData);

// Thread-specific functions
void ThreadLogic_ESTOP (CAObjHandle execHandle, CAObjHandle seqHandle);
void ThreadLogic_FailChuteFull (CAObjHandle execHandle, CAObjHandle seqFileHandle);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestThreads_H__ */
