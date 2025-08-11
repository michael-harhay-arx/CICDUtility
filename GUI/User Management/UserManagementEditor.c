//==============================================================================
//
// Title:		UserManagementEditor.c
// Purpose:		A short description of the implementation.
//
// Created on:	12/19/2024 at 8:50:16 AM by Timothius Prajogi.
// Copyright:	Arxtron Technologies Inc.. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include "UserManagement_LIB.h"
#include "UserManagement_LIB_Definitions.h"
#include "UserEditorPanel.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int glbBlockUntilCommit = 0;
static int glbPasswordCtrl = 0;
static int glbConfirmPasswordCtrl = 0;
static int glbDeleteFlag = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

extern int libInitialized;

int glbEditPanel = 0;
CtrlCallbackPtr glbConfirmEditFuncPtr = NULL;
void *glbConfirmEditFuncCallbackData = NULL;

User_Struct glbSelectedUserData;

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);
void GetLibErrMsg (int error, char errmsg[ERRLEN]);

/***************************************************************************//*!
* \brief Display login prompt
*
* Since button's eventdata1&2 are both always 0, they can be used here
* eventData1 will be if user is Admin, eventData2 will be if login error occurs
* 
*
*******************************************************************************/
int UserManagement_LoadEditPanel (IN int ParentPanel, OPT IN CtrlCallbackPtr EditFuncPtr, OPT IN void *EditFuncCallbackData, char errmsg[ERRLEN])
{
	libInit;
	
	if (!glbEditPanel)
	{
		glbEditPanel = LoadPanelEx (ParentPanel, "UserEditorPanel.uir", UE_PANEL, __CVIUserHInst);
	}
	if (!glbPasswordCtrl)
	{
		UIErrChk (glbPasswordCtrl = PasswordCtrl_ConvertFromString (glbEditPanel, UE_PANEL_EDIT_PASSWORD));
	}
	if (!glbConfirmPasswordCtrl)
	{
		UIErrChk (glbConfirmPasswordCtrl = PasswordCtrl_ConvertFromString (glbEditPanel, UE_PANEL_CONFIRM_EDIT_PASSWORD));
	}
	
	glbConfirmEditFuncPtr = EditFuncPtr;
	glbConfirmEditFuncCallbackData = EditFuncCallbackData;
		
	error = 0;
Error:
	return error;
}

/***************************************************************************//*!
* \brief Display login prompt
*******************************************************************************/
int UserManagement_DisplayEditPanel (IN int BlockUntilCommit, char errmsg[ERRLEN])
{
	libInit;
	
	PROTECT_FUNCTION(DEFAULT_MIN_REQ_USRMAN_LVL, "Only admin may edit user.\n");
	
	tsErrChk (!glbEditPanel, "Call UserManagement_LoadEditPanel beforehand");
	
	/* Populate Dropdown */
	tsErrChk (PopulateUserList (glbEditPanel, UE_PANEL_SEL_USER, errmsg), errmsg);
	UIErrChk (InsertListItem (glbEditPanel, UE_PANEL_SEL_USER, 0, "---new user---", "---new user----"));
	UIErrChk (SetCtrlAttribute (glbEditPanel, UE_PANEL_SEL_USER, ATTR_CTRL_VAL, "---new user----"));
	
	UIErrChk (SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, ""));
	
	UIErrChk (DisplayPanel (glbEditPanel));
	
	glbBlockUntilCommit = BlockUntilCommit;
	if (BlockUntilCommit)
		error = RunUserInterface ();
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Explicitly discard panels. 
*
* May cause issues when used with a parent panel.
*
*******************************************************************************/
int UserManagement_DiscardEditPanel (char errmsg[1024])
{
	libInit;
	
	if (glbEditPanel)
	{
		UIErrChk (DiscardPanel (glbEditPanel));
		glbEditPanel = 0;
	}
	
	error = 0;
Error:
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int CVICALLBACK UserManagementCB_EditSelectUser (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	switch (event)
	{
		case EVENT_RING_BEGIN_MENU:
		case EVENT_COMMIT:
			/* Get pre-existing values */
			char selectedUser[MAX_POSSIBLE_USERNAME_LENGTH] = {0};
			UIErrChk (GetCtrlAttribute (panel, control, ATTR_CTRL_VAL, selectedUser));
			
			/* Reset delete */
			glbDeleteFlag = 0;
			SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, "");
			
			
			memset (&glbSelectedUserData, 0, sizeof (glbSelectedUserData));
			if (strcmp (selectedUser, "---new user----") != 0)
			{
				int selectedUserIndex = 0;
				tsErrChk (UserManagement_GetUserIndex (selectedUser, &selectedUserIndex, errmsg), errmsg);
				
				tsErrChk (UserManagement_GetUser (selectedUserIndex, &glbSelectedUserData, errmsg), errmsg);
			}
			
			/* Set pre-existing values */
			UIErrChk (SetCtrlAttribute (panel, UE_PANEL_EDIT_USERNAME, ATTR_CTRL_VAL, glbSelectedUserData.username));
			UIErrChk (SetCtrlAttribute (panel, UE_PANEL_EDIT_ADMIN_LEVEL, ATTR_CTRL_VAL, glbSelectedUserData.isAdmin));
			UIErrChk (SetCtrlAttribute (panel, UE_PANEL_EDIT_PASSWORD, ATTR_CTRL_VAL, ""));
			UIErrChk (SetCtrlAttribute (panel, UE_PANEL_CONFIRM_EDIT_PASSWORD, ATTR_CTRL_VAL, ""));
			break;
	}

Error:
	return 0;
}


/***************************************************************************//*!
* \brief 
*******************************************************************************/
int CVICALLBACK UserManagementCB_EditConfirm (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	switch (event)
	{
		case EVENT_COMMIT:
			/* Get old user info */
			if (glbDeleteFlag)
			{
				tsErrChk (UserManagement_RemoveUser (glbSelectedUserData.username, errmsg), errmsg);
			}
			else
			{
				User_Struct oldUserData;
				memcpy (&oldUserData, &glbSelectedUserData, sizeof(oldUserData));
				
				/* For all non-blank fields, update the struct */
				char unBuffer[MAX_POSSIBLE_USERNAME_LENGTH] = {0};
				char pwdBuffer[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
				char confirmPwdBuffer[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
				UIErrChk (GetCtrlAttribute (panel, UE_PANEL_EDIT_USERNAME, ATTR_CTRL_VAL, unBuffer));
				UIErrChk (PasswordCtrl_GetAttribute (panel, UE_PANEL_EDIT_PASSWORD, ATTR_PASSWORD_VAL, pwdBuffer));
				UIErrChk (PasswordCtrl_GetAttribute (panel, UE_PANEL_CONFIRM_EDIT_PASSWORD, ATTR_PASSWORD_VAL, confirmPwdBuffer));
				UIErrChk (GetCtrlAttribute (panel, UE_PANEL_EDIT_ADMIN_LEVEL, ATTR_CTRL_VAL, &(oldUserData.isAdmin)));
				
				/* 20241219Tim: TODO clean up if branches */
				if (strlen (oldUserData.username) == 0)
				{
					/* New User */
					tsErrChk (strlen (unBuffer) == 0 ? ERR_INVALID_USER:0, "Invalid username.");
					tsErrChk (strcmp (pwdBuffer, confirmPwdBuffer) != 0 ? ERR_INCORRECT_PWD:0, "Passwords do not match");
					
					strcpy (oldUserData.username, unBuffer);
					strcpy (oldUserData.password, pwdBuffer);
					
					/* Create new user from struct */
					tsErrChk (UserManagement_AddUser (oldUserData.username, oldUserData.password, oldUserData.isAdmin, errmsg), errmsg);
				}
				else
				{
					char oldUserName[MAX_POSSIBLE_USERNAME_LENGTH] = {0};
					strcpy(oldUserName, oldUserData.username);
					
					/* Edit User */
					if (strlen (unBuffer) > 0)
						strcpy (oldUserData.username, unBuffer);
					if (strlen (pwdBuffer) > 0 || strlen (confirmPwdBuffer) > 0)
					{
						tsErrChk (strcmp (pwdBuffer, confirmPwdBuffer) != 0 ? ERR_INCORRECT_PWD:0, "Passwords do not match");
						strcpy (oldUserData.password, pwdBuffer); /* Non-encrpyted */
					}
					else
						XOREncrypt (oldUserData.password, strlen(oldUserData.password));
					
					/* Edit existing user from struct */
					tsErrChk (UserManagement_EditUser (0, oldUserName, oldUserData.username, oldUserData.password, &oldUserData.isAdmin, errmsg), errmsg);
				}
			}
			
			// TODO Force logout? or relog in (consider threading as well)
			
			UserManagementCB_EditPanel (panel, EVENT_CLOSE, NULL, eventData1, eventData2);
			
			break;
	}
	
	error = 0;
Error:
	if (error) 
	{ 
		switch (error)
		{
			case ERR_INCORRECT_PWD:
				SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, "Passwords do not match."); 
				break;
			case ERR_INVALID_USER:
				SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, "Invalid username or permissions."); 
				break;
			default:
				sprintf (errmsg, "(%d) Internal Error.", error);
				SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, errmsg); 
		}
	}
	else if (event == EVENT_COMMIT) { SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, ""); }
	return error;
}

/***************************************************************************//*!
* \brief Enter as confirm
*******************************************************************************/
int CVICALLBACK UserManagementCB_EditEnter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_KEYPRESS:
			if (eventData1 == VAL_ENTER_VKEY)
			{
				long dataType = 0;
				
				char temp[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
				int intBuffer = 0;
				if (control == glbPasswordCtrl || control == glbConfirmPasswordCtrl)
					PasswordCtrl_GetAttribute (panel, control, ATTR_PASSWORD_VAL, temp);
				else
				{
					GetCtrlAttribute (panel, control, ATTR_DATA_TYPE, &dataType);
					switch (dataType)
					{
						case VAL_INTEGER:
							GetCtrlAttribute (panel, control, ATTR_CTRL_VAL, (void *)intBuffer);
							break;
						case VAL_STRING:
							GetCtrlAttribute (panel, control, ATTR_CTRL_VAL, (void *)temp);
							break;
					}
				}
				if (0 != stricmp(temp, "") || dataType == VAL_INTEGER)
				{
					UserManagementCB_EditConfirm (panel, control, EVENT_COMMIT, callbackData, 0, 0);
				}
			}
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Cancel edit
*******************************************************************************/
int CVICALLBACK UserManagementCB_EditCancel (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			UserManagementCB_EditPanel (panel, EVENT_CLOSE, NULL, eventData1, eventData2);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Delete user
*******************************************************************************/
int CVICALLBACK UserManagementCB_Delete(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (strlen (glbSelectedUserData.username) > 0)
			{
				/* Set glb flag indicating to delete user 
					(this flag is reset on close or select user) */
				glbDeleteFlag = 1;
				
				/* Populate ERRMSG with delete confirmation */
				SetCtrlVal (glbEditPanel, UE_PANEL_ERRMSG, "CONFIRM DELETION? (OK)");
				
				/* Confirm handler should delete when flag is set and button pressed */
			}
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Edit panel callback
*******************************************************************************/
int CVICALLBACK UserManagementCB_EditPanel (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_DISCARD:
			glbEditPanel = 0;
			glbPasswordCtrl = 0;
			glbConfirmPasswordCtrl = 0;
		case EVENT_CLOSE:
			glbDeleteFlag = 0;
			HidePanel (panel);
			if (glbBlockUntilCommit)
				QuitUserInterface (0);
			
			/* Reset loaded panel to state before display */
			glbBlockUntilCommit = 0;
			break;
	}
	
	return 0;
}