/***************************************************************************//*!
* \file CommonTSIncludes.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/1/2021 3:04:34 PM
* \brief This file contains includes that are used in multiple places, which
* 	simplifies the includes in other files.
* 
* Some includes are done in a specific order in order for compile to work correctly.
* 	Do not shuffle the order around unless necessary.
*******************************************************************************/

#ifndef __CommonTSIncludes_H__
#define __CommonTSIncludes_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "UIDefinitions.h"
		
#ifndef CVIUI_ONLY
 #include "tsapicvi.h"
 #include "tsui.h"
 #include "tsutil.h"
#else
 #include <cviauto.h>
 #include <utility.h>
 #include <ansi_c.h>
 #include <toolbox.h>
#endif
 		
#define ERRMSG_SIZE     1024
typedef char ErrMsg[ERRMSG_SIZE];

#define TS_UIMsg_UserMessageBase 10000

#include "TestExecExecute_DEF.h"	// Execute has to be before TS due to Nest Row/Col reference
#include "TestExecTS_DEF.h"
#include "TestExecUtility.h"
#include "ProjectSpecificGUI_LIB.h"

#include "Splash.h"
#include "MessagePopup.h"
#include "TestExecTS.h"
#include "TestExecExecute.h"
#include "WatchExpressionsWindow.h"

#ifdef CVIUI_ONLY
#include "CVISequencer.h"
#include "TestExecMainPanelCVIOnly.h"		// UIR header
#include "UserManagement_LIB.h"
#else
#include "TestExecMainPanel.h"		// UIR header
#endif
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

extern int glbNestOrigWidth;
extern int glbWindowHeight;
extern int glbWindowWidth;
extern int glbNestExpanded[CVIUI_TOTAL_POSSIBLE_NESTS];
extern int glbNestOrigLeft[CVIUI_TOTAL_POSSIBLE_NESTS];

extern int glbActivePanel;
extern int glbOpModeMenu;
extern int glbEngModeMenu;

//==============================================================================
// Global functions

#ifndef CVIUI_ONLY
void CVICALLBACK ArxCB_ModeSwitchCB (int menuBar, int menuItem, void *callbackData, int panel);
#endif

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __CommonTSIncludes_H__ */
