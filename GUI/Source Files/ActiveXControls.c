//! \cond TESTSTAND
/***************************************************************************//*!
* \file ActiveXControls.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/3/2021 5:11:45 PM
* \brief Contains the ActiveXControl functions related to connecting CVI and
* 	TestStand.  This is copied directly from the example custom UI and has very
* 	few if any customized changes.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "CommonTSIncludes.h"
#ifndef CVIUI_ONLY
#include "tsuisupp.h"		// support API for the TestStand ActiveX controls
#include "ActiveXControls.h"

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

HRESULT CVICALLBACK TSMsg_ApplicationMgr_OnUserMessage(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_UIMessage uiMsg);

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

HRESULT CVICALLBACK ListBar_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL  finalResize);
HRESULT CVICALLBACK SequenceFilesView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK ExecutionView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK FileVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK CallStack_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK ExecutionVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK InsertionPalette_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                       TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
HRESULT CVICALLBACK Label_OnConnectionActivity(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_ConnectionActivityTypes  activity);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Get handles to ActiveX controls in order to call their particular API's
*******************************************************************************/
int GetActiveXControlHandles(void)
{
	int	error = 0;
	
	// the invisible ApplicationManager control creates and destroys the TestStand engine, implements login/logout and shutdown
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.tspanel, DEBUGPANEL_APPLICATIONMGR, &gMainWindow.applicationMgr));
	
	// the invisible SequenceFileViewManager control holds the selected sequence file
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.tspanel, DEBUGPANEL_SEQUENCEFILEVIEWMGR, &gMainWindow.sequenceFileViewMgr));

	// the ExecutionViewManager control manages the display of the selected execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.tspanel, DEBUGPANEL_EXECUTIONVIEWMGR, &gMainWindow.executionViewMgr));

	// the ListBar control lets the user select a sequence file or an execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.tspanel, DEBUGPANEL_LISTBAR, &gMainWindow.listBar));

	// status bar
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.panel, MAINPANEL_STATUSBAR, &gMainWindow.statusBar));
	
	// TABPANEL_STEPLISTVIEW displays the steps in the selected sequence file
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, &gMainWindow.fileStepListView));

	// TABPANEL_2_STEPLISTVIEW displays the steps in the selected execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, &gMainWindow.executionStepListView));

	// the REPORTVIEW control displays the report for the selected execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.reportTab, TABPANEL_3_REPORTVIEW, &gMainWindow.reportView));

	// the SEQUENCESLIST on the fileTab specifies which sequence in the sequence file to display
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_SEQUENCES, &gMainWindow.fileSequencesList));

	// the VARIABLES on the fileTab displays the variables for the selected sequence file
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_VARIABLES, &gMainWindow.fileVariables));
	
	// the INSERTIONPALETTE on the fileTab displays a palette of items that can be inserted into the selected sequence file
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, &gMainWindow.insertionPalette));
	
	// the SEQUENCEFILELABEL on the fileTab is hidden. It is used to receive ConnectionActivity events whenever the current sequence file caption changes, so we can update the title bar
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_SEQUENCEFILELABEL, &gMainWindow.sequenceFileLabel));

	// buttons on the file tab
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_ENTRYPOINT1BTN, &gMainWindow.entryPoint1Button));
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_ENTRYPOINT2BTN, &gMainWindow.entryPoint2Button));
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.fileTab, TABPANEL_RUNSEQUENCEBTN, &gMainWindow.runCurrentSequenceButton));
	
	
	// the VARIABLES on the executionTab displays the variables for the selected execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_VARIABLES, &gMainWindow.executionVariables));
	

	// the THREADS, and CALLSTACK controls on the executionTab indicate which part of the sequence file is executing
	// and let you navigate within the suspended sequences at a breakpoint
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_THREADS, &gMainWindow.threads));
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_CALLSTACK, &gMainWindow.callStack));

	// buttons on the executionTab
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_BREAKRESUMEBTN, &gMainWindow.breakResumeButton));
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_TERMINATERESTARTBTN, &gMainWindow.terminateRestartButton));
	
	// the EXECUTIONLABEL on the executionTab is hidden. It is used to receive ConnectionActivity events whenever the current execution caption changes, so we can update the title bar
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.executionTab, TABPANEL_2_EXECUTIONLABEL, &gMainWindow.executionLabel));

Error:
	return error;
}

/***************************************************************************//*!
* \brief Installs all UI ActiveX Callbacks
*******************************************************************************/
int RegisterActiveXEventCallbacks(void)
{
	int	error = 0;
	
	// ApplicationMgr event callbacks
	errChk( TSUI__ApplicationMgrEventsRegOnQueryShutdown(gMainWindow.applicationMgr, 		ApplicationMgr_OnQueryShutdown, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnExitApplication(gMainWindow.applicationMgr, 		ApplicationMgr_OnExitApplication, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnWait(gMainWindow.applicationMgr, 				ApplicationMgr_OnWait, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnDisplayReport(gMainWindow.applicationMgr, 		ApplicationMgr_OnDisplayReport, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnReportError(gMainWindow.applicationMgr, 			ApplicationMgr_OnReportError, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnStartExecution(gMainWindow.applicationMgr, 		ApplicationMgr_OnStartExecution, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnDisplaySequenceFile(gMainWindow.applicationMgr, 	ApplicationMgr_OnDisplaySequenceFile, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnDisplayExecution(gMainWindow.applicationMgr, 	ApplicationMgr_OnDisplayExecution, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnEditModeChanged(gMainWindow.applicationMgr, 		ApplicationMgr_OnEditModeChanged, NULL, 1, NULL));
	errChk( TSUI__ApplicationMgrEventsRegOnPostCommandExecute(gMainWindow.applicationMgr, 	ApplicationMgr_OnPostCommandExecute, NULL, 1, NULL));
	
	// ArxCustom: Messaging Callbacks
	errChk( TSUI__ApplicationMgrEventsRegOnUserMessage(gMainWindow.applicationMgr,          TSMsg_ApplicationMgr_OnUserMessage, NULL, 1, NULL));
	//errChk( TSUI__ApplicationMgrEventsRegOnAfterUIMessageEvent(gMainWindow.applicationMgr,  ApplicationMgr_OnAfterUIMessage, NULL, 1, NULL));
	//errChk( TSUI__ApplicationMgrEventsRegOnUIMessageEvent (gMainWindow.applicationMgr,      ApplicationMgr_OnUIMessage, NULL, 1, NULL));
	
	// ExecutionViewMgr event callbacks
	errChk( TSUI__ExecutionViewMgrEventsRegOnExecutionChanged(gMainWindow.executionViewMgr, ExecutionViewMgr_OnExecutionChanged, NULL, 1, NULL));	
	
  	// ListBar event callbacks
  	errChk( TSUI__ListBarEventsRegOnCurPageChanged(gMainWindow.listBar, 					ListBar_OnCurPageChanged, NULL, 1, NULL));
	errChk( TSUI__ListBarEventsRegOnCreateContextMenu(gMainWindow.listBar, 					ListBar_OnCreateContextMenu, NULL, 1, NULL));	
	errChk( TSUI__ListBarEventsRegOnBorderDragged(gMainWindow.listBar, 						ListBar_OnBorderDragged, NULL, 1, NULL));	
	
	// SequenceView callbacks
	errChk( TSUI__SequenceViewEventsRegOnCreateContextMenu(gMainWindow.fileStepListView, 	SequenceFilesView_OnCreateContextMenu, NULL, 1, NULL));	
	errChk( TSUI__SequenceViewEventsRegOnCreateContextMenu(gMainWindow.executionStepListView,	ExecutionView_OnCreateContextMenu, NULL, 1, NULL));	
	errChk( TSUI__SequenceViewEventsRegOnBorderDragged(gMainWindow.fileStepListView,		SequenceFilesView_OnBorderDragged, NULL, 1, NULL));
	errChk( TSUI__SequenceViewEventsRegOnBorderDragged(gMainWindow.executionStepListView,	ExecutionView_OnBorderDragged, NULL, 1, NULL));
	
	// ListBox event callbacks
	errChk( TSUI__ListBoxEventsRegOnBorderDragged(gMainWindow.callStack, 					CallStack_OnBorderDragged, NULL, 1, NULL));
	errChk( TSUI__ListBoxEventsRegOnCreateContextMenu(gMainWindow.fileSequencesList, 		SequencesList_OnCreateContextMenu, NULL, 1, NULL));	
	
	
	// Variables view event callbacks
	errChk( TSUI__VariablesViewEventsRegOnBorderDragged(gMainWindow.fileVariables, 			FileVariables_OnBorderDragged, NULL, 1, NULL));
	errChk( TSUI__VariablesViewEventsRegOnBorderDragged(gMainWindow.executionVariables, 	ExecutionVariables_OnBorderDragged, NULL, 1, NULL));
	
	// Insertion Palette event callbacks
	errChk( TSUI__InsertionPaletteEventsRegOnBorderDragged(gMainWindow.insertionPalette, 	InsertionPalette_OnBorderDragged, NULL, 1, NULL));

	// StatusBar event callbacks
	errChk( TSUI__StatusBarEventsRegOnResized(gMainWindow.statusBar, 						StatusBar_OnResized, NULL, 1, NULL));
	
	// Label event callbacks (these are both for updating the title bar, so both controls can share the same callback function)
	errChk( TSUI__LabelEventsRegOnConnectionActivity(gMainWindow.sequenceFileLabel, 		Label_OnConnectionActivity, NULL, 1, NULL));	
	errChk( TSUI__LabelEventsRegOnConnectionActivity(gMainWindow.executionLabel, 			Label_OnConnectionActivity, NULL, 1, NULL));	
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Connects the status bar to display various things
*******************************************************************************/
int ConnectStatusBarPanes(void)
{
	
	int			error = 0;
	CAObjHandle panes = 0;
	CAObjHandle paneUser = 0;
	CAObjHandle paneEngineEnvironment = 0;  
	CAObjHandle paneFileModel = 0;
	CAObjHandle paneExecutionModel = 0;
	CAObjHandle paneFileSelectedSteps = 0;
	CAObjHandle paneFileNumberOfSteps = 0;
	CAObjHandle paneExecutionSelectedSteps = 0;
	CAObjHandle paneExecutionNumberOfSteps = 0;
	CAObjHandle paneReportLocation = 0;
	CAObjHandle paneProgressText = 0;
	CAObjHandle paneProgressPercent = 0;

	int			paneIndex = 0;
	CAObjHandle	fileModelCaptionConnection = 0;
	CAObjHandle	executionModelCaptionConnection = 0;

	// get the status bar panes so we can connect them to various captions
	tsErrChk( TSUI_StatusBarGetPanes(gMainWindow.statusBar, &errorInfo, &panes));
	
	// IMPORTANT: the order of these calls must match the order of the panes as configured in the status bar 
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneUser));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneEngineEnvironment));    
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneFileModel));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneExecutionModel));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneFileSelectedSteps));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneFileNumberOfSteps));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneExecutionSelectedSteps));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneExecutionNumberOfSteps));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneReportLocation));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneProgressText));
	tsErrChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneProgressPercent));

	// User 
	tsErrChk( TSUI_ApplicationMgrConnectCaption(gMainWindow.applicationMgr, &errorInfo, paneUser, TSUIConst_CaptionSource_UserName, VFALSE, NULL));
	// Engine Environment 
	tsErrChk( TSUI_ApplicationMgrConnectCaption(gMainWindow.applicationMgr, &errorInfo, paneEngineEnvironment, TSUIConst_CaptionSource_EngineEnvironment, VFALSE, NULL))	
	// File Process Model
	tsErrChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, paneFileModel, TSUIConst_CaptionSource_CurrentProcessModelFile, VTRUE, &fileModelCaptionConnection));
	tsErrChk( TSUISUPP_CaptionConnectionSetLongName(fileModelCaptionConnection, &errorInfo, VFALSE)); // just show the base file name
	// Execution Process Model
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneExecutionModel, TSUIConst_CaptionSource_CurrentProcessModelFile, VTRUE, &executionModelCaptionConnection));
	tsErrChk( TSUISUPP_CaptionConnectionSetLongName(executionModelCaptionConnection, &errorInfo, VFALSE));	// just show the base file name
	// File Selected Steps
	tsErrChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, paneFileSelectedSteps, TSUIConst_CaptionSource_SelectedSteps_ZeroBased, VFALSE, NULL));
	// File Number of Steps
	tsErrChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, paneFileNumberOfSteps, TSUIConst_CaptionSource_NumberOfSteps, VTRUE, NULL));
	// Execution Selected Steps
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneExecutionSelectedSteps, TSUIConst_CaptionSource_SelectedSteps_ZeroBased, VFALSE, NULL));
	// Execution Number of Steps
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneExecutionNumberOfSteps, TSUIConst_CaptionSource_NumberOfSteps, VTRUE, NULL));
	// Report Location
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneReportLocation, TSUIConst_CaptionSource_ReportLocation, VTRUE, NULL));
	// Progress Text
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneProgressText, TSUIConst_CaptionSource_ProgressText, VFALSE, NULL));
	// Progress Percent Text
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneProgressPercent, TSUIConst_CaptionSource_ProgressPercent, VFALSE, NULL));
	// Progress Percent Bar
	tsErrChk( TSUI_ExecutionViewMgrConnectNumeric(gMainWindow.executionViewMgr, &errorInfo, paneProgressPercent, TSUIConst_NumericSource_ProgressPercent, NULL));

Error:
	CA_DiscardObjHandle(paneUser);
	CA_DiscardObjHandle(paneEngineEnvironment);  
	CA_DiscardObjHandle(paneFileModel);
	CA_DiscardObjHandle(fileModelCaptionConnection);
	CA_DiscardObjHandle(paneExecutionModel);
	CA_DiscardObjHandle(executionModelCaptionConnection);
	CA_DiscardObjHandle(paneFileSelectedSteps);
	CA_DiscardObjHandle(paneFileNumberOfSteps);
	CA_DiscardObjHandle(paneExecutionSelectedSteps);
	CA_DiscardObjHandle(paneExecutionNumberOfSteps);
	CA_DiscardObjHandle(paneReportLocation);
	CA_DiscardObjHandle(paneProgressText);
	CA_DiscardObjHandle(paneProgressPercent);
	CA_DiscardObjHandle(panes);
	
	return error;
}

/***************************************************************************//*!
* \brief Connect the sequence/executtion/report tabs of the teststand window to its various callbacks
*******************************************************************************/
int ConnectTestStandControls(void)
{
	int			error = 0;
	CAObjHandle	pages = 0;
	CAObjHandle	page = 0;
	CAObjHandle sequenceListConnection = 0;
	CAObjHandle executionListConnection = 0;
	
	
	// connect controls on the Sequence File tab
	
	// connect fileStepListView to SequenceFileViewMgr
	tsErrChk( TSUI_SequenceFileViewMgrConnectSequenceView(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.fileStepListView, NULL));

	// connect first entry point button 
	tsErrChk( TSUI_SequenceFileViewMgrConnectCommand(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.entryPoint1Button, TSUIConst_CommandKind_ExecutionEntryPoints_Set, 0, TSUIConst_CommandConnection_NoOptions, NULL));
	
	// connect second entry point button 
	tsErrChk( TSUI_SequenceFileViewMgrConnectCommand(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.entryPoint2Button, TSUIConst_CommandKind_ExecutionEntryPoints_Set, 1, TSUIConst_CommandConnection_NoOptions, NULL));

	// connect run-current-sequence button 
	tsErrChk( TSUI_SequenceFileViewMgrConnectCommand(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.runCurrentSequenceButton, TSUIConst_CommandKind_RunCurrentSequence, 0, TSUIConst_CommandConnection_NoOptions, NULL));
	
	// connect the hidden sequence file label
	tsErrChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.sequenceFileLabel, TSUIConst_CaptionSource_CurrentSequenceFile, VFALSE, NULL));
	
	// connect fileSequences listbox to SequenceList
	tsErrChk( TSUI_SequenceFileViewMgrConnectSequenceList(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.fileSequencesList, &sequenceListConnection));
	tsErrChk( TSUISUPP_SequenceListConnectionSetColumnVisible(sequenceListConnection, &errorInfo, TSUISUPPConst_SeqListConnectionColumn_Comments, VTRUE));
	
	// connect fileVariables to SequenceFileViewMgr
	tsErrChk( TSUI_SequenceFileViewMgrConnectVariables(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.fileVariables, NULL));

	// connect insertionPalette to SequenceFileViewMgr
	tsErrChk( TSUI_SequenceFileViewMgrConnectInsertionPalette(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.insertionPalette, NULL));
	
	
	// connect controls on the Execution tab

	// connect executionStepListView to ExecutionViewMgr
	tsErrChk( TSUI_ExecutionViewMgrConnectExecutionView(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.executionStepListView, TSUIConst_ExecutionViewConnection_NoOptions, NULL));

	// connect reportView to ExecutionViewMgr
	tsErrChk( TSUI_ExecutionViewMgrConnectReportView(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.reportView, NULL));

	// connect execution variables to ExecutionViewMgr
	tsErrChk( TSUI_ExecutionViewMgrConnectVariables(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.executionVariables, NULL));
	
	// connect callStack listbox to ExecutionViewMgr
	tsErrChk( TSUI_ExecutionViewMgrConnectCallStack(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.callStack, NULL));

	// connect thread listbox to ExecutionViewMgr
	tsErrChk( TSUI_ExecutionViewMgrConnectThreadList(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.threads, NULL));
	
	// connect break/resume button
	tsErrChk( TSUI_ExecutionViewMgrConnectCommand(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.breakResumeButton, TSUIConst_CommandKind_BreakResume, 0, TSUIConst_CommandConnection_NoOptions, NULL));

	// connect terminate/restart button
	tsErrChk( TSUI_ExecutionViewMgrConnectCommand(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.terminateRestartButton, TSUIConst_CommandKind_TerminateRestart, 0, TSUIConst_CommandConnection_NoOptions, NULL));

	// connect the hidden execution label
	tsErrChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.executionLabel, TSUIConst_CaptionSource_CurrentExecution, VFALSE, NULL));

		
	// connect listbar pages
	
	tsErrChk( TSUI_ListBarGetPages(gMainWindow.listBar, &errorInfo, &pages));

	// connect listbar page 0 to SequenceFileList
	tsErrChk( TSUI_ListBarPagesGetItem(pages, &errorInfo, CA_VariantLong(SEQUENCE_FILES_PAGE_INDEX), &page));
	tsErrChk( TSUI_SequenceFileViewMgrConnectSequenceFileList(gMainWindow.sequenceFileViewMgr, &errorInfo, page, VFALSE, NULL));	
	CA_DiscardObjHandle(page);
	
	// connect listbar page 1 to ExecutionList
	tsErrChk( TSUI_ListBarPagesGetItem(pages, &errorInfo, CA_VariantLong(EXECUTIONS_PAGE_INDEX), &page));
	tsErrChk( TSUI_ExecutionViewMgrConnectExecutionList(gMainWindow.executionViewMgr, &errorInfo, page, &executionListConnection));	
	// display the execution name on the first line, the serial number (if any) on the next line, the socket index (if any) on the next line, and the model execution state on the last line (the expression string looks complicated here because we have to escape the quotes and backslashes for the C compiler.)
	tsErrChk( TSUISUPP_ExecutionListConnectionSetDisplayExpression(executionListConnection, &errorInfo, "\"%CurrentExecution%\\n\" + (\"%UUTSerialNumber%\" == \"\" ? \"\" : (ResStr(\"TSUI_OI_MAIN_PANEL\",\"SERIAL_NUMBER\") + \" %UUTSerialNumber%\\n\")) + (\"%TestSocketIndex%\" == \"\" ? \"\" : ResStr(\"TSUI_OI_MAIN_PANEL\",\"SOCKET_NUMBER\") + \" %TestSocketIndex%\\n\") + \"%ModelState%\""));
	CA_DiscardObjHandle(page);

	errChk( ConnectStatusBarPanes());
	
Error:
	CA_DiscardObjHandle(sequenceListConnection);
	CA_DiscardObjHandle(executionListConnection);
	CA_DiscardObjHandle(pages);
	CA_DiscardObjHandle(page);
	
	return error;
}
//! \cond
/// REGION END
//! \endcond
#endif
//! \endcond TESTSTAND