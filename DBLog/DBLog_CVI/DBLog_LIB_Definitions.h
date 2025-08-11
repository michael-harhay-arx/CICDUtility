/***************************************************************************//*!
* \file DBLog_LIB_Definitions.h
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/11/2024 10:38:29 AM
* \brief Used to store all private (Library specific) definitions, such as error
* 	codes.
*******************************************************************************/

#ifndef __DBLog_LIB_Definitions_H__
#define __DBLog_LIB_Definitions_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
		
#include "DBLog_LIB.h"

//==============================================================================
// Constants

#define MAX_SQL_STATEMENT_LENGTH 	(256*1024)	/*!< 128KB TODO find actual max */
#define SHORT_SQL_STATEMENT_LENGTH 	(1024)		/*!< 1KB (Use when sql statement is guaranteed to be small
													e.g. SELECT * FROM TableName WHERE Something = Something;) */

/***************************************************************************//*!
* Error checking for sqlite3 functions
*******************************************************************************/
#define sqlErrChk(fnCall, ...)\
	if (error = fnCall, (error != SQLITE_OK) && (error != SQLITE_ROW) && (error != SQLITE_DONE))\
	{\
		char sqlMsg[ERRLEN] = {0};\
		strncpy (sqlMsg, sqlite3_errstr (error), ERRLEN-1);\
		sqlMsg[ERRLEN-1] = 0;\
		tsErrChk (error, sqlMsg);\
	}
		
#define LOG_SQL_STATEMENT(__statement)\
	if (glbLogFp)\
	{\
		fwrite (fullSQLStatement, 1, strlen (fullSQLStatement), glbLogFp);\
		fflush (glbLogFp);/* Flush every time? */\
	}

//==============================================================================
// Types

/***************************************************************************//*!
* Structure used for SQL query results
*******************************************************************************/
typedef struct {
	int 	recordCount;
	int 	resultCount;
	char 	**values;
} QueryResult;

/***************************************************************************//*!
* TODO dynamic allocation or SHORT_SQL_STATEMENT_LENGTH
*******************************************************************************/
typedef struct {
	char	PassFail			[SHORT_SQL_STATEMENT_LENGTH];
	char	TestName			[SHORT_SQL_STATEMENT_LENGTH];
	char	TestNum				[SHORT_SQL_STATEMENT_LENGTH];
	char	TestVal				[SHORT_SQL_STATEMENT_LENGTH];
	//char	TestValUnit			[SHORT_SQL_STATEMENT_LENGTH];
	char	TestTime			[SHORT_SQL_STATEMENT_LENGTH];
	char	TestLimMin			[SHORT_SQL_STATEMENT_LENGTH];
	char	TestLimMax			[SHORT_SQL_STATEMENT_LENGTH];
	char	TestAdditionalNotes	[SHORT_SQL_STATEMENT_LENGTH];
} TestStepsSQLStrings;

/***************************************************************************//*!
* TODO dynamic allocation or SHORT_SQL_STATEMENT_LENGTH
*******************************************************************************/
typedef struct {
	char	Label				[SHORT_SQL_STATEMENT_LENGTH];
	char	NonTestNotes		[SHORT_SQL_STATEMENT_LENGTH];
	char	NonTestTimes		[SHORT_SQL_STATEMENT_LENGTH];
} NonTestStepsSQLStrings;

//==============================================================================
// External variables

//==============================================================================
// Global functions

int DBLog_GenerateSQLStatementBasedOnTemplate (IN DBHandle DBRef, IN char *ReplaceElements, IN int NumReplaceElements, IN int ReplaceElementsBufSize, IN char *TemplateDir, IN char *TemplateName, IN char *TableName, char errmsg[ERRLEN]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __DBLog_LIB_Definitions_H__ */
