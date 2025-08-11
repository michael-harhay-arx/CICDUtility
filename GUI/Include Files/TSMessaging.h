/***************************************************************************//*!
* \addtogroup msg
* @{
*******************************************************************************/
/***************************************************************************//*!
* \file TSMessaging.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 12/2/2021 4:54:37 PM
*******************************************************************************/

#ifndef __TSMessaging_H__
#define __TSMessaging_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

//==============================================================================
// Constants

/***************************************************************************//*!
* \brief Maximum number of 1st level message key
*******************************************************************************/
#define MAX_KEYS	10

//==============================================================================
// Types

/***************************************************************************//*!
* \brief 1 way linked list for nested message functions (\ref msg\)
*******************************************************************************/
typedef struct msgstruct {
	char key[32];	//!< All caps keyword up to 4 letters
	void (*msgCBFunction)(IN int, IN char[16][512]);	//!< Function pointer to the function corresponding to the key
	struct msgstruct *subStruct;	//!< Link to 2nd level keys' structures
	short numSubStructs;	//!< Number of 2nd level keys
} MsgStruct;

//==============================================================================
// External variables

/***************************************************************************//*!
* \brief Struct for storing messaging function details
*******************************************************************************/
MsgStruct glbMsgStruct[MAX_KEYS];
/***************************************************************************//*!
* \brief Thread lock for setting variables in Variable Watch panel
*******************************************************************************/
CmtThreadLockHandle glbSetVarLock;

//==============================================================================
// Global functions

void Deinit_TSMsgCB (MsgStruct* MStruct);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TSMessaging_H__ */
//!@}