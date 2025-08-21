/***************************************************************************//*!
* \file Callbacks.h
* \author Michael Harhay
* \copyright . All Rights Reserved.
* \date 2025-08-15 4:12:48 PM
*******************************************************************************/

#ifndef __Callbacks_H__
#define __Callbacks_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "tsutil.h"			// helpful functions for using TestStand APIs in CVI

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

HRESULT CVICALLBACK ApplicationMgr_OnQueryShutdown(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_QueryShutdownOptions *opt);
HRESULT CVICALLBACK ApplicationMgr_OnExitApplication(CAObjHandle caServerObjHandle, void *caCallbackData);
HRESULT CVICALLBACK ApplicationMgr_OnWait(CAObjHandle caServerObjHandle, void *caCallbackData, VBOOL  bShowWait);
HRESULT CVICALLBACK ApplicationMgr_OnDisplayReport(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution execution);
HRESULT CVICALLBACK ApplicationMgr_OnDisplayExecution(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution execution, enum TSUIEnum_ExecutionDisplayReasons reason);
HRESULT CVICALLBACK ApplicationMgr_OnEditModeChanged(CAObjHandle caServerObjHandle, void *caCallbackData);
HRESULT CVICALLBACK ApplicationMgr_OnPostCommandExecute(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Command  command);
HRESULT CVICALLBACK ApplicationMgr_OnDisplaySequenceFile(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_SequenceFile file, enum TSUIEnum_SequenceFileDisplayReasons reason);
HRESULT CVICALLBACK ApplicationMgr_OnReportError(CAObjHandle caServerObjHandle, void *caCallbackData, long  errorCode, char *errorMessage);
HRESULT CVICALLBACK ApplicationMgr_OnStartExecution(CAObjHandle caServerObjHandle, void* caCallBackData, TSUIObj_Execution execution, TSUIObj_Thread thread,  VBOOL initiallyHidden);
HRESULT CVICALLBACK ExecutionViewMgr_OnExecutionChanged(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution  execution);
HRESULT CVICALLBACK SequenceFilesView_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
 															TSUIType_OLE_XPOS_PIXELS x, TSUIType_OLE_YPOS_PIXELS y);
HRESULT CVICALLBACK ExecutionView_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y);
HRESULT CVICALLBACK ListBar_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y);
HRESULT CVICALLBACK SequencesList_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y);
HRESULT CVICALLBACK ListBar_OnCurPageChanged(CAObjHandle caServerObjHandle, void *caCallbackData, long  currentPage);
HRESULT CVICALLBACK StatusBar_OnResized(CAObjHandle caServerObjHandle, void *caCallbackData, short  newWidth, short newHeight);

HRESULT CVICALLBACK ListBar_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, 
											TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              				TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL  finalResize);
HRESULT CVICALLBACK SequenceFilesView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK FileVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK InsertionPalette_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK ExecutionView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK ExecutionVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK CallStack_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK Label_OnConnectionActivity(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_ConnectionActivityTypes  activity);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Callbacks_H__ */
