/***************************************************************************//*!
* \file TSMessaging.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 5/19/2021 2:35:16 PM
* \brief This file contains all functions related to message exchange between UI
* 	and Sequencer.
* 
* Code here requires extensive knowledge of how preprocessor macros and pointers
* 	work. It should cover most use cases needed for message exchange, but if
* 	more messages are to be added, you must understand how the rest of it works
* 	before doing so.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "CommonTSIncludes.h"
#include "TSTypeInsertMacros.h"
#include "UIUtility.h"
#include "TSMessaging.h"

#include <userint.h>
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

#undef MsgKey
/***************************************************************************//*!
* \brief First definition found in TSTypeInsertMacros.h. MsgKey(tmp2,b,c) defines
* 	all possible message combinations as forward function declarations.
*******************************************************************************/
#define MsgKey(tmp2,b,c)\
	void TSMsg_##b				(IN int NestNum, IN char Params[16][512]);\
	void TSMsg_##b##_##c		(IN int NestNum, IN char Params[16][512]);

MsgKeys;

#undef MsgKey
/***************************************************************************//*!
* \brief First definition found in TSTypeInsertMacros.h. MsgKey(keystruct,skey,...)
* 	initializes all possible messages in the glbMsgStruct.
*******************************************************************************/
#define MsgKey(keystruct,skey,...)\
	if (1) /* Localize variables used*/\
	{\
		strcpy (keystruct.key,#skey);\
		char subKeys[256] = {0};\
		strcpy (subKeys, #__VA_ARGS__ );\
		if (strlen(subKeys) && strcmp(subKeys,"NIL")!=0)\
		{\
			++keystruct.numSubStructs;\
			keystruct.subStruct = (MsgStruct*) realloc (keystruct.subStruct,sizeof(MsgStruct)*keystruct.numSubStructs);\
			keystruct.subStruct[keystruct.numSubStructs-1] = blankStruct;\
			MsgKey2(keystruct.subStruct[keystruct.numSubStructs-1],skey,__VA_ARGS__);\
		}\
		else\
		{\
			keystruct.msgCBFunction = TSMsg_##skey;\
		}\
	}

/***************************************************************************//*!
* \brief Helper macro to define the 2nd (sub) layer of message commands.
* 	Technically this can be expanded to deeper layers, but try to keep to
* 	only 2 layers to limit complexity. Adding a 3rd layer will require a
* 	lot of change (essentially manual recursion, unless there's a better way
* 	to define the macros such that it self recurses).
*******************************************************************************/
#define MsgKey2(keystruct,skey1,skey)\
	strcpy (keystruct.key,#skey);\
	keystruct.msgCBFunction = TSMsg_##skey1##_##skey;

/***************************************************************************//*!
* \brief Idle timer commands. TestStand equivalent struct was defined manually
* 	for this enum in GUIMessage.seq
*******************************************************************************/
typedef enum {
	TimerStop = 0,
	TimerStart = 1,
	TimerReset = 2,
	TimerStartAndReset = 3
} TimerVals;

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

#ifdef CVIUI_ONLY
extern CmtThreadLockHandle glbMsgLock[MAX_NESTS+1];
#endif

extern TSSeqVar glbWatchVars[100];
extern int glbNumWatching;
extern TSSeqVar glbSetVars[100];
extern int glbNumSetting;
extern int glbGetNewEventTime;

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Sets up the call back functions for all types of messages
* 
* \sa MsgKeys
*******************************************************************************/
void Initialize_TSMsgCB (void)
{
	CmtNewLock ("SetVar", OPT_TL_PROCESS_EVENTS_WHILE_WAITING | OPT_TL_SUPPORT_TIMEOUT, &glbSetVarLock);
	
	for (int i=0;i<MAX_KEYS;++i)
	{
		glbMsgStruct[i].key[0] = 0;
		glbMsgStruct[i].msgCBFunction = 0;
		glbMsgStruct[i].subStruct = 0;
		glbMsgStruct[i].numSubStructs = 0;
	}
	
	MsgStruct blankStruct = {.key[0] = 0, .msgCBFunction = 0, .subStruct = 0, .numSubStructs = 0};
	MsgKeys;
	
#ifdef CVIUI_ONLY
	for (int j=0; j<MAX_NESTS+1; ++j)
	{
		CmtNewLock("", OPT_TL_PROCESS_EVENTS_WHILE_WAITING|OPT_TL_SUPPORT_TIMEOUT, glbMsgLock+j);
	}
#endif
}

/***************************************************************************//*!
* \brief Probably not necessary since malloc only once and memory is freed when
* 	software is closed.  Memory should persist while program is running anyway
*******************************************************************************/
void Deinit_TSMsgCB (MsgStruct* MStruct)
{
	if (MStruct->subStruct)
	{
		Deinit_TSMsgCB (MStruct->subStruct);
		free (MStruct->subStruct);
		MStruct->subStruct = 0;
	}
}

/***************************************************************************//*!
* \brief Processes the UIMsgStringData and sends it to the appropriate callback
* 	based on UIMsgEvent.
* 
* Originally a sub-function of TSMsg_ApplicationMgr_OnUserMessage that's now
* 	been pulled out to be it's own function in order for the CVI sequencer
* 	to share the same functionality.
*******************************************************************************/
void RunMsgFn (int NestNum, int UIMsgEvent, char *UIMsgStringData, CAObjHandle CallerSeq, char *SeqName)
{
	char delim[8] = {'`', 0};
	char params[16][512] = {0};
	
	// Get key and parameters
	char *key = strtok(UIMsgStringData, delim);
	char *tok = strtok(NULL, delim);
	for (int i=0; tok; ++i)
	{
		strcpy (params[i],tok);
		tok = strtok(NULL, delim);
	}
	
#ifndef CVIUI_ONLY
	// Get/Set functions have a different structure than others
	if ((UIMsgEvent-TS_UIMsg_UserMessageBase) == MSG_VAR_GET || (UIMsgEvent-TS_UIMsg_UserMessageBase) == MSG_VAR_SET)
	{
		sprintf (params[0], "%d", CallerSeq);
		strcpy (params[1], SeqName);
	}
#endif
	
	/* 20210525Biye: Check TSEnum_UIMessageCodes, is there any point in creating handlers for system messages? */
	if (UIMsgEvent == TS_UIMsg_UserMessageBase)		// Raw send, determine fn based on message
	{
		/* 20210521Biye: If "Build with C99 extensions" is not checked in build options,
		it won't let you reused loop variables within the same context. Which is why we
		use j here.  This is a default setting from the provided GUI example for TS,
		I guess it's used as a portability option
		See https://www.ni.com/en-ca/support/documentation/supplemental/09/ansi-c99-extensions-in-labwindows-cvi.html */
		for (int j=0; j<MAX_KEYS; ++j)
		{
			if (strnicmp(key,glbMsgStruct[j].key,strlen(glbMsgStruct[j].key))==0)
			{
				int k = 0;
				for (k; k<glbMsgStruct[j].numSubStructs; ++k)
				{
					if (strnicmp(params[0],glbMsgStruct[j].subStruct[k].key,strlen(glbMsgStruct[j].subStruct[k].key))==0)
					{
						(glbMsgStruct[j].subStruct[k].msgCBFunction)(NestNum, params);
						break;
					}
				}
				if (k==glbMsgStruct[j].numSubStructs)	// if no substructs match
				{
					(glbMsgStruct[j].msgCBFunction)(NestNum, params);
					break;
				}
			}
		}
	}
	else	// Sent using sequence, code determines keys used
	{
		int key1 = (UIMsgEvent%TS_UIMsg_UserMessageBase)/1000;
		int key2 = (UIMsgEvent%1000)/100;
		//int key3 = (uiMsgEvent%100)/10;
		//int key4 = (uiMsgEvent%10);
		if (key2)
		{
			(glbMsgStruct[key1-1].subStruct[key2-1].msgCBFunction)(NestNum, params);
		}
		else
		{
			(glbMsgStruct[key1-1].msgCBFunction)(NestNum, params);
		}
	}
}

#ifndef CVIUI_ONLY
/***************************************************************************//*!
* \brief Pass data to and from TestStand via messages. See GUIMessage.seq for
* 	the TestStand counterpart to this function.
* 
* This callback is used for more than the custom commands (10000+) by TestStand.
* 	Therefore there will be times where this callback function is triggered and
* 	is related to a system message. Therefore, do not put any code here that can
* 	be executed by those calls and strictly put functional code in their respective
* 	specific message callbacks.
*******************************************************************************/
HRESULT CVICALLBACK TSMsg_ApplicationMgr_OnUserMessage(CAObjHandle caServerObjHandle, void *caCallbackData, TSUIObj_UIMessage uiMsg)
{
	int error = 0;
	
	char *uiMsgStringData = NULL;
	enum TSEnum_UIMessageCodes uiMsgEvent;

	TSObj_Thread currentThread = 0;
	CAObjHandle currentSeqContext = 0;
	CAObjHandle seqContextAsPropObj = 0;
	double currentPanel = 0;
	double specifiedPanel = 0;
	int nestNum = 0;
	
	CAObjHandle callerSeqContext = 0;
	CAObjHandle callerSeqContext2 = 0;
	CAObjHandle callerSeqFile = 0;
	CAObjHandle callerSeq = 0;
	char *seqPath = NULL;
	char tmp[MAX_PATHNAME_LEN] = {0};
	char seqName[MAX_PATHNAME_LEN] = {0};

	tsErrChk( TS_UIMessageGetEvent(uiMsg, &errorInfo, &uiMsgEvent));
	tsErrChk( TS_UIMessageGetThread(uiMsg, &errorInfo, &currentThread));
	tsErrChk( TS_ThreadGetSequenceContext(currentThread, NULL, 0, NULL, &currentSeqContext));
	tsErrChk( TS_SeqContextAsPropertyObject(currentSeqContext,  &errorInfo, &seqContextAsPropObj));
	tsErrChk( TS_PropertyGetValNumber(seqContextAsPropObj, &errorInfo, "RunState.TestSockets.MyIndex", 0, &currentPanel));
	tsErrChk( TS_UIMessageGetNumericData(uiMsg, &errorInfo, &specifiedPanel));	// Default 0
	
	// Specifically for get/set
	tsErrChk( TS_SeqContextGetCaller(currentSeqContext, &errorInfo, &callerSeqContext));
	tsErrChk( TS_SeqContextGetCaller(callerSeqContext, &errorInfo, &callerSeqContext2));
	tsErrChk( TS_SeqContextGetSequenceFile(callerSeqContext2, &errorInfo, &callerSeqFile));
	tsErrChk( TS_SeqContextAsPropertyObject(callerSeqContext2, &errorInfo, &callerSeq));
	tsErrChk( TS_SeqFileGetPath(callerSeqFile, &errorInfo, &seqPath));
	SplitPath(seqPath, tmp, tmp, seqName);
	seqName[strlen(seqName)-4] = 0;		// Cut off .seq
	
	if (specifiedPanel || currentPanel==-1 /*Outside of nest calls*/)
		nestNum = specifiedPanel;
	else
		nestNum = currentPanel;
	
	tsErrChk( TS_UIMessageGetStringData(uiMsg, &errorInfo, &uiMsgStringData));
	
	if (uiMsgEvent >= TS_UIMsg_UserMessageBase)	// Ignore all system level messages and only process custom messages
	{
		RunMsgFn (nestNum, uiMsgEvent, uiMsgStringData, callerSeq, seqName);
	}

Error:
	if (uiMsgStringData)
		CA_FreeMemory(uiMsgStringData);
	CA_DiscardObjHandle(currentThread);
	CA_DiscardObjHandle(currentSeqContext);
	CA_DiscardObjHandle(seqContextAsPropObj);
	CA_DiscardObjHandle(callerSeq);
	CA_DiscardObjHandle(callerSeqFile);
	CA_DiscardObjHandle(callerSeqContext);
	CA_DiscardObjHandle(callerSeqContext2);
	CA_FreeMemory(seqPath);
	DisplayError(error);
	return S_OK;
}
#endif

/***************************************************************************//*!
* \brief Sets value of a UI element
* 
* \param [in] Params	Additional parameters
* * Param1 = isNest flag (1 = Is Nest UI)
* * Param2 = UI element ID (use pre-defined enum to specify)
* 	ARX_UI_LED
* 	ARX_UI_COUNTER
* 	ARX_UI_TEXT
* * Param3 = Value to set the UI element
* * Param4 = EXEC_PANEL_TESTINFO->Col, NEST_PANEL_LABEL(_ROT)_STATUS->Text color, EXEC_PANEL_LED_TRACE->On color
* * Param5 = EXEC_PANEL_TESTINFO->Row, NEST_PANEL_LABEL(_ROT)_STATUS->Text bgcolor
* * Param6 = NEST_PANEL_LABEL(_ROT)_STATUS->Text size
*******************************************************************************/
void TSMsg_UI_SET (IN int NestNum, IN char Params[16][512])
{
	int isNest = atoi(Params[1]);
	int panelHandle = (isNest==1?gMainWindow.nests[NestNum]:gMainWindow.execpanel);
	int UIID = atoi(Params[2]);
	int val = atoi(Params[3]);
	
	if (!isNest && UIID==EXEC_PANEL_TESTINFO)	// Test Info table
		SetTableCellVal (panelHandle, UIID, MakePoint(atoi(Params[4])/*col*/, atoi(Params[5])/*row*/), Params[3]);
	else if (!isNest && UIID==EXEC_PANEL_TAB_NESTS)	// Set active nest tab
		SetActiveTabPage (panelHandle, UIID, val);
	else if ((isNest && UIID==NEST_PANEL_SERIAL_NUMBER) ||
			 (isNest && UIID==NEST_PANEL_CYCLE_CNT) ||
			 (isNest && UIID==NEST_PANEL_NEST_TEST_TIME) ||
			 (isNest && UIID==NEST_PANEL_NEST_PASS) ||
			 (isNest && UIID==NEST_PANEL_NEST_FAIL) ||
			 (!isNest && UIID==EXEC_PANEL_LT_PASS) ||
			 (!isNest && UIID==EXEC_PANEL_LT_FAIL) ||
			 (!isNest && UIID==EXEC_PANEL_ST_PASS) ||
			 (!isNest && UIID==EXEC_PANEL_ST_FAIL) ||
			 (isNest && UIID==NEST_PANEL_NEST_S2S_TIME) ||
			 (isNest && UIID==NEST_PANEL_LABEL_INSTRUCTIONS) || 
			 (isNest && UIID==NEST_PANEL_LABEL_STATUS) || 
			 (isNest && UIID==NEST_PANEL_LABEL_ROT_STATUS))	// String controls
	{
		SetCtrlVal (panelHandle, UIID, Params[3]);
		if (UIID==NEST_PANEL_LABEL_STATUS || UIID==NEST_PANEL_LABEL_ROT_STATUS)	// Set color
		{
			SetCtrlAttribute (panelHandle,UIID,ATTR_TEXT_COLOR,atoi(Params[4]));
			SetCtrlAttribute (panelHandle,UIID,ATTR_TEXT_BGCOLOR,atoi(Params[5]));
			if (atoi(Params[6]))
			{
				SetCtrlAttribute (panelHandle,UIID,ATTR_TEXT_POINT_SIZE,atoi(Params[6]));
			}
		}
	}
	else if (!isNest && UIID==EXEC_PANEL_IDLE_TIME)	// Idle timer
	{
		if ((val & TimerReset)==TimerReset)
		{
			glbGetNewEventTime = 1;
		}
		SetCtrlAttribute (gMainWindow.execpanel, EXEC_PANEL_IDLE_TIMER, ATTR_ENABLED, (val & TimerStart)==TimerStart);
	}
	else if (!isNest && UIID==9999)	/* 20241121Biye: Need to officially add this as an option*/
	{
		SetPanelAttribute (panelHandle, ATTR_BACKCOLOR, val);		// Panel background color
	}
	else	// LED
		SetCtrlVal (panelHandle, UIID, val);
	
	if (!isNest && UIID==EXEC_PANEL_LED_TRACE)
	{
		int OnColor = atoi(Params[4]);
		SetCtrlAttribute (panelHandle, UIID, ATTR_ON_COLOR, OnColor);
		SetPanelAttribute (panelHandle, ATTR_BACKCOLOR, (OnColor==VAL_RED?VAL_RED:0x00F0F0F0));		// Panel background color
	}
	
	// Update yield
	UIUtil_UpdateYield (panelHandle, UIID);
}

/***************************************************************************//*!
* \brief Formats and inserts the test result to test list
* 
* \param [in] Params	Additional parameters
* * Param1 = Text to display
*******************************************************************************/
void TSMsg_TEST_INFO (IN int NestNum, IN char Params[16][512])
{
	InsertListItem		(gMainWindow.nests[NestNum], NEST_PANEL_ADDITIONAL_RESULTS, -1, Params[1], 0);

	int index = 0;
	GetNumListItems		(gMainWindow.nests[NestNum], NEST_PANEL_ADDITIONAL_RESULTS, &index);
	SetCtrlAttribute	(gMainWindow.nests[NestNum], NEST_PANEL_ADDITIONAL_RESULTS, ATTR_CTRL_INDEX, index-1);
}

/***************************************************************************//*!
* \brief Formats and inserts the test result to test list
* 
* \param [in] Params	Additional parameters
* * Param1 = testNum
* * Param2 = testName
* * Param3 = testResult
* * Param4 = testResultVal
* * Param5 = testTime
*******************************************************************************/
void TSMsg_TEST_RESU (IN int NestNum, IN char Params[16][512])
{
	char insertText[256] = {0};
	
	char color[8] = {0};
	if (0==stricmp(Params[3],"pass"))
		strcpy (color, "00AF00");
	else if (0==stricmp(Params[3],"report"))
		strcpy (color, "0000FF");
	else if (0==stricmp(Params[3],"skipped"))
		strcpy (color, "A0A0A0");
	else
		strcpy (color, "FF0000");
	
	sprintf (insertText, "\033fg%s %s \033p30l%s \033p300l%s \033p400l%s \033p500l%s \033p%dr%s",
			 color,
			 Params[1], /* testNum */
			 Params[2], /* testName */
			 Params[3], /* testResult */
			 Params[4], /* testResultVal */
			 CVIUI_TEST_INFO_WIDTH-30, /* spacing */
			 Params[5] /* testTime */);
	InsertListItem		(gMainWindow.nests[NestNum], NEST_PANEL_DATA_LISTBOX, -1, insertText, 0);

	int index = 0;
	GetNumListItems		(gMainWindow.nests[NestNum], NEST_PANEL_DATA_LISTBOX, &index);
	SetCtrlAttribute	(gMainWindow.nests[NestNum], NEST_PANEL_DATA_LISTBOX, ATTR_CTRL_INDEX, index-1);
}

/***************************************************************************//*!
* \brief Clears the test result list
*******************************************************************************/
void TSMsg_TEST_CLEA (IN int NestNum, IN char Params[16][512])
{
	DeleteListItem (gMainWindow.nests[NestNum], NEST_PANEL_DATA_LISTBOX, 0, -1);
	DeleteListItem (gMainWindow.nests[NestNum], NEST_PANEL_ADDITIONAL_RESULTS, 0, -1);
}

/***************************************************************************//*!
* \brief Displays a popup message panel
* 
* \param [in] Params	Additional parameters
* * Param0 = Popup type (ARX_UI_POPUP)
* * Param1 = Message to show, "0" to hide panel
* * Param2 = Exit button visible flag (1 = visible)
* * Param3 = Flag for Break All in TestStand
*******************************************************************************/
void TSMsg_DISP (IN int NestNum, IN char Params[16][512])
{
	int popupType = atoi(Params[0]);
	int hide = (strcmp(Params[1],"0")==0);
	int exitBn = atoi(Params[2]);
	int pauseSequence = atoi(Params[3]);
	int bgcolor = 0;
	char title[32] = {0};
	
	if (hide)
	{
		HidePanel (gMainWindow.arxPopupPanel);
	}
	else
	{
		switch (popupType)
		{
			case POPUP_ARXERROR:
				strcpy (title, "ERROR!");
				bgcolor = VAL_RED;
				break;
			case POPUP_ARXWARNING:
				strcpy (title, "WARNING!");
				bgcolor = VAL_YELLOW;
				break;
		}
		SetPanelAttribute (gMainWindow.arxPopupPanel, ATTR_TITLE, title);
		SetPanelAttribute (gMainWindow.arxPopupPanel, ATTR_BACKCOLOR, bgcolor);
		ResetTextBox (gMainWindow.arxPopupPanel, POPUP_MSG_TEXTBOX, Params[1]);
		SetCtrlAttribute (gMainWindow.arxPopupPanel, POPUP_MSG_EXIT, ATTR_VISIBLE, exitBn);
		
		if (pauseSequence)
		{
#ifndef CVIUI_ONLY
			TS_EngineBreakAll (gMainWindow.engine, &errorInfo);
#endif
			SetCtrlAttribute (gMainWindow.arxPopupPanel, POPUP_MSG_DISMISS, ATTR_VISIBLE, 1);
		}
		
		SetActivePanel (gMainWindow.arxPopupPanel);
	}
}

/***************************************************************************//*!
* \brief Used for Variable Watch panel, gets the specified variable and stores it in
* 	glbWatchVars
* 
* \param [in] Params	Additional parameters
* * Param0 = Sequence Handle
* * Param1 = Sequence Name
*******************************************************************************/
void TSMsg_VAR_GET (IN int NestNum, IN char Params[16][512])
{
	CAObjHandle seq = atoi(Params[0]);
	double doubleVal = 0.0;
	short shortVal = 0;
	char *str = 0;
	
	for (int i=0; i<glbNumWatching; ++i)
	{
		if (strcmp(Params[1], glbWatchVars[i].seqName)==0)
		{
#ifndef CVIUI_ONLY
			switch (glbWatchVars[i].varType)
			{
				case VARTYPE_NUM:
					if (TS_PropertyGetValNumber (seq, &errorInfo, glbWatchVars[i].lookupString, 0, &doubleVal))
						strcpy (glbWatchVars[i].varVal, "not found");
					else
						sprintf (glbWatchVars[i].varVal, "%lf", doubleVal);
					break;
				case VARTYPE_STR:
					if (TS_PropertyGetValString (seq, &errorInfo, glbWatchVars[i].lookupString, 0, &str))
						strcpy (glbWatchVars[i].varVal, "not found");
					else
						strcpy (glbWatchVars[i].varVal, str);
					break;
				case VARTYPE_BOOL:
					if (TS_PropertyGetValBoolean (seq, &errorInfo, glbWatchVars[i].lookupString, 0, &shortVal))
						strcpy (glbWatchVars[i].varVal, "not found");
					else
					{
						switch(shortVal)
						{
							case VTRUE:
								strcpy(glbWatchVars[i].varVal, "True");
								break;
							case VFALSE:
								strcpy(glbWatchVars[i].varVal, "False");
								break;
						}
					}
					break;
			}
#endif
		}
	}
	CA_FreeMemory(str);
}

/***************************************************************************//*!
* \brief Set a variable in the sequence
* 
* \param [in] Params	Additional parameters
* * Param0 = Sequence Handle
* * Param1 = Sequence Name
*******************************************************************************/
void TSMsg_VAR_SET (IN int NestNum, IN char Params[16][512])
{
	CAObjHandle seq = atoi(Params[0]);
	int hasLock = 0;
	
	for (int i=0; i<glbNumSetting && i<99 /* Protects from shifting from the 101st element*/; ++i)
	{
#ifndef CVIUI_ONLY
		if (strcmp(Params[1], glbSetVars[i].seqName)==0)
		{
			switch (glbSetVars[i].varType)
			{
				case VARTYPE_NUM:
					TS_PropertySetValNumber (seq, &errorInfo, glbSetVars[i].lookupString, 0, atof(glbSetVars[i].varVal));
					break;
				case VARTYPE_STR:
					TS_PropertySetValString (seq, &errorInfo, glbSetVars[i].lookupString, 0, glbSetVars[i].varVal);
					break;
				case VARTYPE_BOOL:
					TS_PropertySetValBoolean (seq, &errorInfo, glbSetVars[i].lookupString, 0, (VBOOL)atoi(glbSetVars[i].varVal));
					break;
			}
#else
		{
			char *strippedLookupString = glbSetVars[i].lookupString;
			for (; (*strippedLookupString > 0) && (*strippedLookupString != '.'); strippedLookupString++);
			strippedLookupString++; // Start after '.'
			
			switch (glbSetVars[i].varType)
			{
				case VARTYPE_NUM:
					CVI_PropertySetValNumber (strippedLookupString, atof(glbSetVars[i].varVal), errMsg);
					break;
				case VARTYPE_STR:
					CVI_PropertySetValString (strippedLookupString, glbSetVars[i].varVal, errMsg);
					break;
				case VARTYPE_BOOL:
					CVI_PropertySetValBoolean (strippedLookupString, atoi(glbSetVars[i].varVal) == VARIANT_TRUE, errMsg);
					break;
			}
#endif
			CmtGetLockEx (glbSetVarLock, 1, 2000, &hasLock);
			if (hasLock)
			{
				// Shift array
				memcpy (glbSetVars+i, glbSetVars+i+1, sizeof(TSSeqVar)*(100-i-1));
				--glbNumSetting;
				--i;
				CmtReleaseLock (glbSetVarLock);
			}
		}
	}
}
//! \cond
/// REGION END

/// REGION START Place Holders
//! \endcond
/* 20210521Biye: There's gotta be a better way to do this, especially when there will be a 3rd level? */
//!@{
/***************************************************************************//*!
* \brief Placeholder blank function calls in order for macros to work
*******************************************************************************/
void TSMsg_UI (IN int NestNum, IN char Params[16][512])
{
	
}

void TSMsg_TEST (IN int NestNum, IN char Params[16][512])
{
	
}

void TSMsg_DISP_NIL (IN int NestNum, IN char Params[16][512])
{
	
}

void TSMsg_VAR (IN int NestNum, IN char Params[16][512])
{
	
}
//!@}
//! \cond
/// REGION END
//! \endcond
//!@}