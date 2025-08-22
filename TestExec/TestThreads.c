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
#include "toolbox.h"
#include "ArxtronToolslib.h"

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

static int libInitialized = 0;

static CmtThreadPoolHandle glbBackgroundThreads = 0;
static CmtThreadFunctionID glbBackgroundThreadIDs[MAX_BACKGROUND_THREADS] = {0};

static int glbTerminate = 0;

static char glbErrorMsgs[MAX_POPUP_MSGS][256] = {0};
static char glbWarningMsgs[MAX_POPUP_MSGS][256] = {0};

//==============================================================================
// Static functions

//==============================================================================
// Global variables

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
*******************************************************************************/
int StartTestThreads (char errmsg[ERRLEN])
{
	libInit;
	
	int threadIndex = 0;
	CmtErrChk (CmtNewThreadPool(MAX_BACKGROUND_THREADS, &glbBackgroundThreads));
	CmtErrChk (CmtScheduleThreadPoolFunction(glbBackgroundThreads, Thread_EStop		, 0, glbBackgroundThreadIDs+threadIndex++));
	//CmtErrChk (CmtScheduleThreadPoolFunction(glbBackgroundThreads, Thread_EStop		, 0, glbBackgroundThreadIDs+threadIndex++));
	
	// Custom prj menus
	//CmtErrChk (CmtScheduleThreadPoolFunction(glbBackgroundThreads, Thread_MonitorPrjMenus			, 0, glbBackgroundThreadIDs+threadIndex++));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int CVICALLBACK ThreadTemplate (void *functionData)
{
	while (1)
	{
		
		
		if (glbTerminate)
		{
			CmtExitThreadPoolThread (0);
			break;
		}
		
		DelayWithEventProcessing (0.001);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Estop injection thread function
*******************************************************************************/
int CVICALLBACK Thread_EStop (void *functionData)
{
	char errmsg[ERRLEN] = {0};
	int estop = 0;
	
	while (1)
	{
		// 20250821Michael TODO: Get ESTOP with GUI Var Get function
		if (estop && 0==strlen(glbErrorMsgs[E_ESTOP]))
		{
			strcpy (glbErrorMsgs[E_ESTOP], "ESTOP ACTIVE");
		}
		else if (0==estop && strlen(glbErrorMsgs[E_ESTOP]))
		{
			glbErrorMsgs[E_ESTOP][0] = 0;
		}
		
		if (glbTerminate)
		{
			CmtExitThreadPoolThread (0);
			break;
		}
		
		DelayWithEventProcessing (0.001);
	}
	
	return 0;
}

///***************************************************************************//*!
//* \brief OLD
//*******************************************************************************/
//DWORD WINAPI Thread_EStop_OLD (LPVOID lpParam)
//{
//	DWORD exitCode = 0;
//	
//	int paramVal = *(int *) lpParam;
//	VBOOL eStopHigh = 0;
//	int continueFlag = 0;
//	
//	// Inject EStop midway into the sequence
//	//TS_ThreadGetSequenceContext (
//	TS_PropertyGetValBoolean (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, &eStopHigh);
//	if (!eStopHigh)
//	{
//		eStopHigh = -1;
//		TS_PropertySetValBoolean (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, eStopHigh);
//	}
//	
//	// TODO: Check if sequence has stopped
//	//TS_PropertyGetValNumber (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, continueFlag);
//	//ASSERT_EQ_INT (0, continueFlag)
//	
//	// Close thread
//    return exitCode;
//}
//! \cond
/// REGION END
//! \endcond