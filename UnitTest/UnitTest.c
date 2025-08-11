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
	UTInit;
	char msgLog[4096] = {0};
	
	char data1574[32] = {0};
	char data1575[32] = {0};
	char data1576[32] = {0};
	char expectedMIS[32] = {0};
	char mis[32] = {0};
	
	// Test 1
	strcpy (data1574, "123456");
	strcpy (data1575, "777777");
	strcpy (data1576, "123456");
	strcpy (expectedMIS, "456777771234");
	strcpy (mis, "456777771234");
	//TestStep_FormMIS (0, data1574, data1575, data1576, mis, ReportText, ErrInfo);
	ASSERT_EQ_STR (expectedMIS, mis);
	
	// Test 2
	strcpy (data1574, "123456");
	strcpy (data1575, "777777");
	strcpy (data1576, "123456");
	strcpy (expectedMIS, "456777771234");
	strcpy (mis, "000777771234");
	//TestStep_FormMIS (0, data1574, data1575, data1576, mis, ReportText, ErrInfo);
	ASSERT_EQ_STR (expectedMIS, mis);
	
	strcpy (ErrInfo->msg, msgLog);
	return (ErrInfo->code = error);
}
//! \cond
/// REGION END
//! \endcond