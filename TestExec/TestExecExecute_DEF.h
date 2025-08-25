/***************************************************************************//*!
* \file TestExecExecute_DEF.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/30/2021 5:13:58 PM
* \brief This file contains additional definitions for the Execute panel. Typically
* 	dynamically generated UI components.
*******************************************************************************/

#ifndef __TestExecExecute_DEF_H__
#define __TestExecExecute_DEF_H__

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
* \brief Popup panel reference
*******************************************************************************/
#define POPUP_ARXERROR			0
#define POPUP_ARXWARNING		1
//!@}

//!@{
/***************************************************************************//*!
* \brief Menu callback flags
*******************************************************************************/
#define MENU_RECIPE				1
#define MENU_GOLDEN_POSITIVE	2
#define MENU_GOLDEN_NEGATIVE	3
#define MENU_REINJECT			4
#define MENU_RESET_STATISTICS	5
#define MENU_CAPABILITY			6
#define MENU_TRACE_TOGGLE		7
//!@}

//!@{
/***************************************************************************//*!
* \brief Menu callback flags for opening panels
*******************************************************************************/
#define MENU_EDIT_RECIPE			2
#define MENU_EDIT_SYSTEM_CONFIG		3
#define MENU_EDIT_STATION_CONFIG	4
/* 20250312Biye: Obsolete */
//#define MENU_AIO_PANEL				5
//#define MENU_DIO_PANEL				6
//#define MENU_VISA_PANEL				7
#define MENU_WATCHEXP_PANEL			8
#define MENU_DEBUG_PANEL			100
#define MENU_EDIT_CVI_TS			1001
//!@}

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestExecExecute_DEF_H__ */
