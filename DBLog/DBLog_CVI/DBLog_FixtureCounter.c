/***************************************************************************//*!
* \file DBLog_FixtureCounter.c
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/14/2024 3:25:44 PM
* \brief A short description.
* 
* Fixture Counter functions
* 
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "inifile.h"
#include <formatio.h>
#include "toolbox.h"
#include <ansi_c.h>
#include <utility.h>
#include "ArxUtils_LIB.h"
#include "DBLog_LIB.h"
#include "DBLog_LIB_Definitions.h"
#include "DBLog_LIB_Helper.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static FILE *glbLogFp = NULL;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

extern int libInitialized;

extern char glbTemplateDir[MAX_PATHNAME_LEN];
extern char glbConfigDir[MAX_PATHNAME_LEN];

char glbFixtureID[TESTSTRUCTTEXTSIZE] = {0};
int glbTestCountLimit = 0;

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);

int DBLog_AddFixtureIDs (DBHandle DBRef, IN char *FixtureIDs, IN int NumFixtureIDs, IN int FixtureIDBufSize, char errmsg[ERRLEN]);

//! \cond
/// REGION END
/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief 
*******************************************************************************/
int DBLog_InitializeFixtureCounter (
	IN char *ProjectFixtureID, IN int TestCountLimit, 
	IN bool LogSQL, OPT IN char *SQLLogFilePath, 
	char errmsg[ERRLEN])
{
	libInit;

	/* Put initialization code here */
	if (LogSQL)
	{
		glbLogFp = fopen (SQLLogFilePath, "w");
		tsErrChk (glbLogFp==NULL, "Failed to open log file");
	}
	
	/* Get "StationGlobals.Project.FixtureID" and "StationGlobals.Project.TestCountLimit" 
		(pass from StationConfig.ini?) */
	sprintf (glbFixtureID, ProjectFixtureID);
	glbTestCountLimit = TestCountLimit;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int DBLog_UninitializeFixtureCounter (IN DBHandle DBRef, char errmsg[ERRLEN])
{
	libInit;
	
	sqlErrChk (sqlite3_close_v2 (DBRef));
	
	if (glbLogFp) fclose (glbLogFp);
	glbLogFp = NULL;
	
	memset (glbFixtureID, 0, TESTSTRUCTTEXTSIZE);
	glbTestCountLimit = 0;
	
	error = 0;
Error:
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_AddFixtureIDs (
	DBHandle DBRef, 
	IN char *FixtureIDs, 
	IN int NumFixtureIDs, 
	IN int FixtureIDBufSize, 
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	
	fnInit;
	
	char fullSQLStatement[SHORT_SQL_STATEMENT_LENGTH] = \
			"BEGIN TRANSACTION;\n"
			"INSERT INTO FixtureCounter ('FixtureID','CountLimit') VALUES\n";
	char tmpStr[SHORT_SQL_STATEMENT_LENGTH] = {0};
	
	for (int i = 0; i < NumFixtureIDs; i++)
	{
		strcat (fullSQLStatement, "('");	
		strcat (fullSQLStatement, FixtureIDs+(i*FixtureIDBufSize));
		sprintf (tmpStr, "', %d)", glbTestCountLimit);
		strcat (fullSQLStatement, tmpStr);
		
		if (i < NumFixtureIDs-1)
			strcat (fullSQLStatement, ",");	
	}
	strcat (fullSQLStatement, ";\nEND;\n");	
	
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg);
	
Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_OpenFixtureCounter (
	DBHandle *DBRef, 
	IN char *FixtureIDs, 
	IN int NumFixtureIDs, 
	IN int FixtureIDBufSize, 
	char errmsg[ERRLEN])
{
	fnInit;
	
	/* Check DB File exists */// TODO this is almost duplicate from DBLog_Open
	char dbFilePath[MAX_PATHNAME_LEN] = "C:\\Arxtron\\Logs\\FixtureCounter.db";
	int fileExists = FileExists (dbFilePath, NULL);
	tsErrChk (fileExists<0?fileExists:0, "Failed to verify file");

	sqlErrChk (sqlite3_open_v2 (dbFilePath, DBRef, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE/*TODO allow others?(notably mutex options)*/, NULL), errmsg);

	if (!fileExists) /* Assumes file was setup correctly if exists */
	{
		tsErrChk (DBLog_GenerateSQLStatementBasedOnTemplate (*DBRef, NULL, 0, 0, glbTemplateDir, "FixtureCounter", NULL, errmsg), errmsg);
		tsErrChk (DBLog_AddFixtureIDs (*DBRef, FixtureIDs, NumFixtureIDs, FixtureIDBufSize, errmsg), errmsg);
	}
	
Error:
	/* Close database on failure */
	if (error) sqlite3_close (*DBRef);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_GetFixtureCounter (
	IN DBHandle DBRef, 
	IN int NestNum, 
	int *PassCounter, 
	int *FailCounter, 
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	QueryResult result = { .recordCount = 0, .resultCount = 0, .values = NULL};

	libInit;
	
	char fullSQLStatement[SHORT_SQL_STATEMENT_LENGTH] = {0};
	
	sprintf (fullSQLStatement, 
			 "SELECT PassCount%d, FailCount%d, FixtureID FROM FixtureCounter WHERE FixtureID = '%s';\n",
			NestNum, NestNum, glbFixtureID);
	
	memset (&result, 0, sizeof (result));
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, SQLReadCallback, &result, &dbErrmsg), dbErrmsg);
	
	if (result.recordCount == 0)
	{
		*PassCounter = 0;
		*FailCounter = 0;
	}
	else
	{
		*PassCounter = atoi(result.values[0]);
		*FailCounter = atoi(result.values[1]);
	}
	
Error:
	if (result.values) SQLFreeResults (&result);
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_UpdateFixtureCounter (
	IN DBHandle DBRef, 
	IN int NestNum, 
	IN int PassCounter, 
	IN int FailCounter, 
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;

	libInit;
	
	int currPassCounter = 0;
	int currFailCounter = 0;
	tsErrChk (DBLog_GetFixtureCounter (DBRef, NestNum, &currPassCounter, &currFailCounter, errmsg), errmsg);
	
	char fullSQLStatement[SHORT_SQL_STATEMENT_LENGTH] = {0};
	
	sprintf (fullSQLStatement, 
			 "BEGIN TRANSACTION;\n"
			 "UPDATE FixtureCounter\n"
			 	"SET PassCount%d = %d, FailCount%d = %d\n"
				"WHERE FixtureID = '%s';\n"
			 "END;\n",
			NestNum, currPassCounter+PassCounter, NestNum, currFailCounter+FailCounter, glbFixtureID);
	
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg);

Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

//! \cond
/// REGION END
//! \endcond

