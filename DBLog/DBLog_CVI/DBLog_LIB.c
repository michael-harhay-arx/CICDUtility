/***************************************************************************//*!
* \file DBLog_LIB.c
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/11/2024 10:38:29 AM
* \brief A short description.
* 
* A longer description.
* 
* This Library was created using
* * Template version 2.0.3
* * ArxtronToolslib version 1.3.6
* * DebugToolslib version 0.0.0
* * GUIToolslib version 0.0.0
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

#define ArchiveDBFile(dbDir, dbName)\
{\
	char archiveFile[128] = {0};\
	int year, month, day, hour, min, sec;\
	GetSystemDate (&month, &day, &year);\
	GetSystemTime (&hour, &min, &sec);\
	\
	sprintf (archiveFile, "%s\\%s_%04d%02d%02d%02d%02d%02d.db", dbDir, dbName, year, month, day, hour, min, sec);\
	tsErrChk (RenameFile (dbFilePath, archiveFile), "Failed to archive file");\
}

//==============================================================================
// Types

//==============================================================================
// Static global variables

static FILE *glbLogFp = NULL;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

int libInitialized = 0;

char glbTemplateDir[MAX_PATHNAME_LEN] = "C:\\Arxtron\\DBLog\\Templates"; /*!< default path */
char glbConfigDir[MAX_PATHNAME_LEN] = {0};

int glbAsciiIFilter[6][2] = 
{
	{48, 57},
	{65, 90},
	{97, 122},
	{32, 32},
	{95, 95},
	{45, 45}
};
int glbNumAsciiRanges = 6;

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief 
*******************************************************************************/
int Initialize_DBLog_LIB (
	IN char *TemplateDir, IN char *ConfigDir, 
	IN bool LogSQL, OPT IN char *SQLLogFilePath, 
	char errmsg[ERRLEN])
{
	fnInit;
	
	// Uncomment if depending on another lib, then update [baselibname] with actual library name (without quotes, Ex. Ini_LIB)
	// Make additional copies if depending on multiple libs
	//DEPENDENCY_VERSION_CHECK([baselibname],ExpectedVersionMajor,ExpectedVersionMinor,error,errmsg);
	
	/* Put initialization code here */
	strcpy (glbTemplateDir, TemplateDir);
	strcpy (glbConfigDir, ConfigDir);
	
	if (LogSQL)
	{
		glbLogFp = fopen (SQLLogFilePath, "w");
		tsErrChk (glbLogFp==NULL, "Failed to open log file");
	}
	
	libInitialized = 1;
	
	/* error will be -99999 from fnInit if no calls to any of the ErrChk macros are made */
	error = 0;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_Uninitialize (IN DBHandle DBRef, char errmsg[ERRLEN])
{
	libInit;
	
	sqlErrChk (sqlite3_close_v2 (DBRef));
	
	if (glbLogFp) fclose (glbLogFp);
	glbLogFp = NULL;
	
	libInitialized = 0;
	
	error = 0;
Error:
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_Fn (char errmsg[ERRLEN])
{
	libInit;
	
	
Error:
	return error;
}

//! \cond
/// REGION START Helpers
//! \endcond

/***************************************************************************//*!
* \brief Template for copy pasting
*
* From DB Get Sequence Names
*
*******************************************************************************/
int DBLog_GetTestNames (
	char *TestStepNames, 
	IN int MaxNumTestStepNames, 
	IN int TestStepNamesBufSize, 
	int *NumTestSteps,
	char errmsg[ERRLEN])
{
	fnInit;
	
	char tempStr[SHORT_SQL_STATEMENT_LENGTH] = {0};
	char testStepsFilePath[MAX_PATHNAME_LEN] = {0};
	
	/* Parse TestSteps.ini */
	sprintf (testStepsFilePath, "%s\\TestSteps.ini", glbConfigDir);
	IniText iniHandle = Ini_New(0);
	tsErrChk (Ini_ReadFromFile (iniHandle, testStepsFilePath), "Failed to load TestSteps.ini file");
	
	char *currTestStepNameOut = NULL;
	char *currTestStepNameIn = NULL;
	
	*NumTestSteps = Ini_NumberOfSections (iniHandle) - 1;
	for (int i = 1/*Skip Template section*/; i <= *NumTestSteps; i++)
	{
		currTestStepNameOut = TestStepNames+((i-1)*TestStepNamesBufSize);
		
		Ini_NthSectionName (iniHandle, i+1, &currTestStepNameIn);
		strcpy (currTestStepNameOut, currTestStepNameIn);
		
		/* Remove invalid characters */
		int n = strlen (currTestStepNameOut);
		ArxUtils_FilterStringByAscii (currTestStepNameOut, &n, glbAsciiIFilter, &glbNumAsciiRanges, tempStr);
		ArxUtils_ReplaceChar (tempStr, " ", "_", currTestStepNameOut);
	}
	error = 0;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_InsertNames (
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN char *ColName, 
	IN /*char **?*/char *TestStepNames, 
	IN int NumTestStepNames, 
	IN int TestStepNamesBufSize, 
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	fnInit;
	
	char fullSQLStatement[MAX_SQL_STATEMENT_LENGTH] = {0};
	char testStepNames[MAX_SQL_STATEMENT_LENGTH] = {0};
	
	for (int i = 0; i < NumTestStepNames; i++)
	{
		strcat (testStepNames, "(\"");
		strcat (testStepNames, TestStepNames+(TestStepNamesBufSize*i));
		if (i < NumTestStepNames-1)
			strcat (testStepNames, "\"),");
		else
			strcat (testStepNames, "\")");
	}

	sprintf (fullSQLStatement, 
				"\nBEGIN TRANSACTION;\n"
				"INSERT OR ROLLBACK INTO [%s]([%s])\nVALUES"
				"%s;\n"/*TestStepNames*/
				"END;\n"
			 , TableName, ColName, testStepNames);
	
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, /*No return*/NULL, NULL, &dbErrmsg), dbErrmsg);
	
Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_InsertTestNames (
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN char *TestStepNames, 
	IN int NumTestStepNames, 
	IN int TestStepNamesBufSize, 
	char errmsg[ERRLEN])
{
	fnInit;

	char tempStr[TESTSTRUCTTEXTSIZE] = {0};
	sprintf (tempStr, "%s_TestSteps", TableName);
	
	tsErrChk (DBLog_InsertNames (DBRef, tempStr, "TestStepName", TestStepNames, NumTestStepNames, TestStepNamesBufSize, errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_InsertNonTestNames (	
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN char *NonTestStepNames, 
	IN int NumNonTestStepNames, 
	IN int NonTestStepNamesBufSize, 
	char errmsg[ERRLEN])
{
	fnInit;
	
	char tempStr[TESTSTRUCTTEXTSIZE] = {0};
	sprintf (tempStr, "%s_NonTestSteps", TableName);
	
	tsErrChk (DBLog_InsertNames (DBRef, tempStr, "NonTestStepName", NonTestStepNames, NumNonTestStepNames, NonTestStepNamesBufSize, errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Build a statement from a .sql file
*
* Expects prefix:  
*	"-- [Replace Here] "
* for each line to be replaced
*
*******************************************************************************/
int DBLog_GenerateSQLStatementBasedOnTemplate (
	IN DBHandle DBRef,
	IN char *ReplaceElements,
	IN int NumReplaceElements, 
	IN int ReplaceElementsBufSize,
	IN char *TemplateDir,
	IN char *TemplateName,
	IN char *TableName,
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	char *line = NULL;
	char *tempLineBuffer = NULL;
	fnInit;
	
	char fullSQLStatement[MAX_SQL_STATEMENT_LENGTH] = {0};
	char templateFilePath[MAX_PATHNAME_LEN] = {0};
	
	/* Validate file */
	sprintf (templateFilePath, "%s\\%s.sql", TemplateDir, TemplateName);
	int fileSize = 0;
	int fileExists = FileExists (templateFilePath, &fileSize);
	tsErrChk (fileExists<0?fileExists:0, "Failed to verify file");
	tsErrChk (fileSize >= MAX_SQL_STATEMENT_LENGTH, "Template file too long");
	
	FILE *fp = NULL;
	fp = fopen (templateFilePath, "r");
	tsErrChk (!fp, "Failed to open file");
	
	{
		/* Parse File (fgets) */
		line = calloc (MAX_SQL_STATEMENT_LENGTH, 1);
		tempLineBuffer = calloc (MAX_SQL_STATEMENT_LENGTH, 1);
		char replaceLineBuffer[1024] = {0};
		int patternIndex = 0;
		char patternPattern[] = "-- [Replace Here]\t>";
		int patternLen = strlen (patternPattern);
		while (fgets (line, MAX_SQL_STATEMENT_LENGTH, fp))
		{
			/* Find target replace strings ('-- [Replace Here]')*/
			patternIndex = FindPattern (line, 0, -1, patternPattern, 1, 0);
			if (patternIndex >= 0)
			{
				/* Replace last " */
				line[strlen(line)-2] = '\n';
				line[strlen(line)-1] = 0;
				
				char *removeCharsStr = strtok (line+patternIndex+patternLen, ">");
				int removeChars = atoi (removeCharsStr);
				int removeCharsLen = strlen (removeCharsStr);
				
				char *currTestName = NULL;
				for (int i = 0; i < NumReplaceElements; i++)
				{
					currTestName = ReplaceElements+(i*ReplaceElementsBufSize);

					ReplaceTokenInString ("\"+Locals.CurrentElement+\"", currTestName, line+patternIndex+patternLen+2+removeCharsLen, tempLineBuffer);				
					
					UnescapeString (tempLineBuffer, replaceLineBuffer);
					
					/* Replace last chars */
					if (i == NumReplaceElements-1 && removeChars)
					{
						int lastCommaIndex = strlen (replaceLineBuffer);
						replaceLineBuffer[lastCommaIndex-removeChars-1] = '\n';
						replaceLineBuffer[lastCommaIndex-removeChars] = 0;
					}
					
					strcat (fullSQLStatement, replaceLineBuffer);
				}
			}
			else		
				strcat (fullSQLStatement, line);
		}
		
		/* Replace table names */
		ReplaceTokenInString ("TableName", TableName, fullSQLStatement, tempLineBuffer);
		memcpy (fullSQLStatement, tempLineBuffer, MAX_SQL_STATEMENT_LENGTH);
	}
	
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, /*TODO generalize for returns?*/NULL, NULL, &dbErrmsg), dbErrmsg);
	
Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	if (line) free (line);
	if (tempLineBuffer) free (tempLineBuffer);
	return error;
}

/***************************************************************************//*!
* \brief Initialize an open database 
*******************************************************************************/
int DBLog_InitializeDB (
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN /*char **?*/char *TestStepNames, 
	IN int NumTestStepNames, 
	IN int TestStepNamesBufSize, 
	IN bool AlterTables,
	char errmsg[ERRLEN])
{
	fnInit;
	
	/* Create tables */
	tsErrChk (DBLog_GenerateSQLStatementBasedOnTemplate (DBRef, TestStepNames, NumTestStepNames, TestStepNamesBufSize, glbTemplateDir, AlterTables?"AlterTables":"CreateTables", TableName, errmsg), errmsg);
	
	/* Create NonTestTimes Tables */
	tsErrChk (DBLog_GenerateSQLStatementBasedOnTemplate (DBRef, NULL, 0, 0, glbTemplateDir, AlterTables?"AlterTables_NonTestTimes":"CreateTables_NonTestTimes", TableName, errmsg), errmsg);
	
	/* Insert Test Names */
	tsErrChk (DBLog_InsertTestNames (DBRef, TableName, TestStepNames, NumTestStepNames, TestStepNamesBufSize, errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Check test names in DB yet or not
*******************************************************************************/
int DBLog_CheckNames (
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN char *ColName,
	IN char *Names, 
	IN int NumNames, 
	IN int NamesBufSize,
	bool *NamesInTable, 
	char *NewNames, /*Should be the same dimensions as Names*/
	int *NumNewNames,
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	QueryResult result = { .recordCount = 0, .resultCount = 0, .values = NULL};
	fnInit;
	
	char fullSQLStatement[SHORT_SQL_STATEMENT_LENGTH] = {0};
	
	sprintf (fullSQLStatement, 
				"\nSELECT %s FROM [%s];\n"
			 , ColName, TableName);
	
	LOG_SQL_STATEMENT (fullSQLStatement);
	memset (&result, 0, sizeof (result));
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, SQLReadCallback, &result, &dbErrmsg), dbErrmsg);

	*NamesInTable = true;
	if (NumNames > result.recordCount)
		*NamesInTable = false;
	
	int newNamesInd = 0;
	for (int i = 0; i < NumNames; i++)
	{
		bool found = false;
		for (int j = 0; j < result.recordCount; j++)
		{
			if (0==stricmp (Names+(i*NamesBufSize), result.values[j]))
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			strcpy (NewNames+(newNamesInd*NamesBufSize), Names+(i*NamesBufSize));
			++newNamesInd;
		}
		*NamesInTable &= found;
	}
	*NumNewNames = newNamesInd;
	
Error:
	if (result.values) SQLFreeResults (&result);
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_CheckTestNames (
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN char *TestStepNames, 
	IN int NumTestStepNames, 
	IN int TestStepNamesBufSize, 
	bool *TestNamesInTable, 
	char *NewTestStepNames, 
	int *NumNewTestStepNames,
	char errmsg[ERRLEN])
{
	fnInit;
	
	char tempStr[TESTSTRUCTTEXTSIZE] = {0};
	sprintf (tempStr, "%s_TestSteps", TableName);
	
	tsErrChk (DBLog_CheckNames (DBRef, tempStr, "TestStepName", TestStepNames, NumTestStepNames, TestStepNamesBufSize, TestNamesInTable, NewTestStepNames, NumNewTestStepNames, errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_CheckNonTestNames (	
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN char *NonTestStepNames, 
	IN int NumNonTestStepNames, 
	IN int NonTestStepNamesBufSize, 
	bool *NonTestNamesInTable, 
	char *NewNonTestStepNames, 
	int *NumNewNonTestStepNames, 
	char errmsg[ERRLEN])
{
	fnInit;
	
	char tempStr[TESTSTRUCTTEXTSIZE] = {0};
	sprintf (tempStr, "%s_NonTestSteps", TableName);
	
	tsErrChk (DBLog_CheckNames (DBRef, tempStr, "NonTestStepName", NonTestStepNames, NumNonTestStepNames, NonTestStepNamesBufSize, NonTestNamesInTable, NewNonTestStepNames, NumNewNonTestStepNames, errmsg), errmsg);
	
Error:
	return error;
}

//! \cond
/// REGION END Helpers
//! \endcond

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_Open (
	DBHandle *DBRef, 
	IN char *DBDir, 
	IN char *DBName, 
	IN int MaxDBSize, 
	IN char *TableName, 
	char errmsg[ERRLEN])
{
	fnInit;
	
	bool newDB = false;
	bool alterTable = false;
	
	/* Get Sequence Names */
#define MAX_NUM_STEPS 			(128) // TODO
	char testStepNames[MAX_NUM_STEPS][TESTSTRUCTTEXTSIZE];
	char newTestStepNames[MAX_NUM_STEPS][TESTSTRUCTTEXTSIZE];
	memset (testStepNames, 0, MAX_NUM_STEPS*TESTSTRUCTTEXTSIZE);
	memset (newTestStepNames, 0, MAX_NUM_STEPS*TESTSTRUCTTEXTSIZE);
	int numTestSteps = 0;
	int numNewTestSteps = 0;
	tsErrChk (DBLog_GetTestNames ((char *)testStepNames, MAX_NUM_STEPS, TESTSTRUCTTEXTSIZE, &numTestSteps, errmsg), errmsg);
	
	/* Check DB File exists */
	char dbFilePath[MAX_PATHNAME_LEN] = {0};
	int fileSize = 0;
	sprintf (dbFilePath, "%s\\%s.db", DBDir, DBName);
	int fileExists = FileExists (dbFilePath, &fileSize);
	tsErrChk (fileExists<0?fileExists:0, "Failed to verify file");

	if (fileExists)
	{
		/* Archive DB if too large */
		if (fileSize >= MaxDBSize)
		{
			ArchiveDBFile (DBDir, DBName);
			newDB = true;
		}
		else 
		{
			sqlErrChk (sqlite3_open_v2 (dbFilePath, DBRef, SQLITE_OPEN_READWRITE/*TODO allow others?(notably mutex options)*/, NULL), errmsg);
			
			tsErrChk (DBLog_CheckTestNames (*DBRef, TableName, (char *)testStepNames, numTestSteps, TESTSTRUCTTEXTSIZE, &alterTable, (char *)newTestStepNames, &numNewTestSteps, errmsg), errmsg);
			alterTable = !alterTable; /* Confirmation skipped in DBLog.seq */
		}
	} 
	else newDB = true;
	
	if (newDB)
	{
		memcpy (newTestStepNames, testStepNames, MAX_NUM_STEPS*TESTSTRUCTTEXTSIZE);
		numNewTestSteps = numTestSteps;
		
		sqlErrChk (sqlite3_open_v2 (dbFilePath, DBRef, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE/*TODO allow others?*/, NULL), errmsg);
		alterTable = false; // Just to ensure false lol
	}
	
	if (newDB || alterTable)
	{
		tsErrChk (DBLog_InitializeDB (*DBRef, TableName, (char *)newTestStepNames, numNewTestSteps, TESTSTRUCTTEXTSIZE, alterTable, errmsg), errmsg);
	}
	
	/* Create Views*/
	tsErrChk (DBLog_GenerateSQLStatementBasedOnTemplate (*DBRef, (char *)testStepNames, numTestSteps, TESTSTRUCTTEXTSIZE, glbTemplateDir, "CreateViews", TableName, errmsg), errmsg);
	
Error:
	/* Close database on failure */
	if (error) sqlite3_close (*DBRef);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*
* NestNumber is 1 based index
*
*******************************************************************************/
int DBLog_LogResults (
	IN DBHandle DBRef, 
	IN char *TableName, 
	IN TestStruct *TestInfo, 
	IN int NumTestInfo,
	IN int TotalTestTime, 
	IN char *FixtureID, 
	IN int NestNumber, 
	OPT IN char *SerialNumber, 
	IN bool TestSequenceFailed, 
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	TestStruct validTestInfo[NumTestInfo];
	
	libInit;

	char fullSQLStatement[MAX_SQL_STATEMENT_LENGTH] = {0};
	char tempStr[SHORT_SQL_STATEMENT_LENGTH] = {0}; // Used only for intermediate operations (e.g. sprintf numbers)
	
	/* Locals */
	memset (validTestInfo, 0, sizeof(validTestInfo));
	int numValidTestInfo = 0; // Will be modified if duplicates removed
	
	/* Default serial number */
	char serialNumber[32] = {0};
	if (!SerialNumber) sprintf (serialNumber, "NEST%d_%s", NestNumber, DateStr ());
	else sprintf (serialNumber, SerialNumber);
	
	/* Attach serialNumber to table */
	sprintf (fullSQLStatement, 
				"\nBEGIN TRANSACTION;\n"
				"INSERT OR REPLACE INTO %s_UUT_SN([SerialNumber])\nVALUES"
				"(\"%s\");\n"
				"END;\n"
			 , TableName, serialNumber);
	
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg);
	
	TestStepsSQLStrings sqlStrings;
	memset (&sqlStrings, 0, sizeof(sqlStrings));
	char failedTestsString[SHORT_SQL_STATEMENT_LENGTH] = {0};
	
	/* Check for duplicate names (should only occur during manual retries)
 		and remove the first instance (only should record the latest run) */
	for (int j = 0; j < NumTestInfo; j++)
	{
		/* Search all future tests */
		bool foundDuplicate = false;
		for (int k = j+1; k < NumTestInfo; k++)
		{
			if (0 == stricmp (TestInfo[j].TestName, TestInfo[k].TestName))
			{
				foundDuplicate = true;
				break;
			}
		}
		if (foundDuplicate)
		{
			/* Duplicate found */
			/* Ignore first duplicate */
		}
		else 
		{
			/* Remove invalid characters */
			int n = strlen (TestInfo[j].TestName);
			ArxUtils_FilterStringByAscii (TestInfo[j].TestName, &n, glbAsciiIFilter, &glbNumAsciiRanges, tempStr);
			ArxUtils_ReplaceChar (tempStr, " ", "_", TestInfo[j].TestName);
			
			/* Build SQL */
			strcat (sqlStrings.TestName, "[");
			strcat (sqlStrings.TestName, TestInfo[j].TestName);
			strcat (sqlStrings.TestName, "],");
			
			/* Insert into valid */
			memcpy (validTestInfo+numValidTestInfo, TestInfo+j, sizeof(TestStruct));
			++numValidTestInfo;
		}
	}
		
#define BuildAllOthers(...)\
		BuildOther (TestVal);\
		/*BuildOther (TestValUnit);*/\
		BuildOther (TestLimMin);\
		BuildOther (TestLimMax);\
		BuildOther (TestAdditionalNotes)
		
	/* Parse Test Results */
	for (int i = 0; i < numValidTestInfo; i++)
	{
		/* Pass/Fail string */
		if (validTestInfo[i].TestPass == 0)
		{
			strcat (sqlStrings.PassFail, "'FAIL',");
			
			sprintf (tempStr, "%s(%d);", validTestInfo[i].TestName, validTestInfo[i].TestNum);
			strcat (failedTestsString, tempStr);
		}
		else
		{
			strcat (sqlStrings.PassFail, "'PASS',");
		}
		
		/* Other strings */
#define BuildOther(ResultTypeName)\
		strcat (sqlStrings.ResultTypeName, "'");\
		ReplaceTokenInString ("'", "''", validTestInfo[i].ResultTypeName, tempStr);\
		strcat (sqlStrings.ResultTypeName, tempStr);\
		strcat (sqlStrings.ResultTypeName, "',")
		
		BuildAllOthers ();
		{
			/* Time */
			sprintf (tempStr, "'%lf',", validTestInfo[i].TestTime);
			strcat (sqlStrings.TestTime, tempStr);
			
			/* TestNumber */
			sprintf (tempStr, "'%d',", validTestInfo[i].TestNum);
			strcat (sqlStrings.TestNum, tempStr);
		}
#undef BuildOther
	}
	
	/* Add all test infos to database */
	{
		/* Pass/Fail */
		memset (fullSQLStatement, 0, MAX_SQL_STATEMENT_LENGTH);\
		sprintf (fullSQLStatement,
				"\nBEGIN TRANSACTION;\n"
					"INSERT OR ROLLBACK INTO %s_PassFailLog (%s TestTime, FixtureID, NestNum, SerialNumber, SequencePassFail, FailedTests)\n"
					"VALUES (%s %d, '%s', %d, '%s', '%s', '%s');\n"
				"END;\n",
				TableName, sqlStrings.TestName, 
				/*values:*/sqlStrings.PassFail, TotalTestTime, FixtureID, NestNumber, serialNumber, (TestSequenceFailed?"FAIL":"PASS"), failedTestsString);
		LOG_SQL_STATEMENT (fullSQLStatement);
		tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg);
		
		/* Other */
#define BuildOther(ResultTypeName)\
		memset (fullSQLStatement, 0, MAX_SQL_STATEMENT_LENGTH);\
		sprintf (fullSQLStatement,\
				"\nBEGIN TRANSACTION;\n"\
					"INSERT OR ROLLBACK INTO %s_"#ResultTypeName"(%s [SerialNumber])\n"\
					"VALUES (%s '%s');\n"\
				"END;\n",\
				TableName, sqlStrings.TestName, sqlStrings.ResultTypeName, serialNumber);\
		LOG_SQL_STATEMENT (fullSQLStatement);\
		tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg)
		
		BuildAllOthers ();
		BuildOther (TestTime);
		BuildOther (TestNum);
#undef BuildOther
#undef BuildAllOthers
	}
	
Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int DBLog_NonTestTimes (
	IN DBHandle DBRef,
	IN char *TableName,
	IN NonTestStruct *NonTestInfo,
	IN int NumNonTestInfo,
	IN int BatchNumber,
	IN int NestNumber,
	OPT IN char *SerialNumber,
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	
	libInit;
	
	/* Temp vars */
	char currNonTestStepName[TESTSTRUCTTEXTSIZE] = {0};
	char tmpStr[SHORT_SQL_STATEMENT_LENGTH] = {0};
	int tempInt = 1;
	
	/* Locals */
	char fullSQLStatement[MAX_SQL_STATEMENT_LENGTH] = {0};
	char lclNonTestNames[MAX_NUM_STEPS][TESTSTRUCTTEXTSIZE];
	char lclNewNonTestNames[MAX_NUM_STEPS][TESTSTRUCTTEXTSIZE];
	memset (lclNonTestNames, 0, MAX_NUM_STEPS*TESTSTRUCTTEXTSIZE);
	memset (lclNewNonTestNames, 0, MAX_NUM_STEPS*TESTSTRUCTTEXTSIZE);
	bool alterTable = false;
	int numNewNonTestSteps = 0;
	
	NonTestStepsSQLStrings sqlStrings;
	memset (&sqlStrings, 0, sizeof(sqlStrings));
	char nestNumstr[12] = {0}; // "[NestNum], "
	char nestNumValStr[8] = {0}; // 2 digits for nest, 4 char for ''%d', ', nullterm
	
	for (int i = 0; i < NumNonTestInfo; i++)
	{
		/* 20241115Tim: This logic means labels "fixed" prior to this will be fixed again
			since underscores will be removed */
		ArxUtils_RemoveNonAlphaNumeric (NonTestInfo[i].Label, &tempInt, tmpStr);
		ArxUtils_ReplaceChar (tmpStr, " ", "_", currNonTestStepName);
		
		/* Check if replacements caused duplicate */
		bool foundDuplicate = true;
		while (foundDuplicate) 
		{
			/* Keep searching until no duplicates are found
				Handles the case in which adding a suffix becomes 
				the name of a previous name 
				20241106Tim: TODO Consider a numeric option rather
				than continuously adding underscores */
			foundDuplicate = false;
			for (int k = 0; k < i; k++)
			{
				if (0==stricmp (currNonTestStepName, lclNonTestNames[k]))
				{
					foundDuplicate = true;
					break;
				}
			}
			if (foundDuplicate)
			{
				/* Add suffix of _*/
				int insert_ = strlen (currNonTestStepName);
				currNonTestStepName[insert_] = '_';
				currNonTestStepName[insert_+1] = 0;
			}
		}
		
		strcpy (NonTestInfo[i].Label, currNonTestStepName);
		strcpy (lclNonTestNames[i], currNonTestStepName);		
	}
	
	/* Check Non Test Names */
	tsErrChk (DBLog_CheckNonTestNames (DBRef, TableName, (char *)lclNonTestNames, NumNonTestInfo, TESTSTRUCTTEXTSIZE, &alterTable, (char *)lclNewNonTestNames, &numNewNonTestSteps, errmsg), errmsg);
	alterTable = !alterTable;
	
	/* Add new columns if alter */
	if (alterTable)
	{
		tsErrChk (DBLog_InsertNonTestNames (DBRef, TableName, (char *)lclNewNonTestNames, numNewNonTestSteps, TESTSTRUCTTEXTSIZE, errmsg), errmsg);
		/* Alter tables */
		tsErrChk (DBLog_GenerateSQLStatementBasedOnTemplate (DBRef, (char *)lclNewNonTestNames, numNewNonTestSteps, TESTSTRUCTTEXTSIZE, glbTemplateDir, "AlterTables_NonTestTimes", TableName, errmsg), errmsg);
	}
	
	/* Log Data */
	/* Add Batch or UUT prefix to table name */
	char tablePrefix[6] = {0};
	if (NestNumber == -1)
	{
		strcpy (tablePrefix, "BATCH");
	}
	else
	{
		strcpy (tablePrefix, "UUT");
		sprintf (nestNumstr, "[NestNum], ");
		sprintf (nestNumValStr, "'%d', ", NestNumber);
	}
	
	/* Default serial number */
	char serialNumber[32] = {0};
	if (!SerialNumber) 
	{
		if (NestNumber == -1)
			strcpy (tmpStr, "PANEL");
		else
			strcpy (tmpStr, "NEST");
		sprintf (serialNumber, "%s%d_%s", tmpStr, NestNumber, DateStr ());
	}
	else sprintf (serialNumber, SerialNumber);
	
	/* Panel_SN case */
	if (NestNumber == -1)
	{
		sprintf (fullSQLStatement,
				"\nBEGIN TRANSACTION;\n"
					"INSERT OR REPLACE INTO %s_Panel_SN([SerialNumber])\n"
					"VALUES ('%s');\n"
				"END;\n",
				TableName, serialNumber);
		
		LOG_SQL_STATEMENT (fullSQLStatement);
		tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg);
	}
	
	/* Build SQL Statement */
	for (int j = 0; j < NumNonTestInfo; j++)
	{
		/* Label (columns to insert) */
		strcat (sqlStrings.Label, "[");
		strcat (sqlStrings.Label, NonTestInfo[j].Label);
		strcat (sqlStrings.Label, "],");
		
		/* NonTestNotes */
		strcat (sqlStrings.NonTestNotes, "'");
		strcat (sqlStrings.NonTestNotes, NonTestInfo[j].NonTestNotes);
		strcat (sqlStrings.NonTestNotes, "',");
		
		/* NonTestTimes */
		strcat (sqlStrings.NonTestTimes, "'");
		sprintf (tmpStr, "%d", NonTestInfo[j].NonTestTimes);
		strcat (sqlStrings.NonTestTimes, tmpStr);
		strcat (sqlStrings.NonTestTimes, "',");
	}
	
	/* Execute SQL */
#define BuildOther(ResultTypeName)\
	sprintf (fullSQLStatement,\
				"\nBEGIN TRANSACTION;\n"\
					"INSERT OR ROLLBACK INTO \"%s_%s_%s\"(%s [SerialNumber], %s [BatchNum])\n"\
					"VALUES (%s '%s', %s '%d');\n"\
				"END;\n",\
			TableName, #ResultTypeName, tablePrefix, sqlStrings.Label, nestNumstr, \
			/*values:*/sqlStrings.ResultTypeName, serialNumber, nestNumValStr, BatchNumber);\
	LOG_SQL_STATEMENT (fullSQLStatement);\
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg)
	
	BuildOther (NonTestNotes);
	BuildOther (NonTestTimes);
#undef BuildOther
	
Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int DBLog_SystemLog (
	IN DBHandle DBRef,
	IN char *TableName,
	IN char *SystemMessage, /* 20241113Tim: By reference in DBSeq, but never modified within */
	char errmsg[ERRLEN])
{
	char *dbErrmsg = 0;
	
	libInit;
	
	char fullSQLStatement[MAX_SQL_STATEMENT_LENGTH] = {0};
	
	sprintf (fullSQLStatement,
				"\nBEGIN TRANSACTION;\n"
					"INSERT INTO %s_SystemLog([SystemMessage])\n"
					"VALUES ('%s');\n"
				"END;\n",
			TableName, SystemMessage);
	LOG_SQL_STATEMENT (fullSQLStatement);
	tsErrChk (sqlite3_exec (DBRef, fullSQLStatement, NULL, NULL, &dbErrmsg), dbErrmsg);
	
Error:
	if (dbErrmsg) sqlite3_free (dbErrmsg);
	return error;
}
//! \cond
/// REGION END
//! \endcond
