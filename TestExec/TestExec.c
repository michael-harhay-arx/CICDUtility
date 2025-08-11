// Note:	This example can function as an editor or an operator interface. The user can change the edit mode with a keystroke (ctrl-alt-shift-insert) or with a command line 
//			argument. For more information and for instructions on how prevent the user from changing the edit mode, refer to the TestStand Reference Manual>>Creating Custom 
//			User Interfaces>>Editor versus Operator Interface Applications>>Creating Editor Applications.

// Note:	TestStand installs the source code files for the default user interfaces in the <TestStand>\UserInterfaces and <TestStand Public>\UserInterfaces directories. 
//			To modify the installed user interfaces or to create new user interfaces, modify the files in the <TestStand Public>\UserInterfaces directory. 
//			You can use the read-only source files for the default user interfaces in the <TestStand>\UserInterfaces directory as a reference. 
//			National Instruments recommends that you track the changes you make to the user interface source code files so you can integrate the changes with any enhancements in future versions of the TestStand User Interfaces.

#include <cvirte.h>		
#include <userint.h>
#include "TestExec.h"		// UIR header
#include "tsapicvi.h"   	// TestStand API
#include "tsui.h"			// API's for the TestStand ActiveX controls
#include "tsuisupp.h"		// support API for the TestStand ActiveX controls
#include <utility.h>
#include "tsutil.h"			// helpful functions for using TestStand APIs in CVI
#include "easytab.h"		// for the tab control
#include "toolbox.h"   		// Max, ...

// page indices for the list bar control corresponding to how it was configured in the UIR editor
#define SEQUENCE_FILES_PAGE_INDEX	0		// first list bar page
#define EXECUTIONS_PAGE_INDEX		1		// second list bar page

#define TABPAGE_FILE		0
#define TABPAGE_EXECUTION	1
#define TABPAGE_REPORT		2

typedef struct 
{
	// panel handles
	int			panel;								// main window
	int			fileTab;							// "Sequences" tab. shows information about the selected sequence file
	int			executionTab;						// "Steps" tab. shows execution information about the selected execution
	int			reportTab;							// "Report" tab. shows the report for the selected execution
	int			aboutBox;							// displays version info and logo bitmap

	// ActiveX control handles:
	CAObjHandle applicationMgr;						// invisible control, manages Startup/Shutdown, and other application functions
	CAObjHandle sequenceFileViewMgr;				// invisible control, manages a SequenceView control that displays loaded sequence files
	CAObjHandle executionViewMgr;					// invisible control, manages a SequenceView control that displays executing sequences
	CAObjHandle	listBar;							// vertical icon bar. user can select a sequence file or an execution
	CAObjHandle fileStepListView;					// displays the steps in the selected sequence file on the "Sequences" tab
	CAObjHandle executionStepListView;				// displays the steps in the selected execution in the "Steps" tab
	CAObjHandle reportView;							// displays the report for the selected execution in the "Report" tab
	CAObjHandle fileSequencesList;					// sequence selector listbox on the "Sequences" tab shown for selected sequence file
	CAObjHandle fileVariables;						// displays the variables for the selected sequence file
	CAObjHandle insertionPalette;					// displays a palette for inserting items into sequences
	CAObjHandle sequenceFileLabel;					// hidden label connected to CaptionSource_CurrentSequenceFile. When the label receives a ConnectionActivity event, we update the application title bar
	CAObjHandle executionVariables;					// displays the variables for the selected execution
	CAObjHandle callStack;							// call stack selector listbox on the execution tab
	CAObjHandle threads;							// thread selector listbox on the execution tab
	CAObjHandle breakResumeButton;					// break/resume button for selected execution
	CAObjHandle terminateRestartButton;				// terminate/restart button for selected execution
	CAObjHandle	entryPoint1Button;					// entryPoint button 1 for selected sequence file
	CAObjHandle	entryPoint2Button;					// entryPoint button 2 for selected sequence file
	CAObjHandle	runCurrentSequenceButton;			// run-current-sequence button for selected sequence file
	CAObjHandle	statusBar;							// status bar at bottom of window
	CAObjHandle executionLabel;						// hidden label connected to CaptionSource_CurrentExecution. When the label receives a ConnectionActivity event, we update the application title bar
	
	int			programmaticallyUpdatingTabPages;	// TRUE while within ShowAppropriateTabs()
	CAObjHandle	engine;								// TestStand Engine Handle, stored here for convenient availability
	int			guiThreadId;						// for filtering out CVI events from other threads in the MainCallback
} ApplicationWindow;

static ApplicationWindow	gMainWindow;	// this application only has one window

// the presence of these two variables is expected by the tsErrChk macro from tsutil.h.  Usually you declare these variables as locals
// in each function that uses tsErrChk. However, since all the code in this file runs in a single thread, they can be globals for convenience
ERRORINFO	errorInfo = {0, 0, "", "", "", 0, 0};
ErrMsg		errMsg = "";

// forward declarations
static void	MenuDimmerCallback(int menuBarHandle, int panelHandle);
static int 	BuildSequenceViewContextMenu(int menuHandle);
static int	ShowAppropriateTabs(void);
static int 	ShowAppropriateStatusBarPanes(void);
static int	ExitApplication(void);
static void ClearErrorMessage(void);
static void DisplayError(int errorCode);
static int	RebuildMenuBar(int menuBar);
static int 	GetActiveXControlHandles(void);
static int 	RegisterActiveXEventCallbacks(void);
static int 	ConnectStatusBarPanes(void);
static int	ConnectTestStandControls(void);
static int	ArrangeControls(int processEvents);
static int	GetActiveViewManager(CAObjHandle *viewManager);
static int 	MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2);
static int	RebuildEditMenu(void);

static HRESULT CVICALLBACK ListBar_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL  finalResize);
static HRESULT CVICALLBACK SequenceFilesView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
static HRESULT CVICALLBACK ExecutionView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
static HRESULT CVICALLBACK FileVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
static HRESULT CVICALLBACK CallStack_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
static HRESULT CVICALLBACK ExecutionVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
static HRESULT CVICALLBACK InsertionPalette_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize);
static HRESULT CVICALLBACK Label_OnConnectionActivity(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_ConnectionActivityTypes  activity);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	int		error = 0;
	int		splashPanel = 0;
	VBOOL	appWillExitOnStart = VFALSE;
	long	exitCode = 0;
	
	nullChk( InitCVIRTE(0, argv, 0));	// initialize CVI runtime engine
	
	// load the panels for the main window from the .UIR file
	errChk( gMainWindow.panel = 		LoadPanelEx(0, "TestExec.uir", MAINPANEL, __CVIUserHInst));
	errChk( GetPanelHandleFromTabPage (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_FILE,      &gMainWindow.fileTab));
	errChk( GetPanelHandleFromTabPage (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_EXECUTION, &gMainWindow.executionTab));	
	errChk( GetPanelHandleFromTabPage (gMainWindow.panel, MAINPANEL_TAB, TABPAGE_REPORT,    &gMainWindow.reportTab));
	
	// Set ATTR_ACTIVATE_WHEN_CLICKED_ON to FALSE to prevent main panel
	// from stealing the focus from embedded ActiveX controls when the
	// menu is clicked.  The menu rendering code inside of
	// TS_InsertCommandsInMenu() calls the Win32 GetFocus() function to
	// determine the context for menu items such as Copy, Cut, Paste,
	// Delete, and "Help Topic..."
	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_ACTIVATE_WHEN_CLICKED_ON, FALSE));
	
	// prepare to use the TestStand ActiveX controls
	errChk( GetActiveXControlHandles());
	
	/**************** MY CODE *******************/
	char newPath[256] = "C:\\Users\\Michael_Harhay\\Documents\\CICDUtility\\Source Code\\UserInterface.xml";
	tsErrChk(TSUI_ApplicationMgrSetConfigFilePath(gMainWindow.applicationMgr, &errorInfo, newPath));
	
	//char *path = NULL;
	//tsErrChk(TSUI_ApplicationMgrGetConfigFilePath(gMainWindow.applicationMgr, &errorInfo, &path));
	//printf("path to config file: %s\n", path);
	/************** MY CODE END *****************/

	tsErrChk( TSUI_ApplicationMgrGetApplicationWillExitOnStart(gMainWindow.applicationMgr, &errorInfo, &appWillExitOnStart));
	
	if (!appWillExitOnStart)
	{
		// show a splash screen while starting up
		errChk( splashPanel = LoadPanelEx(0, "TestExec.uir", SPLASH, __CVIUserHInst));
		errChk( InstallPopup(splashPanel));
	}

	// setup event callbacks for the TestStand User Interface Components
	errChk( RegisterActiveXEventCallbacks());

	errChk( InstallMainCallback(MainCallback, 0, 0));	// handle the EVENT_END_TASK event, etc.
	gMainWindow.guiThreadId = CmtGetCurrentThreadID();  // for filtering out CVI events from other threads in the MainCallback

	// make engine conveniently accessible
	tsErrChk( TSUI_ApplicationMgrGetEngine(gMainWindow.applicationMgr, &errorInfo, &gMainWindow.engine));	
	
	// setup connections for the TestStand User Interface Components   	
	errChk( ConnectTestStandControls());  
	
	// this application allows setting of breakpoints on sequences files, so let them persist
	tsErrChk( TS_EngineSetPersistBreakpoints(gMainWindow.engine, &errorInfo, VARIANT_TRUE));
	
	// Start up the TestStand User Interface Components.
	tsErrChk( TSUI_ApplicationMgrStart(gMainWindow.applicationMgr, &errorInfo));

	// localize strings in CVI controls and menus
	errChk( TS_LoadPanelResourceStrings(0, gMainWindow.engine, gMainWindow.panel, "TSUI_OI_MAIN_PANEL")); 	
	errChk( TS_LoadMenuBarResourceStrings(gMainWindow.engine, GetPanelMenuBar(gMainWindow.panel), 0, "TSUI_OI_MAIN_PANEL", errMsg));
	// localize strings in the TestStand ActiveX controls
	tsErrChk( TSUI_ApplicationMgrLocalizeAllControls(gMainWindow.applicationMgr, &errorInfo, "TSUI_OI_MAIN_PANEL"));
	
	// initialize the state of the tabs
	errChk( ShowAppropriateTabs());
	
	// initialize menubar
	errChk( RebuildMenuBar(GetPanelMenuBar(gMainWindow.panel)))
	errChk( InstallMenuDimmerCallback(GetPanelMenuBar(gMainWindow.panel), MenuDimmerCallback));
		
	// remove splash screen
	DiscardPanel(splashPanel);
	splashPanel = 0;

	// remember window and control positions from last time
	tsErrChk( TS_LayoutPersister_LoadSizes(gMainWindow.applicationMgr,  &errorInfo, NULL, 0, 10,
									gMainWindow.panel, MAINPANEL_LISTBAR, 
									gMainWindow.panel, MAINPANEL_TAB, 
									gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, 
									gMainWindow.fileTab, TABPANEL_SEQUENCES, 
									gMainWindow.fileTab, TABPANEL_VARIABLES, 
									gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, 
									gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, 
									gMainWindow.executionTab, TABPANEL_2_CALLSTACK, 
									gMainWindow.executionTab, TABPANEL_2_THREADS, 
									gMainWindow.executionTab, TABPANEL_2_VARIABLES));
	tsErrChk( TS_LayoutPersister_LoadBounds(gMainWindow.applicationMgr, &errorInfo, NULL, TRUE, 0, 1,  
									gMainWindow.panel, 0));

	errChk( ArrangeControls(FALSE));  // make any adjustments needed due to font sizes, etc. (pass FALSE to processEvents to ensure that the StartExecution event for the login execution isn't processed until the splash screen is gone and the main window is displayed. This makes sure the login dialog is modal to correct window (the main window)

	// give initial control focus to the step list control
	errChk( SetActiveCtrl(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW));
	errChk( SetActivePanel(gMainWindow.fileTab));
	
	// display window and process user input until application exits
	errChk( DisplayPanel(gMainWindow.panel));

	errChk( RunUserInterface());

	errChk( TSUI_ApplicationMgrGetExitCode(gMainWindow.applicationMgr, &errorInfo, &exitCode));

Error:
	if (gMainWindow.panel > 0)
		DiscardPanel(gMainWindow.panel);

	if (splashPanel > 0)
		DiscardPanel(splashPanel);

	if (gMainWindow.aboutBox > 0)
		DiscardPanel(gMainWindow.aboutBox);

	if (UIEActiveXCtrlNotRegistered == error)
	{
		// This usually means the current active version of TestStand does not match the version
		// this application was built with.
#ifndef _WIN64
		const char msg[] = "This TestStand User Interface was built for a version of TestStand different than the version that is currently active.\n"
						   "Use the TestStand Version Selector to activate the correct version and ensure that 32-bit TestStand is installed.";
#else
		const char msg[] = "This TestStand User Interface was built for a version of TestStand different than the version that is currently active.\n"
						   "Use the TestStand Version Selector to activate the correct version and ensure that 64-bit TestStand is installed.";
#endif

		MessagePopup("Error", msg);
	}
	else
	{
		DisplayError(error);	
	}

	return exitCode;
}			

////////////////////////////////////////////////////////////////////////////////////

// if the execution has a report, switch to the report tab either immediately if the execution is visible, or, whenever the execution is viewed next
static int ShowReport(CAObjHandle execution)
{
	int			error = 0;
	int			isTheSelectedExecution;
	CAObjHandle	selectedExecution = 0;

	// switch to report view when this execution is next viewed
	tsErrChk( TS_PropertySetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, TABPAGE_REPORT)); // activate the reportTab when the user views this execution

	tsErrChk( TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &selectedExecution));
	errChk(TS_ActiveXHandlesReferToSameObject(execution, selectedExecution, &isTheSelectedExecution));

	if (isTheSelectedExecution) 
		ShowAppropriateTabs();  // switch to report view tab now
		
Error:
	CA_DiscardObjHandle(selectedExecution);
	return error;
}

///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////

static int MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2)
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

////////////////////////////////////////////////////////////////////////////////////

// user clicked on menubar, make sure all menus have correct items with the correct enabled states
static void	MenuDimmerCallback(int menuBarHandle, int panelHandle)
{
	int	error = 0;
	
	errChk( RebuildMenuBar(menuBarHandle));
	
Error:
	DisplayError(error);
}

///////////////////////////////////////////////////////////////////////////////

// eventData2 is the panel handle of the newly actived tab
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

///////////////////////////////////////////////////////////////////////////

// release any TestStand objects and save any settings here
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

///////////////////////////////////////////////////////////////////////////

// The ApplicationMgr sends this event when it is ok to exit the application
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

///////////////////////////////////////////////////////////////////////////

// The ApplicationMgr sends this event when it's busy doing something so we know to display a hourglass cursor or equivalent
HRESULT CVICALLBACK ApplicationMgr_OnWait(CAObjHandle caServerObjHandle, void *caCallbackData, VBOOL  bShowWait)
{	
	SetMouseCursor(bShowWait ? VAL_HOUR_GLASS_CURSOR : VAL_DEFAULT_CURSOR);
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////

// the ApplicationMgr sends this event to request that the UI display the report for a particular execution
HRESULT CVICALLBACK ApplicationMgr_OnDisplayReport(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution execution)
{
	int	error = 0;
	
	errChk( ShowReport(execution));

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// the ApplicationMgr sends this event to request that the UI display a particular execution
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

///////////////////////////////////////////////////////////////////////////

// the user toggled edit mode. the only way to do that in this application is to type ctrl-shift-alt-insert, which is the edit mode toggle key this application specifies in designer for the ApplicationMgr control. 
// to prevent edit mode from being toggled with a hotkey, set ApplicationMgr.EditModeShortcutKey to ShortcutKey_VK_NOT_A_KEY
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

///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////

// the ApplicationMgr sends this event to request that the UI display a particular sequence file
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

///////////////////////////////////////////////////////////////////////////

// the ApplicationMgr sends this event to report any error that occurs when the TestStand UI controls respond to user input (ie, an error 
// other than one returned by a method you call directly). For example, if a TestStand menu command generates an error, this handler displays it
HRESULT CVICALLBACK ApplicationMgr_OnReportError(CAObjHandle caServerObjHandle, void *caCallbackData, long  errorCode, char *errorMessage)
{
	strcpy(errMsg, errorMessage);	// update global errMsg buffer
	DisplayError(errorCode);
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////

// the ApplicationMgr sends this event whenever an execution starts
HRESULT CVICALLBACK ApplicationMgr_OnStartExecution(CAObjHandle caServerObjHandle, void* caCallBackData, TSUIObj_Execution execution, TSUIObj_Thread thread,  VBOOL initiallyHidden)
{
	int	error = 0;

	// add a custom property to the execution to store which tab we are displaying for this execution. Initially show the execution tab
	tsErrChk( TS_PropertySetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, TABPAGE_EXECUTION));

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////
	
// the ExecutionViewMgr sends this event whenever a new execution is selected
HRESULT CVICALLBACK ExecutionViewMgr_OnExecutionChanged(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution  execution)
{
	int	error = 0;

	// switch to report or steps tab depending on what the execution displayed last
	errChk( ShowAppropriateTabs());
	
Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////
	
// the TABPANEL_STEPLISTVIEW control sends this event when the user right clicks on it
HRESULT CVICALLBACK SequenceFilesView_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
 															TSUIType_OLE_XPOS_PIXELS x, TSUIType_OLE_YPOS_PIXELS y)
{
	int	error = 0;
		
	errChk( BuildSequenceViewContextMenu(menuHandle));
	
Error:		
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// the TABPANEL_2_STEPLISTVIEW ctrl sends this event when the user right clicks on it
HRESULT CVICALLBACK ExecutionView_OnCreateContextMenu(CAObjHandle caServerObjHandle, void *caCallbackData, long menuHandle,
                              								TSUIType_OLE_XPOS_PIXELS  x, TSUIType_OLE_YPOS_PIXELS  y)
{
	int	error = 0;
		
	errChk( BuildSequenceViewContextMenu(menuHandle));

Error:		
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// the MAINPANEL_LISTBAR ctrl sends this event when the user right clicks on it
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

///////////////////////////////////////////////////////////////////////////

// the TABPANEL_SEQUENCES ctrl sends this event when the user right clicks on it
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

///////////////////////////////////////////////////////////////////////////

static int UpdateWindowTitle()
{
	int 	error = 0;
	char	title[1024];
	int		pageIndex;
	char *	documentDescription = NULL;
	
	errChk( TS_GetResourceString(gMainWindow.engine, "TSUI_OI_MAIN_PANEL", "TESTSTAND_USER_INTERFACE", "", sizeof(title), title));

	tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));

	if (pageIndex == SEQUENCE_FILES_PAGE_INDEX)	// sequence files are visible
		tsErrChk( TSUI_SequenceFileViewMgrGetCaptionText(gMainWindow.sequenceFileViewMgr, &errorInfo, TSUIConst_CaptionSource_CurrentSequenceFile, VFALSE, "", &documentDescription));
	else	// executions are visible
		tsErrChk( TSUI_ExecutionViewMgrGetCaptionText(gMainWindow.executionViewMgr, &errorInfo, TSUIConst_CaptionSource_CurrentExecution, VFALSE, "", &documentDescription));
	
	if (*documentDescription)
		strcat(strcat(title, " - "), documentDescription);

	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_TITLE, title));

Error:
	CA_FreeMemory(documentDescription);
	return error;
}

///////////////////////////////////////////////////////////////////////////

// the ListBar sends this event when the listbar switches to a new page
HRESULT CVICALLBACK ListBar_OnCurPageChanged(CAObjHandle caServerObjHandle, void *caCallbackData, long  currentPage)
{
	int	error = 0;

	errChk( ShowAppropriateTabs());
	errChk( UpdateWindowTitle());

Error:
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// the StatusBar sends this event when the host window resizes
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

///////////////////////////////////////////////////////////////////////////

// build a context menu for a sequence view control that has been right-clicked
static int BuildSequenceViewContextMenu(int menuHandle)
{
	int			error = 0;
	CAObjHandle	cmds = 0;
	CAObjHandle	viewMgr;
	
	// determine which view manager menu commands apply to
	errChk( GetActiveViewManager(&viewMgr));
	
	tsErrChk( TSUI_ApplicationMgrNewCommands(gMainWindow.applicationMgr, &errorInfo, &cmds));

	// insert items for default sequence view context menu in the context menu
	tsErrChk( TSUI_CommandsInsertKind(cmds, &errorInfo, TSUIConst_CommandKind_DefaultSequenceViewContextMenu_Set, viewMgr, -1, "", "", NULL));
	errChk( TS_RemoveInvalidShortcutKeys(cmds, errMsg));	
	tsErrChk( TSUI_CommandsInsertIntoWin32Menu (cmds, &errorInfo, menuHandle, -1, VTRUE, VTRUE)); // we are using the context menu that the SequenceView control provides because it requires fewer lines of code. We could have built a CVI context menu instead and displayed it with RunPopupMenu(). If you display a CVI context menu, remember to convert the activeX-control-right-mouse click coordinates from control coordinates to panel coordinates by adding the top and left of the control to the click position.
	
Error:
	CA_DiscardObjHandle(cmds);
	return error;
}

///////////////////////////////////////////////////////////////////////////
// based on the current listbar page, show and hide the tabs that appear in the space to the right of the listbar
static int ShowAppropriateTabs(void)
{
	int			error = 0;
	long		pageIndex;
	CAObjHandle	execution = 0;
	void 	*	curActiveCtrlInfo;
	double	lastActiveExecutionTab = 1; // Default to the Execution tab when showing executions.


	gMainWindow.programmaticallyUpdatingTabPages = TRUE;

	// Note: Store the current Active control as EasyTab can change the active panel and control
	curActiveCtrlInfo = SaveActiveCtrl(0,0);

	tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));
	
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
		tsErrChk( TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &execution));
		if (execution)
			tsErrChk( TS_PropertyGetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, &lastActiveExecutionTab));

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

///////////////////////////////////////////////////////////////////////////

// show the appropriate group of status bar panes, depending on whether we are viewing a file, execution steps, or a report
static int ShowAppropriateStatusBarPanes(void)
{
	int	error = 0;
	int	activeTab;

	errChk( GetActiveTabPage(gMainWindow.panel, MAINPANEL_TAB, &activeTab));		

	if (activeTab == gMainWindow.fileTab)	
		tsErrChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, FileModel, FileSelectedSteps, FileNumberOfSteps"));
	else if (activeTab == gMainWindow.executionTab)
		tsErrChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, ExecutionModel, ExecutionSelectedSteps, ExecutionNumberOfSteps, ProgressText, ProgressPercent"));
	else
		tsErrChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, ExecutionModel, ReportLocation, ProgressText, ProgressPercent"));
		
Error:
	return error;
}

///////////////////////////////////////////////////////////////////////////

// call this function to exit the program
static int ExitApplication(void)
{
	int		error = 0; 
	VBOOL	canExitNow;
	
	tsErrChk( TSUI_ApplicationMgrShutdown(gMainWindow.applicationMgr, &errorInfo, &canExitNow));
	if (canExitNow)
		QuitUserInterface(0);
	
Error:
	DisplayError(error); 
	
	return canExitNow ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////

// call this function after you handle an error, unless you handle the error by calling DisplayError, which also calls this function
static void ClearErrorMessage(void)
{
	// clear out error message globals so that a future error that lacks an error description does not
	// unintentionally use the error description from a prior error.
	*errMsg = '\0';
	memset(&errorInfo, 0, sizeof(ERRORINFO)); 
}

//////////////////////////////////////////////

// displays a message box with the error code, the error message associated with the code, and any error description details 
// does nothing if errorCode is not negative
static void DisplayError(int errorCode)
{
	if (errorCode < 0)
	{
		TS_DisplayError(errorCode, errMsg, gMainWindow.engine);	// errMsg is a global, see top of file
		ClearErrorMessage();	
	}
}

//////////////////////////////////////////////////

// returns the SequenceFileViewManager or the ExecutionViewManager depending on whether the application is displaying sequence files or executions
// the returned viewManager is a weak reference (ie. do not call CA_DiscardObjHandle on it).
static int GetActiveViewManager(CAObjHandle *viewManager)
{
	int	error = 0;
	int	page;
	
	tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &page));
	if (page == SEQUENCE_FILES_PAGE_INDEX)	// sequence files are visible, sequence file menu commands apply
		*viewManager = gMainWindow.sequenceFileViewMgr;
	else
	if (page == EXECUTIONS_PAGE_INDEX)		// executions are visible, execution menu commands apply
		*viewManager = gMainWindow.executionViewMgr;

Error:
	return error;
}

//////////////////////////////////////////////////

// the Edit menu is rebuild both by RebuildMenuBar and RebuildEditMenu, so this array is at file scope so they can both access it
static enum TSUIEnum_CommandKinds	sEditMenuCommands[] = 
{
	TSUIConst_CommandKind_DefaultEditMenu_Set,			// add all the usual commands in an Edit menu
	TSUIConst_CommandKind_NotACommand					// list terminator
};


// make sure all menus have appropriate items with the correct enabled states
static int RebuildMenuBar(int menuBar)
{
	VBOOL isEditor = 0;
	int									error = 0;
	CAObjHandle							viewMgr = 0;
	static enum TSUIEnum_CommandKinds	fileMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultFileMenu_Set,			// add all the usual commands in a File menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	
	// Edit menu commands defined in sEditMenuCommands, above this function
	
	static enum TSUIEnum_CommandKinds	executeMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultExecuteMenu_Set,		// add all the usual commands in a Execute menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	static enum TSUIEnum_CommandKinds	debugMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultDebugMenu_Set,			// add all the usual commands in a Debug menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	static enum TSUIEnum_CommandKinds	configureMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultConfigureMenu_Set,		// add all the usual commands in a Configure menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	static enum TSUIEnum_CommandKinds	configureMenuCommands_EditMode[] = 
	{
		TSUIConst_CommandKind_ConfigureEngineEnvironment,		// add all the usual commands in a Configure menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	
	static enum TSUIEnum_CommandKinds	toolsMenuCommands[] = 
	{
		TSUIConst_CommandKind_DefaultToolsMenu_Set,			// add all the usual commands in a Tools menu
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	static enum TSUIEnum_CommandKinds	helpMenuCommands[] = 
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

///////////////////////////////////////////////////////////////////////////

// We specifically rebuild the Edit menu on EVENT_GOT_FOCUS/EVENT_LOST_FOCUS because TS_InsertCommandsInMenu removes the common edit menu shortcuts (Ctrl-X, Ctrl-C, Ctrl-V, Ctrl-A, Delete) 
// when the focus is not on an ActiveX control such as a TestStand control. It does this because the presence of those shortcut keys in the menu (even on dimmed items) prevents their 
// operation with CVI controls such as tables and text boxes. Because these shortcut keys are not always present in the menu, we do not always get a MenuDimmerCallback when they are pressed.
// thus, we also rebuild the Edit menu when the control focus changes. We could instead rebuild the entire menu bar by calling RebuildMenuBar, but depending on the machine, that might 
// noticably reduce the speed at which you can tab between controls.
static int RebuildEditMenu()
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

///////////////////////////////////////////////////////////////////////////
	
// get handles to ActiveX controls in order to call their particular API's
static int GetActiveXControlHandles(void)
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

///////////////////////////////////////////////////////////////////////////

static int RegisterActiveXEventCallbacks(void)
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

///////////////////////////////////////////////////////////////////////////

static int ConnectTestStandControls()
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
	
	return error;
}

///////////////////////////////////////////////////////////////////////////

static int ConnectStatusBarPanes(void)
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
	
////////////////////////////////////////////////////////////////////////////////////

// displays the about box when the user selects Help>>About...
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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

// adjust controls to fit within current window size
static int ArrangeControls(int processEvents)
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
	tsErrChk( TSUI_ApplicationMgrGetIsEditor(gMainWindow.applicationMgr, &errorInfo, &isEditor));
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

///////////////////////////////////////////////////////////////////////////////

        // user dragged the vertical bar on the listbar that separates it from the tab control
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
                              
///////////////////////////////////////////////////////////////////////////

// user dragged the horizontal bar that separates the step list from the sequence list and file variables
static HRESULT CVICALLBACK SequenceFilesView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, TABPANEL_SEQUENCES, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}


///////////////////////////////////////////////////////////////////////////

// user dragged vertical bar that separates the file variables from the sequences list 
static HRESULT CVICALLBACK FileVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_VARIABLES, TABPANEL_SEQUENCES, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// user dragged the vertical bar that separates the insertion palette from the file step list and the file variables
static HRESULT CVICALLBACK InsertionPalette_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, TABPANEL_STEPLISTVIEW, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// user dragged the horizontal bar that separates the execution step list from the call stack, thread list, and execution variables
static HRESULT CVICALLBACK ExecutionView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, TABPANEL_2_CALLSTACK, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// user dragged the vertical bar that separates the execution variables from the callstack
static HRESULT CVICALLBACK ExecutionVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_VARIABLES, TABPANEL_2_CALLSTACK, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

///////////////////////////////////////////////////////////////////////////

// user dragged the vertical bar that separates the callstack from the threads
static HRESULT CVICALLBACK CallStack_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_CALLSTACK, TABPANEL_2_THREADS, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}


///////////////////////////////////////////////////////////////////////////

// one of the hidden labels connected to display the current file and current execution has changed
static HRESULT CVICALLBACK Label_OnConnectionActivity(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_ConnectionActivityTypes  activity)
{
	int	error = 0;
	
	errChk( UpdateWindowTitle());
	
Error:
	return error < 0 ? E_FAIL : S_OK;	
}

