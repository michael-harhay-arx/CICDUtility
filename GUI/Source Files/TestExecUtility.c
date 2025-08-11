/***************************************************************************//*!
* \file TestExecUtility.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/1/2021 3:02:03 PM
* \brief Functions in this file serve to accomplish some task that's not a
* 	callback or related to messaging.
* 
* This contains both the default functions from the UI example and custom functions.
* 	Custom functions are prefixed with ArxUtil_
* 
* Some of the default functions are modified as well to have additional functionalty
* 	with custom UI elements.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include <userint.h>
#include "CommonTSIncludes.h"
#include "ActiveXControls.h"
#include "UIUtility.h"
#include "TSTypeInsertMacros.h"
#include "inifile.h"

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
* \brief Watch Expression Thread Flag
*******************************************************************************/
int glbWatchExpThreadEn = 0;

//!@{
/***************************************************************************//*!
* \brief UI size variables
*******************************************************************************/
int glbWindowHeight = 0;
int glbWindowWidth = 0;
//!@}
//!@{
/***************************************************************************//*!
* \brief Used for expanding the test detail windows
*******************************************************************************/
int glbNestOrigWidth = 0;
int glbNestOrigLeft[CVIUI_TOTAL_POSSIBLE_NESTS] = {0};
//!@}
/***************************************************************************//*!
* \brief Keeps track of which panel is on top (TS vs CVI)
*******************************************************************************/
int glbActivePanel = 0;

//!@{
/***************************************************************************//*!
* \brief For some reason, the menu handles are not part of gMainWindow in the
* 	original example, so its just kept this way with additional menus. There's
* 	no particular reason why these variables are not part of a struct
*******************************************************************************/
static int glbMenuEdit = 0;
int glbMenuExecute = 0;
static int glbMenuDebug = 0;
static int glbMenuConfigure = 0;
static int glbMenuTools = 0;
int glbOpModeMenu = 0;
int glbEngModeMenu = 0;
int glbOpViewMenu = 0;
int glbEngViewMenu = 0;
//!@}

extern int glbInterfaceStarted;

extern TSSeqVar glbWatchVars[100];
extern int glbNumWatching;

#ifdef CVIUI_ONLY
/***************************************************************************//*!
* \brief Root path of the project, expected to contain the standard sequencer
* 	files and folders	DOCTODO reference folder structure
*******************************************************************************/
char glbRootPath[MAX_PATHNAME_LEN] = {0};
//!@{
/***************************************************************************//*!
* \brief Used to initialize the GUI window	DOCTODO config reference
*******************************************************************************/
IniText glbSequencerConfigHandle = 0;
int glbPanelHeight = 0;
int glbPanelWidth = 0;
int glbPanelTop = 0;
int glbPanelLeft = 0;
//!@}
/***************************************************************************//*!
* \brief Used to determine what to print on GUI test results	DOCTODO config reference
*******************************************************************************/
int glbUserAccessLevel = 0;
//!@{
/***************************************************************************//*!
* \brief Operation mode flags
*******************************************************************************/
int glbReinjectModeActive = 0;
int glbCapabilityModeActive = 0;
//!@}
extern CVIMessageManager glbCVIMsgManager[MAX_NESTS+1];
extern CmtThreadLockHandle glbMsgLock[MAX_NESTS+1];
#endif

extern int glbWatchExpPanelHandle;
//==============================================================================
// Global functions

int CVICALLBACK MainCallback(int panelOrMenuBarHandle, int controlOrMenuItemID, int event, void *callbackData, int eventData1, int eventData2);
int ArxUtil_GetSequencer (void);
int ArxUtil_CheckMinSize (int *panelHeight, int *panelWidth);
int ArxUtil_GetExecPanelHandles (void);
int ArxUtil_MoveCtrlArray (const int PanelHandle, const int CtrlArrayHandle, const int NewTop, const int NewLeft);
int ArxUtil_CloneNests (void);
int ArxUtil_ResizeTSPanel (int MenuHeight, int StatusBarHeight);
int ArxUtil_ResizeExecPanel (int MenuHeight, int StatusBarHeight);
int ArxUtil_RebuildTSMenu (int MenuBar);
int ArxUtil_RebuildExecMenu (int MenuBar, int IsAdmin);

void CVICALLBACK ArxCB_MenuCallback (int menuBar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ArxCB_OpenPanel (int menuBar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ArxCB_SendProjectSpecificMenuStatus (int menuBar, int menuItem, void *callbackData, int panel);

#ifdef CVIUI_ONLY
/* 20241107Tim: Menu Callbacks */
void CVICALLBACK ArxCVICB_DisplayLoginPrompt (int menuBar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ArxCVICB_Logout (int menuBar, int menuItem, void *callbackData, int panel);
#endif

// From TSUtils_LIB
int TSUtils_CreateCustomType (const CAObjHandle Engine, const short VarType, const CAObjHandle SeqRef, const char *SeqPath, const char *VarName,  const int NumSubVars,  const int SubVarSize,  const char **SubVarNames,  const int *SubEnumVals, char errmsg[1024]);

//! \cond
/// REGION END

/// REGION START Arxtron Functions
//! \endcond
#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Attempts to find Arxtron Sequencer.seq and ProjectSpecificSequencer.seq
* 	in list of search paths defined by TestStand
*******************************************************************************/
int ArxUtil_GetSequencer (void)
{
	int error = 0;
	
	char *absPath = NULL;
	
	tsErrChk( TS_EngineFindFile(gMainWindow.engine, &errorInfo, "Arxtron Sequencer.seq", &absPath, NULL, TS_FindFile_PromptDisable, TS_FindFile_AddDirToSrchList_No, VFALSE, CA_DEFAULT_VAL, NULL));
	errChk( strlen(absPath)==0?ARXERR_SEQUENCER_NOT_FOUND:0);
	strcpy (glbSequencerPath,absPath);
	CA_FreeMemory(absPath);
	tsErrChk( TS_EngineFindFile(gMainWindow.engine, &errorInfo, "ProjectSpecificSequencer.seq", &absPath, NULL, TS_FindFile_PromptDisable, TS_FindFile_AddDirToSrchList_No, VFALSE, CA_DEFAULT_VAL, NULL));
	errChk( strlen(absPath)==0?ARXERR_SEQUENCER_NOT_FOUND:0);
	strcpy (glbPrjSequencerPath,absPath);
	
Error:
	CA_FreeMemory(absPath);
	return error;
}
#endif

/***************************************************************************//*!
* \brief Loads uir panels into memory
*******************************************************************************/
int ArxUtil_LoadPanelsInMemory (void)
{
	int error = 0;
	
#ifndef CVIUI_ONLY
	// load the panels for the main window from the .UIR file
	errChk( gMainWindow.panel = 		LoadPanelEx(0, "TestExecMainPanel.uir", MAINPANEL, __CVIUserHInst));
	errChk( gMainWindow.tspanel = 		LoadPanelEx(gMainWindow.panel, "TestExecTS.uir", DEBUGPANEL, __CVIUserHInst));
	errChk( GetPanelHandleFromTabPage (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_FILE,      &gMainWindow.fileTab));
	errChk( GetPanelHandleFromTabPage (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_EXECUTION, &gMainWindow.executionTab));
	errChk( GetPanelHandleFromTabPage (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_REPORT,    &gMainWindow.reportTab));
#else
	// Removed ActiveX components (status bar)
	errChk( gMainWindow.panel = 		LoadPanelEx(0, "TestExecMainPanelCVIOnly.uir", MAINPANEL, __CVIUserHInst));
#endif
	
	errChk( gMainWindow.execpanel = 	LoadPanelEx(gMainWindow.panel, "TestExecExecute.uir", EXEC_PANEL, __CVIUserHInst));
#ifdef USE_TABS
	for (int i=0; i<CVIUI_NUM_NEST_TABS; ++i)
		errChk ( GetPanelHandleFromTabPage (gMainWindow.execpanel, EXEC_PANEL_TAB_NESTS, i, &(gMainWindow.tabpanels[i])));
	errChk( gMainWindow.nests[0] = 		LoadPanelEx(gMainWindow.tabpanels[0], "TestExecExecute.uir", NEST_PANEL, __CVIUserHInst));
#else
	errChk( gMainWindow.nests[0] = 		LoadPanelEx(gMainWindow.execpanel, "TestExecExecute.uir", NEST_PANEL, __CVIUserHInst));
#endif

	errChk( gMainWindow.arxPopupPanel	= LoadPanelEx(0, "MessagePopup.uir", POPUP_MSG, __CVIUserHInst));
	
Error:
	return error;
}

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Groups all the initialization together for TS panel
*******************************************************************************/
int ArxUtil_InitTSPanel (void)
{
	int error = 0;
	CAObjHandle seqTestUUTs = 0;
	CAObjHandle modelOptions = 0;
	HINSTANCE modelSupHandle = 0;
	
	// prepare to use the TestStand ActiveX controls
	errChk( GetActiveXControlHandles());
	
	// setup event callbacks for the TestStand User Interface Components
	errChk( RegisterActiveXEventCallbacks());
	
	// make engine conveniently accessible
	tsErrChk( TSUI_ApplicationMgrGetEngine(gMainWindow.applicationMgr, &errorInfo, &gMainWindow.engine));
	
#ifdef UI_DEBUG
	if (!UI_DEBUG)
#endif
	{
		// setup connections for the TestStand User Interface Components
		errChk( ConnectTestStandControls());
		
		// this application allows setting of breakpoints on sequences files, so let them persist
		tsErrChk( TS_EngineSetPersistBreakpoints(gMainWindow.engine, &errorInfo, VARIANT_TRUE));
		
		// Arxtron custom addition: Get model sequence handle and number of test sockets
		/* 20210407Biye: OK so this was really complicated to get it to work 
			ReadModelOptionDefaults is a function used in the 
			ParallelModel.seq(Initialize Entry Point)->ModelSupport.seq(Read Model Option Defaults)
			which references the DLL modelsupport2.dll
			The function itself is found in ModelOptions.c thats found in 
			%TESTSTAND%\Components\Models\TestStandModels\modelsupport2\modelsupport2.dll (ModelOptions.c)
			This folder will need to be added to the system paths in order to reference the dll
			Basically, the Locals.ModelData.ModelOptions variable in ParallelModel's Test UUTs sequence
			contains default values, and calling ReadModelOptionDefaults function is what updates this variable
			with the actual values set on TestStand.
			The function actually takes the handle of the ModelOptions variable loaded in memory and updates its
			component values.
		*/
		tsErrChk( TS_EngineGetStationModelSeqFile(gMainWindow.engine, &errorInfo, NULL, &gMainWindow.modelSequence));
		tsErrChk( TS_SeqFileGetSequenceByName(gMainWindow.modelSequence, &errorInfo, "Test UUTs", &seqTestUUTs));
		TS_PropertyGetPropertyObject(seqTestUUTs, &errorInfo, "Locals.ModelData.ModelOptions", 0, &modelOptions);	// Get the handle for the container
		
		// Load ReadModelOptionDefaults from dll
		modelSupHandle = LoadLibrary("modelsupport2.dll");
		errChk( (!modelSupHandle?ARXERR_MODEL_SUP_LIB_NOT_FOUND:0));
		typedef int _stdcall (*NISuppFn)(CAObjHandle,CAObjHandle);
		NISuppFn ReadModelOptionDefaults = (NISuppFn) GetProcAddress(modelSupHandle, "ReadModelOptionDefaults");
		errChk( (!ReadModelOptionDefaults?ARXERR_MODEL_SUP_LIB_NOT_FOUND:0));
		
		ReadModelOptionDefaults(gMainWindow.engine,modelOptions);
		tsErrChk( TS_PropertyGetValNumber(seqTestUUTs, &errorInfo, "Locals.ModelData.ModelOptions.NumTestSockets", 0, &glbNumSockets));
		errChk( (glbNumSockets>(CVIUI_TOTAL_POSSIBLE_NESTS)?ARXERR_GRID_CONFIG_WRONG:0));
		
		errChk( ArxUtil_GetSequencer());
		
		// Get handle for sequencer
		tsErrChk( TS_EngineGetSeqFileEx(gMainWindow.engine, &errorInfo, glbSequencerPath, TS_GetSeqFile_FindFile, TS_ConflictHandler_Error, &gMainWindow.arxtronSequencer));
		tsErrChk( TS_EngineGetSeqFileEx(gMainWindow.engine, &errorInfo, glbPrjSequencerPath, TS_GetSeqFile_FindFile, TS_ConflictHandler_Error, &gMainWindow.projectSequencer));
		
		// Get user objects, users must already be created!!
		tsErrChk( TS_EngineGetUser(gMainWindow.engine, &errorInfo, "administrator", &gMainWindow.userAdmin));
		tsErrChk( TS_EngineGetUser(gMainWindow.engine, &errorInfo, "operator", &gMainWindow.userOper));

		errChk( InstallMainCallback(MainCallback, 0, 0));	// handle the EVENT_END_TASK event, etc.
		gMainWindow.guiThreadId = CmtGetCurrentThreadID();  // for filtering out CVI events from other threads in the MainCallback
	}
	
Error:
	if (modelSupHandle)
		FreeLibrary (modelSupHandle);
	CA_DiscardObjHandle(seqTestUUTs);
	CA_DiscardObjHandle(modelOptions);
	return error;
}
#endif

/***************************************************************************//*!
* \brief Groups all the initialization together for exec panel
*******************************************************************************/
int ArxUtil_InitExecPanel (void)
{
	int error = 0;
	
#ifdef CVIUI_ONLY
	IniText stationConfigHandle = NULL;
	// Implement certain features that's normally initialized in ArxUtil_InitTSPanel()
	
	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_TITLE, "Arxtron CVI Sequencer"));
	
	errChk( GetDir(glbRootPath));
	strcat (glbRootPath, "\\..\\..");	// the exec is expected to be stored in [Root]\GUI\win32
	
	// Get NumNests
	char configPath[MAX_PATHNAME_LEN] = {0};
	sprintf (configPath, "%s\\configuration\\Sequencer.ini", glbRootPath);
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
	sprintf (configPath, "%s\\configuration\\StationConfig.ini", glbRootPath);
	stationConfigHandle = Ini_New (0);
	errChk (Ini_ReadFromFile (stationConfigHandle, configPath));
	char *prjNumberStr = 0;
	errChk (0>Ini_GetPointerToRawString (stationConfigHandle, "Station Config", "PrjNumber", &prjNumberStr));
	int prjNumber = atoi (prjNumberStr);
	
	char errmsg[1024] = {0};
	sprintf (configPath, "%s\\configuration\\UserFile.txt", glbRootPath);
	errChk (Initialize_UserManagement_LIB (prjNumber, configPath, errmsg))
	
	errChk( (glbNumSockets>(CVIUI_TOTAL_POSSIBLE_NESTS)?ARXERR_GRID_CONFIG_WRONG:0));
#endif
	errChk( ArxUtil_GetExecPanelHandles());
	errChk( ArxUtil_CloneNests());
	
Error:
#ifdef CVIUI_ONLY
	if (stationConfigHandle) Ini_Dispose (stationConfigHandle);
#endif
	return error;
}

/***************************************************************************//*!
* \brief Localizes UI controls/menus
* 
* Requires adding new language files that define 1:1 translation of all UI components
* CVI only (not coming from TestStand) specific components still need translations added
* CVI sequencer translations not implemented
*******************************************************************************/
int ArxUtil_LocalizePanels (void)
{
	int error = 0;
	
#ifndef CVIUI_ONLY
	// localize strings in CVI controls and menus
	errChk( TS_LoadPanelResourceStrings(0, gMainWindow.engine, gMainWindow.tspanel, "TSUI_OI_MAIN_PANEL"));
	errChk( TS_LoadMenuBarResourceStrings(gMainWindow.engine, GetPanelMenuBar(gMainWindow.panel), 0, "TSUI_OI_MAIN_PANEL", errMsg));
	// localize strings in the TestStand ActiveX controls
	tsErrChk( TSUI_ApplicationMgrLocalizeAllControls(gMainWindow.applicationMgr, &errorInfo, "TSUI_OI_MAIN_PANEL"));
#endif

Error:
	return error;
}

/***************************************************************************//*!
* \brief Checks current window size against minimum size and resizes window to
* 	min size if current window is smaller. This minimum size is based on the
* 	existing UI elements such that they would start to overlap if the window
* 	was any smaller.
*******************************************************************************/
int ArxUtil_CheckMinSize (int *PanelHeight, int *PanelWidth)
{
	int error = 0;
	
	int minWinWidth = (CVIUI_NUM_NEST_COLS>1?CVIUI_MIN_WINDOW_WIDTH2:CVIUI_MIN_WINDOW_WIDTH);
	
	if (*PanelWidth<minWinWidth)
	{
		*PanelWidth = minWinWidth;
		errChk( SetPanelAttribute(gMainWindow.panel, ATTR_WIDTH, minWinWidth));
	}
	if (*PanelHeight<CVIUI_MIN_WINDOW_HEIGHT)
	{
		*PanelHeight = CVIUI_MIN_WINDOW_HEIGHT;
		errChk( SetPanelAttribute(gMainWindow.panel, ATTR_HEIGHT, CVIUI_MIN_WINDOW_HEIGHT));
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Gets all handles from the Execute panel
* 
* The handles for nest 1 are initialized here because this nest is the basis for
* 	cloning the other nests and is therefore not initialized in ArxUtil_CloneNests
*******************************************************************************/
int ArxUtil_GetExecPanelHandles (void)
{
	int error = 0;
	
	errChk( gMainWindow.grpCounters			= GetCtrlArrayFromResourceID(gMainWindow.execpanel, GrpCtrlCounters));
	errChk( gMainWindow.grpCountersST		= GetCtrlArrayFromResourceID(gMainWindow.execpanel, GrpCtrlCountersST));
	errChk( gMainWindow.grpCountersLT		= GetCtrlArrayFromResourceID(gMainWindow.execpanel, GrpCtrlCountersLT));
	errChk( gMainWindow.grpLogoLEDs			= GetCtrlArrayFromResourceID(gMainWindow.execpanel, GrpCtrlLogoLEDs));
	errChk( gMainWindow.grpLEDs				= GetCtrlArrayFromResourceID(gMainWindow.execpanel, GrpCtrlLEDs));
	errChk( gMainWindow.grpLogos			= GetCtrlArrayFromResourceID(gMainWindow.execpanel, GrpCtrlLogos));
	        
	errChk( gMainWindow.nestGrpCtrls[0][0]	= GetCtrlArrayFromResourceID(gMainWindow.nests[0], GrpCtrlNestLEDs));
	errChk( gMainWindow.nestGrpCtrls[0][1]	= GetCtrlArrayFromResourceID(gMainWindow.nests[0], GrpCtrlNestCounters));
	errChk( gMainWindow.nestGrpCtrls[0][2]	= GetCtrlArrayFromResourceID(gMainWindow.nests[0], GrpCtrlNestTestStatus));
	errChk( gMainWindow.nestGrpCtrls[0][3]	= GetCtrlArrayFromResourceID(gMainWindow.nests[0], GrpCtrlNestStatus));
	errChk( gMainWindow.nestGrpCtrls[0][4]	= GetCtrlArrayFromResourceID(gMainWindow.nests[0], GrpCtrlNestRotStatus));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function to move CtrlArray elements individually since SetCtrlArrayAttribute
* 	sets all the elements to the same value instead of moving them as a group
* 
* Considering making this a generic UI function.  Maybe macro in GUIToolsLib.h??
*******************************************************************************/
int ArxUtil_MoveCtrlArray (const int PanelHandle, const int CtrlArrayHandle, const int NewTop, const int NewLeft)
{
	int error = 0;
	int top = 0, left = 0, height = 0, width = 0;
	int moveTop = 0, moveLeft = 0;
	int numCtrls = 0;
	
	errChk( GetCtrlArrayBoundingRect(CtrlArrayHandle,&top,&left,&height,&width));
	errChk( GetNumCtrlArrayItems(CtrlArrayHandle,&numCtrls));
	
	/* 20210405Biye: This function doesn't consider resizing ctrls yet, so no use for height/width for now */
	moveTop = NewTop - top;
	moveLeft = NewLeft - left;
	
	for (int i=0;i<numCtrls;++i)
	{
		int val = 0;
		int handle = GetCtrlArrayItem(CtrlArrayHandle,i);
		errChk( GetCtrlAttribute(PanelHandle,handle,ATTR_TOP,&val));
		errChk( SetCtrlAttribute(PanelHandle,handle,ATTR_TOP,val+moveTop));
		        
		errChk( GetCtrlAttribute(PanelHandle,handle,ATTR_LEFT,&val));
		errChk( SetCtrlAttribute(PanelHandle,handle,ATTR_LEFT,val+moveLeft));
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Clone nest panel based on number of number of sockets
* 
* \sa USE_TABS, CVIUI_NUM_NEST_TABS, CVIUI_NUM_NEST_PER_TAB, CVIUI_NUM_NEST_ROWS, CVIUI_NUM_NEST_COLS
*******************************************************************************/
int ArxUtil_CloneNests (void)
{
	int error = 0;
	int panelHandle = 0;
	int i=1;	// Skip first nest since we are cloning it
	
#ifdef USE_TABS
	for (int j=0; j<CVIUI_NUM_NEST_TABS && i<glbNumSockets; ++j)
	{
		panelHandle = gMainWindow.tabpanels[j];
		for (i;i<(j+1)*CVIUI_NUM_NEST_PER_TAB;++i)
#else
		panelHandle = gMainWindow.execpanel;
		for (i;i<glbNumSockets;++i)
#endif
		{
			char panelName[16] = {0};
			sprintf (panelName, "Nest #%i", i%CVIUI_NUM_NEST_PER_TAB+1);
			errChk( gMainWindow.nests[i] = DuplicatePanel(panelHandle,gMainWindow.nests[0],panelName,
														  CVIUI_MIN_HEADER_HEIGHT+CVIUI_MIN_NEST_HEIGHT*((i%CVIUI_NUM_NEST_PER_TAB)/CVIUI_NUM_NEST_COLS),
														  CVIUI_SPACER+CVIUI_MIN_NEST_WIDTH*(i%CVIUI_NUM_NEST_COLS)));
		
			errChk( gMainWindow.nestGrpCtrls[i][0]	= GetCtrlArrayFromResourceID(gMainWindow.nests[i], GrpCtrlNestLEDs));
			errChk( gMainWindow.nestGrpCtrls[i][1]	= GetCtrlArrayFromResourceID(gMainWindow.nests[i], GrpCtrlNestCounters));
			errChk( gMainWindow.nestGrpCtrls[i][2]	= GetCtrlArrayFromResourceID(gMainWindow.nests[i], GrpCtrlNestTestStatus));
			errChk( gMainWindow.nestGrpCtrls[i][3]	= GetCtrlArrayFromResourceID(gMainWindow.nests[i], GrpCtrlNestStatus));
			errChk( gMainWindow.nestGrpCtrls[i][4]	= GetCtrlArrayFromResourceID(gMainWindow.nests[i], GrpCtrlNestRotStatus));
		}
#ifdef USE_TABS
	}
#endif
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Starts the sequencer
*******************************************************************************/
int ArxUtil_StartSequencer (void)
{
	int error = 0;
	CAObjHandle execution = 0;
	
#ifndef CVIUI_ONLY
	tsErrChk( TS_EngineNewExecution(gMainWindow.engine, &errorInfo, gMainWindow.arxtronSequencer, "Test UUTs", gMainWindow.modelSequence, VFALSE, TS_ExecTypeMask_Normal,
									CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL, &execution));
#else
	char errmsg[1024] = {0};
	memset (glbCVIMsgManager, 0, sizeof(CVIMessageManager)*(MAX_NESTS+1));
	error = Initialize_CVISequencer(glbNumSockets, &glbCVIMsgManager, glbMsgLock, glbRootPath, &glbReinjectModeActive, &glbCapabilityModeActive, errmsg);
	if (error)
	{
		DelayWithEventProcessing(10.0);	// Time to display error
		errChk(error);
	}
#endif
	
Error:
	CA_DiscardObjHandle(execution);
	return error;
}

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Add/Update data structures to DefineNewTypesHereThenCopyElsewhere.seq
* 
* This is a quick way to translate enums defined in code to TestStand structures.
* 	The macro called in this function works for TestStand 2017 but not later versions.
* 	Not sure about previous versions either.
* 
* All pre-defined structures in DefineNewTypesHereThenCopyElsewhere.seq and any
* 	StationGlobals should be deleted before running this function to ensure there's
* 	no conflicts.  Since this function saves the structures' version as the current
* 	date, it should be the highest version compared to all other sequences. Once
* 	the function completes successfully, open DefineNewTypesHereThenCopyElsewhere.seq
* 	first, then open all other sequences in the project in order to update the
* 	respective structures there using the built in TestStand conflict handler.
* 
* \sa TSTypeInsertMacros.h
*******************************************************************************/
int ArxUtil_UpdateStructsToTS (void)
{
	int error = 0;
	
	char currentPath[MAX_PATHNAME_LEN] = {0};
	char seqPath[MAX_PATHNAME_LEN] = {0};
	GetDir (currentPath);
	sprintf (seqPath, "%s\\..\\..\\Sequence\\Sequencer\\DefineNewTypesHereThenCopyElsewhere.seq", currentPath);
	ALL_ENUMS;
	
Error:
	return error;
}
#endif

/***************************************************************************//*!
* \brief Resizes the main panel, called from ArxUtil_ResizeTSPanel() and ArxUtil_ResizeExecPanel()
*******************************************************************************/
int ArxUtil_ResizeMainPanel (int Panel, int MenuHeight, int StatusBarHeight)
{
	int error = 0;
	
	errChk( SetPanelAttribute(Panel, ATTR_HEIGHT, glbWindowHeight-StatusBarHeight-MenuHeight));
	errChk( SetPanelAttribute(Panel, ATTR_WIDTH, glbWindowWidth));
	errChk( SetPanelAttribute(Panel, ATTR_TOP, MenuHeight));
	errChk( SetPanelAttribute(Panel, ATTR_LEFT, 0));
	
Error:
	return error;
}

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Copy pasted from UI template for resizing the TS panels
*******************************************************************************/
int ArxUtil_ResizeTSPanel (int MenuHeight, int StatusBarHeight)
{
	int error = 0;
	
	const int	buttonVerticalMargin = 6;
	const int	buttonHorizontalSpacing = 12;
	Rect		tabInteriorRect, tabExteriorRect;
	int			buttonTop;
	int			buttonHeight;
	int 		resizableTabControlAreaHeight;	
	int			resizableTabControlAreaWidth;
	VBOOL		isEditor;
	int			fileStepsWidth;
	int			val;
	
	ArxUtil_ResizeMainPanel (gMainWindow.tspanel, MenuHeight, StatusBarHeight);

	// resize controls to fit current window height
	errChk( GetCtrlBoundingRect(gMainWindow.tspanel, DEBUGPANEL_TAB, &tabExteriorRect.top, &tabExteriorRect.left, &tabExteriorRect.height, &tabExteriorRect.width))
	
	tabExteriorRect = MakeRect(1, tabExteriorRect.left, Max(glbWindowHeight - StatusBarHeight - MenuHeight, 50), tabExteriorRect.width);
	errChk( SetCtrlAttribute(gMainWindow.tspanel, DEBUGPANEL_TAB, ATTR_TOP, tabExteriorRect.top));
	errChk( SetCtrlAttribute(gMainWindow.tspanel, DEBUGPANEL_TAB, ATTR_LEFT, tabExteriorRect.left));
	errChk( SetCtrlAttribute(gMainWindow.tspanel, DEBUGPANEL_TAB, ATTR_HEIGHT, tabExteriorRect.height));
	errChk( SetCtrlAttribute(gMainWindow.tspanel, DEBUGPANEL_TAB, ATTR_WIDTH, tabExteriorRect.width));
	
	errChk( SetCtrlAttribute(gMainWindow.tspanel, DEBUGPANEL_LISTBAR, ATTR_HEIGHT, tabExteriorRect.height));

    // there is a vertical drag bar between listbar and tab control
	errChk(TS_Splitters_DivideSpaceBetweenPanes(gMainWindow.tspanel, &errorInfo, TRUE, 0, glbWindowWidth, DEBUGPANEL_LISTBAR, DEBUGPANEL_TAB, 0));

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
#endif

/***************************************************************************//*!
* \brief Custom function for resizing Execution panel
*******************************************************************************/
int ArxUtil_ResizeExecPanel (int MenuHeight, int StatusBarHeight)
{
	int error;
	
	int tmp = 0;
	int val1 = 0;
	int val2 = 0;
	int widthLogoLED = 0;
	int widthCounters = 0;
	int widthTestProgInfo = 0;
	int widthTestProgInfoMin = CVIUI_MIN_WINDOW_WIDTH-2*CVIUI_SPACER;
	int glbWindowWidthWithSpace = glbWindowWidth-2*CVIUI_SPACER;
	int rowLogoLED = 0;
	int rowCounters = 0;
	int row2Top = 0;
	
	ArxUtil_ResizeMainPanel (gMainWindow.execpanel, MenuHeight, StatusBarHeight);
	
	// Get control widths for resizing purposes
	errChk( GetCtrlArrayBoundingRect(gMainWindow.grpLogoLEDs,&tmp,&tmp,&tmp,&widthLogoLED));
	
	errChk( GetCtrlArrayBoundingRect(gMainWindow.grpCounters,&tmp,&tmp,&tmp,&widthCounters));
	
	errChk( GetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, ATTR_TOP, &val1));
	errChk( GetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, ATTR_HEIGHT, &val2));
	row2Top = val1+val2+CVIUI_SPACER;
	
	// Determine control configuration
	if (glbWindowWidthWithSpace<(widthTestProgInfoMin+widthLogoLED))
	{
		widthTestProgInfo = glbWindowWidthWithSpace;
		rowLogoLED = rowCounters = 1;
	}
	else if (glbWindowWidthWithSpace>(widthTestProgInfoMin+widthLogoLED) && 
			 glbWindowWidthWithSpace<(widthTestProgInfoMin+widthLogoLED+widthCounters))
	{
		widthTestProgInfo = glbWindowWidthWithSpace - widthLogoLED;
		rowLogoLED = 0;
		rowCounters = 1;
	}
	else
	{
		widthTestProgInfo = glbWindowWidthWithSpace - widthLogoLED - widthCounters;
		rowLogoLED = rowCounters = 0;
	}
	
	// Test Program Info
	errChk( SetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, ATTR_WIDTH, widthTestProgInfo));
	errChk( SetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, ATTR_LABEL_LEFT, VAL_AUTO_CENTER));
	errChk( SetTableColumnAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, 1, ATTR_COLUMN_WIDTH, (widthTestProgInfo-8)/8));
	errChk( SetTableColumnAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, 2, ATTR_COLUMN_WIDTH, (widthTestProgInfo-8)/8*3));
	errChk( SetTableColumnAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, 3, ATTR_COLUMN_WIDTH, (widthTestProgInfo-8)/8));
	errChk( SetTableColumnAttribute(gMainWindow.execpanel, EXEC_PANEL_TESTINFO, 4, ATTR_COLUMN_WIDTH, (widthTestProgInfo-8)/8*3));
	
	// Logos/LEDs
	errChk( ArxUtil_MoveCtrlArray(gMainWindow.execpanel, gMainWindow.grpLogoLEDs, CVIUI_SPACER+row2Top*rowLogoLED, glbWindowWidth-CVIUI_SPACER-widthLogoLED));
	
	// Counters
	errChk( ArxUtil_MoveCtrlArray(gMainWindow.execpanel, gMainWindow.grpCounters, CVIUI_SPACER+row2Top*rowCounters, CVIUI_SPACER+widthTestProgInfo*(rowCounters==0)));
	
	int tabTop = row2Top+rowCounters*110+CVIUI_SPACER;
	int tabHeight = glbWindowHeight-tabTop-MenuHeight-StatusBarHeight;
	int nestTop = 0;
	int tabsOffset = 0;
#ifdef USE_TABS
	tabsOffset = 30;
	// Tabs
	errChk( SetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TAB_NESTS, ATTR_TOP, tabTop));
	errChk( SetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TAB_NESTS, ATTR_WIDTH, glbWindowWidth-CVIUI_SPACER*2));
	errChk( SetCtrlAttribute(gMainWindow.execpanel, EXEC_PANEL_TAB_NESTS, ATTR_HEIGHT, tabHeight));
#endif
	
	// Nests
#ifndef USE_TABS
	nestTop = tabTop;
#endif
	int titleThickness = 0;
	errChk( GetPanelAttribute(gMainWindow.nests[0],ATTR_TITLEBAR_THICKNESS,&titleThickness));
	int glbWindowHeightLeftOver = tabHeight-tabsOffset-titleThickness*CVIUI_NUM_NEST_ROWS;
	int nestHeight = glbWindowHeightLeftOver/CVIUI_NUM_NEST_ROWS;
	int nestWidth = glbNestOrigWidth = glbWindowWidthWithSpace/CVIUI_NUM_NEST_COLS;
	
	for (int i=0;i<glbNumSockets;++i)
	{
		glbNestOrigLeft[i] = CVIUI_SPACER+nestWidth*((i%CVIUI_NUM_NEST_PER_TAB)%CVIUI_NUM_NEST_COLS);
		errChk( SetPanelAttribute(gMainWindow.nests[i],ATTR_TOP,nestTop+nestHeight*((i%CVIUI_NUM_NEST_PER_TAB)/CVIUI_NUM_NEST_COLS)+titleThickness*(1+(i%CVIUI_NUM_NEST_PER_TAB)/CVIUI_NUM_NEST_COLS)));
		errChk( SetPanelAttribute(gMainWindow.nests[i],ATTR_LEFT,glbNestOrigLeft[i]));
		errChk( SetPanelAttribute(gMainWindow.nests[i],ATTR_HEIGHT,nestHeight));
		errChk( SetPanelAttribute(gMainWindow.nests[i],ATTR_WIDTH,nestWidth));
	
		// Nest Elements
		// Counters
		errChk( GetCtrlArrayBoundingRect(gMainWindow.nestGrpCtrls[i][1],&tmp,&tmp,&tmp,&val1));
		errChk( ArxUtil_MoveCtrlArray(gMainWindow.nests[i], gMainWindow.nestGrpCtrls[i][1], CVIUI_SPACER, nestWidth-CVIUI_SPACER-val1));
		
		// Width for all control groups
		for (int j=2;j<5;++j)
			errChk( SetCtrlArrayAttribute(gMainWindow.nestGrpCtrls[i][j],ATTR_WIDTH,nestWidth-2*CVIUI_SPACER));
		
		// Instructions (Fixed height 100)
		errChk( ArxUtil_MoveCtrlArray(gMainWindow.nests[i],gMainWindow.nestGrpCtrls[i][3],nestHeight-100-100*CVIUI_HAS_ROT,CVIUI_SPACER));
		
		// Rotary (Fixed height 100)
		errChk( GetCtrlArrayBoundingRect(gMainWindow.nestGrpCtrls[i][3],&val1,&tmp,&val2,&tmp));
		errChk( ArxUtil_MoveCtrlArray(gMainWindow.nests[i],gMainWindow.nestGrpCtrls[i][4],val1+val2,CVIUI_SPACER));
		
		// Test Status (Fixed top 80)
		errChk( GetCtrlArrayBoundingRect(gMainWindow.nestGrpCtrls[i][3],&val1,&tmp,&tmp,&tmp));
		int leftOverSpace = val1-80;
#ifdef USE_TABS
		leftOverSpace -= 10;
#endif
		int dataListHeight = 0.57*leftOverSpace;
		errChk( SetCtrlAttribute(gMainWindow.nests[i],NEST_PANEL_DATA_LISTBOX,ATTR_HEIGHT,dataListHeight));
		
		errChk( GetCtrlAttribute(gMainWindow.nests[i],NEST_PANEL_DATA_LISTBOX,ATTR_HEIGHT,&val1));
		int additionalresultsHeight = leftOverSpace-CVIUI_SPACER-val1;
		errChk( SetCtrlAttribute(gMainWindow.nests[i],NEST_PANEL_ADDITIONAL_RESULTS,ATTR_HEIGHT,additionalresultsHeight));
		errChk( SetCtrlAttribute(gMainWindow.nests[i],NEST_PANEL_ADDITIONAL_RESULTS,ATTR_TOP,val1+80+CVIUI_SPACER));
	}
	
Error:
	return error;
}

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Function to refresh the TestStand menu content. Mostly copied from template
*******************************************************************************/
int ArxUtil_RebuildTSMenu (int MenuBar)
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
		TSUIConst_CommandKind_Separator,					// separates the existing Mode switch items
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

	// rebuild File menu
	errChk( TS_InsertCommandsInMenu(fileMenuCommands, MenuBar, BAR_FILE, -1, viewMgr, NULL, TRUE, errMsg));
	
	// rebuild Edit menu
	errChk( TS_InsertCommandsInMenu(sEditMenuCommands, MenuBar, glbMenuEdit, -1, viewMgr, NULL, TRUE, errMsg));
	
	// rebuild Execute menu
	errChk( TS_InsertCommandsInMenu(executeMenuCommands, MenuBar, glbMenuExecute, -1, viewMgr, NULL, TRUE, errMsg));

	// rebuild Debug menu
	errChk( TS_InsertCommandsInMenu(debugMenuCommands, MenuBar, glbMenuDebug, -1, viewMgr, NULL, TRUE, errMsg));

	// rebuild Configure menu
	errChk( TS_InsertCommandsInMenu(configureMenuCommands, MenuBar, glbMenuConfigure, -1, viewMgr, NULL, TRUE, errMsg));
	TSUI_ApplicationMgrGetIsEditor(gMainWindow.applicationMgr, &errorInfo, &isEditor);
	if(isEditor)
	errChk( TS_InsertCommandsInMenu(configureMenuCommands_EditMode, MenuBar, glbMenuConfigure, -1, viewMgr, NULL, TRUE, errMsg));    

	// rebuild Tools menu
	errChk( TS_InsertCommandsInMenu(toolsMenuCommands, MenuBar, glbMenuTools, -1, viewMgr, NULL, TRUE, errMsg));

	// rebuild the Help menu. Note that the help menu already contains an "About..." item, which is not a TestStand command item
	errChk( TS_InsertCommandsInMenu(helpMenuCommands, MenuBar, BAR_HELP, -1, viewMgr, NULL, TRUE, errMsg));
	
Error:
	return error;
}
#endif

/***************************************************************************//*!
* \brief (N)ew (M)enu (I)tem, a quick macro to simplify adding menu items
*******************************************************************************/
#define NMI(menu,name,CB,CBData,disable)\
	menuHandle = NewMenuItem(MenuBar,menu,name,-1,0,CB,(void*)CBData);\
	SetMenuBarAttribute(MenuBar,menuHandle,ATTR_DIMMED,disable)
/***************************************************************************//*!
* \brief Builds the menu for exec panel
*******************************************************************************/
int ArxUtil_RebuildExecMenu (int MenuBar, int IsAdmin)
{
	int error = 0;
	CAObjHandle viewMgr = 0;
	
	int menuHandle = 0;
	
#ifndef CVIUI_ONLY
	// Limited version of TS file menu
	static enum TSUIEnum_CommandKinds	fileMenuCommands[] = 
	{
		TSUIConst_CommandKind_Login,
		TSUIConst_CommandKind_Exit,
		TSUIConst_CommandKind_NotACommand					// list terminator
	};
	errChk( GetActiveViewManager(&viewMgr));
	errChk( TS_InsertCommandsInMenu(fileMenuCommands, MenuBar, BAR_FILE, -1, viewMgr, NULL, TRUE, errMsg));
#else
	/* 20241106Tim: Callback for login  */
	NMI (BAR_FILE, "Login", ArxCVICB_DisplayLoginPrompt, NULL, 0==glbInterfaceStarted);
	if (IsAdmin>0)
	{
		NMI (BAR_FILE, "Logout", ArxCVICB_Logout, NULL, 0);
	}

	/* 20241106Tim: TODO display errmsg  */
	char errmsg[1024] = {0};
	errChk (UserManagement_LoadPrompt (0, NULL, NULL, errmsg));
#endif
	
	// Execute
	NMI (glbMenuExecute, "Reload Recipe"		, ArxCB_MenuCallback, MENU_RECIPE				, IsAdmin<1	);
	NMI (glbMenuExecute, "Golden Positive"		, ArxCB_MenuCallback, MENU_GOLDEN_POSITIVE		, IsAdmin<1	);
	NMI (glbMenuExecute, "Golden Negative"		, ArxCB_MenuCallback, MENU_GOLDEN_NEGATIVE		, IsAdmin<1	);
	NMI (glbMenuExecute, "Reinject Mode"		, ArxCB_MenuCallback, MENU_REINJECT				, IsAdmin<1	);
	NMI (glbMenuExecute, "Reset Statistics"		, ArxCB_MenuCallback, MENU_RESET_STATISTICS		, IsAdmin<2	);
	NMI (glbMenuExecute, "Toggle Traceability"	, ArxCB_MenuCallback, MENU_TRACE_TOGGLE			, IsAdmin<1	);
	
	// Debug
	NMI (glbMenuDebug, "Capability Mode"		, ArxCB_MenuCallback, MENU_CAPABILITY	, IsAdmin<3);
	NMI (glbMenuDebug, "Watch View"				, ArxCB_OpenPanel, MENU_WATCHEXP_PANEL	, IsAdmin<3);
	InsertSeparator (MenuBar, glbMenuDebug, -1);
	NMI (glbMenuDebug, "Debug Panel"			, ArxCB_OpenPanel, MENU_DEBUG_PANEL, IsAdmin<2);
	
	// Configure
	NMI (glbMenuConfigure, "Edit Recipe"			, ArxCB_OpenPanel, MENU_EDIT_RECIPE			, IsAdmin<2);
	NMI (glbMenuConfigure, "Edit System Config"		, ArxCB_OpenPanel, MENU_EDIT_SYSTEM_CONFIG	, IsAdmin<2);
	NMI (glbMenuConfigure, "Edit Station Config"	, ArxCB_OpenPanel, MENU_EDIT_STATION_CONFIG	, IsAdmin<2);
#ifdef CVIUI_ONLY
	NMI (glbMenuConfigure, "Edit Test Sequence"		, ArxCB_OpenPanel, MENU_EDIT_CVI_TS, IsAdmin<2);
#endif
	
Error:
	if (viewMgr) CA_DiscardObjHandle(viewMgr);
	return error;
}
//! \cond
/// REGION END

/// REGION START Template Functions
//! \endcond
#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Build a context menu for a sequence view control that has been right-clicked
*******************************************************************************/
int BuildSequenceViewContextMenu(int menuHandle)
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
	CA_DiscardObjHandle(viewMgr);
	CA_DiscardObjHandle(cmds);
	return error;
}

/***************************************************************************//*!
* \brief Returns the SequenceFileViewManager or the ExecutionViewManager depending on whether the application is displaying sequence files or executions
* 	the returned viewManager is a weak reference (ie. do not call CA_DiscardObjHandle on it).
*******************************************************************************/
int GetActiveViewManager(CAObjHandle *viewManager)
{
	int	error = 0;
	int	page = 0;
	
	tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &page));
	if (page == SEQUENCE_FILES_PAGE_INDEX)	// sequence files are visible, sequence file menu commands apply
		*viewManager = gMainWindow.sequenceFileViewMgr;
	else
	if (page == EXECUTIONS_PAGE_INDEX)		// executions are visible, execution menu commands apply
		*viewManager = gMainWindow.executionViewMgr;

Error:
	return error;
}
#endif

//! \cond TESTSTAND
/***************************************************************************//*!
* \brief Call this function after you handle an error, unless you handle the error by calling DisplayError, which also calls this function
*******************************************************************************/
void ClearErrorMessage(void)
{
	// clear out error message globals so that a future error that lacks an error description does not
	// unintentionally use the error description from a prior error.
	*errMsg = '\0';
	memset(&errorInfo, 0, sizeof(ERRORINFO)); 
}

/***************************************************************************//*!
* \brief Displays a message box with the error code, the error message associated with the code, and any error description details 
* 	does nothing if errorCode is not negative
*******************************************************************************/
void DisplayError(int errorCode)
{
#ifndef CVIUI_ONLY
	if (errorCode < 0)
	{
		TS_DisplayError(errorCode, errMsg, gMainWindow.engine);	// errMsg is a global, see top of file
		ClearErrorMessage();	
	}
#endif
}

/***************************************************************************//*!
* \brief Based on the current listbar page, show and hide the tabs that appear in the space to the right of the listbar
*******************************************************************************/
int ShowAppropriateTabs(void)
{
	int			error = 0;
	long		pageIndex = 0;
	CAObjHandle	execution = 0;
	void 	*	curActiveCtrlInfo;
	double	lastActiveExecutionTab = 1; // Default to the Execution tab when showing executions.


#ifndef CVIUI_ONLY
	gMainWindow.programmaticallyUpdatingTabPages = TRUE;

	// Note: Store the current Active control as EasyTab can change the active panel and control
	curActiveCtrlInfo = SaveActiveCtrl(0,0);

	tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));
	
	// Note: EasyTab does not allow the last visible tab to be made invisible. Thus, when hiding and showing tabs, you must make sure there is at least one other tab visible before
	// hiding a tab.  The easiest way to ensure this is to first show all tabs. Then, at least one tab is visible that will remain visible and all other tabs can be hidden succesfully.

	// show all tabs
	errChk( SetTabPageAttribute (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_FILE,      ATTR_VISIBLE, TRUE));
	errChk( SetTabPageAttribute (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_EXECUTION, ATTR_VISIBLE, TRUE));
	errChk( SetTabPageAttribute (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_REPORT,    ATTR_VISIBLE, TRUE));
	
	// show and hide tabs as appropriate
	errChk( SetTabPageAttribute (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_EXECUTION, ATTR_VISIBLE, pageIndex == EXECUTIONS_PAGE_INDEX));
	errChk( SetTabPageAttribute (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_REPORT,    ATTR_VISIBLE, pageIndex == EXECUTIONS_PAGE_INDEX));
	errChk( SetTabPageAttribute (gMainWindow.tspanel, DEBUGPANEL_TAB, TABPAGE_FILE,      ATTR_VISIBLE, pageIndex == SEQUENCE_FILES_PAGE_INDEX));

	// if we are viewing an execution...
	if (pageIndex == EXECUTIONS_PAGE_INDEX)
	{
		// determine which tab page we last displayed for this execution
		tsErrChk( TSUI_ExecutionViewMgrGetExecution(gMainWindow.executionViewMgr, &errorInfo, &execution));
		if (execution)
			tsErrChk( TS_PropertyGetValNumber(execution, &errorInfo, "NIUI.LastActiveTab", TS_PropOption_InsertIfMissing, &lastActiveExecutionTab));
		
		// re-activate previously active tab for the execution
		errChk( SetActiveTabPage (gMainWindow.tspanel, DEBUGPANEL_TAB, (int)lastActiveExecutionTab)); // Cast OK.  We were storing an integer in a TS number.
	}

	// Note: Set the current Active control as EasyTab can change the active panel and control
	RestoreActiveCtrl(curActiveCtrlInfo);
#endif
	
Error:
#ifndef CVIUI_ONLY
	gMainWindow.programmaticallyUpdatingTabPages = FALSE;

	CA_DiscardObjHandle(execution);
#endif
	return error;
}

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief If the execution has a report, switch to the report tab either immediately if the execution is visible, or, whenever the execution is viewed next
*******************************************************************************/
int ShowReport(CAObjHandle execution)
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
#endif

/***************************************************************************//*!
* \brief Updates the application window title
*******************************************************************************/
int UpdateWindowTitle(void)
{
	int 	error = 0;
	char	title[1024];
	int		pageIndex = 0;
	char *	documentDescription = NULL;
	
#ifndef CVIUI_ONLY
	errChk( TS_GetResourceString(gMainWindow.engine, "TSUI_OI_MAIN_PANEL", "TESTSTAND_USER_INTERFACE", "", sizeof(title), title));

	tsErrChk( TSUI_ListBarGetCurrentPage(gMainWindow.listBar, &errorInfo, &pageIndex));

	if (pageIndex == SEQUENCE_FILES_PAGE_INDEX)	// sequence files are visible
		tsErrChk( TSUI_SequenceFileViewMgrGetCaptionText(gMainWindow.sequenceFileViewMgr, &errorInfo, TSUIConst_CaptionSource_CurrentSequenceFile, VFALSE, "", &documentDescription));
	else	// executions are visible
		tsErrChk( TSUI_ExecutionViewMgrGetCaptionText(gMainWindow.executionViewMgr, &errorInfo, TSUIConst_CaptionSource_CurrentExecution, VFALSE, "", &documentDescription));
	
	if (*documentDescription)
		strcat(strcat(title, " - "), documentDescription);

	errChk( SetPanelAttribute(gMainWindow.panel, ATTR_TITLE, title));
#endif

Error:
#ifndef CVIUI_ONLY
	CA_FreeMemory(documentDescription);
#endif
	return error;
}
//! \endcond TESTSTAND

/***************************************************************************//*!
* \brief Adjust controls to fit within current window size
*******************************************************************************/
int ArrangeControls(int processEvents)
{
	int			error = 0;
	int			menuHeight;
	int 		statusBarHeight;
	
	memset (glbNestExpanded, 0, sizeof(glbNestExpanded));

	// get current sizes and positions
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_HEIGHT, &glbWindowHeight));
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_MENU_HEIGHT, &menuHeight));
	errChk( GetPanelAttribute(gMainWindow.panel, ATTR_WIDTH, &glbWindowWidth));
	errChk( ArxUtil_CheckMinSize (&glbWindowHeight, &glbWindowWidth));
	
#ifndef CVIUI_ONLY
	errChk( GetCtrlAttribute(gMainWindow.panel, MAINPANEL_STATUSBAR, ATTR_HEIGHT, &statusBarHeight));
	errChk( ArxUtil_ResizeTSPanel(menuHeight,statusBarHeight));
#endif
	errChk( ArxUtil_ResizeExecPanel(menuHeight,0));
	
Error:
	return error;
}

//! \cond TESTSTAND
#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief We specifically rebuild the Edit menu on EVENT_GOT_FOCUS/EVENT_LOST_FOCUS because TS_InsertCommandsInMenu removes the common edit menu shortcuts (Ctrl-X, Ctrl-C, Ctrl-V, Ctrl-A, Delete) 
* 	when the focus is not on an ActiveX control such as a TestStand control. It does this because the presence of those shortcut keys in the menu (even on dimmed items) prevents their 
* 	operation with CVI controls such as tables and text boxes. Because these shortcut keys are not always present in the menu, we do not always get a MenuDimmerCallback when they are pressed.
* 	thus, we also rebuild the Edit menu when the control focus changes. We could instead rebuild the entire menu bar by calling RebuildMenuBar, but depending on the machine, that might 
* 	noticably reduce the speed at which you can tab between controls.
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
	errChk( TS_RemoveMenuCommands(menuBar, glbMenuEdit, FALSE, FALSE));

	// rebuild Edit menu
#ifdef UI_DEBUG
	if (!UI_DEBUG)
#endif
	{
		errChk( TS_InsertCommandsInMenu(sEditMenuCommands, menuBar, glbMenuEdit, -1, viewMgr, NULL, TRUE, errMsg));
	}

	// remove duplicate separators and shortcut keys
	errChk( TS_CleanupMenu(menuBar, 0));

Error:
	return error;
}
#endif
//! \endcond TESTSTAND

/***************************************************************************//*!
* \brief Make sure all menus have appropriate items with the correct enabled states
*******************************************************************************/
int RebuildMenuBar(int menuBar)
{
	int error = 0;
	CAObjHandle user = 0;
	int isAdmin = 0;
	
#ifndef CVIUI_ONLY
   	// discard any current menu items that were inserted by TS_InsertCommandsInMenu
	errChk( TS_RemoveMenuCommands(menuBar, 0, FALSE, FALSE));
#else
	/* 20241106Tim: Remove FILE menu commands */
	EmptyMenu (menuBar, BAR_FILE);
#endif
	if (glbMenuEdit)		{DiscardMenu(menuBar, glbMenuEdit		); glbMenuEdit = 0;}
	if (glbMenuExecute)		{DiscardMenu(menuBar, glbMenuExecute	); glbMenuExecute = 0;}
	if (glbMenuDebug)		{DiscardMenu(menuBar, glbMenuDebug		); glbMenuDebug = 0;}
	if (glbMenuConfigure)	{DiscardMenu(menuBar, glbMenuConfigure	); glbMenuConfigure = 0;}
	if (glbMenuTools)		{DiscardMenu(menuBar, glbMenuTools		); glbMenuTools = 0;}
	
	glbMenuEdit			= NewMenu(menuBar, "Edit"		, BAR_HELP);
	glbMenuExecute		= NewMenu(menuBar, "Execute"	, BAR_HELP);
	glbMenuDebug		= NewMenu(menuBar, "Debug"		, BAR_HELP);
	glbMenuConfigure	= NewMenu(menuBar, "Configure"	, BAR_HELP);
	glbMenuTools		= NewMenu(menuBar, "Tools"		, BAR_HELP);
	
#ifndef CVIUI_ONLY
	char *username = 0;
	// Check logged in user
	tsErrChk( TS_EngineGetCurrentUser(gMainWindow.engine, &errorInfo, &user));
	if (user)	// no user login at init
	{
		tsErrChk( TS_UserGetLoginName(user, &errorInfo, &username));
		if (0==stricmp(username,"administrator"))
		{
			isAdmin = 3;
		}
	}
#else
	/* 20250417Tim: Get Logged in user's admin level */
	char errmsg[1024] = {0};
	isAdmin = 0;
	UserManagement_IsAdmin (NULL, &isAdmin, errmsg);
#endif
	
	// Build default menus
#ifndef CVIUI_ONLY
	glbEngModeMenu	= NewMenuItem(menuBar, glbMenuDebug, "Engineering Mode"	, -1, 0				, ArxCB_ModeSwitchCB, NULL);
	glbOpModeMenu	= NewMenuItem(menuBar, glbMenuDebug, "Operator Mode"	, -1, 0				, ArxCB_ModeSwitchCB, NULL);
	glbEngViewMenu	= NewMenuItem(menuBar, glbMenuDebug, "View TestStand"	, -1, VAL_MENUKEY_MODIFIER | VAL_F9_VKEY	, ArxCB_ModeSwitchCB, NULL);
	glbOpViewMenu	= NewMenuItem(menuBar, glbMenuDebug, "View Production"	, -1, VAL_MENUKEY_MODIFIER | VAL_F10_VKEY	, ArxCB_ModeSwitchCB, NULL);
	SetMenuBarAttribute (menuBar, glbEngModeMenu, ATTR_DIMMED, !isAdmin);
#endif
	
#ifndef CVIUI_ONLY
	if (glbActivePanel == gMainWindow.tspanel)
	{
		errChk( ArxUtil_RebuildTSMenu(menuBar));
	}
	else if (glbActivePanel == gMainWindow.execpanel)
#endif
	{
		errChk( ArxUtil_RebuildExecMenu(menuBar, isAdmin));
	}

	PRJGUI_GetDefaultMenus (menuBar, glbMenuEdit, glbMenuExecute, glbMenuDebug, glbMenuConfigure, glbMenuTools);
	errChk(PRJGUI_AddProjectMenus(&ArxCB_SendProjectSpecificMenuStatus, isAdmin, 0, 0));		// Purge, Scrap
	
#ifndef CVIUI_ONLY
	// remove duplicate separators and shortcut keys
	errChk( TS_CleanupMenu(menuBar, 0));
#endif
	
Error:
	if (user) CA_DiscardObjHandle(user);
#ifndef CVIUI_ONLY
	if (username) CA_FreeMemory(username);
#endif
	return error;
}

//! \cond TESTSTAND
/***************************************************************************//*!
* \brief Show the appropriate group of status bar panes, depending on whether we are viewing a file, execution steps, or a report
*******************************************************************************/
int ShowAppropriateStatusBarPanes(void)
{
	int	error = 0;
	int	activeTab;

#ifndef CVIUI_ONLY
	errChk( GetActiveTabPage(gMainWindow.tspanel, DEBUGPANEL_TAB, &activeTab));

	if (activeTab == gMainWindow.fileTab)
		tsErrChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, FileModel, FileSelectedSteps, FileNumberOfSteps"));
	else if (activeTab == gMainWindow.executionTab)
		tsErrChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, ExecutionModel, ExecutionSelectedSteps, ExecutionNumberOfSteps, ProgressText, ProgressPercent"));
	else
		tsErrChk( TSUI_StatusBarShowPanes(gMainWindow.statusBar, &errorInfo, "User, EngineEnvironment, ExecutionModel, ReportLocation, ProgressText, ProgressPercent"));
#endif		

Error:
	return error;
}
//! \endcond TESTSTAND

/***************************************************************************//*!
* \brief Update glbWatchVar Global Struct with Values in Watch Expression Table
*******************************************************************************/
int UpdateWatchVarStruct (void *functionData)
{
	while(glbWatchExpThreadEn)
	{
		for(int i = 0; i < glbNumWatching; ++i)
		{
			if(strcmp(glbWatchVars[i].varVal, "not found"))
			{
				SetTableCellVal(glbWatchExpPanelHandle, WATCHPANEL_WATCHTABLE, MakePoint(4,(i+1)), glbWatchVars[i].varVal);
			}
		}
		DelayWithEventProcessing(0.5);
	}
	
	return 0;
}
//! \cond
/// REGION END
//! \endcond