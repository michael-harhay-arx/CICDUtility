/***************************************************************************//*!
* \file TestThreads.c
* \author Michael Harhay
* \copyright . All Rights Reserved.
* \date 2025-08-18 11:27:12 AM
* \brief Thread functions
* 
* A longer description.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "TestThreads.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

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
	DWORD tID;
	int testParam = 67;
	int testParam2 = 123;
	
	// Initialize list of threads to be created
	ThreadConfig threadList[2] = {{NULL, 0, Thread_Test, &testParam, 0, &tID},
								  {NULL, 0, Thread_Test_2, &testParam2, 0, &tID}};
								  
	// Initialize list of thread handles
	int numThreads = sizeof (threadList) / sizeof (threadList[0]);
	HANDLE threadHandles[numThreads];
	memset (threadHandles, 0, sizeof (threadHandles));
								  
	// Create threads
	for (int t = 0; t < numThreads; t++)
	{
		threadHandles[t] = CreateThread (threadList[t].security,
									   threadList[t].stackSize,
									   threadList[t].func, 
									   threadList[t].param,
									   threadList[t].creationFlags, 
									   threadList[t].threadID);

	    if (threadHandles[t] == NULL) {
	        fprintf (stderr, "Error creating thread: %lu\n", GetLastError ());
	       	return 1;
	    }

	    printf("Main thread created new thread with ID: %lu\n", threadList[t].threadID);
	}
	
	// Wait for threads to finish	
	WaitForMultipleObjects (numThreads, threadHandles, TRUE, INFINITE);
	
    for (int h = 0; h < numThreads; h++)
	{
		CloseHandle (threadHandles[h]);
		printf ("Thread handle %d completed and closed.\n", threadHandles[h]);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Test thread function
*******************************************************************************/
DWORD WINAPI Thread_Test (LPVOID lpParam) // 20250821Michael: TODO implement function properly
{
	DWORD exitCode = 0;
	int paramVal = *(int *) lpParam;
	
	printf("\tEntered Thread_Test.\n\tParameter received: %d\n\tNow waiting 3 secs...\n", paramVal);
	Delay (3.0);
	printf("\tExited Thread_Test.\n");
	
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

/***************************************************************************//*!
* \brief Test thread function
*******************************************************************************/
DWORD WINAPI Thread_Test_2 (LPVOID lpParam) // 20250821Michael: TODO implement function properly
{
	DWORD exitCode = 0;
	int paramVal = *(int *) lpParam;
	
	printf("\tEntered Thread_Test.\n\tParameter received: %d\n\tNow waiting 5 secs...\n", paramVal);
	Delay (5.0);
	printf("\tExited Thread_Test.\n");
	
	// Close thread
    return exitCode;
}
//! \cond
/// REGION END
//! \endcond