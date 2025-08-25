/***************************************************************************//*!
* \file UIUtility.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 4/12/2021 1:32:44 PM
*******************************************************************************/

#ifndef __UIUtility_H__
#define __UIUtility_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

/***************************************************************************//*!
* \addtogroup varwatch
* @{
*******************************************************************************/
/***************************************************************************//*!
* @{
* \brief Variable types used for get/set
*******************************************************************************/
#define VARTYPE_NUM		0
#define VARTYPE_STR		1
#define VARTYPE_BOOL	2
//!@}

//==============================================================================
// Types

/***************************************************************************//*!
* \brief Used for storing variables
*******************************************************************************/
typedef struct {
	char seqName[1024];			//!< Store the name of the sequence where the variable in TestStand is found in case there are duplicate local/fileglobals
	char lookupString[1024];	//!< The lookupString parameter used in TestStand to find the variable (Ex. FileGlobals.CapabilityMode)
	int varType;				//!< \ref VARTYPE_NUM "Variable Types"
	char varVal[2048];			//!< Value of the variable
} TSSeqVar;
//!@}

//==============================================================================
// External variables

//==============================================================================
// Global functions

void UIUtil_UpdateYield (int PanelHandle, int CtrlHandle);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UIUtility_H__ */
