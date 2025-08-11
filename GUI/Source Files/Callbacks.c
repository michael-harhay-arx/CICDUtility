/***************************************************************************//*!
* \file Callbacks.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/1/2021 2:30:54 PM
* \brief All UI callback functions.
* 
* This file contains all the UI callback functions in all UI elements. Most functions
* 	are copied from the UI example, with custom functions prefixed with ArxCB_
* 
* Some of the default functions are modified as well to have additional functionalty
* 	with custom UI elements.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "TestExecExecute.h"
#include <userint.h>
#include <string.h>
#include "MessagePopup.h"
#include "CommonTSIncludes.h"
#include <ansi_c.h>
#include <formatio.h>
#include "UIUtility.h"
#include "TSMessaging.h"

#ifdef CVIUI_ONLY
#include "CombinedDebugPanel.h"
#endif

//==============================================================================
// Constants

//==============================================================================
// Types

/* 20250307Biye: Usage seems redundant, refactor later */
#define SetTSSeqVarStruct(tsseqvar, seqname, menucallbackflag, vartype, varval)\
	sprintf(tsseqvar##.lookupString, "StationGlobals.Menus.%s", #menucallbackflag);\
	strcpy(tsseqvar##.seqName, seqname);\
	tsseqvar##.varType = vartype;\
	sprintf(tsseqvar##.varVal, "%d", varval);

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

/***************************************************************************//*!
* \brief Keeps track of whether a nest result box has been expanded from ArxCB_ExpandTestInfo
*******************************************************************************/
int glbNestExpanded[CVIUI_TOTAL_POSSIBLE_NESTS] = {0};
//!@{
/***************************************************************************//*!
* \brief Variable used for tracking idle time
*******************************************************************************/
int glbGetNewEventTime = 0;
double glbNewEventTime = 0.0;
//!@}

extern int glbOpViewMenu;
extern int glbEngViewMenu;

extern HINSTANCE glbHandleAIO;
extern HINSTANCE glbHandleDIO;
extern HINSTANCE glbHandleVisa;

extern TSSeqVar glbWatchVars[100];
extern int glbNumWatching;
extern TSSeqVar glbSetVars[100];
extern int glbNumSetting;

//!@{
/***************************************************************************//*!
* \brief Variables used for tracking TestStand Variable Values
*******************************************************************************/
int glbWatchExpPanelHandle = 0;
int glbWatchExpThreadID = 0;
//!@}
extern int glbWatchExpThreadEn;

#ifdef CVIUI_ONLY
extern char glbRootPath[MAX_PATHNAME_LEN];
extern int glbReinjectModeActive;
extern int glbCapabilityModeActive;
#endif

//==============================================================================
// Global functions

int UpdateWatchVarStruct (void *functionData);
int ExitApplication(void);

//! \cond
/// REGION END

/// REGION START Custom Callbacks
//! \endcond
/***************************************************************************//*!
* \brief Expand the test info and additional info list for viewing additional
* 	information if the default nest size is too small
*******************************************************************************/
int CVICALLBACK ArxCB_ExpandTestInfo (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int nestNum = 0;
	int panelLeft = 0;
	int testInfoWidthWithSpacer = CVIUI_TEST_INFO_WIDTH+2*CVIUI_SPACER;
	int moveLeft = 0;
	
	switch (event)
	{
		case EVENT_LEFT_DOUBLE_CLICK:
			// Check if size already big enough to display info
			if (glbNestOrigWidth>testInfoWidthWithSpacer)
				break;
			
			// Find the nest that was clicked
			for (nestNum; nestNum<glbNumSockets; ++nestNum)
			{
				if (panel==gMainWindow.nests[nestNum])
					break;
			}
			
			GetPanelAttribute (panel, ATTR_LEFT, &panelLeft);
			
			if (glbNestExpanded[nestNum]==1)
			{
				SetPanelAttribute (panel, ATTR_WIDTH, glbNestOrigWidth);
				SetCtrlAttribute (panel, control, ATTR_WIDTH, glbNestOrigWidth-2*CVIUI_SPACER);
				SetPanelAttribute (panel, ATTR_LEFT, glbNestOrigLeft[nestNum]);
				glbNestExpanded[nestNum] = 0;
			}
			else
			{
				SetPanelAttribute (panel, ATTR_WIDTH, testInfoWidthWithSpacer);
				SetCtrlAttribute (panel, control, ATTR_WIDTH, CVIUI_TEST_INFO_WIDTH);
				// If the nest being expanded will go past the right edge of the sequencer
				if (panelLeft>(glbWindowWidth-testInfoWidthWithSpacer))
				{
					moveLeft = CVIUI_TEST_INFO_WIDTH-glbNestOrigWidth;
					SetPanelAttribute (panel, ATTR_LEFT, glbNestOrigLeft[nestNum]-moveLeft);
				}
				glbNestExpanded[nestNum] = 1;
			}
			SetCtrlAttribute (panel, control, ATTR_LEFT, CVIUI_SPACER);
			break;
	}

	return 0;
}

//! \cond TESTSTAND
#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Switch between Engineering and Operator mode DOCTODO add reference to modes?
*******************************************************************************/
void CVICALLBACK ArxCB_ModeSwitchCB (int menuBar, int menuItem, void *callbackData, int panel)
{
	VBOOL enable = VFALSE;
	CAObjHandle user = 0;
	
	if (menuItem==glbEngModeMenu)
	{
		enable				= VTRUE;
		user				= gMainWindow.userAdmin;
	}
	else if (menuItem==glbOpModeMenu)
	{
		enable				= VFALSE;
		user				= gMainWindow.userOper;
	}
	
	glbActivePanel = ((menuItem==glbEngModeMenu||menuItem==glbEngViewMenu)?
					  gMainWindow.tspanel:gMainWindow.execpanel);
	
	// Set stationglobal for mode
	if (menuItem!=glbOpViewMenu && menuItem!=glbEngViewMenu)
	{
		TS_EngineSetCurrentUser(gMainWindow.engine, &errorInfo, user);
		TSUI_ApplicationMgrSetIsEditor(gMainWindow.applicationMgr, &errorInfo, enable);
		SetCtrlAttribute(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, ATTR_VISIBLE, enable);
		TS_EngineSetBreakpointsEnabled(gMainWindow.engine, &errorInfo, enable);
		TS_EngineSetTracingEnabled(gMainWindow.engine, &errorInfo, enable);
		ArrangeControls(FALSE);
	}
	DisplayPanel (glbActivePanel);
}
#endif
//! \endcond TESTSTAND

/***************************************************************************//*!
* \brief Menu callback for default sequencer functionalities
* 
* * Change Recipe
* * Run Golden Positive Test
* * Run Golden Negative Test
* * Reinject finished part
* * Reset pass/fail/yield/count
* * Capability Mode
* * Toggle Traceability
*******************************************************************************/
void CVICALLBACK ArxCB_MenuCallback (int menuBar, int menuItem, void *callbackData, int panel)
{
	int menu = (int) callbackData;
	int hasLock = 0;
	TSSeqVar tmpSeqVar;
	memset(&tmpSeqVar, 0, sizeof(tmpSeqVar));
	char errmsg[1024] = {0};
	char path[256] = {0};
	
	switch (menu)
	{
#ifndef CVIUI_ONLY
		case MENU_RECIPE:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_RECIPE, VARTYPE_BOOL, VTRUE);
			break;
		case MENU_GOLDEN_POSITIVE:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_GOLDEN_POSITIVE, VARTYPE_BOOL, VTRUE);
			break;
		case MENU_GOLDEN_NEGATIVE:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_GOLDEN_NEGATIVE, VARTYPE_BOOL, VTRUE);
			break;
		case MENU_REINJECT:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_REINJECT, VARTYPE_BOOL, VTRUE);
			break;
		case MENU_RESET_STATISTICS:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_RESET_STATISTICS, VARTYPE_BOOL, VTRUE);
			break;
		case MENU_CAPABILITY:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_CAPABILITY, VARTYPE_BOOL, VTRUE);
			break;
		case MENU_TRACE_TOGGLE:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_TRACE_TOGGLE, VARTYPE_BOOL, VTRUE);
			break;
#else
		case MENU_RECIPE:
			sprintf (path, "%s\\Configuration\\Recipe.ini", glbRootPath);
			ReloadRecipe (path, errmsg);
			break;
		case MENU_GOLDEN_POSITIVE:
			break;
		case MENU_GOLDEN_NEGATIVE:
			break;
		case MENU_REINJECT:
			glbReinjectModeActive = !glbReinjectModeActive;
			break;
		case MENU_RESET_STATISTICS:
			ResetStatistics (errmsg);
			break;
		case MENU_CAPABILITY:
			glbCapabilityModeActive = !glbCapabilityModeActive;
			SetCtrlVal (gMainWindow.execpanel, EXEC_PANEL_LED_CAPABILITY, glbCapabilityModeActive);
			break;
		case MENU_TRACE_TOGGLE:
			break;
#endif
	}
	
#ifndef CVIUI_ONLY
	CmtGetLockEx (glbSetVarLock, 1, CMT_WAIT_FOREVER, &hasLock);
	if (hasLock)
	{
		strcpy (glbSetVars[glbNumSetting].lookupString, tmpSeqVar.lookupString);
		strcpy (glbSetVars[glbNumSetting].seqName, tmpSeqVar.seqName);
		glbSetVars[glbNumSetting].varType = tmpSeqVar.varType;
		strcpy (glbSetVars[glbNumSetting].varVal, tmpSeqVar.varVal);
		++glbNumSetting;
		CmtReleaseLock (glbSetVarLock);
	}
#endif
}

/***************************************************************************//*!
* \brief Menu callback for opening various panels
* 
* * Edit Recipe
* * Edit System Config
* * Edit Station Config
* * Watch Panel
* * Combined Debug Panel
*******************************************************************************/
void CVICALLBACK ArxCB_OpenPanel (int menuBar, int menuItem, void *callbackData, int panel)
{
	int menu = (int) callbackData;
	
	int hasLock = 0;
	TSSeqVar tmpSeqVar;
	memset(&tmpSeqVar, 0, sizeof(tmpSeqVar));
	
#ifndef CVIUI_ONLY
	switch (menu)
	{
		case MENU_EDIT_RECIPE:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_EDIT_RECIPE, VARTYPE_BOOL, VTRUE);			
			break;
		case MENU_EDIT_SYSTEM_CONFIG:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_EDIT_SYSTEM_CONFIG, VARTYPE_BOOL, VTRUE);				
			break;
		case MENU_EDIT_STATION_CONFIG:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_EDIT_STATION_CONFIG, VARTYPE_BOOL, VTRUE);				
			break;
		case MENU_WATCHEXP_PANEL:
			if(!glbWatchExpPanelHandle)
			{
				glbWatchExpPanelHandle = LoadPanelEx(0, "WatchExpressionsWindow.uir", WATCHPANEL,  __CVIUserHInst);
				DisplayPanel(glbWatchExpPanelHandle);
			}
			break;
		case MENU_DEBUG_PANEL:
			SetTSSeqVarStruct(tmpSeqVar, ARX_UI_CAPABILITY_SEQNAME, MENU_DEBUG_PANEL, VARTYPE_BOOL, VTRUE);
			CmtGetLockEx (glbSetVarLock, 1, CMT_WAIT_FOREVER, &hasLock);
			if (hasLock)
			{
				strcpy (glbSetVars[glbNumSetting].lookupString, ARX_UI_DEBUGACTIVE_LOOPUP);
				strcpy (glbSetVars[glbNumSetting].seqName, ARX_UI_CAPABILITY_SEQNAME);
				glbSetVars[glbNumSetting].varType = VARTYPE_BOOL;
				sprintf(glbSetVars[glbNumSetting].varVal, "%d", VTRUE);
				++glbNumSetting;
				CmtReleaseLock (glbSetVarLock);
			}
			break;
	}
	
	if(menu != MENU_WATCHEXP_PANEL)
	{
		CmtGetLockEx (glbSetVarLock, 1, CMT_WAIT_FOREVER, &hasLock);
		if (hasLock)
		{
			strcpy (glbSetVars[glbNumSetting].lookupString, tmpSeqVar.lookupString);
			strcpy (glbSetVars[glbNumSetting].seqName, tmpSeqVar.seqName);
			glbSetVars[glbNumSetting].varType = tmpSeqVar.varType;
			strcpy (glbSetVars[glbNumSetting].varVal, tmpSeqVar.varVal);
			++glbNumSetting;
			CmtReleaseLock (glbSetVarLock);
		}
	}
#else
	int Ini_Edit (char *IniPath, char errmsg[ERRLEN]);
	
	char errmsg[1024] = {0};
	char configPath[MAX_PATHNAME_LEN] = {0};
	
	switch (menu)
	{
		case MENU_EDIT_RECIPE:
			sprintf (configPath, "%s\\configuration\\Recipe.ini", glbRootPath);
			Ini_Edit (configPath, errmsg);
			ReloadRecipe (configPath, errmsg);
			break;
		case MENU_EDIT_SYSTEM_CONFIG:
			sprintf (configPath, "%s\\configuration\\SystemConfig.ini", glbRootPath);
			Ini_Edit (configPath, errmsg);
			ReloadSystemConfig (configPath, errmsg);
			break;
		case MENU_EDIT_STATION_CONFIG:
			sprintf (configPath, "%s\\configuration\\StationConfig.ini", glbRootPath);
			Ini_Edit (configPath, errmsg);
			ReloadStationConfig (configPath, errmsg);
			break;
		case MENU_EDIT_CVI_TS:
			EditTestSteps (errmsg);
			ReloadTestSteps (errmsg);
			break;
		case MENU_WATCHEXP_PANEL:
			break;
		case MENU_DEBUG_PANEL:
			LaunchDebugPanel ();
			break;
	}
#endif
	
Error:
	return;
}

/***************************************************************************//*!
* \brief Generic callback for project specific menus, actual logic should come from
* 	the ProjectSpecificGUI_LIB.
* 
* For TestStand implementation, this callback should only set flags in TestStand,
* 	which then invoke the logic in the project lib.
* 
* For CVI implementation, call the functions directly from the project lib.
*******************************************************************************/
void CVICALLBACK ArxCB_SendProjectSpecificMenuStatus (int menuBar, int menuItem, void *callbackData, int panel)
{
	//switch case
	
	char *lookupString = (char *) callbackData;
	int hasLock = 0;
	//strtok is not thread safe, current alternative
	char tok[3][40] = {0}; //Format of callbackData: TestStandVariableName;VarType;VarValue
	int delimIndex = 0;
	int tokArrIndex = 0;
	
#ifndef CVIUI_ONLY
	for(int i = 0; i <= strlen(lookupString); ++i)
	{
		if((lookupString[i] == ';'||lookupString[i] == 0 /*NUL*/))
		{
			CopyString(tok[tokArrIndex], 0, lookupString, delimIndex, (i-delimIndex));
			delimIndex = i+1;
			tokArrIndex++;
		}
	}
	
	CmtGetLockEx (glbSetVarLock, 1, CMT_WAIT_FOREVER, &hasLock);
	if (hasLock)
	{
		strcpy(glbSetVars[glbNumSetting].seqName, ARX_UI_CAPABILITY_SEQNAME);
		strcpy(glbSetVars[glbNumSetting].lookupString, tok[0]);
		glbSetVars[glbNumSetting].varType = atoi(tok[1]);
		strcpy(glbSetVars[glbNumSetting].varVal, tok[2]);
		glbNumSetting++;
		CmtReleaseLock (glbSetVarLock);
	}
#else
	
#endif
}

/***************************************************************************//*!
* \brief Callback for updating the idle timer
* 
* The idle timer control is enabled/disabled via TSMsg_UI_SET()
*******************************************************************************/
int CVICALLBACK ArxCB_IdleTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	double eventTime = 0.0, timeSinceLastCallback = 0.0;
	char time[16] = {0};
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetTimerTickData (&eventTime, &timeSinceLastCallback);
			if (glbGetNewEventTime)
			{
				glbNewEventTime = eventTime;
				glbGetNewEventTime = 0;
			}
			sprintf (time, "%0.0f", eventTime-glbNewEventTime);
			SetCtrlVal (panel, EXEC_PANEL_IDLE_TIME, time);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Calling event close from the warn/error popup
*******************************************************************************/
int CVICALLBACK ArxCB_Exit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			MainPanelCallback (panel, EVENT_CLOSE, 0, 0, 0);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Dismisses the popup window.  Different behavior depending on sequencer used.
* 
* For TestStand, TSMsg_DISP has optional call to TS_EngineBreakAll to break on the current
* 	sequence step. This callback resumes the sequences.  The dismiss button is
* 	only visible if the break is called.
* 
* For CVI, it simply sets the panel to be invisible.  Project specific functionality
* 	may also need to be implemented depending on the error.
*******************************************************************************/
int CVICALLBACK ArxCB_Dismiss (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
#ifndef CVIUI_ONLY
			TSUI_ExecutionsResumeAll (gMainWindow.applicationMgr, &errorInfo);
#endif
			SetCtrlAttribute (gMainWindow.arxPopupPanel, POPUP_MSG_DISMISS, ATTR_VISIBLE, 0);
			break;
	}
	return 0;
}
//! \cond
/// REGION END

/// REGION START Template Callbacks
//! \endcond
/***************************************************************************//*!
* \brief Main panel callback (X button for close)
*******************************************************************************/
int CVICALLBACK MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:		// EVENT_CLOSE indicates the user clicked on window close box
#ifndef CVIUI_ONLY
			ArxCB_ModeSwitchCB (0, glbEngModeMenu, 0, 0);	// Set to eng mode to enable tracing/breakpoints
			TS_EngineTerminateAll(gMainWindow.engine, &errorInfo);
			static CAObjHandle execution = 0;
			if (!execution) TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &execution);
			enum TSEnum_ExecutionRunStates runState = TS_ExecRunState_Running;
			enum TSEnum_ExecutionTerminationStates termState;
			while (runState!=TS_ExecRunState_Stopped)
			{
				TS_ExecutionGetStates(execution, &errorInfo, &runState, &termState);
				DelayWithEventProcessing(0.5);
				TS_EngineTerminateAll(gMainWindow.engine, &errorInfo);
			}
			DelayWithEventProcessing(4.0);
			CA_DiscardObjHandle(execution);
#endif
			ExitApplication(); 	// this function displays errors, if any occur
			break;
	}

	return 0;
}

/***************************************************************************//*!
* \brief The callback for the main panel
*******************************************************************************/
int CVICALLBACK MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2)
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
#ifndef CVIUI_ONLY
			if (gMainWindow.guiThreadId == CmtGetCurrentThreadID())	// these events might also be generate by CVI panels displayed in execution threads. In that case we want to ignore the events to avoid unnecessary multithreading issues in the application UI code.
			{
				if (gMainWindow.engine != 0) // these events can also occur while shutting down and should be ignored then as well
				{
					errChk( RebuildEditMenu());
				}
			}
#else
			{}	//20241125Biye: not sure why this is needed but build fails otherwise, possibly because the previous if statement didn't have brackets?
			// Previously user management code went here for background color changing
#endif
			break;
#ifdef CVIUI_ONLY
		case EVENT_PANEL_SIZE:
			// This is the only good place to put this for CVI, TestStand calls this function in various ActiveX callbacks
			ArrangeControls (FALSE);
			break;
#endif
	}
	
Error:
	DisplayError(error);
	return 0;
}

/***************************************************************************//*!
* \brief User clicked on menubar, make sure all menus have correct items with the correct enabled states
*******************************************************************************/
void CVICALLBACK MenuDimmerCallback(int menuBarHandle, int panelHandle)
{
	int	error = 0;
	
	errChk( RebuildMenuBar(menuBarHandle));
	
Error:
	DisplayError(error);
}

//! \cond TESTSTAND
/***************************************************************************//*!
* \brief Used for when tabbed nests are enabled
* 
* eventData2 is the panel handle of the newly actived tab
*******************************************************************************/
int CVICALLBACK TabControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int			error = 0;
	CAObjHandle	execution = 0;
	int			pageIndex =0;

	switch (event)
	{
		case EVENT_ACTIVE_TAB_CHANGE:
#ifndef CVIUI_ONLY
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
#endif
		break;
	}
	
Error:
#ifndef CVIUI_ONLY
	CA_DiscardObjHandle(execution);
	DisplayError(error);
#endif
	return 0;
}

/***************************************************************************//*!
* \brief Displays the about box when the user selects Help>>About...
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
		errChk( gMainWindow.aboutBox = LoadPanelEx(0, "MessagePopup.uir", ABOUTBOX, __CVIUserHInst));
	
#ifndef CVIUI_ONLY
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
#endif

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
* \brief Closes about panel
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

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Release any TestStand objects and save any settings here
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnQueryShutdown(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_QueryShutdownOptions *opt)
{
	// remember window and control positions for next time
	TS_LayoutPersister_SaveSizes(gMainWindow.applicationMgr,  &errorInfo, NULL, 0, 10,
									gMainWindow.tspanel, DEBUGPANEL_LISTBAR,
									gMainWindow.tspanel, DEBUGPANEL_TAB,
									gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, 
									gMainWindow.fileTab, TABPANEL_SEQUENCES, 
									gMainWindow.fileTab, TABPANEL_VARIABLES, 
									gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, 
									gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, 
									gMainWindow.executionTab, TABPANEL_2_CALLSTACK, 
									gMainWindow.executionTab, TABPANEL_2_THREADS, 
									gMainWindow.executionTab, TABPANEL_2_VARIABLES);
	TS_LayoutPersister_SaveBounds(gMainWindow.applicationMgr, &errorInfo, NULL, 0, 3, 
								  gMainWindow.panel, 0,
								  gMainWindow.tspanel, 0,
								  gMainWindow.execpanel, 0);


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
	
	if (gMainWindow.modelSequence)
	{
		TS_EngineReleaseSeqFileEx(gMainWindow.engine, &errorInfo, gMainWindow.modelSequence, TS_ReleaseSeqFile_UnloadFile, NULL);
		CA_DiscardObjHandle(gMainWindow.modelSequence);
		gMainWindow.modelSequence = 0;
	}
	if (gMainWindow.projectSequencer)
	{
		TS_EngineReleaseSeqFileEx(gMainWindow.engine, &errorInfo, gMainWindow.projectSequencer, TS_ReleaseSeqFile_UnloadFile, NULL);
		CA_DiscardObjHandle(gMainWindow.projectSequencer);
		gMainWindow.projectSequencer = 0;
	}
	if (gMainWindow.arxtronSequencer)
	{
		TS_EngineReleaseSeqFileEx(gMainWindow.engine, &errorInfo, gMainWindow.arxtronSequencer, TS_ReleaseSeqFile_UnloadFile, NULL);
		CA_DiscardObjHandle(gMainWindow.arxtronSequencer);
		gMainWindow.arxtronSequencer = 0;
	}
	if (gMainWindow.userAdmin)
	{
		CA_DiscardObjHandle(gMainWindow.userAdmin);
		gMainWindow.userAdmin = 0;
	}
	if (gMainWindow.userOper)
	{
		CA_DiscardObjHandle(gMainWindow.userOper);
		gMainWindow.userOper = 0;
	}

	CA_DiscardObjHandle(gMainWindow.engine);
	gMainWindow.engine = 0;

	ExitApplication();  // since we have received the ExitApplication event, this call is guaranteed to cause an immediate exit
	return S_OK;
}

/***************************************************************************//*!
* \brief The ApplicationMgr sends this event when it's busy doing something so we know to display a hourglass cursor or equivalent
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnWait(CAObjHandle caServerObjHandle, void *caCallbackData, VBOOL  bShowWait)
{	
	SetMouseCursor(bShowWait ? VAL_HOUR_GLASS_CURSOR : VAL_DEFAULT_CURSOR);
	return S_OK;
}

/***************************************************************************//*!
* \brief the ApplicationMgr sends this event to request that the UI display the report for a particular execution
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
* \brief the ApplicationMgr sends this event to request that the UI display a particular execution
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnDisplayExecution(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Execution execution, enum TSUIEnum_ExecutionDisplayReasons reason)
{
	int	error = 0;

	// bring application to front if we hit a breakpoint
	if (reason == TSUIConst_ExecutionDisplayReason_Breakpoint || reason == TSUIConst_ExecutionDisplayReason_BreakOnRunTimeError)
		errChk( SetActivePanel(gMainWindow.tspanel));

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
* \brief The user toggled edit mode. the only way to do that in this application
* 	is to type ctrl-shift-alt-insert, which is the edit mode toggle key this
* 	application specifies in designer for the ApplicationMgr control. 
* 	to prevent edit mode from being toggled with a hotkey, set
* 	ApplicationMgr.EditModeShortcutKey to ShortcutKey_VK_NOT_A_KEY
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
* \brief Callback that handles the Close Completed Executions command from TestStand
* 
* Not sure if there are other "kinds" of commands that this can handle
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnPostCommandExecute(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_Command  command)
{
	int							error = 0;
	enum TSUIEnum_CommandKinds	kind;
	CAObjHandle					executions = 0;
	long 						numberOfExecutions = 0;

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
	CA_DiscardObjHandle(numberOfExecutions);
	DisplayError(error);
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief ApplicationMgr sends this event to request that the UI display a particular sequence file
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
* \brief the ApplicationMgr sends this event to report any error that occurs
* 	when the TestStand UI controls respond to user input (ie, an error 
* 	other than one returned by a method you call directly). For example,
* 	if a TestStand menu command generates an error, this handler displays it
*******************************************************************************/
HRESULT CVICALLBACK ApplicationMgr_OnReportError(CAObjHandle caServerObjHandle, void *caCallbackData, long  errorCode, char *errorMessage)
{
	strcpy(errMsg, errorMessage);	// update global errMsg buffer
	DisplayError(errorCode);
	return S_OK;
}

/***************************************************************************//*!
* \brief the ApplicationMgr sends this event whenever an execution starts
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
* \brief the ExecutionViewMgr sends this event whenever a new execution is selected
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
* \brief the TABPANEL_STEPLISTVIEW control sends this event when the user right clicks on it
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
* \brief the TABPANEL_2_STEPLISTVIEW ctrl sends this event when the user right clicks on it
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
* \brief the DEBUGPANEL_LISTBAR ctrl sends this event when the user right clicks on it
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
* \brief the TABPANEL_SEQUENCES ctrl sends this event when the user right clicks on it
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
* \brief user dragged the vertical bar on the listbar that separates it from the tab control
*******************************************************************************/
HRESULT CVICALLBACK ListBar_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, 
											TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              				TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL  finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.tspanel, DEBUGPANEL_LISTBAR, DEBUGPANEL_TAB, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(TRUE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief user dragged the horizontal bar that separates the step list from the sequence list and file variables
*******************************************************************************/
HRESULT CVICALLBACK SequenceFilesView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_STEPLISTVIEW, TABPANEL_SEQUENCES, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief user dragged vertical bar that separates the file variables from the sequences list
*******************************************************************************/
HRESULT CVICALLBACK FileVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_VARIABLES, TABPANEL_SEQUENCES, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief user dragged the vertical bar that separates the insertion palette from the file step list and the file variables
*******************************************************************************/
HRESULT CVICALLBACK InsertionPalette_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.fileTab, TABPANEL_INSERTIONPALETTE, TABPANEL_STEPLISTVIEW, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief user dragged the horizontal bar that separates the execution step list from the call stack, thread list, and execution variables
*******************************************************************************/
HRESULT CVICALLBACK ExecutionView_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_STEPLISTVIEW, TABPANEL_2_CALLSTACK, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief user dragged the vertical bar that separates the execution variables from the callstack
*******************************************************************************/
HRESULT CVICALLBACK ExecutionVariables_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_VARIABLES, TABPANEL_2_CALLSTACK, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief user dragged the vertical bar that separates the callstack from the threads
*******************************************************************************/
HRESULT CVICALLBACK CallStack_OnBorderDragged(CAObjHandle caServerObjHandle, void *caCallbackData, long  bordersChanged, TSUIType_OLE_XPOS_PIXELS  newX, TSUIType_OLE_YPOS_PIXELS  newY,
                              TSUIType_OLE_XPOS_PIXELS newWidth, TSUIType_OLE_YPOS_PIXELS newHeight, VBOOL finalResize)
{
	int	error = 0;
	
	errChk( TS_Splitters_DragSplitter(gMainWindow.executionTab, TABPANEL_2_CALLSTACK, TABPANEL_2_THREADS, &errorInfo, newX, newY, newWidth, newHeight, bordersChanged));
	errChk( ArrangeControls(FALSE));  
	
Error:
	return error < 0 ? E_FAIL : S_OK;
}

/***************************************************************************//*!
* \brief one of the hidden labels connected to display the current file and current execution has changed
*******************************************************************************/
HRESULT CVICALLBACK Label_OnConnectionActivity(CAObjHandle caServerObjHandle, void *caCallbackData, enum TSUIEnum_ConnectionActivityTypes  activity)
{
	int	error = 0;
	
	errChk( UpdateWindowTitle());
	
Error:
	return error < 0 ? E_FAIL : S_OK;	
}
#endif

/***************************************************************************//*!
* \brief Callback for Updating Watch Expression Table in WatchExpressionsWindow.uir
*******************************************************************************/
int CVICALLBACK StartWatchExpressionsCheckBox (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			int getVarFlag = 0;
			char tmp[50] = {0};
			GetCtrlVal(panel, WATCHPANEL_ENABLEGETVAR, &getVarFlag);
			
			switch (getVarFlag)
			{
				case 0:
					glbWatchExpThreadEn = 0;
					while(CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, glbWatchExpThreadID, 0));
					CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, glbWatchExpThreadID);
					memset(&glbWatchVars, 0, sizeof(glbWatchVars));	
					glbNumWatching = 0;
					for(int i = 0; i<100; ++i)
					{					
						for(int j=1; j<=2; ++j)
						{
							SetTableCellAttribute(panel, WATCHPANEL_WATCHTABLE, MakePoint(j,(i+1)), ATTR_NO_EDIT_TEXT, 0); //make table cells editable
						}
						SetTableCellAttribute(panel, WATCHPANEL_WATCHTABLE, MakePoint(3,(i+1)), ATTR_CELL_DIMMED, 0);
						SetTableCellVal(panel, WATCHPANEL_WATCHTABLE, MakePoint(4,(i+1)), ""); //clear watch expression table
					}
					break;
				case 1:
					glbWatchExpThreadEn = 1;
					for(i = 0; i<100; ++i)
					{
						GetTableCellVal(panel, WATCHPANEL_WATCHTABLE, MakePoint(1,(i+1)), glbWatchVars[glbNumWatching].seqName);
						if(strlen(glbWatchVars[glbNumWatching].seqName))
						{
							GetTableCellVal(panel, WATCHPANEL_WATCHTABLE, MakePoint(2,(i+1)), glbWatchVars[glbNumWatching].lookupString);
							GetTableCellVal(panel, WATCHPANEL_WATCHTABLE, MakePoint(3,(i+1)), tmp);
							GetTableCellRingIndexFromValue(panel, WATCHPANEL_WATCHTABLE, 0, MakePoint(3,(i+1)), &glbWatchVars[glbNumWatching].varType, tmp);
							++glbNumWatching;
						}
						
						for(int j=1; j<=2; ++j)
						{
							SetTableCellAttribute(panel, WATCHPANEL_WATCHTABLE, MakePoint(j,(i+1)), ATTR_NO_EDIT_TEXT, 1); //make table cells indicators
						}
						SetTableCellAttribute(panel, WATCHPANEL_WATCHTABLE, MakePoint(3,(i+1)), ATTR_CELL_DIMMED, 1);
					}
					
					CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, &UpdateWatchVarStruct, 0, &glbWatchExpThreadID);
					break;
			}
		}
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Callback for Updating Watch Expression Table in WatchExpressionsWindow.uir
*******************************************************************************/
int CVICALLBACK SetTestStandVariableVal (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			char tmp[50] = {0};
			GetTableCellVal(panel, WATCHPANEL_WRITETABLE, MakePoint(1,1), glbSetVars[glbNumSetting].seqName);
			GetTableCellVal(panel, WATCHPANEL_WRITETABLE, MakePoint(2,1), glbSetVars[glbNumSetting].lookupString);
			GetTableCellVal(panel, WATCHPANEL_WRITETABLE, MakePoint(3,1), tmp);
			GetTableCellRingIndexFromValue(panel, WATCHPANEL_WRITETABLE, 0, MakePoint(3,1), &glbSetVars[glbNumWatching].varType, tmp);	
			GetTableCellVal(panel, WATCHPANEL_WRITETABLE, MakePoint(4,1), glbSetVars[glbNumSetting].varVal);
			if(glbSetVars[glbNumWatching].varType == VARTYPE_BOOL)
			{
				if(!stricmp(glbSetVars[glbNumWatching].varVal, "True"))
					strcpy(glbSetVars[glbNumWatching].varVal, "-1");
				else if(!stricmp(glbSetVars[glbNumWatching].varVal, "False"))
					strcpy(glbSetVars[glbNumWatching].varVal, "0");
			}
			glbNumSetting++;
		}
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Main Panel Callback for WatchExpressionsWindow.uir
*******************************************************************************/
int  CVICALLBACK MainWatchExpressionCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panel);
			DiscardPanel(panel);
			glbWatchExpPanelHandle = 0;
			if(glbWatchExpThreadEn)
			{
				glbWatchExpThreadEn = 0;
				while(CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, glbWatchExpThreadID, 0));
				CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, glbWatchExpThreadID);
				memset(&glbWatchVars, 0, sizeof(glbWatchVars));	
				glbNumWatching = 0;				
			}
			break;
	}
	
	return 0;
}
//! \endcond TESTSTAND
//! \cond
/// REGION END

/// REGION START UserManagement Callbacks
//! \endcond
//! \cond CVIONLY
#ifdef CVIUI_ONLY
/***************************************************************************//*!
* \brief User login callback for CVI sequencer
*******************************************************************************/
void CVICALLBACK ArxCVICB_DisplayLoginPrompt (int menuBar, int menuItem, void *callbackData, int panel)
{
	char errmsg[1024] = {0};
	UserManagement_DisplayPrompt (0, 1, 0, 0, errmsg);
}

/***************************************************************************//*!
* \brief User logout callback for CVI sequencer
*******************************************************************************/
void CVICALLBACK ArxCVICB_Logout (int menuBar, int menuItem, void *callbackData, int panel)
{
	char errmsg[1024] = {0};
	UserManagement_Logout (errmsg);
}
#endif
//! \endcond CVIONLY
//! \cond
/// REGION END
//! \endcond