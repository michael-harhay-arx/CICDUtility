/***************************************************************************//*!
* \file DBLog_LIB.h
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/11/2024 10:38:29 AM
*******************************************************************************/

#ifndef __DBLog_LIB_H__
#define __DBLog_LIB_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include <stdbool.h>
#include "sqlite3.h"
#include "ArxtronToolslib.h"
#include "WriteLog_LIB.h"

//==============================================================================
// Constants

#define MAX_DB_FILE_SIZE 		(5*1024*1024) /*!< 5MB */

//==============================================================================
// Types

/*!< Exposed as a handle, sqlite3 should not be used as a pointer */
typedef sqlite3 *DBHandle;

/*!< \struct NonTestStruct 
	\brief Struct as per NonTestStruct type in DBSeq 
*/
typedef struct {
	char 	Label[TESTSTRUCTTEXTSIZE];
	int 	NonTestTimes;
	char 	NonTestNotes[TESTSTRUCTTEXTSIZE];
} NonTestStruct;
		
//==============================================================================
// Global vaiables

//==============================================================================
// External variables

//==============================================================================
// Global functions

int DBLog_LIB_CheckVersionCompatibility (IN int ExpectedVersionMajor, IN int ExpectedVersionMinor, int *VersionMajor, int *VersionMinor);

/* DBLog */
int Initialize_DBLog_LIB (IN char *TemplateDir, IN char *ConfigDir, IN bool LogSQL, OPT IN char *SQLLogFilePath, char errmsg[ERRLEN]);
int DBLog_Uninitialize (IN DBHandle DBRef, char errmsg[ERRLEN]);

int DBLog_Open (DBHandle *DBRef, IN char *DBDir, IN char *DBName, IN int MaxDBSize, IN char *TableName, char errmsg[ERRLEN]);
int DBLog_LogResults (IN DBHandle DBRef, IN char *TableName, IN TestStruct *TestInfo, IN int NumTestInfo, IN int TotalTestTime, IN char *FixtureID, IN int NestNumber, OPT IN char *SerialNumber, IN bool TestSequenceFailed, char errmsg[ERRLEN]);
int DBLog_NonTestTimes (IN DBHandle DBRef, IN char *TableName, IN NonTestStruct *NonTestInfo, IN int NumNonTestInfo, IN int BatchNumber, IN int NestNumber, OPT IN char *SerialNumber, char errmsg[ERRLEN]);
int DBLog_SystemLog (IN DBHandle DBRef, IN char *TableName, IN char *SystemMessage, char errmsg[ERRLEN]);

/* FixtureCounter */
int DBLog_InitializeFixtureCounter (IN char *ProjectFixtureID, IN int TestCountLimit, IN bool LogSQL, OPT IN char *SQLLogFilePath, char errmsg[ERRLEN]);
int DBLog_UninitializeFixtureCounter (IN DBHandle DBRef, char errmsg[ERRLEN]);

int DBLog_OpenFixtureCounter (DBHandle *DBRef, IN char *FixtureIDs, IN int NumFixtureIDs, IN int FixtureIDBufSize, char errmsg[ERRLEN]);
int DBLog_GetFixtureCounter (IN DBHandle DBRef, IN int NestNum, int *PassCounter, int *FailCounter, char errmsg[ERRLEN]);
int DBLog_UpdateFixtureCounter (IN DBHandle DBRef, IN int NestNum, IN int PassCounter, IN int FailCounter, char errmsg[ERRLEN]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __DBLog_LIB_H__ */