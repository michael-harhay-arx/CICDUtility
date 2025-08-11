/***************************************************************************//*!
* \file ProjectSpecificGUI_LIB.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 5/28/2021 5:52:06 PM
*******************************************************************************/

#ifndef __ProjectSpecificGUI_LIB_H__
#define __ProjectSpecificGUI_LIB_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//!@{
/***************************************************************************//*!
* \brief Lookup strings for specific variables used in the sequencer callbacks.
* 	Because they could be changed per project, they are located here instead of
* 	the main sequencer.
*******************************************************************************/
#define ARX_UI_CAPABILITY_LOOKUP	"FileGlobals.CapabilityMode"
#define ARX_UI_CAPABILITY_SEQNAME	"ProjectSpecificSequencer"
#define ARX_UI_DEBUGACTIVE_LOOPUP	"StationGlobals.DebugPanelActive"
//!@}

//==============================================================================
// Types
		
//==============================================================================
// Global vaiables

//==============================================================================
// External variables

//==============================================================================
// Global functions
int PRJGUI_AddProjectMenus (void CVICALLBACK (*CallBack)(int, int, void *, int), int IsAdmin, int BeforeMenu, int BeforeMenuItem);
int PRJGUI_AddProjectGUIControls (int ParentPanel);
void PRJGUI_CloseCustomPanels (void);
void PRJGUI_GetDefaultMenus (int MenuBar, int MenuEdit, int MenuExec, int MenuDebug, int MenuConfig, int MenuTools);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ProjectSpecificGUI_LIB_H__ */