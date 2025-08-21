/***************************************************************************//*!
* \file UIDefinitions.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-20 5:13:43 PM
*******************************************************************************/

#ifndef __UIDefinitions_H__
#define __UIDefinitions_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants
		
/***************************************************************************//*!
* \brief Nest grid layout, will affect minimum/maximum window sizes
*******************************************************************************/
#define CVIUI_NUM_NEST_ROWS			1
#define CVIUI_NUM_NEST_COLS			2
#ifdef USE_TABS
#define CVIUI_NUM_NEST_TABS			5
#else
#define CVIUI_NUM_NEST_TABS			1
#endif
#define CVIUI_NUM_NEST_PER_TAB		(CVIUI_NUM_NEST_ROWS*CVIUI_NUM_NEST_COLS)
#define CVIUI_TOTAL_POSSIBLE_NESTS	(CVIUI_NUM_NEST_PER_TAB*CVIUI_NUM_NEST_TABS)

/***************************************************************************//*!
* \brief UI Sizing parameters
*******************************************************************************/
#define CVIUI_SPACER				5

#define CVIUI_MIN_HEADER_HEIGHT		250		// Should not be changed unless header controls are resized/removed
#define CVIUI_MIN_NEST_WIDTH		475		// Should not be changed unless nest header controls are resized/removed
#define CVIUI_MIN_NEST_HEIGHT		(340+100*CVIUI_HAS_ROT)		// Should not be changed unless font size is modified
#define CVIUI_MIN_WINDOW_WIDTH		660		// Hard coded limit
#define CVIUI_MIN_WINDOW_WIDTH2		(CVIUI_MIN_NEST_WIDTH*CVIUI_NUM_NEST_COLS	+CVIUI_SPACER*2)	// Limit based on nest layout
#define CVIUI_MIN_WINDOW_HEIGHT		(60		+CVIUI_MIN_HEADER_HEIGHT		+CVIUI_MIN_NEST_HEIGHT*CVIUI_NUM_NEST_ROWS	+CVIUI_SPACER*3)	// Limit based on nest layout

#define CVIUI_TEST_INFO_WIDTH		900		// Should not be changed unless TSMsg_TEST_RESU() spacing is updated

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int Declare_Your_Functions_Here (int x);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UIDefinitions_H__ */
