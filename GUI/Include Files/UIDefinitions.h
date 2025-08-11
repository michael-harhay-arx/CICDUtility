/***************************************************************************//*!
* \file UIDefinitions.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/17/2021 6:09:40 PM
* \brief This file contains all the UI parameter definitions. This file is also
* 	included in ProjectSpecificGUI_LIB
*******************************************************************************/

#ifndef __UIDefinitions_H__
#define __UIDefinitions_H__

#ifdef __cplusplus
    extern "C" {
#endif

/***************************************************************************//*!
* \brief This flag sets the GUI to be compiled for CVI when it is uncommented.
* 	Otherwise, the default GUI compile will be for TestStand.
*******************************************************************************/
#define CVIUI_ONLY

//!@{
/***************************************************************************//*!
* \brief Error codes
*******************************************************************************/
#define ARXERR_SEQUENCER_NOT_FOUND		-20000
#define ARXERR_GRID_CONFIG_WRONG		-20001
#define ARXERR_MODEL_SUP_LIB_NOT_FOUND	-20002
#define ARXERR_CUSTOM_UI_FAIL			-20003
//!@}

/***************************************************************************//*!
* \brief Set to 1 when only working with UI elements excluding TS connection
*******************************************************************************/
#define UI_DEBUG	0

/***************************************************************************//*!
* \brief Using tabbed Nests (\ref CVIUI_NUM_NEST_TABS)
*******************************************************************************/
//#define USE_TABS

//!@{
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
//!@}

/***************************************************************************//*!
* \brief Whether the station has a rotary table (20211117Biye: Only for old automation projects)
* 	Phased out?
*******************************************************************************/
#define CVIUI_HAS_ROT				0

//!@{
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
//!@}

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UIDefinitions_H__ */
