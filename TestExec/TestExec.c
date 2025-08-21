/***************************************************************************//*!
* \file TestExec.c
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-15 4:28:07 PM
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

#include "TestExec.h"

#include "SystemLog_LIB.h"

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

/***************************************************************************//*!
* \brief This application only has one window
*******************************************************************************/
ApplicationWindow gMainWindow;

/***************************************************************************//*!
* \brief The presence of these two variables is expected by the errChk 
*		 macro from tsutil.h.  Usually you declare these variables as locals 
*		 in each function that uses errChk. However, since all the code in 
*		 this file runs in a single thread, they can be globals for convenience
*******************************************************************************/
ERRORINFO	errorInfo = {0, 0, "", "", "", 0, 0};
ErrMsg		errMsg = "";

extern int glbNumSockets;
extern int glbPanelHeight;
extern int glbPanelWidth;
extern int glbPanelTop;
extern int glbPanelLeft;

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Main function
*******************************************************************************/
int main(int argc, char *argv[])
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	int		splashPanel = 0;
	VBOOL	appWillExitOnStart = VFALSE;
	long	exitCode = 0;
	
	// Only allow 1 instance of TestExec to run
    int duplicate = 0;
    CheckForDuplicateAppInstance (ACTIVATE_OTHER_INSTANCE, &duplicate);
    if(duplicate) return 0;
	
	nullChk( InitCVIRTE(0, argv, 0));	// initialize CVI runtime engine
	
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
	
	// Init execution panel
	errChk( ArxUtil_InitExecPanel());
	errChk( PRJGUI_AddProjectGUIControls(gMainWindow.execpanel));
	
	// prepare to use the TestStand ActiveX controls
	errChk( GetActiveXControlHandles());

	errChk( TSUI_ApplicationMgrGetApplicationWillExitOnStart(gMainWindow.applicationMgr, &errorInfo, &appWillExitOnStart));
	
	if (!appWillExitOnStart)
	{
		// show a splash screen while starting up
		errChk( splashPanel = LoadPanelEx(0, "SplashPanel.uir", ARXSPLASH, __CVIUserHInst));
		errChk( InstallPopup(splashPanel));
		Delay (3.0); // Arbitrary delay
	}

	// setup event callbacks for the TestStand User Interface Components
	errChk( RegisterActiveXEventCallbacks());

	errChk( InstallMainCallback(MainCallback, 0, 0));	// handle the EVENT_END_TASK event, etc.
	gMainWindow.guiThreadId = CmtGetCurrentThreadID();  // for filtering out CVI events from other threads in the MainCallback

	// make engine conveniently accessible
	errChk( TSUI_ApplicationMgrGetEngine(gMainWindow.applicationMgr, &errorInfo, &gMainWindow.engine));	
	
	// setup connections for the TestStand User Interface Components   	
	errChk( ConnectTestStandControls());  
	
	// this application allows setting of breakpoints on sequences files, so let them persist
	errChk( TS_EngineSetPersistBreakpoints(gMainWindow.engine, &errorInfo, VARIANT_TRUE));
	
	// Start up the TestStand User Interface Components.
	errChk( TSUI_ApplicationMgrStart(gMainWindow.applicationMgr, &errorInfo));

	// localize strings in CVI controls and menus
	errChk( TS_LoadPanelResourceStrings(0, gMainWindow.engine, gMainWindow.panel, "TSUI_OI_MAIN_PANEL")); 	
	errChk( TS_LoadMenuBarResourceStrings(gMainWindow.engine, GetPanelMenuBar(gMainWindow.panel), 0, "TSUI_OI_MAIN_PANEL", errMsg));
	// localize strings in the TestStand ActiveX controls
	errChk( TSUI_ApplicationMgrLocalizeAllControls(gMainWindow.applicationMgr, &errorInfo, "TSUI_OI_MAIN_PANEL"));
	
	// initialize the state of the tabs
	errChk( ShowAppropriateTabs());
	
	// initialize menubar
	errChk( RebuildMenuBar(GetPanelMenuBar(gMainWindow.panel)))
	errChk( InstallMenuDimmerCallback(GetPanelMenuBar(gMainWindow.panel), MenuDimmerCallback));
		
	// remove splash screen
	DiscardPanel(splashPanel);
	splashPanel = 0;

	// remember window and control positions from last time
	errChk( TS_LayoutPersister_LoadSizes(gMainWindow.applicationMgr,  &errorInfo, NULL, 0, 10,
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
	errChk( TS_LayoutPersister_LoadBounds(gMainWindow.applicationMgr, &errorInfo, NULL, TRUE, 0, 2,  
										  gMainWindow.panel, 0,
										  gMainWindow.execpanel, 0));
	
	// Set window size based on config
	SetPanelSize (gMainWindow.panel, glbPanelHeight, glbPanelWidth);
	SetPanelPos (gMainWindow.panel, glbPanelTop, glbPanelLeft);
	SetPanelSize (gMainWindow.execpanel, glbPanelHeight, glbPanelWidth);
	SetPanelPos (gMainWindow.execpanel, glbPanelTop, glbPanelLeft);

	errChk( ArrangeControls(FALSE));  // make any adjustments needed due to font sizes, etc. (pass FALSE to processEvents to ensure that the StartExecution event for the login execution isn't processed until the splash screen is gone and the main window is displayed. This makes sure the login dialog is modal to correct window (the main window)

	// give initial control focus to the step list control
	errChk( SetActiveCtrl(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW));
	errChk( SetActivePanel(gMainWindow.fileTab));
	
	// display window and process user input until application exits
	errChk( DisplayPanel(gMainWindow.panel));
	errChk( DisplayPanel(gMainWindow.execpanel));
	for (int i=0;i<glbNumSockets;++i)
		errChk( DisplayPanel(gMainWindow.nests[i]));
	
	errChk( RunUserInterface());

	errChk( TSUI_ApplicationMgrGetExitCode(gMainWindow.applicationMgr, &errorInfo, &exitCode));

Error:
	if (gMainWindow.panel > 0)
		DiscardPanel(gMainWindow.panel);
	
	/*if (gMainWindow.execpanel > 0)
		DiscardPanel(gMainWindow.execpanel);*/

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

/***************************************************************************//*!
* \brief Call this function to exit the program 
*******************************************************************************/
int ExitApplication(void)
{
	int		error = 0; 
	VBOOL	canExitNow;
	
	errChk( TSUI_ApplicationMgrShutdown(gMainWindow.applicationMgr, &errorInfo, &canExitNow));
	if (canExitNow)
		QuitUserInterface(0);
	
Error:
	DisplayError(error); 
	
	return canExitNow ? TRUE : FALSE;
}
//! \cond
/// REGION END
//! \endcond