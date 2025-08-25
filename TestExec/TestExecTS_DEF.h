/***************************************************************************//*!
* \file TestExecTS_DEF.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/1/2021 2:41:43 PM
* \brief This file contains additional definitions for the TestStand panel.
*******************************************************************************/

#ifndef __TestExec_DEF_H__
#define __TestExec_DEF_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

//==============================================================================
// Constants

//!@{
/***************************************************************************//*!
* \brief page indices for the list bar control corresponding to how it was configured in the UIR editor
*******************************************************************************/
#define SEQUENCE_FILES_PAGE_INDEX	0		// first list bar page
#define EXECUTIONS_PAGE_INDEX		1		// second list bar page

#define TABPAGE_FILE		0
#define TABPAGE_EXECUTION	1
#define TABPAGE_REPORT		2
//!@}

//==============================================================================
// Types

/***************************************************************************//*!
* \brief The main structure containing most of the references required for the GUI
* 	and links to the TestStand ActiveX controls
*******************************************************************************/
typedef struct 
{
	/*******************************************************************************
	* TestStand Panel (TestStand)
	*******************************************************************************/
	// panel handles
	int			panel;								//!< main window
	int			tspanel;							//!< TestStand window
	int			fileTab;							//!< "Sequences" tab. shows information about the selected sequence file
	int			executionTab;						//!< "Steps" tab. shows execution information about the selected execution
	int			reportTab;							//!< "Report" tab. shows the report for the selected execution
	int			aboutBox;							//!< displays version info and logo bitmap

#ifndef CVIUI_ONLY
	// ActiveX control handles:
	CAObjHandle applicationMgr;						//!< invisible control, manages Startup/Shutdown, and other application functions
	CAObjHandle sequenceFileViewMgr;				//!< invisible control, manages a SequenceView control that displays loaded sequence files
	CAObjHandle executionViewMgr;					//!< invisible control, manages a SequenceView control that displays executing sequences
	CAObjHandle	listBar;							//!< vertical icon bar. user can select a sequence file or an execution
	CAObjHandle fileStepListView;					//!< displays the steps in the selected sequence file on the "Sequences" tab
	CAObjHandle executionStepListView;				//!< displays the steps in the selected execution in the "Steps" tab
	CAObjHandle reportView;							//!< displays the report for the selected execution in the "Report" tab
	CAObjHandle fileSequencesList;					//!< sequence selector listbox on the "Sequences" tab shown for selected sequence file
	CAObjHandle fileVariables;						//!< displays the variables for the selected sequence file
	CAObjHandle insertionPalette;					//!< displays a palette for inserting items into sequences
	CAObjHandle sequenceFileLabel;					//!< hidden label connected to CaptionSource_CurrentSequenceFile. When the label receives a ConnectionActivity event, we update the application title bar
	CAObjHandle executionVariables;					//!< displays the variables for the selected execution
	CAObjHandle callStack;							//!< call stack selector listbox on the execution tab
	CAObjHandle threads;							//!< thread selector listbox on the execution tab
	CAObjHandle breakResumeButton;					//!< break/resume button for selected execution
	CAObjHandle terminateRestartButton;				//!< terminate/restart button for selected execution
	CAObjHandle	entryPoint1Button;					//!< entryPoint button 1 for selected sequence file
	CAObjHandle	entryPoint2Button;					//!< entryPoint button 2 for selected sequence file
	CAObjHandle	runCurrentSequenceButton;			//!< run-current-sequence button for selected sequence file
	CAObjHandle	statusBar;							//!< status bar at bottom of window
	CAObjHandle executionLabel;						//!< hidden label connected to CaptionSource_CurrentExecution. When the label receives a ConnectionActivity event, we update the application title bar

	CAObjHandle	engine;								//!< TestStand Engine Handle, stored here for convenient availability
#endif

	int			programmaticallyUpdatingTabPages;	//!< TRUE while within ShowAppropriateTabs()
	int			guiThreadId;						//!< for filtering out CVI events from other threads in the MainCallback

#ifndef CVIUI_ONLY
	/*******************************************************************************
	* Additional Custom Handles 
	*******************************************************************************/
	CAObjHandle modelSequence;						//!< Arx Custom, handle to the model sequence for starting execution/getting some model properties
	CAObjHandle arxtronSequencer;					//!< Arx Custom, handle to the Arxtron Sequencer sequence
	CAObjHandle projectSequencer;					//!< Arx Custom, handle to the ProjectSpecific sequencer sequence
#endif

	/*******************************************************************************
	* Execute Panel (CVI)
	*******************************************************************************/
	int			execpanel;							//!< Execution window (CVI)
	int			tabpanels[CVIUI_NUM_NEST_TABS];		//!< Tabbed Nest pages
	
	int			grpCounters;						//!< Control array for counters (resize purpose only)
	int			grpCountersST;						//!< Control array for short term counters
	int			grpCountersLT;						//!< Control array for long term counters
	int			grpLogoLEDs;						//!< Control array for Logos and LEDs (resize purpose only)
	int			grpLEDs;							//!< Control array for LEDs
	int			grpLogos;							//!< Control array for Logos
	
	int			nests[CVIUI_TOTAL_POSSIBLE_NESTS];					//!< Handles for each nest panel
	/*!
		\brief Handles for each nest panel
		
		* 0 = LED/Serial
		* 1 = Counters
		* 2 = Test Status
		* 3 = Instruction Status
		* 4 = Rotary Status
		
		See functions ArxUtil_GetExecPanelHandles, ArxUtil_CloneNests in source code \ref TestExecUtility.c
	*/
	int			nestGrpCtrls[CVIUI_TOTAL_POSSIBLE_NESTS][10];		// 
	
	/*******************************************************************************
	* Other Panels
	*******************************************************************************/
	int			arxPopupPanel;					//!< Popup panel for displaying messages
	
	/*******************************************************************************
	* User objects
	*******************************************************************************/
#ifndef CVIUI_ONLY
	CAObjHandle userAdmin;							//!< Administrator user object
	CAObjHandle userOper;							//!< Operator user object
#endif
} ApplicationWindow;

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief the Edit menu is rebuild both by RebuildMenuBar and RebuildEditMenu, so this array is at file scope so they can both access it
*******************************************************************************/
static enum TSUIEnum_CommandKinds	sEditMenuCommands[] = 
{
	TSUIConst_CommandKind_DefaultEditMenu_Set,			// add all the usual commands in an Edit menu
	TSUIConst_CommandKind_NotACommand					// list terminator
};
#endif

//==============================================================================
// External variables

extern ApplicationWindow	gMainWindow;
extern ERRORINFO			errorInfo;
extern ErrMsg				errMsg;
extern char					glbSequencerPath[MAX_PATHNAME_LEN];
extern char					glbPrjSequencerPath[MAX_PATHNAME_LEN];
extern double				glbNumSockets;

//==============================================================================
// Global functions

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExec_DEF_H__ */
