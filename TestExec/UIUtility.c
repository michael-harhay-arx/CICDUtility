/***************************************************************************//*!
* \file UIUtility.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 4/12/2021 1:31:45 PM
* \brief This file contains all functions related to UI but are not callbacks.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "CommonTSIncludes.h"
#include "UIUtility.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

/***************************************************************************//*!
* \defgroup varwatch Variable Watch Debug Panel
* @{
*******************************************************************************/
TSSeqVar glbWatchVars[100];
int glbNumWatching = 0;
TSSeqVar glbSetVars[100];
int glbNumSetting = 0;
//!@}

//==============================================================================
// Global functions

void UIUtil_GetYield (int PanelHandle, int NumPass, int NumFail, int Ctrl);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Updates yield counters
* 
* This might be replaced with database logging
*******************************************************************************/
void UIUtil_UpdateYield (int PanelHandle, int CtrlHandle)
{
	char numPass[64] = {0};
	char numFail[64] = {0};
	
	if (PanelHandle==gMainWindow.execpanel)
	{
		if (CtrlHandle==EXEC_PANEL_LT_PASS || CtrlHandle==EXEC_PANEL_LT_FAIL)
		{
			/* 20210520Biye: is there a better way to generalize this calculation? */
			GetCtrlVal (PanelHandle, EXEC_PANEL_LT_PASS, numPass);
			GetCtrlVal (PanelHandle, EXEC_PANEL_LT_FAIL, numFail);
			UIUtil_GetYield (PanelHandle, atoi(numPass), atoi(numFail), EXEC_PANEL_LT_YIELD);
		}
		else if (CtrlHandle==EXEC_PANEL_ST_PASS || CtrlHandle==EXEC_PANEL_ST_FAIL)
		{
			GetCtrlVal (PanelHandle, EXEC_PANEL_ST_PASS, numPass);
			GetCtrlVal (PanelHandle, EXEC_PANEL_ST_FAIL, numFail);
			UIUtil_GetYield (PanelHandle, atoi(numPass), atoi(numFail), EXEC_PANEL_ST_YIELD);
		}
	}
	else
	{
		if (CtrlHandle==NEST_PANEL_NEST_PASS || CtrlHandle==NEST_PANEL_NEST_FAIL)
		{
			GetCtrlVal (PanelHandle, NEST_PANEL_NEST_PASS, numPass);
			GetCtrlVal (PanelHandle, NEST_PANEL_NEST_FAIL, numFail);
			UIUtil_GetYield (PanelHandle, atoi(numPass), atoi(numFail), NEST_PANEL_NEST_YIELD);
		}
	}
}

/***************************************************************************//*!
* \brief Calculates the yield value
*******************************************************************************/
void UIUtil_GetYield (int PanelHandle, int NumPass, int NumFail, int Ctrl)
{
	double sum = 0.0;
	double yield = 0.0;
	char yieldStr[8] = {0};
	
	sum = NumPass + NumFail + 0.0;
	yield = NumPass/(sum==0.0?1.0:sum)*100;
	sprintf (yieldStr, "%0.2lf", yield);
	SetCtrlVal (PanelHandle, Ctrl, yieldStr);
}
//! \cond
/// REGION END
//! \endcond