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
		
#include "tsui.h"

#include "TestExec.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int ShowReport(CAObjHandle execution);
int MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2);
void	MenuDimmerCallback(int menuBarHandle, int panelHandle);
int UpdateWindowTitle();
int BuildSequenceViewContextMenu(int menuHandle);
int ShowAppropriateTabs(void);
int ShowAppropriateStatusBarPanes(void);
void ClearErrorMessage(void);
void DisplayError(int errorCode);
int GetActiveViewManager(CAObjHandle *viewManager);
int RebuildMenuBar(int menuBar);
int RebuildEditMenu();
int GetActiveXControlHandles(void);
int RegisterActiveXEventCallbacks(void);
int ConnectTestStandControls();
int ConnectStatusBarPanes(void);
int ArrangeControls(int processEvents);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExecUtils_H__ */
