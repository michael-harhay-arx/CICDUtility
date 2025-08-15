/***************************************************************************//*!
* \file Callbacks.c
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-15 4:12:48 PM
* \brief A short description.
* 
* A longer description.
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

#include "Callbacks.h"

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
* \brief Main panel callback 
*******************************************************************************/
int CVICALLBACK MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:		// EVENT_CLOSE indicates the user clicked on window close box
			ExitApplication(); 	// this function displays errors, if any occur
			break;
	}

	return 0;
}

/***************************************************************************//*!
* \brief Tab control callback
*******************************************************************************/
int CVICALLBACK TabControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			error = 0;
	CAObjHandle	execution = 0;
	int			pageIndex =0;

	switch (event)
	{
		case EVENT_ACTIVE_TAB_CHANGE:
			if (!gMainWindow.programmaticallyUpdatingTabPages)	// filter out programmatically triggered activation events that might be due only to hidden tabs being made visible again
			{
				tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));

				// remember which tab is active so when execution is revisited in the future, we can activate the same tab
				if (EXECUTIONS_PAGE_INDEX == pageIndex) // is the new tab an execution tab?
				{
					// store the activated tab in a custom property added to the execution
					tsErrChk( TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &execution));
					if (execution)
						tsErrChk( TS_PropertySetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, eventData2));
				}				
			}
				
			errChk(  ShowAppropriateStatusBarPanes());
		break;
	}
	
Error:
	CA_DiscardObjHandle(execution);
	DisplayError(error);
	return 0;
}

/***************************************************************************//*!
* \brief Release any TestStand objects and save any settings here
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnQueryShutdown(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_QueryShutdownOptions *opt)
{
	// remember window and control positions for next time
	TS_LayoutPersister_SaveSizes(gMainWindow.applicationMgr,  &errorInfo, NULL, 0, 10,
									gMainWindow.panel, MAINPANEL_LISTBAR, 
									gMainWindow.panel, MAINPANEL_TAB, 
									gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, 
									gMainWindow.fileTab, TABPANEL_SEQUENCES, 
									gMainWindow.fileTab, TABPANEL_VARIABLES, 
									gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, 
									gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, 
									gMainWindow.executionTab, TABPANEL_2_CALLSTACK, 
									gMainWindow.executionTab, TABPANEL_2_THREADS, 
									gMainWindow.executionTab, TABPANEL_2_VARIABLES);
	TS_LayoutPersister_SaveBounds(gMainWindow.applicationMgr, &errorInfo, NULL, 0, 1, 
									gMainWindow.panel, 0);


	return S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event when it is ok to exit the application
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnExitApplication(CAObjHandle caServerObjHandle, void *caCallbackData)
{
	// discard any current menu items that were inserted by TS_InsertCommandsInMenu. These menu items might refer to TestStand objects, so delete them before the engine is destroyed. Note that it is too early to do this in QueryShutdown because a menu might be used after QueryShutdown returns, particularly if an unload callback runs.
	if (gMainWindow.panel > 0)
		TS_RemoveMenuCommands(GetPanelMenuBar(gMainWindow.panel), 0, FALSE, TRUE);

	CA_DiscardObjHandle(gMainWindow.engine);
	gMainWindow.engine = 0;

	ExitApplication();  // since we have received the ExitApplication event, this call is guaranteed to cause an immediate exit
	return S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event when it's busy doing 
*		 something so we know to display a hourglass cursor or equivalent
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnWait(CAObjHandle caServerObjHandle, void *caCallbackData, VBOOL  bShowWait)
{	
	SetMouseCursor(bShowWait ? VAL_HOUR_GLASS_CURSOR : VAL_DEFAULT_CURSOR);
	return S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event to request that the UI 
*		 display the report for a particular execution
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnDisplayReport(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution execution)
{
	int	error = 0;
	
	errChk( ShowReport(execution));

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event to request that the UI 
*		 display a particular execution
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnDisplayExecution(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution execution, enum TSUIEnum_ExecutionDisplayReasons reason)
{
	int	error = 0;

	// bring application to front if we hit a breakpoint
	if (reason == TSUIConst_ExecutionDisplayReason_Breakpoint || reason == TSUIConst_ExecutionDisplayReason_BreakOnRunTimeError)
		errChk( SetActivePanel(gMainWindow.panel));

	// show this execution
	tsErrChk( TSUI_ExecutionViewMgrSetByRefExecution(gMainWindow.executionViewMgr, &errorInfo, execution));
	// show the executions page in the list bar
	tsErrChk( TSUI_ListBarSetCurrentPage(gMainWindow.listBar, &errorInfo, EXECUTIONS_PAGE_INDEX));
	// in case we are already showing the executions page, ensure we switch to steps or report tab as appropriate
	errChk( ShowAppropriateTabs());  
	
Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The user toggled edit mode. the only way to do that in this 
*		 application is to type ctrl-shift-alt-insert, which is the edit 
*		 mode toggle key this application specifies in designer for the 
*		 ApplicationMgr control. To prevent edit mode from being toggled 
*		 with a hotkey, set ApplicationMgr.EditModeShortcutKey to 
*		 ShortcutKey_VK_NOT_A_KEY
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnEditModeChanged(CAObjHandle caServerObjHandle, void *caCallbackData)
{
	int	error = 0;
	int width;
	
	errChk( GetCtrlAttribute(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, ATTR_WIDTH, &width));
	width = Max(width, 260);
	errChk( SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, ATTR_WIDTH, width));	// make sure the palette is wide enough in case it is going to be shown
	ArrangeControls(TRUE);	// relayout the controls to reflect the change in edit mode
	
Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief Application manager post command execute
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnPostCommandExecute(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Command  command)
{
	int							error = 0;
	enum TSUIEnum_CommandKinds	kind;
	CAObjHandle					executions = 0;
	long 						numberOfExecutions;

	tsErrChk( TSUI_CommandGetKind (command, &errorInfo, &kind));

	if (kind == TSUIConst_CommandKind_CloseCompletedExecutions)
	{
		tsErrChk( TSUI_ApplicationMgrGetExecutions(gMainWindow.applicationMgr, &errorInfo, &executions));
		tsErrChk( TSUI_ExecutionsGetCount(executions, &errorInfo, &numberOfExecutions));
		
		// if we closed all the executions, switch to the files page instead of showing an empty executions page
		if (numberOfExecutions == 0)
			tsErrChk( TSUI_ListBarSetCurrentPage(gMainWindow.listBar, &errorInfo, SEQUENCE_FILES_PAGE_INDEX));
	}
		
Error:
	CA_DiscardObjHandle(executions);
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event to request that the UI 
*		 display a particular sequence file
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnDisplaySequenceFile(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_SequenceFile file, enum TSUIEnum_SequenceFileDisplayReasons reason)
{
	int	error = 0;

	// show this sequence file
	tsErrChk( TSUI_SequenceFileViewMgrSetByRefSequenceFile(gMainWindow.sequenceFileViewMgr, &errorInfo, file));
	// show the sequence files page in the list bar
	tsErrChk( TSUI_ListBarSetCurrentPage(gMainWindow.listBar, &errorInfo, SEQUENCE_FILES_PAGE_INDEX));

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event to report any error that 
*		 occurs when the TestStand UI controls respond to user input 
*	     (ie, an error other than one returned by a method you call 
*		 directly). For example, if a TestStand menu command generates an 
*		 error, this handler displays it
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnReportError(CAObjHandle caServerObjHandle, void *caCallbackData, long  errorCode, char *errorMessage)
{
	strcpy(errMsg, errorMessage);	// update global errMsg buffer
	DisplayError(errorCode);
	return S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event whenever an execution starts
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnStartExecution(CAObjHandle caServerObjHandle, void* caCallBackData, TSUIObj_Execution execution, TSUIObj_Thread thread,  VBOOL initiallyHidden)
{
	int	error = 0;

	// add a custom property to the execution to store which tab we are displaying for this execution. Initially show the execution tab
	tsErrChk( TS_PropertySetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, TABPAGE_EXECUTION));

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The ExecutionViewMgr sends this event whenever a new execution is selected
*******************************************************************************/
HRESULT CVICALLBACK ExecutionViewMgr_OnExecutionChanged(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution  execution)
{
	int	error = 0;

	// switch to report or steps tab depending on what the execution displayed last
	errChk( ShowAppropriateTabs());
	
Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The TABPANEL_STEPLISTVIEW control sends this event when the user right clicks on it
*******************************************************************************/
HRESULT CVICALLBACK SequenceFilesView_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
 															TSUIType_OLE_XPOS_PIXELS x, TSUIType_OLE_YPOS_PIXELS y)
{
	int	error = 0;
		
	errChk( BuildSequenceViewContextMenu(menuHandle));
	
Error:		
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The TABPANEL_2_STEPLISTVIEW ctrl sends this event when the user right clicks on it
*******************************************************************************/
HRESULT CVICALLBACK ExecutionView_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y)
{
	int	error = 0;
		
	errChk( BuildSequenceViewContextMenu(menuHandle));

Error:		
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The MAINPANEL_LISTBAR ctrl sends this event when the user right clicks on it
*******************************************************************************/
HRESULT CVICALLBACK ListBar_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y)
{
	int			error = 0;
	CAObjHandle	cmds = 0;
	CAObjHandle	viewMgr;;

	// determine which view manager menu commands apply to
	errChk( GetActiveViewManager(&viewMgr));

	tsErrChk( TSUI_ApplicationMgrNewCommands(gMainWindow.applicationMgr, &errorInfo, &cmds));

	// insert items for default listbar context menu in the context menu
	tsErrChk( TSUI_CommandsInsertKind(cmds, &errorInfo, TSUIConst_CommandKind_DefaultListBarContextMenu_Set, viewMgr, -1, "", "", NULL));
	errChk( TS_RemoveInvalidShortcutKeys(cmds, errMsg));	
	tsErrChk( TSUI_CommandsInsertIntoWin32Menu(cmds, &errorInfo, menuHandle, -1, VTRUE, VTRUE)); // we are using the context menu that the control provides because it requires fewer lines of code. We could have built a CVI context menu instead and displayed it with RunPopupMenu(). If you display a CVI context menu, remember to convert the activeX-control-right-mouse click coordinates from control coordinates to panel coordinates by adding the top and left of the control to the click position.
	
Error:
	CA_DiscardObjHandle(cmds);
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The TABPANEL_SEQUENCES ctrl sends this event when the user right clicks on it
*******************************************************************************/
HRESULT CVICALLBACK SequencesList_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y)
{
	int			error = 0;
	CAObjHandle	cmds = 0;
	CAObjHandle	viewMgr;;

	// determine which view manager menu commands apply to
	errChk( GetActiveViewManager(&viewMgr));

	tsErrChk( TSUI_ApplicationMgrNewCommands(gMainWindow.applicationMgr, &errorInfo, &cmds));

	// insert items for a default sequence list context menu
	tsErrChk( TSUI_CommandsInsertKind(cmds, &errorInfo, TSUIConst_CommandKind_DefaultSequenceListContextMenu_Set, viewMgr, -1, "", "", NULL));
	errChk( TS_RemoveInvalidShortcutKeys(cmds, errMsg));	
	tsErrChk( TSUI_CommandsInsertIntoWin32Menu(cmds, &errorInfo, menuHandle, -1, VTRUE, VTRUE)); // we are using the context menu that the control provides because it requires fewer lines of code. We could have built a CVI context menu instead and displayed it with RunPopupMenu(). If you display a CVI context menu, remember to convert the activeX-control-right-mouse click coordinates from control coordinates to panel coordinates by adding the top and left of the control to the click position.
	
Error:
	CA_DiscardObjHandle(cmds);
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The ListBar sends this event when the listbar switches to a new page
*******************************************************************************/
HRESULT CVICALLBACK ListBar_OnCurPageChanged(CAObjHandle caServerObjHandle, void *caCallbackData, long  currentPage)
{
	int	error = 0;

	errChk( ShowAppropriateTabs());
	errChk( UpdateWindowTitle());

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief The StatusBar sends this event when the host window resizes
*******************************************************************************/
HRESULT CVICALLBACK StatusBar_OnResized(CAObjHandle caServerObjHandle, void *caCallbackData, short  newWidth, short newHeight)
{
	int	error = 0;
	int	visible;
	
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_VISIBLE, &visible));

	
	if (visible)	// resizing the panel due to TS_LayoutPersister_LoadBounds will trigger this callback. However, we don't want to do the layout in that case, because we are already going to do it in Main, so it would be called twice. To prevent this, we ignore this callback if the panel is not yet visible because we show the panel after the call to TS_LayoutPersister_LoadBounds
		errChk( ArrangeControls(TRUE));  // fit controls in new window size
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief displays the about box when the user selects Help>>About...
*******************************************************************************/
void CVICALLBACK AboutBoxMenuItemCallback(int menuBar, int menuItem, void *callbackData, int panel)
{
	const char 	*oiVersionLabel = "2017 (17.0.0.184)";
	int			error = 0;
	char		*string = NULL;
	char 		*engineVersionString = NULL;   	
	char 		*licenseDescriptionString = NULL;   	
	VBOOL		isEditor;
	
	if (gMainWindow.aboutBox <= 0)
		errChk( gMainWindow.aboutBox = LoadPanelEx(0, "TestExec.uir", ABOUTBOX, __CVIUserHInst));
	
	// localize the strings on the about box

	// for strings that are the same regardless of editor mode
	errChk( TS_LoadPanelResourceStrings(0, gMainWindow.engine, gMainWindow.aboutBox, "TSUI_OI_ABOUT_BOX")); 	

	// for strings that are different when we are an editor
	errChk( TSUI_ApplicationMgrGetIsEditor(gMainWindow.applicationMgr, &errorInfo, &isEditor));
	errChk( TS_SetAttrFromResourceString(gMainWindow.engine, isEditor ? "TSUI_OI_EDITOR_ABOUT_BOX" : "TSUI_OI_ABOUT_BOX", "USER_INT_MSG_CVI", "", gMainWindow.aboutBox, ABOUTBOX_USER_INT_MSG, ATTR_CTRL_VAL));
	
	// set the version strings
	errChk( TS_AllocResourceString(gMainWindow.engine, "TSUI_OI_ABOUT_BOX", "VERSION_MSG", "Missing Resource String", &string));
	errChk( AppendString(&string, " ", -1));       
	errChk( AppendString(&string, oiVersionLabel, -1));
	errChk( SetCtrlVal(gMainWindow.aboutBox, ABOUTBOX_VERSION, string));
	free(string);
	string = NULL;
		
	tsErrChk( TS_EngineGetVersionString(gMainWindow.engine, &errorInfo, &engineVersionString));		
	errChk( TS_AllocResourceString(gMainWindow.engine, "TSUI_OI_ABOUT_BOX", "ENGINE_VERSION_MSG", "Missing Resource String", &string));
	errChk( AppendString(&string, " ", -1));
	errChk( AppendString(&string, engineVersionString, -1));
	errChk( SetCtrlVal(gMainWindow.aboutBox, ABOUTBOX_ENGINE_VERSION, string));
	free(string);
	string = NULL;
	
	// set the license description
	tsErrChk( TS_EngineGetLicenseDescription(gMainWindow.engine, &errorInfo, 0, &licenseDescriptionString));
	errChk( TS_AllocResourceString(gMainWindow.engine, "TSUI_OI_ABOUT_BOX", "LICENSE_MSG", "Missing Resource String", &string));
	errChk( AppendString(&string, licenseDescriptionString, -1));
	errChk( SetCtrlVal(gMainWindow.aboutBox, ABOUTBOX_LICENSE, string));
	free(string);
	string = NULL;

	// display popup and wait for user to close dialog
	errChk( InstallPopup(gMainWindow.aboutBox));
	errChk( RunUserInterface());
	
Error:
	if (gMainWindow.aboutBox > 0)
		HidePanel(gMainWindow.aboutBox);

	free(string);
	CA_FreeMemory(engineVersionString);
	CA_FreeMemory(licenseDescriptionString);
	
	DisplayError(error);
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int CVICALLBACK AboutBoxOKCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0); // exits the RunUserInterface() call in AboutBoxMenuItemCallback
			break;
	}
		
	return 0;
}

/***************************************************************************//*!
* \brief User dragged the vertical bar on the listbar that separates it 
*		 from the tab control
*******************************************************************************/
static HRESULT CVICALLBACK ListBar_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, 
											TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              				TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL  finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.panel, MAINPANEL_LISTBAR, MAINPANEL_TAB, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(TRUE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}                         

/***************************************************************************//*!
* \brief User dragged the horizontal bar that separates the step list 
*		 from the sequence list and file variables
*******************************************************************************/
static HRESULT CVICALLBACK SequenceFilesView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, TABPANEL_SEQUENCES, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief User dragged vertical bar that separates the file variables 
*		 from the sequences list 
*******************************************************************************/
static HRESULT CVICALLBACK FileVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_VARIABLES, TABPANEL_SEQUENCES, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief User dragged the vertical bar that separates the insertion 
*		 palette from the file step list and the file variables
*******************************************************************************/
static HRESULT CVICALLBACK InsertionPalette_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, TABPANEL_STEPLISTVIEW, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief User dragged the horizontal bar that separates the execution step 
*		 list from the call stack, thread list, and execution variables
*******************************************************************************/
static HRESULT CVICALLBACK ExecutionView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, TABPANEL_2_CALLSTACK, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief User dragged the vertical bar that separates the execution 
*		 variables from the callstack
*******************************************************************************/
static HRESULT CVICALLBACK ExecutionVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_VARIABLES, TABPANEL_2_CALLSTACK, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief User dragged the vertical bar that separates the callstack 
*		 from the threads
*******************************************************************************/
static HRESULT CVICALLBACK CallStack_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_CALLSTACK, TABPANEL_2_THREADS, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief One of the hidden labels connected to display the current file 
*		 and current execution has changed
*******************************************************************************/
static HRESULT CVICALLBACK Label_OnConnectionActivity(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_ConnectionActivityTypes  activity)
{
	int	error = 0;
	
	errChk( UpdateWindowTitle());
	
Error:
	return error < 0 ? E_FAIL : S_OK;	
}
//! \cond
/// REGION END
//! \endcond