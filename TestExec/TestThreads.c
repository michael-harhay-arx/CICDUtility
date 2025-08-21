/***************************************************************************//*!
* \file TestThreads.c
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-18 11:27:12 AM
* \brief A short description.
* 
* A longer description.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

/***************************************************************************//*!
* \brief Disables system logging completely.  Needs to be defined before including
* 	ArxtronToolslib.h.  By default, it is defined in each source file to allow
* 	for source file level control for disabling.
*******************************************************************************/
//#define SYSLOGDISABLE
/***************************************************************************//*!
* \brief Overrides config log level.  Needs to be defined before including
* 	ArxtronToolslib.h.  By default, it is defined in each source file to allow
* 	for source file level control for overrides.
*******************************************************************************/
//#define SYSLOGOVERRIDE 3

#include "Windows.h"
#include <utility.h>
#include <ansi_c.h>

#include "TestThreads.h"
#include "SystemLog_LIB.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

/***************************************************************************//*!
* \brief Stores the log level used for SYSLOG macro
*******************************************************************************/
static int glbSysLogLevel = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Start all background test threads
*******************************************************************************/
int StartThreads (void)
{
	int error = 0;
	
	HANDLE tHandle;
	DWORD tId = 0;
	int param = 67;
	
	// Create threads
    tHandle = CreateThread (NULL, 0, Thread_Test, &param, 0, &tId);

    if (tHandle == NULL) {
        fprintf (stderr, "Error creating thread: %lu\n", GetLastError ());
        return 1;
    }

    printf("Main thread created new thread with ID: %lu\nNow waiting for thread to finish...\n", tId);
	
	// Wait for thread to finish
	WaitForSingleObject (tHandle, INFINITE);
    CloseHandle (tHandle);
	printf ("Thread completed.\n");
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Test thread function
*******************************************************************************/
DWORD WINAPI Thread_Test (LPVOID lpParam) // 20250821Michael: TODO implement function properly
{
	DWORD exitCode = 0;
	int paramVal = *(int *) lpParam;
	
	printf("Entered Thread_Test.\nParameter received: %d\nNow waiting 3 secs...\n", paramVal);
	Delay (3.0);
	printf("Exited Thread_Test.\n");
	
	// Close thread
    return exitCode;
	
	
	/*int error = 0;
	VBOOL eStopHigh = 0;
	int continueFlag = 0;
	
	// Set EStop to high

	TS_PropertyGetValBoolean (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, &eStopHigh);
	if (!eStopHigh)
	{
		eStopHigh = -1;
		TS_PropertySetValBoolean (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, eStopHigh);
	}
	
	// TODO: Check if sequence has stopped
	TS_PropertyGetValNumber (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, continueFlag);
	ASSERT_EQ_INT (0, continueFlag)
	
Error:
	return error;*/
}
//! \cond
/// REGION END
//! \endcond