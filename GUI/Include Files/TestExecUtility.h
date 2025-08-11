/***************************************************************************//*!
* \file TestExecUtility.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/1/2021 3:21:01 PM
*******************************************************************************/

#ifndef __TestExecUtility_H__
#define __TestExecUtility_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

// Arxtron Functions
int ArxUtil_LoadPanelsInMemory (void);
int ArxUtil_InitTSPanel (void);
int ArxUtil_InitExecPanel (void);
int ArxUtil_LocalizePanels (void);
int ArxUtil_StartSequencer (void);
int ArxUtil_UpdateStructsToTS (void);

// Template Functions
void ClearErrorMessage(void);
void DisplayError(int errorCode);
int ShowAppropriateTabs(void);
int ShowReport(CAObjHandle execution);
int UpdateWindowTitle(void);
int ArrangeControls(int processEvents);
int BuildSequenceViewContextMenu(int menuHandle);
int GetActiveViewManager(CAObjHandle *viewManager);
int RebuildEditMenu();
int RebuildMenuBar(int menuBar);
int ShowAppropriateStatusBarPanes(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExecUtility_H__ */
