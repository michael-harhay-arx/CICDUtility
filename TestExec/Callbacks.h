/***************************************************************************//*!
* \file Callbacks.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-15 4:12:48 PM
*******************************************************************************/

#ifndef __Callbacks_H__
#define __Callbacks_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include <cvirte.h>		
#include <userint.h>
#include "TestExec.h"		// UIR header
#include "tsapicvi.h"   	// TestStand API
#include "tsui.h"			// API's for the TestStand ActiveX controls
#include "tsuisupp.h"		// support API for the TestStand ActiveX controls
#include <utility.h>
#include "tsutil.h"			// helpful functions for using TestStand APIs in CVI
#include "easytab.h"		// for the tab control
#include "toolbox.h"   		// Max, ...
		
#include "TestExec.h"

//==============================================================================
// Constants

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

#endif  /* ndef __Callbacks_H__ */
