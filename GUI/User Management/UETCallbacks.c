//==============================================================================
//
// Title:		UETCallbacks.c
// Purpose:		A short description of the implementation.
//
// Created on:	3/20/2025 at 10:56:51 AM by Timothius Prajogi.
// Copyright:	Arxtron Technologies Inc.. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <utility.h>
#include <ansi_c.h>
#include "UserManagement_LIB.h"
#include "UserManagement_LIB_Definitions.h"
#include "UserTableEditorPanel.h"

//==============================================================================
// Constants

#define TABLE_COLUMN(__nameStr, __cellType, __colWidth, __accessStruct, __structMember)
#define TABLE_COLUMNS\
	TABLE_COLUMN("Username"				, VAL_CELL_STRING	, 0.75, 1, username);\
	TABLE_COLUMN("Admin Level"			, VAL_CELL_NUMERIC	, 0.25, 1, isAdmin)/*;\*/
	//TABLE_COLUMN("New Password"			, VAL_CELL_STRING	, 0, password1);\
	//TABLE_COLUMN("Confirm New Password"	, VAL_CELL_STRING	, 0, password2)
	

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

extern int libInitialized;

// UI Components
int glbEditTablePanel = 0; /*!< Root panel */
int glbPwdPanels[MAX_USERS]; /*!< Password fields UI components. */
Point glbClickedPoint;	/*!< The last selected cell in table. */

// Saved Data
User_Struct glbSavedTableData[MAX_USERS];
int glbNumUsedTableRows = 0;
int glbLastUser = 0;
User_Struct glbLoggedInUser;
int glbLoggedInUserIndex = 0;

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);
void GetLibErrMsg (int error, char errmsg[ERRLEN]);

// Table Editor Generation
int TableEditor_GetCurrentUser (char errmsg[ERRLEN]);
int TableEditor_InitializePasswordFields (char errmsg[ERRLEN]);
int TableEditor_GenerateTable (char errmsg[ERRLEN]);
int TableEditor_PopulateTable (char errmsg[ERRLEN]);
int TableEditor_SetDefaultTableProperties (char errmsg[ERRLEN]);

// Helpers
int ToggleDimRow (int rowIndex, int dim, char errmsg[ERRLEN]);
int GetRowData (IN int RowIndex, User_Struct *UserData, char errmsg[ERRLEN]);
int GetRowDataRaw (IN int RowIndex, User_Struct *UserData, char errmsg[ERRLEN]);
int GetOkToSubmit (int *OkToSubmit, char errmsg[ERRLEN]);

void UserManagement_ExitEditTable (int control);
int UserManagement_PostExitTable (int ExitCode, char errmsg[ERRLEN]);

/***************************************************************************//*!
* \brief Loads then displays edit table. 
*
* Blocks until panel is closed.
*
*******************************************************************************/
int UserManagement_DisplayEditTable (char errmsg[ERRLEN])
{
	libInit;
	
	PROTECT_FUNCTION (DEFAULT_MIN_REQ_USRMAN_LVL, "Admin required to edit and add users.");
	libErrChk (TableEditor_GetCurrentUser (errmsg), errmsg);
	
	if (!glbEditTablePanel)
	{
		UIErrChk (glbEditTablePanel = LoadPanelEx (0, "UserTableEditorPanel.uir", UET_PANEL, __CVIUserHInst));
	}
	
	// Reset Fields
	memset (glbSavedTableData, 0, sizeof(glbSavedTableData));
	glbNumUsedTableRows = 0;
	tsErrChk (UserManagement_GetNumUsers (&glbNumUsedTableRows, errmsg), errmsg);
	tsErrChk (UserManagement_GetUsers (glbSavedTableData, errmsg), errmsg);
	
	tsErrChk (TableEditor_InitializePasswordFields (errmsg), errmsg);
	tsErrChk (TableEditor_GenerateTable (errmsg), errmsg);
	tsErrChk (TableEditor_PopulateTable (errmsg), errmsg);
	tsErrChk (TableEditor_SetDefaultTableProperties (errmsg), errmsg);
	
	// Disable editing current user's admin level, and any users with higher admin level 
	UIErrChk (SetTableCellAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, MakePoint(2, glbLoggedInUserIndex), ATTR_CELL_DIMMED, 1));
	UIErrChk (SetCtrlAttribute (glbEditTablePanel, UET_PANEL_UE_DELETE_ROW, ATTR_DIMMED, 1)); // disable delete on start
	
	// Display current default admin level
	char label[64] = {0};
	sprintf (label, "MIN ADMIN USER LEVEL: (%d)", DEFAULT_MIN_REQ_USRMAN_LVL);
	UIErrChk (SetCtrlVal (glbEditTablePanel, UET_PANEL_MIN_USR_LVL, label));

	// Display
	UIErrChk (DisplayPanel (glbEditTablePanel));
	
	// Handle post commit
	libErrChk (UserManagement_PostExitTable (RunUserInterface (), errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Get the current logged in user.
*******************************************************************************/
int TableEditor_GetCurrentUser (char errmsg[ERRLEN])
{
	libInit;
	
	/* Get current user */
	char tmpBuffer[MAX_POSSIBLE_USERNAME_LENGTH] = {0};
	UserManagement_GetCurrentUser (tmpBuffer);
	glbLoggedInUserIndex = 0;
	tsErrChk (UserManagement_GetUserIndex (tmpBuffer, &glbLoggedInUserIndex, errmsg), errmsg);
	tsErrChk (glbLoggedInUserIndex == 0, "Internal error.");
	
	libErrChk (UserManagement_GetUser (glbLoggedInUserIndex, &glbLoggedInUser, errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Convert string controls into password fields, and store handles.
*******************************************************************************/
int TableEditor_InitializePasswordFields (char errmsg[ERRLEN])
{
	libInit;
	
	/* Password fields */
	int sourcePasswordLinePanel = 0;
	memset (glbPwdPanels, 0, sizeof(glbPwdPanels));
	
	int left = 405;
	int top = 28;
	int skip = 29;
	
	UIErrChk (sourcePasswordLinePanel = LoadPanelEx (0, "UserTableEditorPanel.uir", TPWD_PANEL, __CVIUserHInst));
	
	for (int i = 0; i < MAX_USERS; i++)
	{
		UIErrChk (glbPwdPanels[i] = DuplicatePanel (glbEditTablePanel, sourcePasswordLinePanel, "", top+i*skip, left));
		UIErrChk (PasswordCtrl_ConvertFromString (glbPwdPanels[i], TPWD_PANEL_PWD1));
		UIErrChk (PasswordCtrl_ConvertFromString (glbPwdPanels[i], TPWD_PANEL_PWD2));
		DisplayPanel (glbPwdPanels[i]);
	}
	
	UIErrChk (DiscardPanel (sourcePasswordLinePanel));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Generate a table to display user information.
*******************************************************************************/
int TableEditor_GenerateTable (char errmsg[ERRLEN])
{
	libInit;
	
	/* Global table settings */
	UIErrChk (SetCtrlAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, ATTR_TABLE_MODE, VAL_COLUMN));
	
	/* Populate table layout */
	int numCols = 0;
#undef TABLE_COLUMN
#define TABLE_COLUMN(...) numCols++;
	
	int tableWidth = 0;
	GetCtrlAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, ATTR_WIDTH, &tableWidth);
	tableWidth -= 45;
	
	TABLE_COLUMNS;
	
	int colInd = 1;
#undef TABLE_COLUMN
#define TABLE_COLUMN(__nameStr, __cellType, __colWidth, __accessStruct, __structMember)\
 	UIErrChk (InsertTableColumns (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, 1, __cellType));\
	UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_CELL_TYPE, __cellType));\
	if (__cellType == VAL_CELL_NUMERIC)\
	{\
		/* TODO this is hardcoded for admin */\
		UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_DATA_TYPE, VAL_INTEGER));\
		UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_INCR_VALUE, 1));\
		UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_MIN_VALUE, 0));\
		UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_MAX_VALUE, MAX_USR_LVL));\
		UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_SHOW_INCDEC_ARROWS, 1));\
	}\
	UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd, ATTR_LABEL_TEXT, __nameStr));\
	UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, colInd++, ATTR_COLUMN_WIDTH, (int)(tableWidth*__colWidth)))
	
	TABLE_COLUMNS;

	UIErrChk (SetTableColumnAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_CELL_MODE, VAL_HOT));
	
	UIErrChk (InsertTableRows (glbEditTablePanel, UET_PANEL_UE_TABLE, -1, MAX_USERS, VAL_USE_MASTER_CELL_TYPE));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Populate the table with current user data.
*******************************************************************************/
int TableEditor_PopulateTable (char errmsg[ERRLEN])
{
	libInit;
	
	/* Populate table cells */
	UIErrChk (SetCtrlAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, ATTR_TABLE_MODE, VAL_ROW));
	
	User_Struct userData;
	glbLastUser = 0;
	for (int i = 1; i <= MAX_USERS; i++) // i is 1-based index
	{
		error = UserManagement_GetUser (i, &userData, errmsg);
		if (error)
		{
			if (glbLastUser == 0)
			{
				glbLastUser = i;
			}
			else
			{
				tsErrChk (ToggleDimRow (i, 1, errmsg), errmsg);
			}
			continue;
		}

		int colInd = 1;
#define ACCESS_STRUCT_0(...) 			colInd++
#define ACCESS_STRUCT_1(__structMember) UIErrChk (SetTableCellVal (glbEditTablePanel, UET_PANEL_UE_TABLE, MakePoint(colInd++, i), userData.__structMember))

		// Populate row
#undef TABLE_COLUMN
#define TABLE_COLUMN(__nameStr, __cellType, __colWidth, __accessStruct, __structMember)\
		ACCESS_STRUCT_##__accessStruct(__structMember)

		TABLE_COLUMNS;
		
		/* Disable row if higher level */
		if (glbLoggedInUser.isAdmin < userData.isAdmin)
		{
			tsErrChk (ToggleDimRow (i, 1, errmsg), errmsg);
		}
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Set table settings that apply to all cells.
*******************************************************************************/
int TableEditor_SetDefaultTableProperties (char errmsg[ERRLEN])
{
	libInit;
	
	/* Global table settings */
	UIErrChk (SetTableRowAttribute(glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_ROW_HEIGHT, 40));
	UIErrChk (SetTableRowAttribute(glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_TEXT_BOLD, 1));
	UIErrChk (SetTableColumnAttribute(glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_USE_LABEL_TEXT, 1));
	UIErrChk (SetTableColumnAttribute(glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_LABEL_BOLD, 1));
	UIErrChk (SetTableColumnAttribute(glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_LABEL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED));
	UIErrChk (SetTableColumnAttribute(glbEditTablePanel, UET_PANEL_UE_TABLE, -1, ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED)); // doesn't apply for some reason
		
Error:
	return error;
}

/***************************************************************************//*!
* \brief Attempt to quit from table editor interface.
*******************************************************************************/
void UserManagement_ExitEditTable (int control)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	int exitCode = EXIT_CODE_DEF;
	switch (control)
	{
		case UET_PANEL_UE_CANCEL:
			exitCode |= EXIT_CODE_CANCEL;
			break;
		case UET_PANEL_UE_OK:
			exitCode |= EXIT_CODE_OK;
			break;
	}
	
	if (EXIT_CODE_OK == (exitCode & EXIT_CODE_OK))
	{
		/* Validate fields */
		int okToSubmit = 0;
		tsErrChk (GetOkToSubmit (&okToSubmit, errmsg), errmsg);
		if (error = okToSubmit == 0, error)
		{
			sprintf (errmsg, "Fix errors before submitting.");
			goto Error;
		}
		
		/* Store fields */
		User_Struct currentUser;
		glbNumUsedTableRows = 0;
		for (int rowIndex = 1; rowIndex <= MAX_USERS; rowIndex++)
		{
			tsErrChk (GetRowData (rowIndex, &currentUser, errmsg), errmsg);
			if (strlen (currentUser.username) > 0)
			{
				if (strlen (currentUser.password) > 0)
				{
					currentUser.isAdmin += MAX_USR_LVL+1; // Indicate that this is a new password
				}
				else
				{
					// Should be validated not empty by GetRowData.
					strcpy (currentUser.password, glbSavedTableData[rowIndex-1].password);
				}
				
				memcpy (glbSavedTableData+glbNumUsedTableRows, &currentUser, sizeof(currentUser));
				++glbNumUsedTableRows;
			}
		}
	}
	
	DiscardPanel (glbEditTablePanel);
	glbEditTablePanel = 0;
	QuitUserInterface (exitCode);
	
	return;
Error:
	MessagePopup ("Error:", errmsg);
	// Reset Fields
	memset (glbSavedTableData, 0, sizeof(glbSavedTableData));
	UserManagement_GetNumUsers (&glbNumUsedTableRows, errmsg);
	UserManagement_GetUsers (glbSavedTableData, errmsg);
}

/***************************************************************************//*!
* \brief Handle exit table.
*******************************************************************************/
int UserManagement_PostExitTable (int ExitCode, char errmsg[ERRLEN])
{
	libInit;
	
	int submitChangesFlag = 0;
	switch (ExitCode)
	{
		case EXIT_CODE_CANCEL:
			// do nothing
			break;
		case EXIT_CODE_OK:
			submitChangesFlag = 1;
			break;
		default:
			// do nothing by default
			break;
	}
	
	if (submitChangesFlag)
	{
		User_Struct *currUserData = NULL;
		tsErrChk (UserManagement_SetUsers (0, NULL, errmsg), errmsg);
		for (int i = 0; i < glbNumUsedTableRows; i++)
		{
			currUserData = glbSavedTableData+i;
			int isEncrypted = 1;
			if (currUserData->isAdmin > MAX_USR_LVL)
			{
				isEncrypted = 0;
				currUserData->isAdmin -= MAX_USR_LVL+1;
			}
			tsErrChk (UserManagement_AdminOverrideAddUser (currUserData->username, currUserData->password, isEncrypted, currUserData->isAdmin, errmsg), errmsg);
		}
	}
	
	error = 0;
Error:
	return error;
}

//! \cond
/// REGION START Helpers
//! \endcond
/***************************************************************************//*!
* \brief Toggle dim of specified row.
*******************************************************************************/
int ToggleDimRow (int rowIndex, int dim, char errmsg[ERRLEN])
{
	fnInit;
	
	int control = UET_PANEL_UE_TABLE;
	
	int numCols = 0;
	UIErrChk (GetNumTableColumns (glbEditTablePanel, control, &numCols));
	for (int i = 1; i <= numCols; i++)
	{
		/* Table */
		UIErrChk (SetTableCellAttribute (glbEditTablePanel, control, MakePoint(i, rowIndex), ATTR_CELL_DIMMED, dim));
	}
	
	/* Password fields */
	UIErrChk (SetCtrlAttribute (glbPwdPanels[rowIndex-1], TPWD_PANEL_PWD1, ATTR_DIMMED, dim));
	UIErrChk (SetCtrlAttribute (glbPwdPanels[rowIndex-1], TPWD_PANEL_PWD2, ATTR_DIMMED, dim));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Reset values of specified row.
*******************************************************************************/
int ClearRow (int rowIndex, char errmsg[ERRLEN])
{
	fnInit;
	
	int control = UET_PANEL_UE_TABLE;
	
	int colInd = 1;
#undef TABLE_COLUMN
#define TABLE_COLUMN(__nameStr, __cellType, ...)\
	if (__cellType == VAL_CELL_NUMERIC)\
	{\
		UIErrChk (SetTableCellVal (glbEditTablePanel, control, MakePoint(colInd++, rowIndex), 0));\
	}\
	else if (__cellType == VAL_CELL_STRING)\
	{\
		UIErrChk (SetTableCellVal (glbEditTablePanel, control, MakePoint(colInd++, rowIndex), ""));\
	}

	TABLE_COLUMNS;
	
	/* Password fields */
	UIErrChk (SetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_PWD1, ""));
	UIErrChk (SetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_PWD2, ""));
	
	/* Clear from saved data */
	memset (glbSavedTableData+(rowIndex-1), 0, sizeof(User_Struct));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Check if a specified row is a valid user.
*******************************************************************************/
int ValidateRow (int rowIndex, char errmsg[ERRLEN])
{
	fnInit;
	
	/* Table fields */
	User_Struct userData;
	tsErrChk (GetRowDataRaw (rowIndex, &userData, errmsg), errmsg);
	
	/* Password fields */
	char password1[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
	char password2[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
	
	UIErrChk (GetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_PWD1, password1));
	UIErrChk (GetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_PWD2, password2));
	
	if (strlen (password1) > 0 || strlen (password2) > 0 || strlen (userData.username) > 0)
	{
		int pwdCmp = strcmp (password1, password2) == 0;
		int pwdEmptyCmp = strlen (password1) > 0 || strlen (glbSavedTableData[rowIndex-1].password) > 0;
		int unCmp = strlen (userData.username) > 0;
		int cellDimmed = 0;
		UIErrChk (GetTableCellAttribute (glbEditTablePanel, UET_PANEL_UE_TABLE, MakePoint (2, rowIndex), ATTR_CELL_DIMMED, &cellDimmed));
		int adminCmp = (userData.isAdmin >= 0 && userData.isAdmin <= glbLoggedInUser.isAdmin) || cellDimmed;
		if (pwdCmp && pwdEmptyCmp && unCmp && adminCmp)
		{
			int numRows = 0;
			UIErrChk (GetNumTableRows (glbEditTablePanel, UET_PANEL_UE_TABLE, &numRows));
			if (rowIndex < numRows && rowIndex >= glbLastUser) // undim next row (TODO never dims again)
				tsErrChk (ToggleDimRow (rowIndex+1, 0, errmsg), errmsg);
			UIErrChk (SetCtrlAttribute (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, ATTR_VISIBLE, 0));
		}
		else
		{
			// Display errmsg
			if (!pwdCmp)
			{
				UIErrChk (SetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, "Passwords do not match."));
			}
			else if (!pwdEmptyCmp)
			{
				UIErrChk (SetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, "Password cannot be empty."));
			}
			else if (!unCmp)
			{
				UIErrChk (SetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, "Username cannot be empty."));
			}
			else if (!adminCmp)
			{
				UIErrChk (SetCtrlVal (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, "Admin level invalid."));
			}
			
			UIErrChk (SetCtrlAttribute (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, ATTR_VISIBLE, 1));
			tsErrChk (1, "Invalid row.");
		}
	}
	else
	{
		UIErrChk (SetCtrlAttribute (glbPwdPanels[rowIndex-1], TPWD_PANEL_ERRMSG, ATTR_VISIBLE, 0));
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Check if the table is valid. 
*
* Depends on ValidateRow to display/hide error messages correctly.
*
*******************************************************************************/
int GetOkToSubmit (int *OkToSubmit, char errmsg[ERRLEN])
{
	fnInit;
	
	int numVisible = 0;
	for (int i = 0; i < MAX_USERS; i++)
	{
		int isVisible = 0;
		UIErrChk (GetCtrlAttribute (glbPwdPanels[i], TPWD_PANEL_ERRMSG, ATTR_VISIBLE, &isVisible));
		numVisible += isVisible;
	}
	*OkToSubmit = (numVisible > 0) ? 0:1;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Populates UserData with RAW (not encrypted) values.
*******************************************************************************/
int GetRowData (IN int RowIndex, User_Struct *UserData, char errmsg[ERRLEN])
{
	fnInit;
	
	/* Validate row */
	tsErrChk (ValidateRow (RowIndex, errmsg), errmsg);

	tsErrChk (GetRowDataRaw (RowIndex, UserData, errmsg), errmsg);
	
	// hardcoded: get password fields
	UIErrChk (GetCtrlVal (glbPwdPanels[RowIndex-1], TPWD_PANEL_PWD1, UserData->password))
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Populates UserData with RAW (not encrypted) values available from table.
*
* Does not validate or populate password fields. See GetRowData for that.
*
*******************************************************************************/
int GetRowDataRaw (IN int RowIndex, User_Struct *UserData, char errmsg[ERRLEN])
{
	fnInit;
	
	int colInd = 1;
	memset (UserData, 0, sizeof(User_Struct));
	UserData->isAdmin = (int)&(UserData->isAdmin); // sets the isAdmin value to be the pointer to itself.
	
#undef ACCESS_STRUCT_0
#undef ACCESS_STRUCT_1

#define ACCESS_STRUCT_0(...) // nothing
#define ACCESS_STRUCT_1(__structMember)\
	UIErrChk (GetTableCellVal (glbEditTablePanel, UET_PANEL_UE_TABLE, MakePoint(colInd++, RowIndex), (void*)(UserData->__structMember)))

	// Populate struct
#undef TABLE_COLUMN
#define TABLE_COLUMN(__nameStr, __cellType, __colWidth, __accessStruct, __structMember)\
	ACCESS_STRUCT_##__accessStruct(__structMember)

	TABLE_COLUMNS;
	
Error:
	return error;
}
//! \cond
/// REGION END Helpers
/// REGION START Callbacks
//! \endcond
/***************************************************************************//*!
* \brief Exit event
*******************************************************************************/
int CVICALLBACK UserManagementCB_EXIT (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			UserManagement_ExitEditTable (control);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Table callback.
*******************************************************************************/
int CVICALLBACK UserManagementCB_Table (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	
	switch (event)
	{
		// Try to make cell password control, however, cannot handle mid password delete or backspace.
		//case EVENT_KEYPRESS:
		//	if (glbClickedPoint.x > 2)
		//	{
		//		GetTableCellVal(panel, control, glbClickedPoint, tmpBuffer);
		//		
		//		if (eventData1 >= 32 && eventData1 <= 126 && strlen(tmpBuffer) < MAX_POSSIBLE_PASSWORD_LENGTH - 2)
		//		{
		//			int pwdLength = strlen(tmpBuffer);
		//			tmpBuffer[pwdLength+1] = 0;
		//			tmpBuffer[pwdLength] = '*';
		//			SetTableCellVal(panel, control, glbClickedPoint, tmpBuffer);
		//			SetActiveTableCell(panel, control, glbClickedPoint); //
		//			return 1;
		//		}
		//	}
		//	break;
			
		case EVENT_LEFT_CLICK_UP:
			GetActiveTableCell (panel, control, &glbClickedPoint);
			/* Disable buttons based on selected row */
			int cellDimmed = 0;
			GetTableCellAttribute (panel, control, MakePoint(2, glbClickedPoint.y), ATTR_CELL_DIMMED, &cellDimmed);
			// disable delete
			SetCtrlAttribute (panel, UET_PANEL_UE_DELETE_ROW, ATTR_DIMMED, cellDimmed);
			break;
		
		case EVENT_LOST_FOCUS:
			for (int rowIndex = 1; rowIndex <= MAX_USERS; rowIndex++)
			{
				int cellDimmed = 0;
				GetTableCellAttribute (panel, control, MakePoint(1, rowIndex), ATTR_CELL_DIMMED, &cellDimmed);
				if (cellDimmed) continue;
				
				ValidateRow (rowIndex, errmsg);
			}
			break;
			
		case EVENT_EDIT_MODE_STATE_CHANGE:
			if (glbClickedPoint.y == 0) break; // Started editing
			// Finished editing
			ValidateRow (glbClickedPoint.y, errmsg);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Password control field callback.
*******************************************************************************/
int CVICALLBACK UserManagementCB_PwdField (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	
	switch (event)
	{
		case EVENT_COMMIT:
		case EVENT_GOT_FOCUS:
			for (int i = 0; i < MAX_USERS; i++)
			{
				if (glbPwdPanels[i] != panel) continue;

				ValidateRow (i+1, errmsg);
				break;
			}
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Reset the current row and mark user for deletion if exists.
*******************************************************************************/
int CVICALLBACK UserManagementCB_DeleteUser(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	
	switch (event)
	{
		case EVENT_COMMIT:
			ClearRow (glbClickedPoint.y, errmsg);
			break;
	}
	return 0;
}
//! \cond
/// REGION END Callbacks
//! \endcond