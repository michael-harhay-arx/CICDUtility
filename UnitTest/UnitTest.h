/***************************************************************************//*!
* \file UnitTest.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-08-08 3:39:49 PM
*******************************************************************************/

#ifndef __UnitTest_H__
#define __UnitTest_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "tsutil.h"
#include <ansi_c.h>
#include <utility.h>
#include <assert.h>
#include "cvidef.h"
#include "ArxtronToolslib.h"
#include "TestSteps.h"

//==============================================================================
// Constants

/***************************************************************************//*!
* Initialization macro for unit tests
*******************************************************************************/
#define UTInit \
	char msg[256] = {0};
	int error = 0;
		
/***************************************************************************//*!
* Assert equality of integer values
*******************************************************************************/
#define ASSERT_EQ_INT(expected, actual) \
	memset (msg, 0, 256); \
    do \
	{ \
        if ((expected) != (actual)) \
		{ \
            sprintf (msg, "FAIL: %s (line %d): expected %d, got %d\n", __func__, __LINE__, (expected), (actual)); \
			strcat (msgLog, msg); \
            error = 1; \
        } \
		else \
		{ \
			sprintf (msg, "PASS: %s (line %d): expected %d, got %d\n", __func__, __LINE__, (expected), (actual)); \
			strcat (msgLog, msg); \
		} \
    } while (0)

/***************************************************************************//*!
* Assert equality of strings
*******************************************************************************/
#define ASSERT_EQ_STR(expected, actual) \
	memset (msg, 0, 256); \
    do \
	{ \
        if (strcmp (expected, actual) != 0) \
		{ \
            sprintf (msg, "FAIL: %s (line %d): expected %s, got %s\n", __func__, __LINE__, (expected), (actual)); \
			strcat (msgLog, msg); \
            error = 1; \
        } \
		else \
		{ \
			sprintf (msg, "PASS: %s (line %d): expected %s, got %s\n", __func__, __LINE__, (expected), (actual)); \
			strcat (msgLog, msg); \
		} \
    } while (0)

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int UnitTest_FormMIS (tsErrorDataType *ErrInfo);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UnitTest_H__ */
