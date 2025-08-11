/***************************************************************************//*!
* \file UnitTest.c
* \author 
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

/// REGION START General Unit Tests
//! \endcond
/***************************************************************************//*!
* Unit test CVI version
*******************************************************************************/
int UnitTest_CVIVersion (tsErrorDataType *ErrInfo)
{
	UTInit;
	char msgLog[4096] = {0};
	
	char version[16] = {0};
	GetCVIVersionYear (version);
	ASSERT_EQ_STR ("2019", version);
	
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
	
	char mis[32] = {0}; // for now, remove later
	strcpy (mis, "446777771234");
		
	// Run tests
	int numTests = 2;
	for (int testIdx = 0; testIdx < numTests; testIdx++)
	{
		//TestStep_FormMIS (0, testVals[testIdx][0], testVals[testIdx][1], testVals[testIdx][2], mis, ReportText, ErrInfo);
		ASSERT_EQ_STR (testVals[testIdx][3], mis);
	}
	
	// Log results	
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}
//! \cond
/// REGION END
//! \endcond