/***************************************************************************//*!
* \file TestExec.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-15 4:28:07 PM
*******************************************************************************/

#ifndef __TestExec_H__
#define __TestExec_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

// Project file includes
#include "Callbacks.h"
#include "TestExecUtils.h"
#include "TestThreads.h"
#include "UIDefinitions.h"

// GUI includes		
#include "TestExecPanel.h"
#include "SplashPanel.h"
#include "TestExecExecute.h"
#include "ProjectSpecificGUI_LIB.h"

//==============================================================================
// Constants

#define SEQUENCE_FILES_PAGE_INDEX	0		// first list bar page
#define EXECUTIONS_PAGE_INDEX		1		// second list bar page

#define TABPAGE_FILE		0
#define TABPAGE_EXECUTION	1
#define TABPAGE_REPORT		2		
		
//==============================================================================
// Types
		
typedef struct 
{
	// panel handles
	int			panel;								// main window
	int			tspanel;							// TestStand window
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
	
	CAObjHandle	engine;								// TestStand Engine Handle, stored here for convenient availability
	
	int			programmaticallyUpdatingTabPages;	// TRUE while within ShowAppropriateTabs()
	int			guiThreadId;						// for filtering out CVI events from other threads in the MainCallback
	
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
	
} ApplicationWindow;

//==============================================================================
// External variables

//==============================================================================
// Global functions

int ExitApplication(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExec_H__ */
