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
#include "TestExecExecute_DEF.h"
#include "TSMessaging.h"
#include "TestThreads.h"

//==============================================================================
// Constants

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

	// Start thread pool
	int threadIndex = 0;
	
	CmtErrChk (CmtNewThreadPool (MAX_BACKGROUND_THREADS, &glbBackgroundThreads));
	
	// Call ESTOP thread
	ThreadParams *paramsESTOP = malloc (sizeof (ThreadParams));
    paramsESTOP->mainWindow = GMainWindow;
    paramsESTOP->logicFn = ThreadLogic_ESTOP;
	CmtErrChk (CmtScheduleThreadPoolFunction (glbBackgroundThreads, GenericTestThread, paramsESTOP, glbBackgroundThreadIDs+threadIndex++));
	
	// Call FailChuteFull thread
	/*ThreadParams *paramsFailChute = malloc (sizeof (ThreadParams));
    paramsFailChute->mainWindow = GMainWindow;
    paramsFailChute->logicFn = ThreadLogic_FailChuteFull;
	CmtErrChk (CmtScheduleThreadPoolFunction (glbBackgroundThreads, GenericTestThread, paramsFailChute, glbBackgroundThreadIDs+threadIndex++));*/
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Generic thread function, which calls thread-specific logic
*******************************************************************************/
int CVICALLBACK GenericTestThread (void *functionData)
{	
	char errmsg[ERRLEN] = {0};
	
	ThreadParams params = *(ThreadParams *) functionData;
	ApplicationWindow gMainWindow = params.mainWindow;
	TS_ThreadLogicFn logicFn = params.logicFn;
	
	ERRORINFO errInfo = {0};
	TSUIObj_Executions executions = 0;
	long count = 0;
	TSUIObj_Execution uiExec = 0;
	CAObjHandle execHandle = 0;
	CAObjHandle threadHandle = 0;
	CAObjHandle seqContext = 0;
	CAObjHandle seqHandle = 0;
	char *seqName = NULL;
			
	// Loop and get current executions
	while (1)
	{	
		TSUI_ApplicationMgrGetExecutions (gMainWindow.applicationMgr, &errInfo, &executions);
        TSUI_ExecutionsGetCount (executions, &errInfo, &count);
		
		// If executing, get relevant handles and call thread-specific logic when applicable
		if (!glbTerminate && count > 1)
		{
            TSUI_ExecutionsGetItem (executions, &errInfo, 1, &uiExec);
            execHandle = uiExec;
			TS_ExecutionGetThread (execHandle, &errInfo, 0, &threadHandle);
			TS_ThreadGetSequenceContext (threadHandle, &errInfo, 0, 0, &seqContext);
			TS_SeqContextGetSequence (seqContext, &errInfo, &seqHandle);
			TS_SequenceGetName (seqHandle, &errInfo, &seqName);
			
			// Get run state
			enum TSEnum_ExecutionRunStates runState = 0;
			enum TSEnum_ExecutionTerminationStates terminationState = 0;
			TS_ExecutionGetStates (execHandle, &errInfo, &runState, &terminationState);
			
			// While sequence file is running, execute logic
            if (runState == 1 && strcmp (seqName, "MainSequence") == 0)
            { 
				printf ("Currently running sequence: %s\n", seqName);
				logicFn (execHandle, seqHandle); // calls thread-specific logic
                
				TS_ExecutionGetStates (execHandle, &errInfo, &runState, &terminationState);
				DelayWithEventProcessing(0.001);
				break;
            }
			
			CA_DiscardObjHandle (uiExec);
			CA_DiscardObjHandle (threadHandle);
			CA_DiscardObjHandle (seqContext);
			CA_DiscardObjHandle (seqHandle);
		}
		
		CA_DiscardObjHandle (executions);
		DelayWithEventProcessing (0.001);
	}	
	
	CA_DiscardObjHandle (executions);	
	free (functionData);
	CmtExitThreadPoolThread (0);
	return 0; // suppress errors
}

/***************************************************************************//*!
* \brief EStop error injection thread
*******************************************************************************/
void ThreadLogic_ESTOP (CAObjHandle execHandle, CAObjHandle seqHandle)
{
    printf("\tESTOP Logic Start\n");
	
	TSSeqVar tmpSeqVar;
	memset(&tmpSeqVar, 0, sizeof(tmpSeqVar));
	int hasLock = 0;
	
	// Timing logic
	DelayWithEventProcessing (0.5);
	
	// Set tmpSeqVar, then lock thread before actually applying change to ESTOP value
	SetTSSeqVarStruct (tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, "StationGlobals.GlobalAlarmConditions.EStopHigh", VARTYPE_BOOL, VTRUE);
	
	CmtGetLockEx (glbSetVarLock, 1, CMT_WAIT_FOREVER, &hasLock);
	if (hasLock)
	{
		strcpy (glbSetVars[glbNumSetting].lookupString, tmpSeqVar.lookupString);
		strcpy (glbSetVars[glbNumSetting].seqName, tmpSeqVar.seqName);
		glbSetVars[glbNumSetting].varType = tmpSeqVar.varType;
		strcpy (glbSetVars[glbNumSetting].varVal, tmpSeqVar.varVal);
		glbNumSetting++;
		CmtReleaseLock (glbSetVarLock);
	}
	printf("\t\tESTOP set to: True\n");
	
	// Check if ESTOP correctly activated
	DelayWithEventProcessing (0.001);
	
    printf("\tESTOP Logic End\n");
}

/***************************************************************************//*!
* \brief Fail chute full injection thread
*******************************************************************************/
void ThreadLogic_FailChuteFull (CAObjHandle execHandle, CAObjHandle seqFileHandle)
{
    printf("\tFailChuteFull Logic Start\n");

    char TSMsgParams[16][512] = {0};
    char seqFileHandleStr[512] = {0};
    sprintf (seqFileHandleStr, "%d", seqFileHandle);
	
    strcpy (TSMsgParams[0], seqFileHandleStr);
    strcpy (TSMsgParams[1], "MainSequence");
    TSMsg_VAR_GET (0, TSMsgParams);
	
	// Wait 2 seconds, activate FailChuteFull
	DelayWithEventProcessing (2.0);
	
	// If not yet set, set FailChuteFull to high
	if (strcmp (glbWatchVars[0].varVal, "False") == 0)
	{
		TSMsg_VAR_SET (0, TSMsgParams);
	}

    printf("\tFailChuteFull Logic End\n");
}
//! \cond
/// REGION END
//! \endcond