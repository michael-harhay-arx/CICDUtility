//! \cond CVIONLY
/***************************************************************************//*!
* \file CVIUIMessaging.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 10/26/2024 6:20:39 PM
* \brief Handlers for CVI UI Messages
* 
* Checks for CVI UI messages queued by the CVI Sequencer and processes them with
* 	RunMsgFn()
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

#include "toolbox.h"
#include <utility.h>
#include "SystemLog_LIB.h"
#include "CVISequencer.h"

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

/***************************************************************************//*!
* \brief Flag used to exit the messaging threads when the application is closing
*******************************************************************************/
int glbCVIMsgExitFlag = 0;
//!@{
/***************************************************************************//*!
* \brief Thread locks to prevent race conditions between the UI and Sequencer
*******************************************************************************/
CmtThreadLockHandle glbMsgLock[MAX_NESTS+1] = {0};
//!@}
//==============================================================================
// Static functions

static int __compare_int (void *first, void *second)
{
	return (*(int *)first) == (*(int *)second);
}

//==============================================================================
// Global variables

//!@{
/***************************************************************************//*!
* \brief Message queues
*******************************************************************************/
CVIMessageManager glbCVIMsgManager[MAX_NESTS+1];
//!@}
/***************************************************************************//*!
* \brief Static storage of nest numbers used as callback data for the messaging
* 	functions CVIMessageSender() and CVILEDSender().
*******************************************************************************/
int glbMessageNestNums[MAX_NESTS+1] = {0};

//==============================================================================
// Global functions

void RunMsgFn (int NestNum, int UIMsgEvent, char *UIMsgStringData, int CallerSeq, char *SeqName);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Checks the message queue and processes the messages
*******************************************************************************/
int CVICALLBACK CVIMessageSender (void *FunctionData)
{
	int nestNum = *(int*)FunctionData;
	CVIMessageQueue *receivedMessage = NULL;
	int handled = 0;
	
	LLInit (glbCVIMsgManager[nestNum], __compare_int);
	
	while (1)
	{
		LLAccess (glbCVIMsgManager[nestNum], handled, receivedMessage);
		if (receivedMessage != NULL)
		{
			RunMsgFn (receivedMessage->NestNum, receivedMessage->UIMsgEvent, receivedMessage->Msg, 0, 0);
			receivedMessage->handle = 1; /* Indicate handled. */
		}
		receivedMessage = NULL;
		
		ProcessSystemEvents ();
		
		if (glbCVIMsgExitFlag)
		{
			CmtExitThreadPoolThread (0);
			break;
		}
		//DelayWithEventProcessing (0.001);
	}
	return 0;
}
//! \cond
/// REGION END
//! \endcond
//! \endcond CVIONLY