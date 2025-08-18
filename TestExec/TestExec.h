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

#include "Callbacks.h"
#include "TestExecUtils.h"
		
#include "TestExecPanel.h"
#include "SplashPanel.h"

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

//==============================================================================
// External variables

//==============================================================================
// Global functions

int ExitApplication(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExec_H__ */
