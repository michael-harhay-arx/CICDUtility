/***************************************************************************//*!
* \file UserManagement_LIB.c
* \author Timothius Prajogi
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/5/2024 4:39:25 PM
* \brief A short description.
* 
* A longer description.
* 
* This Library was created using
* * Template version 2.1.0
* * ArxtronToolslib version 1.5.0
* * DebugToolslib version 0.0.0
* * GUIToolslib version 0.0.0
* PLEASE ONLY UPDATE THE VERSION NUMBERS WHEN UPDATING THE TEMPLATE OR RELATED TOOLSLIB
* THESE VERSION NUMBERS SHOULD BE SEPARATE FROM THE LIBRARY VERSION
* THE ALL CAPS PORTION OF THIS COMMENT SECTION CAN BE REMOVED AFTER CREATING THE LIBRARY
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

/***************************************************************************//*!
* \brief Disables system logging completely.  Needs to be defined before including
* 	ArxtronToolslib.h.  By default, it is defined in each source file to allow
* 	for source file level control for disabling.
*******************************************************************************/
//#define SYSLOGDISABLE
/***************************************************************************//*!
* \brief Overrides config log level.  Needs to be defined before including
* 	ArxtronToolslib.h.  By default, it is defined in each source file to allow
* 	for source file level control for overrides.
*******************************************************************************/
//#define SYSLOGOVERRIDE 3

#include "toolbox.h"
#include <ansi_c.h>
#include <utility.h>
#include "UserManagement_LIB.h"
#include "UserManagement_LIB_Definitions.h"
#include "SystemLog_LIB.h"
#include "PwdPanel.h"

//==============================================================================
// Constants

#define XOR_VAL(__ind) 			(__secret[(__ind+glbProjectNumber)%(MAX_POSSIBLE_PASSWORD_LENGTH+1)])

//==============================================================================
// Types

//==============================================================================
// Static global variables

/***************************************************************************//*!
* \brief Stores the log level used for SYSLOG macro
*******************************************************************************/
static int glbSysLogLevel = 0;

/***************************************************************************//*!
* \brief Simple XOR secret key
*
* Generated such that alphanumeric characters and certain special characters
* 	cannot become EOF or New Line characters
*
*******************************************************************************/
static unsigned char __secret[MAX_POSSIBLE_PASSWORD_LENGTH] = 
{/* 0	, 1		, 2		, 3		, 4		, 5		, 6		, 7		, 8		, 9		, 10	, 11	, 12	, 13	, 14	, 15*/
	10	, 227	, 242	, 20	, 21	, 1		, 246	, 20	, 14	, 244	, 24	, 4		, 238	, 24	, 236	, 8		,	/*15*/
	26	, 128	, 4		, 227	, 229	, 247	, 248	, 2		, 234	, 6		, 247	, 5		, 12	, 247	, 17	, 25	,	/*31*/
	1	, 30	, 231	, 2		, 14	, 12	, 241	, 3		, 233	, 31	, 227	, 5		, 243	, 15	, 1		, 253	,	/*47*/
	244	, 15	, 228	, 7		, 7		, 247	, 248	, 11	, 235	, 10	, 243	, 253	, 17	, 226	, 128	, 14	,	/*63*/
	6	, 254	, 246	, 17	, 253	, 7		, 128	, 241	, 239	, 229	, 4		, 30	, 239	, 2		, 224	, 224	,	/*79*/
	240	, 224	, 238	, 13	, 6		, 5		, 235	, 226	, 245	, 26	, 6		, 234	, 26	, 250	, 15	, 236	,	/*95*/
	8	, 13	, 28	, 17	, 240	, 245	, 26	, 235	, 249	, 17	, 22	, 228	, 22	, 2		, 29	, 249	,	/*111*/
	30	, 2		, 242	, 225	, 4		, 236	, 225	, 3		, 247	, 17	, 8		, 13	, 230	, 26	, 6		, 7		,	/*127*/
	29	, 246	, 224	, 251	, 224	, 246	, 230	, 22	, 22	, 29	, 23	, 18	, 228	, 17	, 234	, 128	,	/*143*/
	225	, 22	, 224	, 23	, 27	, 16	, 7		, 11	, 249	, 237	, 235	, 236	, 241	, 236	, 239	, 226	,	/*159*/
	4	, 17	, 11	, 17	, 241	, 251	, 226	, 18	, 250	, 15	, 235	, 8		, 235	, 10	, 27	, 4		,	/*175*/
	237	, 241	, 17	, 17	, 16	, 239	, 6		, 2		, 240	, 26	, 26	, 9		, 224	, 6		, 15	, 14	,	/*191*/
	237	, 20	, 17	, 20	, 14	, 244	, 254	, 229	, 28	, 252	, 25	, 29	, 237	, 30	, 12	, 233	,	/*207*/
	8	, 241	, 18	, 7		, 31	, 251	, 4		, 232	, 28	, 16	, 227	, 18	, 128	, 247	, 15	, 17	,	/*223*/
	31	, 4		, 12	, 21	, 14	, 17	, 29	, 13	, 227	, 15	, 30	, 12	, 231	, 228	, 16	, 244	,	/*239*/
	22	, 20	, 245	, 245	, 230	, 6		, 19	, 240	, 242	, 237	, 230	, 8		, 10	, 13	, 28	/*EMPTY*/	/*255*/
                                                                                                                                      
};

/***************************************************************************//*!
* \brief Project number
*******************************************************************************/
static int glbProjectNumber = 0;

/***************************************************************************//*!
* \brief Config file containing user information
*******************************************************************************/
static char glbFileLocation[MAX_PATHNAME_LEN] = {0};

/***************************************************************************//*!
* \brief All users found in config file
*******************************************************************************/
static User_Struct glbUsers[MAX_USERS];
static int glbNumUsers = 0;

/***************************************************************************//*!
* \brief 1 based index of logged in user
*******************************************************************************/
static int glbLoggedIn = 0;

int glbBlockUntilCommit = 0;

#ifdef THREADED
/***************************************************************************//*!
* \brief 
*******************************************************************************/
static CmtThreadLockHandle glbUserLock = 0;
#endif

//==============================================================================
// Static functions

//==============================================================================
// Global variables

int libInitialized = 0;

/***************************************************************************//*!
* \brief Stores the SystemLog.arx modified time to determine whether the file has
* 	changed.  This definition should be shared across all source files via extern
*******************************************************************************/
SysLogInfo glbSysLogInfo = {.hr=-1, .min=-1, .sec=-1};

/***************************************************************************//*!
* \brief Panel Handle for prompt
*******************************************************************************/
int glbLoginPanel = 0;
int glbPasswordCtrl = 0;
CtrlCallbackPtr glbConfirmLoginFuncPtr = NULL;
void *glbConfirmLoginFuncCallbackData = NULL;

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);
void GetLibErrMsg (int error, char errmsg[ERRLEN]);

void XOREncrypt (char *Array, IN int ArrSize);
int PwdCmp (IN char *Pwd1, IN char *Pwd2);
void PwdMask (IN char *RawPwd, IN int Encrypt, char *MaskedPwd, int *PwdLength);
void UserMask (IN char *RawUn, IN int IsAdmin, char *MaskedUn, int *UnLength);
int TerminateString (char *Array, IN int ArrSize, IN char TermAtChar);

int SaveChanges (char errmsg[ERRLEN]);
int AddUserToFile (IN int User, FILE *OutputFile, char errmsg[ERRLEN]);

int UserManagement_GetUserIndex (IN char *Username, int *UserIndex, char errmsg[ERRLEN]);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief 
*
* INI file's section must contain fields:
* * password
*
*******************************************************************************/
int Initialize_UserManagement_LIB (IN int ProjectNumber, IN char *UserFile, char errmsg[ERRLEN])
{	
	FILE *userFileHandle = NULL;
	
	fnInit;
	SYSLOG ("Function Start", 0, error, "");
	
	// Uncomment if depending on another lib, then update [baselibname] with actual library name (without quotes, Ex. Ini_LIB)
	// Make additional copies if depending on multiple libs
	//DEPENDENCY_VERSION_CHECK([baselibname],ExpectedVersionMajor,ExpectedVersionMinor,error,errmsg);
	
	/* Put initialization code here */
	glbProjectNumber = ProjectNumber;
	
	memset (glbUsers, 0, sizeof(glbUsers));
	
	strcpy (glbFileLocation, UserFile);
	int createDefaultUserFlag = !FileExists (glbFileLocation, NULL);
	
	if (!createDefaultUserFlag)
	{
		/* Get users */
		userFileHandle = fopen (glbFileLocation, "r+");
		tsErrChk (!userFileHandle, "Failed to load config file");
		tsErrChk (fseek (userFileHandle, 0, SEEK_SET), "Failed to load config file");
				
		// Line can be username/password + '\n'
		char line[Max (MAX_POSSIBLE_PASSWORD_LENGTH, MAX_POSSIBLE_USERNAME_LENGTH)+1] = {0};
		int currLine = 0;
		int lineLen = (int)sizeof(line);
		int termInd = 0;
		int highestLvl = 0;
		while (fgets (line, lineLen, userFileHandle))
		{
			termInd = TerminateString (line, lineLen, '\n');
			switch (currLine % NUM_FIELDS)
			{
				case 0:
					strcpy (glbUsers[glbNumUsers].username, line);
					XOREncrypt (glbUsers[glbNumUsers].username, termInd);
					/* 20241218Tim: Using this format, only 10 levels (0-9) of administration are supported */
					glbUsers[glbNumUsers].isAdmin = atoi(&(glbUsers[glbNumUsers].username[termInd-1]));
					highestLvl = Max(glbUsers[glbNumUsers].isAdmin, highestLvl);
					glbUsers[glbNumUsers].username[termInd-1] = 0;
					break;
					
				case 1:
					memcpy (glbUsers[glbNumUsers].password, line, termInd);
					break;
			}
			currLine++;
			glbNumUsers = currLine / 2;
		}
		if (highestLvl < MAX_USR_LVL)
		{
			createDefaultUserFlag = 1;
		}
	}
	
#ifdef THREADED
	/* Initialize lock for glbUsers */
	tsErrChk (CmtNewLock ("User Lock", OPT_TL_PROCESS_EVENTS_WHILE_WAITING | OPT_TL_SUPPORT_TIMEOUT, &glbUserLock), "Failed to create lock");
#endif
	
	libInitialized = 1;
	
	if (createDefaultUserFlag)
	{
		if (userFileHandle != NULL)
			fclose (userFileHandle);
		userFileHandle = NULL;
		
		error = UserManagement_AdminOverrideAddUser ("administrator", "admin", 0, MAX_USR_LVL, errmsg);
		if (error)
		{
			int existingUser = 0;
			libErrChk (UserManagement_GetUserIndex ("administrator", &existingUser, errmsg), errmsg);
			GetUser(existingUser).isAdmin = MAX_USR_LVL;
			tsErrChk (SaveChanges (errmsg), errmsg);
		}
	}
	
	/* error will be -99999 from fnInit if no calls to any of the ErrChk macros are made */
	
Error:
	if (userFileHandle != NULL)
		fclose (userFileHandle);
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int UserManagement_Uninitialize (char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	glbNumUsers = 0;
	memset (glbUsers, 0, sizeof(glbUsers));
	
	libInitialized = 0;
	error = 0;
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Library Function Template. Name all exported functions with LibName_Description,
* 	where LibName is the same for all functions and Description tells the user
* 	roughly what the function accomplishes.
* 
* This function is also set up to demonstrate how SYSLOG is expected to be used.
* 	In order to use SYSLOG, all function parameters have to be pointers.  The IN
* 	macro should be used to indicate input parameters.
* 
* All functions should use all components of this template function unless it must
* 	be excluded from system logging for speed reasons.
* 
* See the SYSLOG comment block for more details on the parameters being passed in.
*******************************************************************************/
int UserManagement_Fn (IN int *a, IN long long *b, IN float *c, double *d, short *e, char *f, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 6, f, "ilfdsc");
	
	
	
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
int UserManagement_Fn2 (char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	
	
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

//! \cond
/// REGION START Helpers
//! \endcond

/***************************************************************************//*!
* \brief 
*******************************************************************************/
void XOREncrypt (char *Array, IN int ArrSize)
{
	for(int i = 0; i < ArrSize; i++)
	    Array[i] ^= XOR_VAL(i);
}

/***************************************************************************//*!
* \brief Validate that __secret returns a valid array of values
*
* Ensures that __secret does not XOR a valid character to '\n' or '0'
*
*******************************************************************************/
int XORValidate (char errmsg[ERRLEN])
{
	fnInit;
	
	// 32 -> 126 inclusive (' ' -> '~')
	char validationArray[MAX_POSSIBLE_PASSWORD_LENGTH];
	for (unsigned char chr = 32; chr <= 126; chr++)	
	{
		memset (validationArray, chr, MAX_POSSIBLE_PASSWORD_LENGTH);
		XOREncrypt (validationArray, MAX_POSSIBLE_PASSWORD_LENGTH);
		
		for (int i = 0; i < MAX_POSSIBLE_PASSWORD_LENGTH; i++)
		{
			tsErrChk ((validationArray[i] == '\n') || (validationArray[i] == '\0') || (validationArray[i] == EOF), "Invalid key value at index: %d->%c", i, chr);
		}
	}

Error:
	return error;
}

/***************************************************************************//*!
* \brief Convert raw password to file version bytes.
*
* MaskedPwd is not valid until this function returns.
* RawPwd can overlap MaskedPwd.
* 
* \param [IN/OUT] PwdLength		In: defines how many bytes to encrypt, and where
*								to put the newline. Out: the new length of MaskedPwd.
*
*******************************************************************************/
void PwdMask (IN char *RawPwd, IN int Encrypt, char *MaskedPwd, int* PwdLength)
{
	int pwdLength = *PwdLength;
	
	memmove (MaskedPwd, RawPwd, pwdLength);
	if (Encrypt)
		XOREncrypt (MaskedPwd, pwdLength);
	MaskedPwd[pwdLength++] = '\n';
	MaskedPwd[pwdLength--] = 0;
	
	*PwdLength = pwdLength;
}

/***************************************************************************//*!
* \brief Convert raw password to file version bytes.
*
* MaskedUn is not valid until this function returns.
* Assumes IsAdmin is valid.
*
* \param [IN/OUT] UnLength		In: defines how many bytes to encrypt, and where
*								to put the newline. Out: the new length of MaskedUn.
*
*******************************************************************************/
void UserMask (IN char *RawUn, IN int IsAdmin, char *MaskedUn, int *UnLength)
{
	int unLength = *UnLength;
	memcpy (MaskedUn, RawUn, unLength);
	// not guranteed to be null terminated
	
	/* Assign admin level */
	sprintf (&(MaskedUn[unLength]), "%d", IsAdmin); // null terminated here
	unLength++;

	PwdMask (MaskedUn, 1, MaskedUn, &unLength);
	
	*UnLength = unLength;
}

/***************************************************************************//*!
* \brief 
*
* Passwords must be null terminated
*
*******************************************************************************/
int PwdCmp (IN char *Pwd1, IN char *Pwd2)
{
	char pwdCpy[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
	strcpy (pwdCpy, Pwd1);
	XOREncrypt (pwdCpy, strlen(pwdCpy));
	for (int i = 0; pwdCpy[i] || Pwd2[i]; i++)
	{
		if (pwdCpy[i] != Pwd2[i]) 
			return ERR_INCORRECT_PWD;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Null terminate a string by replacing a specified character
*******************************************************************************/
int TerminateString (char *Array, IN int ArrSize, IN char TermAtChar)
{
	for (int i = 0; i < ArrSize; i++)
	{
		if (Array[i] == TermAtChar)
		{
			Array[i] = 0;
			return i;
		}
	}
	return -1; // or terminate at ArrSize-1?
}

/***************************************************************************//*!
* \brief Get the 1 based index of user
*******************************************************************************/
int UserManagement_GetUserIndex (IN char *Username, int *UserIndex, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");

	BEGIN_CRIT_SECTION;
	*UserIndex = 0;
	for (int i = 0; i < glbNumUsers; i++)
	{
		if (0 == strcmp (Username, glbUsers[i].username))
		{
			*UserIndex = i+1;
			break;
		}
	}
	tsErrChk (!*UserIndex ? ERR_USER_NOT_FOUND : 0, "User not found.\n");
	
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Get the data of user based on 1 based index
*******************************************************************************/
int UserManagement_GetUser (IN int UserIndex, User_Struct *UserData, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	BEGIN_CRIT_SECTION;
	VALIDATE_USER (UserIndex);
	
	if (UserData != NULL)
	{
		memcpy (UserData, glbUsers+(UserIndex-1), sizeof(User_Struct));
	}
	
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief If the user is an admin
*
* If username not provided uses current logged in user
*
*******************************************************************************/
int UserManagement_IsAdmin (OPT IN char *Username, int *IsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	/* If user was deleted, global array may have been shifted */
	BEGIN_CRIT_SECTION;
	
	int user = glbLoggedIn;
	if (Username)
	{
		libErrChk (UserManagement_GetUserIndex (Username, &user, errmsg), errmsg);
	}
	else
	{
		tsErrChk (!user, "No user logged in.\n");
	}
	
	//*IsAdmin = (0 == stricmp (glbUsers[user-1].username, "administrator"));
	*IsAdmin = glbUsers[user-1].isAdmin;
	
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Completely overwrites user struct.
*******************************************************************************/
int UserManagement_SetUsers (IN int NewNumUsers, IN User_Struct *NewUsers, char errmsg[ERRLEN])
{
	libInit;
	
	tsErrChk (NewNumUsers > MAX_USERS || NewNumUsers < 0, "Invalid number of users.");
	glbNumUsers = NewNumUsers;
	
	glbLoggedIn = 0; // Force relogin 
	memcpy (glbUsers, NewUsers, sizeof(User_Struct)*NewNumUsers);
	
	libErrChk (SaveChanges (errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Get number of users in the context.
*******************************************************************************/
int UserManagement_GetNumUsers (int *NumUsers, char errmsg[ERRLEN])
{
	libInit;
	
	*NumUsers = glbNumUsers;
	
	error = 0;
Error:
	return error;
}

/***************************************************************************//*!
* \brief Get all users in the context.
*******************************************************************************/
int UserManagement_GetUsers (User_Struct *Users, char errmsg[ERRLEN])
{
	libInit;
	
	if (glbNumUsers > 0)
		memcpy (Users, glbUsers, sizeof(User_Struct)*glbNumUsers);
	
	error = 0;
Error:
	return error;
}

/***************************************************************************//*!
* \brief Populate a dropdown with all users
*******************************************************************************/
int PopulateUserList (IN int Panel, IN int CtrlID, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	/* If user was deleted, global array may have been shifted */
	BEGIN_CRIT_SECTION;
	
	/* Populate username list */
	UIErrChk (ClearListCtrl (Panel, CtrlID));
	for (int i = 0; i < glbNumUsers; i++)
	{
		InsertListItem (Panel, CtrlID, i, glbUsers[i].username, glbUsers[i].username);
	}
	if (glbLoggedIn)
	{
		UIErrChk (SetCtrlVal (Panel, CtrlID, glbUsers[glbLoggedIn-1].username));
	}
	
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Save all users to file. Overwrites existing file.
*
* Should probably change to a binary file so that XOR encrypting has 
* no restrictions, and retrieval and storage is easier.
*
*******************************************************************************/
int SaveChanges (char errmsg[ERRLEN])
{
	FILE *userFileHandle = NULL;
	
	fnInit;
	SYSLOG ("Function Start", 0, error, "");
	
	userFileHandle = fopen (glbFileLocation, "w+");
	
	for (int i = 0; i < glbNumUsers; i++)
	{
		tsErrChk (AddUserToFile (i+1, userFileHandle, errmsg), errmsg);
	}
	
	error = 0;
Error:
	if (userFileHandle != NULL)
		fclose (userFileHandle);
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Add user to file, given 1 based index.
*
* Expects global array to store Usernames not encrypted and passwords 
* encrypted.
*
* \param [IN] User					The 1-based index of User.
* \param [IN/OUT] OutputFile 		The file to write to. The file should be 
* 									 opened as writable (w/r) and seeking should
* 									 be done by the caller.	
* 
*******************************************************************************/
int AddUserToFile (IN int User, FILE *OutputFile, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");

	const User_Struct *userData = &GetUser(User);
	int unLength = strlen(userData->username);
	int pwdLength = strlen(userData->password);
	
	/* Mask username and add admin level. */
	char maskedUn[MAX_POSSIBLE_USERNAME_LENGTH+2] = {0};
	UserMask (userData->username, userData->isAdmin, maskedUn, &unLength);
	
	/* Add newline. */
	char maskedPwd[MAX_POSSIBLE_PASSWORD_LENGTH+2] = {0};
	PwdMask (userData->password, 0/*already expected to be encrypted*/, maskedPwd, &pwdLength);
	
	/* Put user in config */
	tsErrChk (fputs (maskedUn, OutputFile), "Failed to add to config file");
	tsErrChk (fputs (maskedPwd, OutputFile), "Failed to add to config file");
	
Error:
	return error;
}

//! \cond
/// REGION END Helpers
//! \endcond

/***************************************************************************//*!
* \brief Admin override create user
*
* Should not be exposed
*
*******************************************************************************/
int UserManagement_AdminOverrideAddUser (IN char *Username, IN char *Password, IN int IsEncrypted, IN int IsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	char maskedPwd[MAX_POSSIBLE_PASSWORD_LENGTH+1] = {0};
	strcpy (maskedPwd, Password);
	if (!IsEncrypted)
		XOREncrypt (maskedPwd, strlen(maskedPwd));

	BEGIN_CRIT_SECTION;
	/* Check IsAdmin valid. */
	tsErrChk (IsAdmin > MAX_USR_LVL, "Only %d (0-%d) levels of administration are supported.", MAX_USR_LVL+1, MAX_USR_LVL);
	
	/* Check username unique */
	int dummy = 0;
	libErrChk (0==UserManagement_GetUserIndex (Username, &dummy, errmsg) ? ERR_INVALID_USER:0, "Username already exists");
	(void)dummy; // no purpose
	
	/* Set user struct */
	++glbNumUsers;
	strcpy (GetUser(glbNumUsers).username, Username);
	strcpy (GetUser(glbNumUsers).password, maskedPwd);
	GetUser(glbNumUsers).isAdmin = IsAdmin;
	
	tsErrChk (SaveChanges(errmsg), errmsg);
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Add a user to config file
*******************************************************************************/
int UserManagement_AddUser (IN char *Username, IN char *Password, IN int IsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	PROTECT_FUNCTION(DEFAULT_MIN_REQ_USRMAN_LVL, "Only admin may create user.\n");
	PROTECT_FUNCTION(IsAdmin, "Only higher level user can create a user with higher level.\n");
	
	libErrChk (UserManagement_AdminOverrideAddUser (Username, Password, 0, IsAdmin, errmsg), errmsg);
	
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Remove user from a config file
*
* TODO critical section protection for threaded use cases
*
*******************************************************************************/
int UserManagement_RemoveUser (IN char *Username, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	BEGIN_CRIT_SECTION;
	int user = 0;
	libErrChk (UserManagement_GetUserIndex (Username, &user, errmsg), "User does not exist."); // error out?
	
	/* Logged in user must have higher privileges than to be deleted user. */
	PROTECT_FUNCTION(GetUser(user).isAdmin, "Only higher level user may delete %s.\n", Username);
	
	/* Set user struct */
	memmove (glbUsers+user-1, glbUsers+user, sizeof (glbUsers) - (sizeof (User_Struct) * (user)));
	--glbNumUsers;
	
	/* Update current logged in if affected */
	if (glbLoggedIn == user)
	{
		glbLoggedIn = 0;
	}
	else if (user < glbLoggedIn)
	{
		--glbLoggedIn;
	}
		
	/* Force panel refresh? */
	if (glbLoginPanel /*&& panel is displayed */)
	{ tsErrChk (PopulateUserList (glbLoginPanel, PWD_PANEL_USERNAME, errmsg), errmsg); }

	tsErrChk (SaveChanges(errmsg), errmsg);
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Replace user in config file
*
* Pass in null or empty for *New* arguments to not change.
*
* \param [IN] UserIndex					The 1-based index of User. Pass in 0 to use username.
* \param [IN] Username					The original Username of the user to edit. Can only be NULL if UserIndex > 0.
*
*******************************************************************************/
int UserManagement_EditUser (IN int UserIndex, OPT IN char *Username, IN char *NewUsername, IN char *NewPassword, IN int *NewIsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	BEGIN_CRIT_SECTION;
	int user = UserIndex;
	if (user < 1)
	{
		tsErrChk (Username == NULL, "Username not provided.");
		libErrChk (UserManagement_GetUserIndex (Username, &user, errmsg), "User does not exist."); // error out?
	}
	else
	{
		VALIDATE_USER (user);
	}
	
	/* Logged in user must be admin */
	PROTECT_FUNCTION(Max(GetUser(user).isAdmin, DEFAULT_MIN_REQ_USRMAN_LVL), "Only higher level user may edit %s.\n", GetUser(user).username);
	
	/* Default to existing data */	
	if (NewUsername != NULL && strlen(NewUsername) > 0 && strcmp (GetUser(user).username, NewUsername) != 0)
	{
		/* Check username unique */
		int dummy = 0;
		libErrChk (0==UserManagement_GetUserIndex (NewUsername, &dummy, errmsg) ? ERR_INVALID_USER:0, "Username already exists");
		(void)dummy; // no purpose
		strcpy (GetUser(user).username, NewUsername);
	}
	if (NewPassword != NULL && strlen(NewPassword) > 0)
	{
		char maskedPwd[MAX_POSSIBLE_PASSWORD_LENGTH+1] = {0};
		strcpy (maskedPwd, NewPassword);
		XOREncrypt (maskedPwd, strlen(maskedPwd));
		strcpy (GetUser(user).password, maskedPwd);
	}
	if (NewIsAdmin != NULL)
	{
		GetUser(user).isAdmin = *NewIsAdmin;
	}

	/* Force panel refresh? */
	if (glbLoginPanel /*&& panel is displayed */)
	{ tsErrChk (PopulateUserList (glbLoginPanel, PWD_PANEL_USERNAME, errmsg), errmsg); }
	
	tsErrChk (SaveChanges(errmsg), errmsg);
Error:
	END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Validate a user (returns the user index)
*******************************************************************************/
int UserManagement_ValidateInternal (IN char *Username, IN char *Password, OPT int *User, OPT int *IsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	/* Get user */
	//BEGIN_CRIT_SECTION;
	int user = 0;
	libErrChk (UserManagement_GetUserIndex (Username, &user, errmsg), errmsg);
	tsErrChk (PwdCmp (Password, glbUsers[user-1].password), "Incorrect password.\n");
	
	/* Return if admin + user index */
	if (IsAdmin)	*IsAdmin = glbUsers[user-1].isAdmin;
	if (User)		*User = user;
	
Error:
	//END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Validate a user
*******************************************************************************/
int UserManagement_Validate (IN char *Username, IN char *Password, OPT int *IsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	/* Get user */
	tsErrChk (UserManagement_ValidateInternal (Username, Password, NULL, IsAdmin, errmsg), errmsg);
	
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Attempt to login
*******************************************************************************/
int UserManagement_Login (IN char *Username, IN char *Password, OPT int *IsAdmin, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	/* Get user */
	int user = 0;
	tsErrChk (UserManagement_ValidateInternal (Username, Password, &user, IsAdmin, errmsg), errmsg);
	
	/* Set logged in */
	glbLoggedIn = user;
	
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Logout
*******************************************************************************/
int UserManagement_Logout (char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	//BEGIN_CRIT_SECTION;
	glbLoggedIn = 0;
	
Error:
	//END_CRIT_SECTION;
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Logout
*******************************************************************************/
void UserManagement_GetCurrentUser (char *UserName)
{
	if (glbLoggedIn)
	{
		strcpy (UserName, glbUsers[glbLoggedIn-1].username);
	}
}

//! \cond
/// REGION START UI
//! \endcond

/***************************************************************************//*!
* \brief Display login prompt
*
* Since button's eventdata1&2 are both always 0, they can be used here
* eventData1 will be if user is Admin, eventData2 will be if login error occurs
* 
* The prompt will only be loaded into memory once and not discarded until parent
* 	is discarded.
*
*******************************************************************************/
int UserManagement_LoadPrompt (IN int ParentPanel, OPT IN CtrlCallbackPtr ConfirmFuncPtr, OPT IN void *ConfirmFuncCallbackData, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	if (!glbLoginPanel)
	{
		glbLoginPanel = LoadPanelEx (ParentPanel, "PwdPanel.uir", PWD_PANEL, __CVIUserHInst);
	}
	if (!glbPasswordCtrl)
	{
		UIErrChk (glbPasswordCtrl = PasswordCtrl_ConvertFromString (glbLoginPanel, PWD_PANEL_PASSWORD));
	}
	
	glbConfirmLoginFuncPtr = ConfirmFuncPtr;
	glbConfirmLoginFuncCallbackData = ConfirmFuncCallbackData;
	
	error = 0;
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Display login prompt
*******************************************************************************/
int UserManagement_DisplayPrompt (IN int BlockUntilCommit, OPT unsigned int DefaultIndex, OPT int TempLogin, OPT char *TempLoginUser, char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	tsErrChk (!glbLoginPanel, "Call UserManagement_LoadPrompt beforehand");
	
	tsErrChk (PopulateUserList (glbLoginPanel, PWD_PANEL_USERNAME, errmsg), errmsg);
	
	if (DefaultIndex)
	{
		if (DefaultIndex>=glbNumUsers) DefaultIndex = glbNumUsers-1;
		SetCtrlIndex (glbLoginPanel, PWD_PANEL_USERNAME, DefaultIndex);
	}
	
	tsErrChk (TempLogin && !BlockUntilCommit, "Temporary logins must block."); /* 20241218Tim: Could just force block */
	
	char tempLoginUser[MAX_POSSIBLE_USERNAME_LENGTH] = {0};
	if (TempLogin)
	{
		UIErrChk (SetCtrlAttribute (glbLoginPanel, PWD_PANEL_OK, ATTR_CALLBACK_DATA, (void *)tempLoginUser));
	}
	
	UIErrChk (SetCtrlVal (glbLoginPanel, PWD_PANEL_ERRMSG, ""));
	
	UIErrChk (DisplayPanel (glbLoginPanel));
	
	glbBlockUntilCommit = BlockUntilCommit;
	if (BlockUntilCommit)
		error = RunUserInterface ();
	
	if (TempLoginUser)		strcpy (TempLoginUser, tempLoginUser);
	
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Explicitly discard panels. 
*
* May cause issues when used with a parent panel.
*
*******************************************************************************/
int UserManagement_DiscardPrompt (char errmsg[ERRLEN])
{
	libInit;
	SYSLOG ("Function Start", 0, error, "");
	
	if (glbLoginPanel)
	{
		UIErrChk (DiscardPanel (glbLoginPanel));
		glbLoginPanel = 0;
	}
	glbPasswordCtrl = 0;
	
	glbConfirmLoginFuncPtr = NULL;
	glbConfirmLoginFuncCallbackData = NULL;
	
	error = 0;
Error:
	SYSLOG ("Function End", 0, error, "");
	return error;
}

/***************************************************************************//*!
* \brief Enter Login
*******************************************************************************/
int CVICALLBACK UserManagementCB_Confirm (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	fnInit;	
	
	switch (event)
	{
		case EVENT_COMMIT:
			/* Attempt login */
			char username[MAX_POSSIBLE_USERNAME_LENGTH] = {0};
			UIErrChk (GetCtrlVal (panel, PWD_PANEL_USERNAME, username));
			
			char password[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
			UIErrChk (PasswordCtrl_GetAttribute (panel, PWD_PANEL_PASSWORD, ATTR_PASSWORD_VAL, password));
			
			/* Empty password field */
			UIErrChk (PasswordCtrl_SetAttribute (panel, PWD_PANEL_PASSWORD, ATTR_PASSWORD_VAL, ""));
			
			int isAdmin = 0;
			if (callbackData)
			{
				tsErrChk (UserManagement_Validate (username, password, &isAdmin, errmsg), errmsg);
				strcpy (callbackData, username);
			}
			else
			{
				error = UserManagement_Login (username, password, &isAdmin, errmsg);
				
				/* External function */
				if (glbConfirmLoginFuncPtr)
				{
					tsErrChk (glbConfirmLoginFuncPtr (panel, control, event, glbConfirmLoginFuncCallbackData, isAdmin, error), errmsg);
				}
				else 
				{
					tsErrChk (error, errmsg);
				}
			}

			/* Close panel */
			UserManagementCB_Panel (panel, EVENT_CLOSE, NULL, eventData1, eventData2);

			break;
	}
	
	error = 0;
Error:
	if (error) { SetCtrlVal (glbLoginPanel, PWD_PANEL_ERRMSG, "Incorrect Password" /*TODO full errmsg?*/); }
	else if (event == EVENT_COMMIT) { SetCtrlVal (glbLoginPanel, PWD_PANEL_ERRMSG, ""); }
	return error;
}
	
/***************************************************************************//*!
* \brief Cancel login
*******************************************************************************/
int CVICALLBACK UserManagementCB_Cancel (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			UserManagementCB_Panel (panel, EVENT_CLOSE, callbackData, eventData1, eventData2);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Enter works as confirm
*******************************************************************************/
int CVICALLBACK UserManagementCB_Password (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_KEYPRESS:
			if (eventData1 == VAL_ENTER_VKEY)
			{
				char temp[MAX_POSSIBLE_PASSWORD_LENGTH] = {0};
				PasswordCtrl_GetAttribute (panel, control, ATTR_PASSWORD_VAL, temp);
				if (0 != stricmp(temp, ""))
				{
					GetCtrlAttribute (panel, PWD_PANEL_OK, ATTR_CALLBACK_DATA, &callbackData);
					UserManagementCB_Confirm (panel, control, EVENT_COMMIT, callbackData, 0, 0);
				}
			}
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Login prompt panel callback
*******************************************************************************/
int CVICALLBACK UserManagementCB_Panel (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_DISCARD:
			glbLoginPanel = 0;
			glbPasswordCtrl = 0;
		case EVENT_CLOSE:
			HidePanel (panel);
			if (glbBlockUntilCommit)
				QuitUserInterface (0);
			
			/* Reset loaded panel to state before display */
			glbBlockUntilCommit = 0;
			break;
	}
	
	return 0;
}
//! \cond
/// REGION END UI
//! \endcond

//! \cond
/// REGION END

/// REGION START Standard Lib Error Handling
//! \endcond
/***************************************************************************//*!
* \brief Get error message for library error codes
* 
* Define error codes in the header file under CONSTANTS
*******************************************************************************/
void GetLibErrMsg (int error, char errmsg[ERRLEN])
{
	memset (errmsg,0,ERRLEN);
	
	switch (error)
	{
		case ERR_INVALID_USER:
			strcpy (errmsg, "Invalid User parameters.");
			break;
		case ERR_INCORRECT_PWD:
			strcpy (errmsg, "Incorrect password. Passwords are case sensitive.");
			break;
		case ERR_USER_NOT_FOUND:
			strcpy (errmsg, "User does not exist in current state.");
			break;
		//case ERR_[Lib]_[Err1]:
		//	strcpy (errmsg,"Err1 msg");
		//	break;
	}
}
//! \cond
/// REGION END
//! \endcond
