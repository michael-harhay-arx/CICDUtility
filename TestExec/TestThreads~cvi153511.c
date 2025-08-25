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

#include "tsapicvi.h"
#include "tsui.h"
#include "toolbox.h"
#include "ArxtronToolslib.h"

#include "UIUtility.h"
#include "TSMessaging.h"
#include "TestThreads.h"

//==============================================================================
// Constants

#define MAX_BACKGROUND_THREADS	(20)
#define MAX_POPUP_MSGS			(100)

#define E_ESTOP					(99)

#define CmtErrChk(fncall)\
	if ((error = fncall))\
	{\
		CmtGetErrorMessage(error, errmsg);\
		tsErrChk (error, errmsg);\
	}

//==============================================================================
// Types

//==============================================================================
// Static global variables

static CmtThreadPoolHandle glbBackgroundThreads = 0;
static CmtThreadFunctionID glbBackgroundThreadIDs[MAX_BACKGROUND_THREADS] = {0};

static int glbTerminate = 0;

static char glbErrorMsgs[MAX_POPUP_MSGS][256] = {0};
static char glbWarningMsgs[MAX_POPUP_MSGS][256] = {0};

//==============================================================================
// Static functions

//==============================================================================
// Global variables

extern TSSeqVar glbWatchVars[100];
extern int glbNumWatching;
extern TSSeqVar glbSetVars[100];
extern int glbNumSetting;

//==============================================================================
// Global functions

/***************************************************************************//*!
* \brief Get error message for standard error codes
*******************************************************************************/
void GetStandardErrMsg (int error, char errmsg[ERRLEN])
{
	memset (errmsg, 0, ERRLEN);
	
	switch (error)
	{
		case ERR_LIB_NOT_INITIALIZED:
			strcpy (errmsg,"Library not initialized");
			break;
	}
}
//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Start all background test threads
*
* \param [in] GMainWindow		Reference to the main application window
*******************************************************************************/
int StartTestThreads (ApplicationWindow GMainWindow, char errmsg[ERRLEN])
{
	fnInit;

	// Start threads
	int threadIndex = 0;
	CmtErrChk (CmtNewThreadPool (MAX_BACKGROUND_THREADS, &glbBackgroundThreads));
	CmtErrChk (CmtScheduleThreadPoolFunction (glbBackgroundThreads, Thread_EStop, &GMainWindow, glbBackgroundThreadIDs+threadIndex++));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Default thread template function
*
* \param [in] functionData		By default, this parameter takes a pointer to 
								GMainWindow from the StartTestThreads() function. 
								This can be changed; a pointer to any struct 
								can be passed to allow for more params.
*******************************************************************************/
int CVICALLBACK ThreadTemplate (void *functionData)
{	
	char errmsg[ERRLEN] = {0};
	
	ApplicationWindow gMainWindow = *(ApplicationWindow *) functionData;
	ERRORINFO errInfo = {0};
	TSUIObj_Executions executions = 0;
	long count = 0;
	TSUIObj_Execution uiExec = 0;
	CAObjHandle execHandle = 0;
	CAObjHandle seqFileHandle = 0;
	
	// Continuously poll for execution
	while (1)
	{
        TSUI_ApplicationMgrGetExecutions (gMainWindow.applicationMgr, &errInfo, &executions);
        TSUI_ExecutionsGetCount (executions, &errInfo, &count);
		
        if (count > 0)
        {
            // Get first execution and sequence file handles
            TSUI_ExecutionsGetItem (executions, &errInfo, 0, &uiExec);
            execHandle = uiExec;
            TS_ExecutionGetSequenceFile (execHandle, &errInfo, &seqFileHandle);
			
			// Get run state
			enum TSEnum_ExecutionRunStates runState = 0;
			enum TSEnum_ExecutionTerminationStates terminationState = 0;
			TS_ExecutionGetStates (execHandle, &errInfo, &runState, &terminationState);
			
			// While sequence file is running, execute logic
            while (runState == 1)
            { 
				// Thread logic goes here
                
				TS_ExecutionGetStates (execHandle, &errInfo, &runState, &terminationState);
            }
			
			CA_DiscardObjHandle (seqFileHandle);
            CA_DiscardObjHandle (uiExec);
        }
		
		CA_DiscardObjHandle (executions);
		
		// If terminate enabled, exit thread
		if (glbTerminate)
		{
			CmtExitThreadPoolThread (0);
			break;
		}
		
		DelayWithEventProcessing (0.1);
	}
	return 0;
}

/***************************************************************************//*!
* \brief EStop error injection thread
*
* \param [in] functionData		A void pointer to GMainWindow
*******************************************************************************/
int CVICALLBACK Thread_EStop (void *functionData)
{
	printf ("Entered Thread_EStop\n");
	
	char errmsg[ERRLEN] = {0};
	
	ApplicationWindow gMainWindow = *(ApplicationWindow *) functionData;
	ERRORINFO errInfo = {0};
	TSUIObj_Executions executions = 0;
	long count = 0;
	TSUIObj_Execution uiExec = 0;
	CAObjHandle execHandle = 0;
	CAObjHandle seqFileHandle = 0;
	
	// Continuously poll for execution
	while (1)
	{
        TSUI_ApplicationMgrGetExecutions (gMainWindow.applicationMgr, &errInfo, &executions);
        TSUI_ExecutionsGetCount (executions, &errInfo, &count);
		
        if (count > 0)
        {
            // Get first execution and sequence file handles
            TSUI_ExecutionsGetItem (executions, &errInfo, 0, &uiExec);
            execHandle = uiExec;
            TS_ExecutionGetSequenceFile (execHandle, &errInfo, &seqFileHandle);
			
			// Get run state
			enum TSEnum_ExecutionRunStates runState = 0;
			enum TSEnum_ExecutionTerminationStates terminationState = 0;
			TS_ExecutionGetStates (execHandle, &errInfo, &runState, &terminationState);
			
			// While sequence file is running, execute logic
            while (runState == 1)
            { 
				// 20250821Michael TODO: ESTOP logic goes here
				printf( "\tESTOP Logic Start\n");
				
				char TSMsgParams[16][512] = {0};
				char seqFileHandleStr[512] = {0};
				sprintf (seqFileHandleStr, "%d", seqFileHandle);
				char seqName[512] = "MainSequence";
				
				strcpy (TSMsgParams[0], seqFileHandleStr);
				strcpy (TSMsgParams[1], seqName);
				TSMsg_VAR_GET (0, TSMsgParams);
				
				printf( "\tESTOP Logic End\n");
				
                
				TS_ExecutionGetStates (execHandle, &errInfo, &runState, &terminationState);
            }
			
			CA_DiscardObjHandle (seqFileHandle);
            CA_DiscardObjHandle (uiExec);
        }
		
		CA_DiscardObjHandle (executions);
		
		// If terminate enabled, exit thread
		if (glbTerminate)
		{
			CmtExitThreadPoolThread (0);
			break;
		}
		
		DelayWithEventProcessing (0.1);
	}
	
	printf ("Exited Thread_EStop\n");
	return 0;
}
//! \cond
/// REGION END
//! \endcond