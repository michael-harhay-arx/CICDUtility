/***************************************************************************//*!
* \file UserManagement_LIB.h
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/5/2024 4:39:25 PM
*******************************************************************************/

#ifndef __UserManagement_LIB_H__
#define __UserManagement_LIB_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "pwctrl.h"
#include "userint.h"

//==============================================================================
// Constants

#define ERR_INCORRECT_PWD 		(-30000)
#define ERR_USER_NOT_FOUND		(-30001)
#define ERR_INVALID_USER 		(-30002)
		
//==============================================================================
// Types
		
//==============================================================================
// Global vaiables

//==============================================================================
// External variables

//==============================================================================
// Global functions

int UserManagement_LIB_CheckVersionCompatibility (const int ExpectedVersionMajor, const int ExpectedVersionMinor, int *VersionMajor, int *VersionMinor);
int Initialize_UserManagement_LIB (const int ProjectNumber, const char *ConfigFile, char errmsg[1024]);
int UserManagement_Uninitialize (char errmsg[1024]);

int UserManagement_AddUser (const char *Username, const char *Password, const int IsAdmin, char errmsg[1024]);
int UserManagement_RemoveUser (const char *Username, char errmsg[1024]);
int UserManagement_EditUser (const int UserIndex, const char *Username, const char *NewUsername, const char *NewPassword, const int *NewIsAdmin, char errmsg[1024]);
int UserManagement_Validate (const char *Username, const char *Password, int *IsAdmin, char errmsg[1024]);
int UserManagement_Login (const char *Username, const char *Password, int *IsAdmin, char errmsg[1024]);
int UserManagement_Logout (char errmsg[1024]);
int UserManagement_IsAdmin (const char *Username, int *IsAdmin, char errmsg[1024]);
void UserManagement_GetCurrentUser (char *UserName);

int UserManagement_LoadPrompt (const int ParentPanel, const CtrlCallbackPtr ConfirmFuncPtr, const void *ConfirmFuncCallbackData, char errmsg[1024]);
int UserManagement_DisplayPrompt (const int BlockUntilCommit, unsigned int DefaultIndex, int TempLogin, char *TempLoginUser, char errmsg[1024]);
int UserManagement_DiscardPrompt (char errmsg[1024]);

int UserManagement_LoadEditPanel (const int ParentPanel, const CtrlCallbackPtr EditFuncPtr, const void *EditFuncCallbackData, char errmsg[1024]);
int UserManagement_DisplayEditPanel (const int BlockUntilCommit, char errmsg[1024]);
int UserManagement_DiscardEditPanel (char errmsg[1024]);

int UserManagement_DisplayEditTable (char errmsg[1024]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UserManagement_LIB_H__ */