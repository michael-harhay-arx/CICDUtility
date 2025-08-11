/***************************************************************************//*!
* \file main.c
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/5/2024 4:39:25 PM
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

//==============================================================================
// Include files

#include "UserManagement_LIB.h"
#include "UserManagement_LIB_Definitions.h"
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "toolbox.h"
#include <string.h>
#include "Ini_LIB.h"

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
	fnInit;
	
	/* Run this executable to initialize a sample user config file*/
	int cancelled = 0;
	tsErrChk (Initialize_Ini_LIB (0/*Unused*/, "StationConfig.ini", &cancelled, "Station Config", errmsg), errmsg);
	char prjNumber[32] = {0};
	tsErrChk (Ini_GetIniVal ("PrjNumber", prjNumber, errmsg), errmsg);
	tsErrChk (Initialize_UserManagement_LIB (atoi (prjNumber), "UserFile.txt", errmsg), errmsg);
	
	tsErrChk (UserManagement_LoadPrompt(0, 0, 0, errmsg), errmsg);
    tsErrChk (UserManagement_DisplayPrompt(1, 0, 0, 0, errmsg), errmsg);
	tsErrChk (UserManagement_DiscardPrompt(errmsg), errmsg);
	
    //tsErrChk (UserManagement_LoadEditPanel(0, 0, 0, errmsg), errmsg);
    //tsErrChk (UserManagement_DisplayEditPanel(1, errmsg), errmsg);
	//tsErrChk (UserManagement_DiscardEditPanel(errmsg), errmsg);
    
	tsErrChk (UserManagement_DisplayEditTable (errmsg), errmsg);
	
	tsErrChk (UserManagement_Uninitialize (errmsg), errmsg);
	
Error:
	/* clean up */
	Ini_ReleaseIniFile ();
	if (error)
	{
		fprintf (stderr, errmsg);
		GetKey();
	}
	return error;
}
//! \cond
/// REGION END
//! \endcond