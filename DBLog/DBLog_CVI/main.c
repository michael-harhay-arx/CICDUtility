/***************************************************************************//*!
* \file main.c
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/11/2024 10:38:29 AM
* \brief Used to create an example of how the library should be used
* 
* Main should contain a series of functions from the library demonstrating how
* 	its expected to be used. Make sure to comment adequetly if anything might be
* 	confusing. It should cover every public function of the library.
* 
* If there is a debug panel for the library, also add the ability to launch
* 	it as a standalone application.
* 
* Main MUST also contain AUTOMATED test cases that cover possible error cases
* 	and should be ran and updated every time new features are added or major
* 	changes are made.
* 
* To use main.c, include it in the build and change the build target to Executable
*******************************************************************************/

//! \cond
/// REGION START HEADER
//! \endcond

/* Uncomment if using GUI debug panel */
//#define HASGUI

//==============================================================================
// Include files

#include "DBLog_LIB.h"
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "toolbox.h"
#include <string.h>

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

static void usage (char *name)
{
	fprintf (stderr, "usage: %s <argument>\n", name);
	fprintf (stderr, "A short summary of the functionality.\n");
	fprintf (stderr, "    <argument>    is an argument\n");
	fprintf (stderr, "Order of operation: Change based on library\n");
	fprintf (stderr, "flag: function 1\n");
	fprintf (stderr, "flag: function 2\n");
	exit (1);
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Main
//! \endcond
int main (int argc, char *argv[])
{
	char errmsg[ERRLEN] = {0};
	DBHandle dbRef = NULL;
	DBHandle dbCounterRef = NULL;
	
	fnInit;
	
	/* Initialize */
	char fixtureIDs[4][16] = {"FixtureID123", "fixid1", "fixid2", "fixid3"};
	
	tsErrChk (Initialize_DBLog_LIB (
				  "C:\\Arxtron\\Timothius_Prajogi\\Misc\\DBSeq\\DBLog_LIB\\DBLog", 
				  "C:\\Arxtron\\Timothius_Prajogi\\Misc\\DBSeq\\DBLog_LIB\\DBLog", 
				  true, "C:\\Arxtron\\Timothius_Prajogi\\Misc\\DBSeq\\DBs\\parsed_sql.sql", 
				  errmsg), errmsg);
	tsErrChk (DBLog_InitializeFixtureCounter (
				  "FixtureID123", 99, 
				  true, "C:\\Arxtron\\Timothius_Prajogi\\Misc\\DBSeq\\DBs\\parsed_sql_counter.sql", 
				  errmsg), errmsg);
	
	tsErrChk (DBLog_Open (&dbRef, "C:\\Arxtron\\Timothius_Prajogi\\Misc\\DBSeq\\DBs", "TestDB2", MAX_DB_FILE_SIZE, "DBLog", errmsg), errmsg);
	tsErrChk (DBLog_OpenFixtureCounter (&dbCounterRef, (char *)fixtureIDs, 4, 16, errmsg), errmsg);
	
	/* DBLog */
	TestStruct testInfo[12];
	memset (testInfo, 0, sizeof(TestStruct)*12);
	
	strcpy (testInfo[0].TestAdditionalNotes, "Test1 additional notes");
			testInfo[0].TestNum = 1;
			testInfo[0].TestPass = 0;
	strcpy (testInfo[0].TestName, "CHECK_TOP_TAB");
			testInfo[0].TestTime = 2.5;
	strcpy (testInfo[0].TestVal, "123456");
	
	strcpy (testInfo[1].TestAdditionalNotes, "Test1 'additional' notes");
			testInfo[1].TestNum = 1;
			testInfo[1].TestPass = 0;
	strcpy (testInfo[1].TestLimMax, "999");
	strcpy (testInfo[1].TestName, "CHECK_BOTTOM_TAB");
			testInfo[1].TestTime = 2.5;
	strcpy (testInfo[1].TestVal, "123456");
	
	tsErrChk (DBLog_LogResults (dbRef, "DBLog", testInfo, 2, 32, "FixtureID1", 1, NULL, false, errmsg), errmsg);	
	
	NonTestStruct nonTestinfo[12];
	memset (nonTestinfo, 0, sizeof(NonTestStruct)*12);
	
	strcpy (nonTestinfo[0].Label, "Wait (1)");
	strcpy (nonTestinfo[0].NonTestNotes, "Wait for something to settle");
			nonTestinfo[0].NonTestTimes = 99;
			
	strcpy (nonTestinfo[1].Label, "Wait (1)");
	strcpy (nonTestinfo[1].NonTestNotes, "Wait for something to settle");
			nonTestinfo[1].NonTestTimes = 99;
	
	tsErrChk (DBLog_NonTestTimes (dbRef, "DBLog", nonTestinfo, 2, 1, -1, NULL, errmsg), errmsg);
	tsErrChk (DBLog_NonTestTimes (dbRef, "DBLog", nonTestinfo, 2, 1, 1, NULL, errmsg), errmsg);
	
	tsErrChk (DBLog_SystemLog (dbRef, "DBLog", "This is a system message", errmsg), errmsg);
	
	/* FixtureCounter */
	int passCounter = 0;
	int failCounter = 0;
	
	tsErrChk (DBLog_GetFixtureCounter (dbCounterRef, 1, &passCounter, &failCounter, errmsg), errmsg);
	tsErrChk (DBLog_UpdateFixtureCounter (dbCounterRef, 1, passCounter+1, failCounter+1, errmsg), errmsg);
	
	/* Uninit */
	tsErrChk (DBLog_UninitializeFixtureCounter (dbCounterRef, errmsg), errmsg);
	tsErrChk (DBLog_Uninitialize (dbRef, errmsg), errmsg);
	
Error:
	/* clean up */
	if (error)
	{
		fprintf (stderr, errmsg);
		GetKey ();
	}
	return error;
}
//! \cond
/// REGION END
//! \endcond