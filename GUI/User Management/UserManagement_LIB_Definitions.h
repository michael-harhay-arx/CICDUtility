/***************************************************************************//*!
* \file UserManagement_LIB_Definitions.h
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/5/2024 4:39:25 PM
* \brief Used to store all private (Library specific) definitions, such as error
* 	codes.
*******************************************************************************/

#ifndef __UserManagement_LIB_Definitions_H__
#define __UserManagement_LIB_Definitions_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "UserManagement_LIB.h"
#include "ArxtronToolslib.h"
		
//==============================================================================
// Constants

//#define THREADED
		
#ifdef THREADED
#define BEGIN_CRIT_SECTION\
	__beganCritSection = 1;\
	{\
		int obtainedLock = 0;\
		while (!obtainedLock)\
		{\
			tsErrChk (CmtGetLockEx (glbUserLock, 0/*no event processing?*/, CMT_WAIT_FOREVER, &obtainedLock), "Failed to acquire lock");\
			/*DelayWithEventProcessing (0.001);*/ /* have to check if random wake ups are a thing or not*/\
		}\
	}

#define END_CRIT_SECTION\
	if (__beganCritSection)\
	{\
		CmtReleaseLock (glbUserLock);\
		__beganCritSection = 0;\
	}
#undef fnInit;
#define fnInit\
	int error = -99999;\
	memset (errmsg, 0, ERRLEN);\
	int __beganCritSection;\
	/*to suppress unused variable warning*/__beganCritSection = 0

#else
#define BEGIN_CRIT_SECTION
#define END_CRIT_SECTION
#endif
		
#define MAX_USERS 						(8)
#define MAX_POSSIBLE_USERNAME_LENGTH 	(64)
		
#define NUM_FIELDS						2		/*!< Number of lines associated to a user in the specified file.
													Username and admin are combined into 1 field. */
		
#define DEFAULT_MIN_REQ_USRMAN_LVL		(5) 	/*!< Minimum level to interact with user file */
#define MAX_USR_LVL 					(9) 	/*!< Maximum level a user can be. */
		
/*!< Macro to error out of function when logged in admin level is too low */
#define PROTECT_FUNCTION(__min_level, __errmsg, ...)\
	if (__min_level > 0)\
	{\
		int loggedInIsAdmin = 0;\
		libErrChk (UserManagement_IsAdmin (NULL, &loggedInIsAdmin, errmsg), errmsg); /* 20241218Tim: TODO enforce certain level to create user? */\
		tsErrChk ((loggedInIsAdmin < __min_level) ? ERR_INVALID_USER:0, __errmsg, __VA_ARGS__);\
	}

/*!< Macro to error out of function when invalid user index. */
#define VALIDATE_USER(__userIndex) tsErrChk ((__userIndex > glbNumUsers || __userIndex <= 0) ? ERR_USER_NOT_FOUND:0, "Invalid user index");

/*!< Macro to get the struct of the 1 based index of User */
#define GetUser(__one_based_index) (glbUsers[__one_based_index-1])

/* Table exit callback codes. */
#define EXIT_CODE_DEF  			0
#define EXIT_CODE_OK 			(1 << 0)
#define EXIT_CODE_CANCEL 		(1 << 1)

//==============================================================================
// Types
		
/*! \struct User_Struct
    \brief Structure for the information of a User.
*/
typedef struct {
	char username[MAX_POSSIBLE_USERNAME_LENGTH];
	char password[MAX_POSSIBLE_PASSWORD_LENGTH];
	int  isAdmin;
} User_Struct; 

//==============================================================================
// External variables

//==============================================================================
// Global functions

void XOREncrypt (char *Array, IN int ArrSize);

int PopulateUserList (IN int Panel, IN int CtrlID, char errmsg[ERRLEN]);

int UserManagement_GetUser (IN int UserIndex, User_Struct *UserData, char errmsg[ERRLEN]);
int UserManagement_GetUserIndex (IN char *Username, int *UserIndex, char errmsg[ERRLEN]);
int UserManagement_SetUsers (IN int NewNumUsers, IN User_Struct *NewUsers, char errmsg[ERRLEN]);
int UserManagement_GetNumUsers (int *NumUsers, char errmsg[ERRLEN]);
int UserManagement_GetUsers (User_Struct *Users, char errmsg[ERRLEN]);
int UserManagement_AdminOverrideAddUser (IN char *Username, IN char *Password, IN int IsEncrypted, IN int IsAdmin, char errmsg[ERRLEN]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UserManagement_LIB_Definitions_H__ */
