/***************************************************************************//*!
* \file UnitTest.c
* \author Michael Harhay
* \copyright . All Rights Reserved.
* \date 2025-08-08 3:39:49 PM
* \brief A short description.
* 
* A longer description.
* 
* This Library was created using
* * Template version 1.1.3
* * ArxtronToolslib version 1.5.0
* PLEASE ONLY UPDATE THE VERSION NUMBERS WHEN UPDATING THE TEMPLATE OR RELATED TOOLSLIB
* THESE VERSION NUMBERS SHOULD BE SEPARATE FROM THE LIBRARY VERSION
* THE ALL CAPS PORTION OF THIS COMMENT SECTION CAN BE REMOVED AFTER CREATING THE LIBRARY
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "UnitTest.h"

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

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START General Tests
//! \endcond
/***************************************************************************//*!
* Test CVI version
*******************************************************************************/
int Test_WindowsVersion (tsErrorDataType *ErrInfo)
{
	UTInit;
	char msgLog[4096] = {0};
	
	// Set up Windows API
	typedef LONG (WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
	
	// Check Windows version by build number
	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (!hMod) return 0;
    RtlGetVersionPtr fxPtr = (RtlGetVersionPtr) GetProcAddress (hMod, "RtlGetVersion");
    if (!fxPtr) return 0;

    RTL_OSVERSIONINFOW rovi = {0};
    rovi.dwOSVersionInfoSize = sizeof (rovi);
    if (fxPtr (&rovi) != 0) return 0;
	
	ASSERT_EQ_INT (10, rovi.dwMajorVersion); // Check that major version is 10
	ASSERT_GTE_INT (rovi.dwBuildNumber, 22000); // Check that build number is at least 22000 (denoting windows 11)
	
	// Check Windows edition
	DWORD productType = 0;
    if (!GetProductInfo (rovi.dwMajorVersion, rovi.dwMinorVersion, rovi.dwBuildNumber, 0, &productType)) return 0;

    ASSERT_EQ_INT (PRODUCT_PROFESSIONAL, productType); // Check that Windows Pro is running
	
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}

/***************************************************************************//*!
* Test CVI version
*******************************************************************************/
int Test_CVIVersion (tsErrorDataType *ErrInfo)
{
	UTInit;
	char msgLog[4096] = {0};
	
	char version[16] = {0};
	GetCVIVersionYear (version);
	ASSERT_EQ_STR ("2019", version);
	
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}

/***************************************************************************//*!
* Test EStop
*******************************************************************************/
int Test_EStopInjection (CAObjHandle seqContext, tsErrorDataType *ErrInfo) // TODO: Fix
{
	UTInit;
	char msgLog[4096] = {0};
	ERRORINFO errInfo = {0};
	
	// Set EStop to high
	VBOOL eStopHigh = 0;
	TS_PropertyGetValBoolean (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, &eStopHigh);
	if (!eStopHigh)
	{
		eStopHigh = -1;
		TS_PropertySetValBoolean (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, eStopHigh);
	}
	
	// TODO: Check if sequence has stopped
	int continueFlag = 0;
	//TS_PropertyGetValNumber (seqContext, &errInfo, "StationGlobals.GlobalAlarmConditions.EStopHigh", 0, continueFlag);
	ASSERT_EQ_INT (0, continueFlag)
	
		
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}

/***************************************************************************//*!
* Test 
*******************************************************************************/
int Test_SequenceFailHandler (CAObjHandle seqContext, tsErrorDataType *ErrInfo)
{
	UTInit;
	char msgLog[4096] = {0};
	
	// Set FAIL_PRESSED to true
	VBOOL failPressed = 0;
	TS_PropertyGetValBoolean (seqContext, &errInfo, "Locals.FAIL_PRESSED", 0, &failPressed);
	if (!failPressed)
	{
		failPressed = -1;
		TS_PropertySetValBoolean (seqContext, &errInfo, "Locals.FAIL_PRESSED", 0, failPressed);
	}
	
	// Check if FAIL_PRESSED worked
	
		
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}
//! \cond
/// REGION END

/// REGION START Project Specific Unit Tests
//! \endcond
/***************************************************************************//*!
* Unit test for TestStep_FormMIS()
*******************************************************************************/
int UnitTest_FormMIS (tsErrorDataType *ErrInfo)
{
	// Initialize unit test
	UTInit;
	char msgLog[4096] = {0};
	
	// Set test params and expected outputs
	char testVals[2][4][32] = // numTests = 2, numParams = 4, string size = 32
	{
		{"123456", "777777", "123456", "456777771234"}, // format: data1574, data1575, data1576, expected value
        {"123456", "777777", "123456", "000000000000"}
	};
	
	// Run tests
	char mis[32] = {0}; 
	strcpy (mis, "456777771234"); // remove later
	
	int numTests = sizeof (testVals) / sizeof (testVals[0]);
	for (int testIdx = 0; testIdx < numTests; testIdx++)
	{
		//TestStep_FormMIS (0, testVals[testIdx][0], testVals[testIdx][1], testVals[testIdx][2], mis, ErrInfo->msg, ErrInfo);
		ASSERT_EQ_STR (testVals[testIdx][3], mis);
	}
	
	// Log results	
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}
//! \cond
/// REGION END
//! \endcond