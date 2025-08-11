/***************************************************************************//*!
* \file TestExec.c
* \author Biye Chen
* \author Jai Prajapati
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/1/2021 12:49:12 PM
* \brief A rework of the TestStand Sequencer custom GUI based on work by
* 	Michael Foe
* 
* This file only contains the application entry (main) and exit (ExitApplication)
* 	points.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
/* 20220125Biye: This shortcut is removed! */
// Note:	This example can function as an editor or an operator interface. The user can change the edit mode with a keystroke (ctrl-alt-shift-insert) or with a command line 
//			argument. For more information and for instructions on how prevent the user from changing the edit mode, refer to the TestStand Reference Manual>>Creating Custom 
//			User Interfaces>>Editor versus Operator Interface Applications>>Creating Editor Applications.

// Note:	TestStand installs the source code files for the default user interfaces in the <TestStand>\UserInterfaces and <TestStand Public>\UserInterfaces directories. 
//			To modify the installed user interfaces or to create new user interfaces, modify the files in the <TestStand Public>\UserInterfaces directory. 
//			You can use the read-only source files for the default user interfaces in the <TestStand>\UserInterfaces directory as a reference. 
//			National Instruments recommends that you track the changes you make to the user interface source code files so you can integrate the changes with any enhancements in future versions of the TestStand User Interfaces.

//==============================================================================
// Include files

// Arxtron Includes
#include "CommonTSIncludes.h"
#include "TSMessaging.h"

// Default includes from CVI template
#include <cvirte.h>
#include <userint.h>
#include <utility.h>

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

/***************************************************************************//*!
* \brief This struct contains all UI handles
*******************************************************************************/
ApplicationWindow	gMainWindow;	// this application only has one window

/* 20210301Biye: this tsErrChk is DIFFERENT than the one found in ArxtronToolslib.  Do NOT include ArxtronToolslib in this application. The naming being the same was a coincidence */
/* 20220125Biye: This modified UI may have threads (Ex. threads in TestStand sending messages), though the error variables are currently rarely used in a way where this would be an issue*/
//!@{
/***************************************************************************//*!
* \brief The presence of this variable is expected by the tsErrChk macro from tsutil.h.  Usually you declare these variables as locals
* 	in each function that uses tsErrChk. However, since all the code in this file runs in a single thread, they can be globals for convenience
*******************************************************************************/
ERRORINFO	errorInfo = {0, 0, "", "", "", 0, 0};
ErrMsg		errMsg = "";
//!@}

//!@{
/***************************************************************************//*!
* \brief Full path to the "Arxtron Sequencer.seq" or "ProjectSpecificSequencer.seq"
* 	file, this variable is populated using the TS_EngineFindFile function.
* 	The sequence file MUST be in one of TestStand's search paths.  If multiple
* 	instances are found, the first found instance will be used.
* 
* \sa ArxUtil_GetSequencer
*******************************************************************************/
char glbSequencerPath[MAX_PATHNAME_LEN] = {0};
char glbPrjSequencerPath[MAX_PATHNAME_LEN] = {0};
//!@}

/***************************************************************************//*!
* \brief This flag is set right before RunUserInterface for the main UI
*******************************************************************************/
int glbInterfaceStarted = 0;

/***************************************************************************//*!
* \brief Initialized value is only used for UI debugging purposes, otherwise
* 	this value is replaced by the **Number of TestSockets** value under **Model Options**
* 	setting in TestStand. When CVIUI_ONLY is defined, this is instead updated in
* 	ArxUtil_InitExecPanel()
* 
* \sa UI_DEBUG
*******************************************************************************/
double glbNumSockets = 30;

#ifdef CVIUI_ONLY
extern int glbPanelHeight;
extern int glbPanelWidth;
extern int glbPanelTop;
extern int glbPanelLeft;
extern int glbCVIMsgExitFlag;

extern int glbMessageNestNums[MAX_NESTS+1];

//!@{
/***************************************************************************//*!
* \brief Variables used to handle message communication between the CVI sequencer
* 	and the GUI.
* 
* This feature is intended to be similar to the TestStand equivalent in order to
* 	use the same UI for both sequencers.  Although currently the CVI implementation
* 	is not as optimized (slower/can lag) compared to TestStand
*******************************************************************************/
CmtThreadPoolHandle glbCVIMsgThreadPool = 0;
CmtThreadFunctionID glbCVIMsgThread[MAX_NESTS+1] = {0};
//!@}
int CVICALLBACK CVIMessageSender (void *FunctionData);
int CVICALLBACK CVILEDSender (void *FunctionData);
#endif

//==============================================================================
// Global functions

int	ExitApplication(void);
void CVICALLBACK MenuDimmerCallback(int menuBarHandle, int panelHandle);
void Initialize_TSMsgCB (void);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Entry function.  Initializes UI, TestStand connection, and various
* 	settings.
* 
* Based on the example UI main, mostly added a few more custom function calls.
* 
* This function contains an arbitrary (default 5s) delay to display the Arxtron
* 	Splash screen.  Enable/disable this to your discretion.
*******************************************************************************/
int main(int argc, char *argv[])
{
	int		error = 0;
	int		splashPanel = 0;
	VBOOL	appWillExitOnStart = VFALSE;
	long	exitCode = 0;
	
	// Only allow 1 instance of TestExec to run
    int duplicate = 0;
    CheckForDuplicateAppInstance (ACTIVATE_OTHER_INSTANCE, &duplicate);
    if(duplicate) return 0;
	
	nullChk( InitCVIRTE(0, argv, 0));	// initialize CVI runtime engine

	// Handle any additional inputs from command line
	if (argc > 1)
	{
		// Start with edit mode?
		// Start with debug mode?
		// Update types only
	}
	
	memset (glbNestOrigLeft, 0, sizeof(glbNestOrigLeft));
	
	// Load UI panels to memory
	SetSystemAttribute (ATTR_RESOLUTION_ADJUSTMENT, 0);		// Nest panels will resize to resolution settings otherwise
	errChk( ArxUtil_LoadPanelsInMemory());
	
	// Set ATTR_ACTIVATE_WHEN_CLICKED_ON to FALSE to prevent main panel
	// from stealing the focus from embedded ActiveX controls when the
	// menu is clicked.  The menu rendering code inside of
	// TS_InsertCommandsInMenu() calls the Win32 GetFocus() function to
	// determine the context for menu items such as Copy, Cut, Paste,
	// Delete, and "Help Topic..."
	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_ACTIVATE_WHEN_CLICKED_ON, FALSE));
	
#ifndef CVIUI_ONLY
	errChk( ArxUtil_InitTSPanel());
	if (argc==999 && UI_DEBUG)	/* 20210412Biye: Temp, implement properly when cmd line is implemented*/
	{
		errChk( ArxUtil_UpdateStructsToTS());
		return error;
	}
#endif
	errChk( ArxUtil_InitExecPanel());
	errChk( PRJGUI_AddProjectGUIControls(gMainWindow.execpanel));
	
	//==============================================================================
	// End of Panel Setup
	//==============================================================================
	
#ifdef UI_DEBUG
	if (!UI_DEBUG)
#endif
	{
#ifndef CVIUI_ONLY
		tsErrChk( TSUI_ApplicationMgrGetApplicationWillExitOnStart(gMainWindow.applicationMgr, &errorInfo, &appWillExitOnStart));
		
		if (!appWillExitOnStart)
#endif
		{
			// show a splash screen while starting up
			errChk( splashPanel = LoadPanelEx(0, "Splash.uir", ARXSPLASH, __CVIUserHInst));
			errChk( InstallPopup(splashPanel));
			Delay (5.0);	/* 20220127Biye: Arbitrary delay to show splash screen, comment out if need fast boot*/
		}

#ifndef CVIUI_ONLY
		// Start up the TestStand User Interface Components.
		tsErrChk( TSUI_ApplicationMgrStart(gMainWindow.applicationMgr, &errorInfo));

		errChk( ArxUtil_LocalizePanels());
		
		// initialize the state of the tabs
		errChk( ShowAppropriateTabs());
#endif
		
		// initialize menubar
		errChk( RebuildMenuBar(GetPanelMenuBar(gMainWindow.panel)));
		errChk( InstallMenuDimmerCallback(GetPanelMenuBar(gMainWindow.panel), MenuDimmerCallback));
		
		// remove splash screen
		DiscardPanel(splashPanel);
		splashPanel = 0;

#ifndef CVIUI_ONLY
		// remember window and control positions from last time
		tsErrChk( TS_LayoutPersister_LoadSizes(gMainWindow.applicationMgr,  &errorInfo, NULL, 0, 10,
										gMainWindow.tspanel, DEBUGPANEL_LISTBAR,
										gMainWindow.tspanel, DEBUGPANEL_TAB,
										gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, 
										gMainWindow.fileTab, TABPANEL_SEQUENCES, 
										gMainWindow.fileTab, TABPANEL_VARIABLES, 
										gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, 
										gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, 
										gMainWindow.executionTab, TABPANEL_2_CALLSTACK, 
										gMainWindow.executionTab, TABPANEL_2_THREADS, 
										gMainWindow.executionTab, TABPANEL_2_VARIABLES));
		tsErrChk( TS_LayoutPersister_LoadBounds(gMainWindow.applicationMgr, &errorInfo, NULL, TRUE, 0, 3, 
												gMainWindow.panel, 0,
												gMainWindow.tspanel, 0,
												gMainWindow.execpanel, 0));
#else
		// Set window size based on config
		SetPanelSize (gMainWindow.panel, glbPanelHeight, glbPanelWidth);
		SetPanelPos (gMainWindow.panel, glbPanelTop, glbPanelLeft);
#endif
	}
	
	errChk( ArrangeControls(FALSE));  // make any adjustments needed due to font sizes, etc. (pass FALSE to processEvents to ensure that the StartExecution event for the login execution isn't processed until the splash screen is gone and the main window is displayed. This makes sure the login dialog is modal to correct window (the main window)

#ifndef CVIUI_ONLY
	// give initial control focus to the step list control
	errChk( SetActiveCtrl(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW));
	errChk( SetActivePanel(gMainWindow.fileTab));
#endif
	
	// display window and process user input until application exits
	errChk( DisplayPanel(gMainWindow.panel));
	errChk( DisplayPanel(gMainWindow.execpanel));
	for (int i=0;i<glbNumSockets;++i)
		errChk( DisplayPanel(gMainWindow.nests[i]));
	
#ifdef UI_DEBUG
	if (!UI_DEBUG)
#endif
	{
#ifndef CVIUI_ONLY
		ArxCB_ModeSwitchCB (0, (argc==999?glbEngModeMenu:glbOpModeMenu), 0, 0);
#endif
		Initialize_TSMsgCB ();
#ifdef CVIUI_ONLY
		// Create all message threads
		errChk (CmtNewThreadPool((MAX_NESTS+1)*2, &glbCVIMsgThreadPool));
		errChk (CmtSetThreadPoolAttribute(glbCVIMsgThreadPool, ATTR_TP_THREAD_PRIORITY, THREAD_PRIORITY_HIGHEST));
		glbMessageNestNums[MAX_NESTS] = MAX_NESTS;
		errChk (CmtScheduleThreadPoolFunction(glbCVIMsgThreadPool, CVIMessageSender, (void*)(glbMessageNestNums+MAX_NESTS), glbCVIMsgThread+MAX_NESTS));
		for (int i=0; i<glbNumSockets; ++i)
		{
			glbMessageNestNums[i] = i;
			errChk (CmtScheduleThreadPoolFunction(glbCVIMsgThreadPool, CVIMessageSender, (void*)(glbMessageNestNums+i), glbCVIMsgThread+i));
		}
#endif
		errChk( ArxUtil_StartSequencer());
	}
	
	glbInterfaceStarted = 1;
	errChk( RunUserInterface());

#ifdef UI_DEBUG
	if (!UI_DEBUG)
#endif
	{
#ifndef CVIUI_ONLY
		errChk( TSUI_ApplicationMgrGetExitCode(gMainWindow.applicationMgr, &errorInfo, &exitCode));
#endif
	}

Error:
	PRJGUI_CloseCustomPanels();
	
	if (gMainWindow.panel > 0)
		DiscardPanel(gMainWindow.panel);
	// No need to discard ts/exec panels as they are children of mainpanel
	
	if (gMainWindow.arxPopupPanel > 0)
		DiscardPanel(gMainWindow.arxPopupPanel);

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
	else if (ARXERR_SEQUENCER_NOT_FOUND == error)
	{
		MessagePopup("Sequencer Not Found", "Arxtron Sequencer.seq and/or ProjectSpecificSequencer.seq not found in TestStand search paths\n"
					"Please check that search paths are set correctly in TestStand settings");
	}
	else if (ARXERR_GRID_CONFIG_WRONG == error)
	{
		char msg[512] = {0};
		sprintf (msg,"Nest Grid configuration does not accomodate number of sockets found in TestStand\n"
				 	 "Please update TestExecExecute_DEF.h and recompile\n"
				 	 "Number of sockets retrieved from TestStand: %d\n"
				 	 "Number of rows configured for UI: %d\n"
					 "Number of cols configured for UI: %d",
					 (int)glbNumSockets,CVIUI_NUM_NEST_ROWS,CVIUI_NUM_NEST_COLS);
		MessagePopup("Wrong Grid Configuration", msg);
	}
	else if (ARXERR_MODEL_SUP_LIB_NOT_FOUND == error)
	{
		MessagePopup("Support Library Not Found", "ReadModelOptionDefaults function was not able to be loaded from modelsupport2.dll. "
												  "Please make sure that the %TESTSTAND%\\Components\\Models\\TestStandModels\\modelsupport2 path"
												  "is added to system paths");
	}
	else if (ARXERR_CUSTOM_UI_FAIL == error)
	{
		MessagePopup("Failed to load Custom UI", "Failed to load some or all of the custom UI defined in ProjectSpecificGUI_LIB.");
	}
	else
	{
		DisplayError(error);	
	}

	return exitCode;
}

/***************************************************************************//*!
* \brief Call this function to exit the program
*******************************************************************************/
int ExitApplication(void)
{
	int		error = 0; 
	VBOOL	canExitNow = FALSE;
	
	SetCtrlVal (gMainWindow.nests[0], NEST_PANEL_LABEL_STATUS, "Attempting Shut Down...");
	SetCtrlVal (gMainWindow.nests[0], NEST_PANEL_LABEL_INSTRUCTIONS, " ");
	
#ifndef CVIUI_ONLY
	tsErrChk( TSUI_ApplicationMgrShutdown(gMainWindow.applicationMgr, &errorInfo, &canExitNow));
	if (canExitNow)
	{
#else
	if (0==glbCVIMsgExitFlag)
	{
		glbCVIMsgExitFlag = 1;
		CVI_TerminateSequencer (1);
		DelayWithEventProcessing(1.0);
		for (int i=0;i<MAX_KEYS;++i)
			Deinit_TSMsgCB (glbMsgStruct+i);
		if (glbSetVarLock) CmtDiscardLock (glbSetVarLock);
		if (glbCVIMsgThreadPool) CmtDiscardThreadPool (glbCVIMsgThreadPool);
		/* 20241106Tim: Uninitialize UserManagement */
		char errmsg[1024] = {0};
		UserManagement_Uninitialize (errmsg);
		canExitNow = 1;
#endif
		QuitUserInterface(0);
	}
	
Error:
	DisplayError(error); 
	
	return canExitNow ? TRUE : FALSE;
}
//! \cond
/// REGION END
//! \endcond
