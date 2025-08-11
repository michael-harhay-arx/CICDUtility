/***************************************************************************//*!
* \file ActiveXControls.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 3/3/2021 5:11:45 PM
*******************************************************************************/

#ifndef __ActiveXControls_H__
#define __ActiveXControls_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int GetActiveXControlHandles(void);
int RegisterActiveXEventCallbacks(void);
int ConnectStatusBarPanes(void);
int ConnectTestStandControls(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ActiveXControls_H__ */
