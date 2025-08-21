/***************************************************************************//*!
* \file TestExecUtils.c
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-15 5:04:32 PM
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

#include "TestExecUtils.h"

#include "SystemLog_LIB.h"

//==============================================================================
// Constants

/***************************************************************************//*!
* \brief The Edit menu is rebuild both by RebuildMenuBar and RebuildEditMenu, 
*		 so this array is at file scope so they can both access it
*******************************************************************************/
enum TSUIEnum_CommandKinds sEditMenuCommands[] = 
{
	TSUIConst_CommandKind_DefaultEditMenu_Set,			// add all the usual commands in an Edit menu
	TSUIConst_CommandKind_NotACommand					// list terminator
};

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

extern ApplicationWindow gMainWindow;
extern ERRORINFO errorInfo;
extern ErrMsg errMsg;

IniText glbSequencerConfigHandle = 0;
double glbNumSockets = 0;
int glbPanelHeight = 0;
int glbPanelWidth = 0;
int glbPanelTop = 0;
int glbPanelLeft = 0;

char glbRootPath[MAX_PATHNAME_LEN] = {0};

/***************************************************************************//*!
* \brief UI size variables
*******************************************************************************/
int glbWindowHeight = 0;
int glbWindowWidth = 0;

/***************************************************************************//*!
* \brief Used for expanding the test detail windows
*******************************************************************************/
int glbNestOrigWidth = 0;
int glbNestOrigLeft[CVIUI_TOTAL_POSSIBLE_NESTS] = {0};

//==============================================================================
// Global functions

//! \cond
/// REGION END


/// REGION START Arxtron Functions
//! \endcond
/***************************************************************************//*!
* \brief Loads uir panels into memory
*******************************************************************************/
int ArxUtil_LoadPanelsInMemory (void)
{
	int error = 0;
	
	errChk( gMainWindow.panel = 		LoadPanelEx(0, "TestExecPanel.uir", MAINPANEL, __CVIUserHInst));
	errChk( GetPanelHandleFromTabPage (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_FILE,      &gMainWindow.fileTab));
	errChk( GetPanelHandleFromTabPage (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_EXECUTION, &gMainWindow.executionTab));	
	errChk( GetPanelHandleFromTabPage (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_REPORT,    &gMainWindow.reportTab));
	
	errChk( gMainWindow.execpanel = 	LoadPanelEx(gMainWindow.panel, "TestExecExecute.uir", EXEC_PANEL, __CVIUserHInst));
	
	// Nests
	for (int i=0; i<CVIUI_NUM_NEST_TABS; ++i)
		errChk ( GetPanelHandleFromTabPage (gMainWindow.execpanel, EXEC_PANEL_TAB_NESTS, i, &(gMainWindow.tabpanels[i])));

	errChk( gMainWindow.nests[0] = 		LoadPanelEx(gMainWindow.execpanel, "TestExecExecute.uir", NEST_PANEL, __CVIUserHInst));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Groups all the initialization together for exec panel
*******************************************************************************/
int ArxUtil_InitExecPanel (void)
{
	int error = 0;
	
	IniText stationConfigHandle = NULL;
	// Implement certain features that's normally initialized in ArxUtil_InitTSPanel()
	
	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_TITLE, "Arxtron CVI Sequencer"));
	
	errChk( GetDir(glbRootPath));
	strcat (glbRootPath, "\\..\\..");	// the exec is expected to be stored in [Root]\GUI\win32
	
	// Get NumNests
	char configPath[MAX_PATHNAME_LEN] = {0};
	sprintf (configPath, "%s\\Configuration\\Sequencer.ini", glbRootPath);
	glbSequencerConfigHandle = Ini_New (0);
	errChk (Ini_ReadFromFile(glbSequencerConfigHandle, configPath));
	
	char *val[8] = {0};
	errChk (0>Ini_GetPointerToRawString (glbSequencerConfigHandle, "GUI Options", "NumNests", val));
	glbNumSockets = atof(*val);
	errChk (0>Ini_GetPointerToRawString (glbSequencerConfigHandle, "GUI Options", "UIHeight", val));
	glbPanelHeight = atoi(*val);
	errChk (0>Ini_GetPointerToRawString (glbSequencerConfigHandle, "GUI Options", "UIWidth", val));
	glbPanelWidth = atoi(*val);
	errChk (0>Ini_GetPointerToRawString (glbSequencerConfigHandle, "GUI Options", "UITop", val));
	glbPanelTop = atoi(*val);
	errChk (0>Ini_GetPointerToRawString (glbSequencerConfigHandle, "GUI Options", "UILeft", val));
	glbPanelLeft = atoi(*val);
	
	// Install panel callback
	errChk( InstallMainCallback(MainCallback, 0, 0));	// handle the EVENT_END_TASK event, etc.
	
	/* 20250526Tim: Initialize UserManagement (With project number from Station config) */
	//sprintf (configPath, "%s\\Configuration\\StationConfig.ini", glbRootPath);
	//stationConfigHandle = Ini_New (0);
	//errChk (Ini_ReadFromFile (stationConfigHandle, configPath));
	//char *prjNumberStr = 0;
	//errChk (0>Ini_GetPointerToRawString (stationConfigHandle, "Station Config", "PrjNumber", &prjNumberStr));
	//int prjNumber = atoi (prjNumberStr);*/
	//
	//char errmsg[1024] = {0};
	//sprintf (configPath, "%s\\Configuration\\UserFile.txt", glbRootPath);
	//errChk (Initialize_UserManagement_LIB (prjNumber, configPath, errmsg))
	//
	//errChk( (glbNumSockets>(CVIUI_TOTAL_POSSIBLE_NESTS)?ARXERR_GRID_CONFIG_WRONG:0));

	//errChk( ArxUtil_GetExecPanelHandles());
	//errChk( ArxUtil_CloneNests());
	
Error:
	if (stationConfigHandle) Ini_Dispose (stationConfigHandle);
	return error;
}
//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief If the execution has a report, switch to the report tab either 
*		 immediately if the execution is visible, or, whenever the 
*		 execution is viewed next
*******************************************************************************/
int ShowReport(CAObjHandle execution)
{
	int			error = 0;
	int			isTheSelectedExecution;
	CAObjHandle	selectedExecution = 0;

	// switch to report view when this execution is next viewed
	errChk( TS_PropertySetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, TABPAGE_REPORT)); // activate the reportTab when the user views this execution

	errChk( TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &selectedExecution));
	errChk(TS_ActiveXHandlesReferToSameObject(execution, selectedExecution, &isTheSelectedExecution));

	if (isTheSelectedExecution) 
		ShowAppropriateTabs();  // switch to report view tab now
		
Error:
	CA_DiscardObjHandle(selectedExecution);
	return error;
}

/***************************************************************************//*!
* \brief Main callback
*******************************************************************************/
int MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2)
{
	int error = 0;
	
	switch (event)
	{
		case EVENT_END_TASK:	// EVENT_END_TASK can occur when windows shuts down or when the user selects Close from the context menu for the application's task bar item
			if (!ExitApplication() && !eventData1) 	
				return 1; //  don't immediately exit if we have cleanup to do and the whole computer is not shutting down
			break;
		case EVENT_GOT_FOCUS:
		case EVENT_LOST_FOCUS:
			if (gMainWindow.guiThreadId == CmtGetCurrentThreadID())	// these events might also be generate by CVI panels displayed in execution threads. In that case we want to ignore the events to avoid unnecessary multithreading issues in the application UI code.
				if (gMainWindow.engine != 0) // these events can also occur while shutting down and should be ignored then as well
					errChk( RebuildEditMenu());
			break;
	}
	
Error:
	DisplayError(error);
	return 0;
}

/***************************************************************************//*!
* \brief User clicked on menubar, make sure all menus have correct items 
*		 with the correct enabled states
*******************************************************************************/
void	MenuDimmerCallback(int menuBarHandle, int panelHandle)
{
	int	error = 0;
	
	errChk( RebuildMenuBar(menuBarHandle));
	
Error:
	DisplayError(error);
}

/***************************************************************************//*!
* \brief
*******************************************************************************/
int UpdateWindowTitle()
{
	int 	error = 0;
	char	title[1024];
	int		pageIndex;
	char *	documentDescription = NULL;
	
	errChk( TS_GetResourceString(gMainWindow.engine, "TSUI_OI_MAIN_PANEL", "TESTSTAND_USER_INTERFACE", "", sizeof(title), title));

	errChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));

	if (pageIndex == SEQUENCE_FILES_PAGE_INDEX)	// sequence files are visible
		errChk( TSUI_SequenceFileViewMgrGetCaptionText(gMainWindow.sequenceFileViewMgr, &errorInfo, TSUIConst_CaptionSource_CurrentSequenceFile, VFALSE, "", &documentDescription));
	else	// executions are visible
		errChk( TSUI_ExecutionViewMgrGetCaptionText(gMainWindow.executionViewMgr, &errorInfo, TSUIConst_CaptionSource_CurrentExecution, VFALSE, "", &documentDescription));
	
	if (*documentDescription)
		strcat(strcat(title, " - "), documentDescription);

	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_TITLE, title));

Error:
	CA_FreeMemory(documentDescription);
	return error;
}

/***************************************************************************//*!
* \brief Build a context menu for a sequence view control that has 
*		 been right-clicked
*******************************************************************************/
int BuildSequenceViewContextMenu(int menuHandle)
{
	int			error = 0;
	CAObjHandle	cmds = 0;
	CAObjHandle	viewMgr;
	
	// determine which view manager menu commands apply to
	errChk( GetActiveViewManager(&viewMgr));
	
	errChk( TSUI_ApplicationMgrNewCommands(gMainWindow.applicationMgr, &errorInfo, &cmds));

	// insert items for default sequence view context menu in the context menu
	errChk( TSUI_CommandsInsertKind(cmds, &errorInfo, TSUIConst_CommandKind_DefaultSequenceViewContextMenu_Set, viewMgr, -1, "", "", NULL));
	errChk( TS_RemoveInvalidShortcutKeys(cmds, errMsg));	
	errChk( TSUI_CommandsInsertIntoWin32Menu (cmds, &errorInfo, menuHandle, -1, VTRUE, VTRUE)); // we are using the context menu that the SequenceView control provides because it requires fewer lines of code. We could have built a CVI context menu instead and displayed it with RunPopupMenu(). If you display a CVI context menu, remember to convert the activeX-control-right-mouse click coordinates from control coordinates to panel coordinates by adding the top and left of the control to the click position.
	
Error:
	CA_DiscardObjHandle(cmds);
	return error;
}

/***************************************************************************//*!
* \brief Based on the current listbar page, show and hide the tabs that 
*		 appear in the space to the right of the listbar
*******************************************************************************/
int ShowAppropriateTabs(void)
{
	int			error = 0;
	long		pageIndex;
	CAObjHandle	execution = 0;
	void 	*	curActiveCtrlInfo;
	double	lastActiveExecutionTab = 1; // Default to the Execution tab when showing executions.


	gMainWindow.programmaticallyUpdatingTabPages = TRUE;

	// Note: Store the current Active control as EasyTab can change the active panel and control
	curActiveCtrlInfo = SaveActiveCtrl(0,0);

	errChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));
	
	// Note: EasyTab does not allow the last visible tab to be made invisible. Thus, when hiding and showing tabs, you must make sure there is at least one other tab visible before
	// hiding a tab.  The easiest way to ensure this is to first show all tabs. Then, at least one tab is visible that will remain visible and all other tabs can be hidden succesfully.

	// show all tabs
	errChk( SetTabPageAttribute (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_FILE,      ATTR_VISIBLE, TRUE));
	errChk( SetTabPageAttribute (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_EXECUTION, ATTR_VISIBLE, TRUE));
	errChk( SetTabPageAttribute (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_REPORT,    ATTR_VISIBLE, TRUE));
	
	// show and hide tabs as appropriate
	errChk( SetTabPageAttribute (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_EXECUTION, ATTR_VISIBLE, pageIndex == EXECUTIONS_PAGE_INDEX));
	errChk( SetTabPageAttribute (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_REPORT,    ATTR_VISIBLE, pageIndex == EXECUTIONS_PAGE_INDEX));
	errChk( SetTabPageAttribute (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_FILE,      ATTR_VISIBLE, pageIndex == SEQUENCE_FILES_PAGE_INDEX));

	// if we are viewing an execution...
	if (pageIndex == EXECUTIONS_PAGE_INDEX)
	{
		// determine which tab page we last displayed for this execution
		errChk( TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &execution));
		if (execution)
			errChk( TS_PropertyGetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, &lastActiveExecutionTab));

		// re-activate previously active tab for the execution
		errChk( SetActiveTabPage (gMainWindow.panel, MAINPANEL_TAB, (int)lastActiveExecutionTab)); // Cast OK.  We were storing an integer in a TS number.
	}

	// Note: Set the current Active control as EasyTab can change the active panel and control
	RestoreActiveCtrl(curActiveCtrlInfo);
	
Error:
	gMainWindow.programmaticallyUpdatingTabPages = FALSE;

	CA_DiscardObjHandle(execution);
	return error;
}

/***************************************************************************//*!
* \brief Show the appropriate group of status bar panes, depending on 
*		 whether we are viewing a file, execution steps, or a report
*******************************************************************************/
int ShowAppropriateStatusBarPanes(void)
{
	int	error = 0;
	int	activeTab;

	errChk( GetActiveTabPage(gMainWindow.panel, MAINPANEL_TAB, &activeTab));		

	if (activeTab == gMainWindow.fileTab)	
		errChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, FileModel, FileSelectedSteps, FileNumberOfSteps"));
	else if (activeTab == gMainWindow.executionTab)
		errChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, ExecutionModel, ExecutionSelectedSteps, ExecutionNumberOfSteps, ProgressText, ProgressPercent"));
	else
		errChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, ExecutionModel, ReportLocation, ProgressText, ProgressPercent"));
		
Error:
	return error;
}

/***************************************************************************//*!
* \brief Call this function after you handle an error, unless you handle 
*		 the error by calling DisplayError, which also calls this function
*******************************************************************************/
void ClearErrorMessage(void)
{
	// clear out error message globals so that a future error that lacks an error description does not
	// unintentionally use the error description from a prior error.
	*errMsg = '\0';
	memset(&errorInfo, 0, sizeof(ERRORINFO)); 
}

/***************************************************************************//*!
* \brief Displays a message box with the error code, the error message 
*		 associated with the code, and any error description details 
*		 does nothing if errorCode is not negative
*******************************************************************************/
void DisplayError(int errorCode)
{
	if (errorCode < 0)
	{
		TS_DisplayError(errorCode, errMsg, gMainWindow.engine);	// errMsg is a global, see top of file
		ClearErrorMessage();	
	}
}

/***************************************************************************//*!
* \brief Returns the SequenceFileViewManager or the ExecutionViewManager 
*		 depending on whether the application is displaying sequence files or 
*		 executions the returned viewManager is a weak reference (ie. do not 
*		 call CA_DiscardObjHandle on it).
*******************************************************************************/
int GetActiveViewManager(CAObjHandle *viewManager)
{
	int	error = 0;
	int	page;
	
	errChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &page));
	if (page == SEQUENCE_FILES_PAGE_INDEX)	// sequence files are visible, sequence file menu commands apply
		*viewManager = gMainWindow.sequenceFileViewMgr;
	else
	if (page == EXECUTIONS_PAGE_INDEX)		// executions are visible, execution menu commands apply
		*viewManager = gMainWindow.executionViewMgr;

Error:
	return error;
}

/***************************************************************************//*!
* \brief Make sure all menus have appropriate items with the correct enabled states
*******************************************************************************/
int RebuildMenuBar(int menuBar)
{
	VBOOL isEditor = 0;
	int									error = 0;
	CAObjHandle							viewMgr = 0;
	enum TSUIEnum_CommandKinds	fileMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultFileMenu_Set,			// add all the usual commands in a File menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	
	// Edit menu commands defined in sEditMenuCommands, above this function
	
	enum TSUIEnum_CommandKinds	executeMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultExecuteMenu_Set,		// add all the usual commands in a Execute menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	enum TSUIEnum_CommandKinds	debugMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultDebugMenu_Set,			// add all the usual commands in a Debug menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	enum TSUIEnum_CommandKinds	configureMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultConfigureMenu_Set,		// add all the usual commands in a Configure menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	enum TSUIEnum_CommandKinds	configureMenuCommands_EditMode[] = 
	{
		TSUIConst_CommandKind_ConfigureEngineEnvironment,		// add all the usual commands in a Configure menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	
	enum TSUIEnum_CommandKinds	toolsMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultToolsMenu_Set,			// add all the usual commands in a Tools menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	enum TSUIEnum_CommandKinds	helpMenuCommands[] = 
	{
		TSUIConst_CommandKind_Separator,					// separates the existing About... item
		TSUIConst_CommandKind_DefaultHelpMenu_Set,			// add all the usual commands in a Help menu. Note that most help items appear only when in Edit mode.
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	
	// determine which view manager the menu commands apply to
	errChk( GetActiveViewManager(&viewMgr));
	
   	// discard any current menu items that were inserted by TS_InsertCommandsInMenu
	errChk( TS_RemoveMenuCommands(menuBar, 0, FALSE, FALSE));

	// rebuild File menu
	errChk( TS_InsertCommandsInMenu(fileMenuCommands, menuBar, BAR_FILE, -1, viewMgr, NULL, TRUE, errMsg));
	
	// rebuild Edit menu
	errChk( TS_InsertCommandsInMenu(sEditMenuCommands, menuBar, BAR_EDIT, -1, viewMgr, NULL, TRUE, errMsg));
	
	// rebuild Execute menu
	errChk( TS_InsertCommandsInMenu(executeMenuCommands, menuBar, BAR_EXECUTE, -1, viewMgr, NULL, TRUE, errMsg));

	// rebuild Debug menu
	errChk( TS_InsertCommandsInMenu(debugMenuCommands, menuBar, BAR_DEBUG, -1, viewMgr, NULL, TRUE, errMsg));

	// rebuild Configure menu
	errChk( TS_InsertCommandsInMenu(configureMenuCommands, menuBar, BAR_CONFIGURE, -1, viewMgr, NULL, TRUE, errMsg));
	TSUI_ApplicationMgrGetIsEditor(gMainWindow.applicationMgr, &errorInfo, &isEditor);
	if(isEditor)
	errChk( TS_InsertCommandsInMenu(configureMenuCommands_EditMode, menuBar, BAR_CONFIGURE, -1, viewMgr, NULL, TRUE, errMsg));    

	// rebuild Tools menu
	errChk( TS_InsertCommandsInMenu(toolsMenuCommands, menuBar, BAR_TOOLS, -1, viewMgr, NULL, TRUE, errMsg));

	// rebuild the Help menu. Note that the help menu already contains an "About..." item, which is not a TestStand command item
	errChk( TS_InsertCommandsInMenu(helpMenuCommands, menuBar, BAR_HELP, -1, viewMgr, NULL, TRUE, errMsg));

	// remove duplicate separators and shortcut keys
	errChk( TS_CleanupMenu(menuBar, 0));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief We specifically rebuild the Edit menu on EVENT_GOT_FOCUS/EVENT_LOST_FOCUS 
*		 because TS_InsertCommandsInMenu removes the common edit menu shortcuts 
*		 (Ctrl-X, Ctrl-C, Ctrl-V, Ctrl-A, Delete) when the focus is not on an ActiveX 
*		 control such as a TestStand control. It does this because the presence of 
*		 those shortcut keys in the menu (even on dimmed items) prevents their 
*		 operation with CVI controls such as tables and text boxes. Because these 
*		 shortcut keys are not always present in the menu, we do not always get a 
*		 MenuDimmerCallback when they are pressed. Thus, we also rebuild the Edit
*		 menu when the control focus changes. We could instead rebuild the entire 
*		 menu bar by calling RebuildMenuBar, but depending on the machine, that might 
*		 noticably reduce the speed at which you can tab between controls.
*******************************************************************************/
int RebuildEditMenu()
{
	int			error = 0;
	CAObjHandle	viewMgr = 0;
	int			menuBar;
	
	menuBar = GetPanelMenuBar(gMainWindow.panel);
	
	// determine which view manager the menu commands apply to
	errChk( GetActiveViewManager(&viewMgr));

   	// discard any current menu items that were inserted by TS_InsertCommandsInMenu
	errChk( TS_RemoveMenuCommands(menuBar, BAR_EDIT, FALSE, FALSE));

	// rebuild Edit menu
	errChk( TS_InsertCommandsInMenu(sEditMenuCommands, menuBar, BAR_EDIT, -1, viewMgr, NULL, TRUE, errMsg));

	// remove duplicate separators and shortcut keys
	errChk( TS_CleanupMenu(menuBar, 0));

Error:
	return error;
}

/***************************************************************************//*!
* \brief Get handles to ActiveX controls in order to call their particular API's
*******************************************************************************/
int GetActiveXControlHandles(void)
{
	int	error = 0;
	
	// the invisible ApplicationManager control creates and destroys the TestStand engine, implements login/logout and shutdown
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.panel, MAINPANEL_APPLICATIONMGR, &gMainWindow.applicationMgr));
	
	// the invisible SequenceFileViewManager control holds the selected sequence file
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.panel, MAINPANEL_SEQUENCEFILEVIEWMGR, &gMainWindow.sequenceFileViewMgr));

	// the ExecutionViewManager control manages the display of the selected execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.panel, MAINPANEL_EXECUTIONVIEWMGR, &gMainWindow.executionViewMgr));

	// the ListBar control lets the user select a sequence file or an execution
	errChk( GetObjHandleFromActiveXCtrl(gMainWindow.panel, MAINPANEL_LISTBAR, &gMainWindow.listBar));

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
* \brief
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
* \brief
*******************************************************************************/
int ConnectTestStandControls()
{
	int			error = 0;
	CAObjHandle	pages = 0;
	CAObjHandle	page = 0;
	CAObjHandle sequenceListConnection = 0;
	CAObjHandle executionListConnection = 0;
	
	
	// connect controls on the Sequence File tab
	
	// connect fileStepListView to SequenceFileViewMgr
	errChk( TSUI_SequenceFileViewMgrConnectSequenceView(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.fileStepListView, NULL));

	// connect first entry point button 
	errChk( TSUI_SequenceFileViewMgrConnectCommand(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.entryPoint1Button, TSUIConst_CommandKind_ExecutionEntryPoints_Set, 0, TSUIConst_CommandConnection_NoOptions, NULL));
	
	// connect second entry point button 
	errChk( TSUI_SequenceFileViewMgrConnectCommand(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.entryPoint2Button, TSUIConst_CommandKind_ExecutionEntryPoints_Set, 1, TSUIConst_CommandConnection_NoOptions, NULL));

	// connect run-current-sequence button 
	errChk( TSUI_SequenceFileViewMgrConnectCommand(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.runCurrentSequenceButton, TSUIConst_CommandKind_RunCurrentSequence, 0, TSUIConst_CommandConnection_NoOptions, NULL));
	
	// connect the hidden sequence file label
	errChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.sequenceFileLabel, TSUIConst_CaptionSource_CurrentSequenceFile, VFALSE, NULL));
	
	// connect fileSequences listbox to SequenceList
	errChk( TSUI_SequenceFileViewMgrConnectSequenceList(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.fileSequencesList, &sequenceListConnection));
	errChk( TSUISUPP_SequenceListConnectionSetColumnVisible(sequenceListConnection, &errorInfo, TSUISUPPConst_SeqListConnectionColumn_Comments, VTRUE));
	
	// connect fileVariables to SequenceFileViewMgr
	errChk( TSUI_SequenceFileViewMgrConnectVariables(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.fileVariables, NULL));

	// connect insertionPalette to SequenceFileViewMgr
	errChk( TSUI_SequenceFileViewMgrConnectInsertionPalette(gMainWindow.sequenceFileViewMgr, &errorInfo, gMainWindow.insertionPalette, NULL));
	
	
	// connect controls on the Execution tab

	// connect executionStepListView to ExecutionViewMgr
	errChk( TSUI_ExecutionViewMgrConnectExecutionView(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.executionStepListView, TSUIConst_ExecutionViewConnection_NoOptions, NULL));

	// connect reportView to ExecutionViewMgr
	errChk( TSUI_ExecutionViewMgrConnectReportView(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.reportView, NULL));

	// connect execution variables to ExecutionViewMgr
	errChk( TSUI_ExecutionViewMgrConnectVariables(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.executionVariables, NULL));
	
	// connect callStack listbox to ExecutionViewMgr
	errChk( TSUI_ExecutionViewMgrConnectCallStack(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.callStack, NULL));

	// connect thread listbox to ExecutionViewMgr
	errChk( TSUI_ExecutionViewMgrConnectThreadList(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.threads, NULL));
	
	// connect break/resume button
	errChk( TSUI_ExecutionViewMgrConnectCommand(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.breakResumeButton, TSUIConst_CommandKind_BreakResume, 0, TSUIConst_CommandConnection_NoOptions, NULL));

	// connect terminate/restart button
	errChk( TSUI_ExecutionViewMgrConnectCommand(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.terminateRestartButton, TSUIConst_CommandKind_TerminateRestart, 0, TSUIConst_CommandConnection_NoOptions, NULL));

	// connect the hidden execution label
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, gMainWindow.executionLabel, TSUIConst_CaptionSource_CurrentExecution, VFALSE, NULL));

		
	// connect listbar pages
	
	errChk( TSUI_ListBarGetPages(gMainWindow.listBar, &errorInfo, &pages));

	// connect listbar page 0 to SequenceFileList
	errChk( TSUI_ListBarPagesGetItem(pages, &errorInfo, CA_VariantLong(SEQUENCE_FILES_PAGE_INDEX), &page));
	errChk( TSUI_SequenceFileViewMgrConnectSequenceFileList(gMainWindow.sequenceFileViewMgr, &errorInfo, page, VFALSE, NULL));	
	CA_DiscardObjHandle(page);
	
	// connect listbar page 1 to ExecutionList
	errChk( TSUI_ListBarPagesGetItem(pages, &errorInfo, CA_VariantLong(EXECUTIONS_PAGE_INDEX), &page));
	errChk( TSUI_ExecutionViewMgrConnectExecutionList(gMainWindow.executionViewMgr, &errorInfo, page, &executionListConnection));	
	// display the execution name on the first line, the serial number (if any) on the next line, the socket index (if any) on the next line, and the model execution state on the last line (the expression string looks complicated here because we have to escape the quotes and backslashes for the C compiler.)
	errChk( TSUISUPP_ExecutionListConnectionSetDisplayExpression(executionListConnection, &errorInfo, "\"%CurrentExecution%\\n\" + (\"%UUTSerialNumber%\" == \"\" ? \"\" : (ResStr(\"TSUI_OI_MAIN_PANEL\",\"SERIAL_NUMBER\") + \" %UUTSerialNumber%\\n\")) + (\"%TestSocketIndex%\" == \"\" ? \"\" : ResStr(\"TSUI_OI_MAIN_PANEL\",\"SOCKET_NUMBER\") + \" %TestSocketIndex%\\n\") + \"%ModelState%\""));
	CA_DiscardObjHandle(page);

	errChk( ConnectStatusBarPanes());
	
Error:
	CA_DiscardObjHandle(sequenceListConnection);
	CA_DiscardObjHandle(executionListConnection);
	CA_DiscardObjHandle(pages);
	
	return error;
}

/***************************************************************************//*!
* \brief
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
	errChk( TSUI_StatusBarGetPanes(gMainWindow.statusBar, &errorInfo, &panes));
	
	// IMPORTANT: the order of these calls must match the order of the panes as configured in the status bar 
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneUser));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneEngineEnvironment));    
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneFileModel));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneExecutionModel));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneFileSelectedSteps));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneFileNumberOfSteps));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneExecutionSelectedSteps));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneExecutionNumberOfSteps));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneReportLocation));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneProgressText));
	errChk( TSUI_StatusBarPanesGetItem(panes, &errorInfo, CA_VariantLong(paneIndex++), &paneProgressPercent));

	// User 
	errChk( TSUI_ApplicationMgrConnectCaption(gMainWindow.applicationMgr, &errorInfo, paneUser, TSUIConst_CaptionSource_UserName, VFALSE, NULL));
	// Engine Environment 
	errChk( TSUI_ApplicationMgrConnectCaption(gMainWindow.applicationMgr, &errorInfo, paneEngineEnvironment, TSUIConst_CaptionSource_EngineEnvironment, VFALSE, NULL))	
	// File Process Model
	errChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, paneFileModel, TSUIConst_CaptionSource_CurrentProcessModelFile, VTRUE, &fileModelCaptionConnection));
	errChk( TSUISUPP_CaptionConnectionSetLongName(fileModelCaptionConnection, &errorInfo, VFALSE)); // just show the base file name
	// Execution Process Model
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneExecutionModel, TSUIConst_CaptionSource_CurrentProcessModelFile, VTRUE, &executionModelCaptionConnection));
	errChk( TSUISUPP_CaptionConnectionSetLongName(executionModelCaptionConnection, &errorInfo, VFALSE));	// just show the base file name
	// File Selected Steps
	errChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, paneFileSelectedSteps, TSUIConst_CaptionSource_SelectedSteps_ZeroBased, VFALSE, NULL));
	// File Number of Steps
	errChk( TSUI_SequenceFileViewMgrConnectCaption(gMainWindow.sequenceFileViewMgr, &errorInfo, paneFileNumberOfSteps, TSUIConst_CaptionSource_NumberOfSteps, VTRUE, NULL));
	// Execution Selected Steps
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneExecutionSelectedSteps, TSUIConst_CaptionSource_SelectedSteps_ZeroBased, VFALSE, NULL));
	// Execution Number of Steps
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneExecutionNumberOfSteps, TSUIConst_CaptionSource_NumberOfSteps, VTRUE, NULL));
	// Report Location
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneReportLocation, TSUIConst_CaptionSource_ReportLocation, VTRUE, NULL));
	// Progress Text
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneProgressText, TSUIConst_CaptionSource_ProgressText, VFALSE, NULL));
	// Progress Percent Text
	errChk( TSUI_ExecutionViewMgrConnectCaption(gMainWindow.executionViewMgr, &errorInfo, paneProgressPercent, TSUIConst_CaptionSource_ProgressPercent, VFALSE, NULL));
	// Progress Percent Bar
	errChk( TSUI_ExecutionViewMgrConnectNumeric(gMainWindow.executionViewMgr, &errorInfo, paneProgressPercent, TSUIConst_NumericSource_ProgressPercent, NULL));

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
* \brief Adjust controls to fit within current window size
*******************************************************************************/
int ArrangeControls(int processEvents)
{
	int			error = 0;
	const int	buttonVerticalMargin = 6;
	const int	buttonHorizontalSpacing = 12;
	int			panelHeight, panelWidth;
	int			menuHeight;
	int 		statusBarHeight;
	Rect		tabInteriorRect, tabExteriorRect;
	int			buttonTop;
	int			buttonHeight;
	int 		resizableTabControlAreaHeight;	
	int			resizableTabControlAreaWidth;
	VBOOL		isEditor;
	int			fileStepsWidth;
	int			val;

	// get current sizes and positions
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_HEIGHT, &panelHeight));
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_MENU_HEIGHT, &menuHeight));
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_WIDTH, &panelWidth));
	errChk( GetCtrlAttribute(gMainWindow.panel, MAINPANEL_STATUSBAR, ATTR_HEIGHT, &statusBarHeight));

	// resize controls to fit current window height
	errChk( GetCtrlBoundingRect(gMainWindow.panel, MAINPANEL_TAB, &tabExteriorRect.top, &tabExteriorRect.left, &tabExteriorRect.height, &tabExteriorRect.width))
	
	tabExteriorRect = MakeRect(menuHeight + 1, tabExteriorRect.left, Max(panelHeight - statusBarHeight - menuHeight, 50), tabExteriorRect.width);
	errChk( SetCtrlAttribute(gMainWindow.panel, MAINPANEL_TAB, ATTR_TOP, tabExteriorRect.top));
	errChk( SetCtrlAttribute(gMainWindow.panel, MAINPANEL_TAB, ATTR_LEFT, tabExteriorRect.left));
	errChk( SetCtrlAttribute(gMainWindow.panel, MAINPANEL_TAB, ATTR_HEIGHT, tabExteriorRect.height));
	errChk( SetCtrlAttribute(gMainWindow.panel, MAINPANEL_TAB, ATTR_WIDTH, tabExteriorRect.width));
	
	errChk( SetCtrlAttribute(gMainWindow.panel, MAINPANEL_LISTBAR, ATTR_HEIGHT, tabExteriorRect.height));

    // there is a vertical drag bar between listbar and tab control
	errChk(TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.panel, &errorInfo, TRUE, 0, panelWidth, MAINPANEL_LISTBAR, MAINPANEL_TAB, 0));

	// obtain new size of tab area interior
	errChk( GetPanelAttribute (gMainWindow.fileTab, ATTR_TOP, &tabInteriorRect.top));
	errChk( GetPanelAttribute (gMainWindow.fileTab, ATTR_LEFT, &tabInteriorRect.left));
	errChk( GetPanelAttribute (gMainWindow.fileTab, ATTR_HEIGHT, &tabInteriorRect.height));
	errChk( GetPanelAttribute (gMainWindow.fileTab, ATTR_WIDTH, &tabInteriorRect.width));

    
    // File and Execution Tabs have buttons at the bottom. figure out how much space is left after the buttons are placed
	errChk( GetCtrlAttribute(gMainWindow.executionTab, TABPANEL_2_BREAKRESUMEBTN, ATTR_HEIGHT, &buttonHeight));
    buttonTop = Max(tabInteriorRect.height - buttonHeight - buttonVerticalMargin, 40 + buttonVerticalMargin);

	// compute the size of the tab control interior area minus space for the buttons. this area is shared by controls that be resized
    // don't shrink control area to less than 40 by 40 pixels so that if the window is resized to have no visible area, the controls aren't made so small that we forget their relative sizes
    resizableTabControlAreaHeight = Max(buttonTop - buttonVerticalMargin, 40);
    resizableTabControlAreaWidth = Max(tabInteriorRect.width, 40);

    // FILE TAB:

    // place the file tab buttons
    errChk( SetAttributeForCtrls(gMainWindow.fileTab, ATTR_TOP, buttonTop, 3, TABPANEL_ENTRYPOINT1BTN, TABPANEL_ENTRYPOINT2BTN, TABPANEL_RUNSEQUENCEBTN));
	errChk( PositionCtrlRelativeToCtrl(gMainWindow.fileTab, TABPANEL_ENTRYPOINT1BTN, 0, TABPANEL_ENTRYPOINT2BTN, 0, buttonHorizontalSpacing, kPositionCtrlCenterRight, kPositionCtrlCenterJustification));
	errChk( PositionCtrlRelativeToCtrl(gMainWindow.fileTab, TABPANEL_ENTRYPOINT2BTN, 0, TABPANEL_RUNSEQUENCEBTN, 0, buttonHorizontalSpacing, kPositionCtrlCenterRight, kPositionCtrlCenterJustification));

    // hide editor-only controls if not an editor
	errChk( TSUI_ApplicationMgrGetIsEditor(gMainWindow.applicationMgr, &errorInfo, &isEditor));
	errChk( SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, ATTR_VISIBLE, isEditor));

    // there is a horizontal drag bar between FileSteps and SequenceList/FileVariables
	errChk( TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.fileTab, &errorInfo, FALSE, 0, resizableTabControlAreaHeight, TABPANEL_STEPLISTVIEW, TABPANEL_SEQUENCES, 0));
	
	// file variables has same height and top as the sequences list
	errChk( GetCtrlAttribute(gMainWindow.fileTab, TABPANEL_SEQUENCES, ATTR_TOP, &val));
	errChk( SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_VARIABLES, ATTR_TOP, val));
	errChk( GetCtrlAttribute(gMainWindow.fileTab, TABPANEL_SEQUENCES, ATTR_HEIGHT, &val));
	errChk( SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_VARIABLES, ATTR_HEIGHT, val));

    // there is a vertical drag bar between the FileSteps and the Insertion Palette
    if (!isEditor)
    	errChk( SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, ATTR_WIDTH, resizableTabControlAreaWidth)); // Insertion Palette is not visible, nothing to split
    else
		errChk( TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.fileTab, &errorInfo, TRUE, 0, resizableTabControlAreaWidth, TABPANEL_STEPLISTVIEW, TABPANEL_INSERTIONPALETTE, 0));

	errChk( SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, ATTR_HEIGHT, resizableTabControlAreaHeight));

   	// there is a vertical drag bar between the FileVariablesand the SequencesList
	errChk( GetCtrlAttribute(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, ATTR_WIDTH, &fileStepsWidth));
	errChk( TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.fileTab, &errorInfo, TRUE, 0, fileStepsWidth, TABPANEL_VARIABLES, TABPANEL_SEQUENCES, 0));

    // EXECUTION TAB:

    // place the execution tab buttons
    errChk( SetAttributeForCtrls(gMainWindow.executionTab, ATTR_TOP, buttonTop, 2, TABPANEL_2_BREAKRESUMEBTN, TABPANEL_2_TERMINATERESTARTBTN));
	errChk( PositionCtrlRelativeToCtrl(gMainWindow.executionTab, TABPANEL_2_BREAKRESUMEBTN, 0, TABPANEL_2_TERMINATERESTARTBTN, 0, buttonHorizontalSpacing, kPositionCtrlCenterRight, kPositionCtrlCenterJustification));
	
    // size ExecutionSteps to tab control interior width
    errChk( SetCtrlAttribute(gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, ATTR_WIDTH, resizableTabControlAreaWidth));

    // there is a horizontal drag bar between ExecutionSteps and ExecutionVariables/CallStack/Threads
	errChk( TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.executionTab, &errorInfo, FALSE, 0, resizableTabControlAreaHeight, TABPANEL_2_STEPLISTVIEW, TABPANEL_2_CALLSTACK, 0));
	
	// execution variables and threads have the same height and top as the callstack
	errChk( GetCtrlAttribute(gMainWindow.executionTab, TABPANEL_2_CALLSTACK, ATTR_HEIGHT, &val));
    errChk( SetAttributeForCtrls(gMainWindow.executionTab, ATTR_HEIGHT, val, 2, TABPANEL_2_THREADS, TABPANEL_2_VARIABLES));
	errChk( GetCtrlAttribute(gMainWindow.executionTab, TABPANEL_2_CALLSTACK, ATTR_TOP, &val));
    errChk( SetAttributeForCtrls(gMainWindow.executionTab, ATTR_TOP, val, 2, TABPANEL_2_THREADS, TABPANEL_2_VARIABLES));

    // there are vertical drag bars between the ExecutionVariables, CallStack, and Threads
	errChk( TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.executionTab, &errorInfo, TRUE, 0, resizableTabControlAreaWidth, TABPANEL_2_VARIABLES, TABPANEL_2_CALLSTACK, TABPANEL_2_THREADS, 0));

    // REPORT TAB:
    errChk( SetCtrlAttribute(gMainWindow.reportTab, TABPANEL_3_REPORTVIEW, ATTR_WIDTH, tabInteriorRect.width)); 
    errChk( SetCtrlAttribute(gMainWindow.reportTab, TABPANEL_3_REPORTVIEW, ATTR_HEIGHT, tabInteriorRect.height)); 

    // needed so that drawing keeps pace with drag bar movement		
	ProcessDrawEvents();
	
Error:
	return error;
}
//! \cond
/// REGION END
//! \endcond