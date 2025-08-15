/***************************************************************************//*!
* \file TestExecUtils.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-15 5:04:32 PM
*******************************************************************************/

#ifndef __TestExecUtils_H__
#define __TestExecUtils_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "TestExec.h"

//==============================================================================
// Constants
		
/***************************************************************************//*!
* \brief The Edit menu is rebuild both by RebuildMenuBar and RebuildEditMenu, 
*		 so this array is at file scope so they can both access it
*******************************************************************************/
static enum TSUIEnum_CommandKinds sEditMenuCommands[] = 
{
	TSUIConst_CommandKind_DefaultEditMenu_Set,			// add all the usual commands in an Edit menu
	TSUIConst_CommandKind_NotACommand					// list terminator
};

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

static int ShowReport(CAObjHandle execution);
static int MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2);
static void	MenuDimmerCallback(int menuBarHandle, int panelHandle);
static int UpdateWindowTitle();
static int BuildSequenceViewContextMenu(int menuHandle);
static int ShowAppropriateTabs(void);
static int ShowAppropriateStatusBarPanes(void);
static void ClearErrorMessage(void);
static void DisplayError(int errorCode);
static int GetActiveViewManager(CAObjHandle *viewManager);
static int RebuildMenuBar(int menuBar);
static int RebuildEditMenu();
static int GetActiveXControlHandles(void);
static int RegisterActiveXEventCallbacks(void);
static int ConnectTestStandControls();
static int ConnectStatusBarPanes(void);
static int ArrangeControls(int processEvents);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExecUtils_H__ */
