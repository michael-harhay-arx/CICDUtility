/***************************************************************************//*!
* \file TSTypeInsertMacros.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 4/9/2021 7:49:31 PM
* \brief These macros are used to sync the enums defined here and various sequences
* 	by creating the enums as data types within the sequence file's Custom Data Types.
*******************************************************************************/

#ifndef __TSTypeInsertMacros_H__
#define __TSTypeInsertMacros_H__

#ifdef __cplusplus
    extern "C" {
#endif

//! \cond
/// REGION START Message Macros
//! \endcond
/***************************************************************************//*!
* \defgroup msg Sequencer<->UI Communication
* @{
*******************************************************************************/
/***************************************************************************//*!
* \brief Used to define all levels of message functions. The array index is
* 	arbitrary and hardcoded but must be under MAX_KEYS.
*******************************************************************************/
#define MsgKeys\
	MsgKey(glbMsgStruct[0],UI,SET)\
	MsgKey(glbMsgStruct[1],TEST,RESU)\
	MsgKey(glbMsgStruct[1],TEST,CLEA)\
	MsgKey(glbMsgStruct[1],TEST,INFO)\
	MsgKey(glbMsgStruct[2],DISP,NIL)\
	MsgKey(glbMsgStruct[4],VAR,GET)\
	MsgKey(glbMsgStruct[4],VAR,SET)

/***************************************************************************//*!
* \brief First definition. Used to transfer MSG codes below to TestStand as a struct
*******************************************************************************/
#define MsgKey(tmp1,b,c)\
	CTRL(MSG,b)\
	CTRL(MSG,b##_##c)

/***************************************************************************//*!
* @{
* \brief Message keys. These are defined in TestStand using the enum definition
* 	function (ENUM(enumName,enumFields)). If the message function does not have a 2nd layer of sub functions,
* 	add _NIL to the code of 999 to make sure it fits under the macro definition
* 	structure. The code must reflect the MsgKeys index assigned above by
* 	(index+1)*1000
*******************************************************************************/
#define MSG_UI			1000
#define MSG_UI_SET		1100
#define MSG_TEST		2000
#define MSG_TEST_RESU	2100
#define MSG_TEST_CLEA	2200
#define MSG_TEST_INFO	2300
#define MSG_DISP		3000
#define MSG_DISP_NIL	3999
#define MSG_VAR			5000
#define MSG_VAR_GET		5100
#define MSG_VAR_SET		5200
/*!
* @}
* @}
*/

//! \cond
/// REGION END

/// REGION START Enums
//! \endcond
/***************************************************************************//*!
* \defgroup tsenum Enums defined in TestStand
* @{
*******************************************************************************/
/***************************************************************************//*!
* \brief Used as a null option for all enums
*******************************************************************************/
#define UI_NONE		0

/***************************************************************************//*!
* @{
* \brief Defines all UI elements as enums to be created as structs in TestStand
*******************************************************************************/
#define ENUM_FIELDS_LEDS\
		CTRL(UI,NONE)\
		EXEC_CTRL(LED_TRACE)\
		EXEC_CTRL(LED_REJECT)\
		EXEC_CTRL(LED_CAPABILITY)\
		EXEC_CTRL(LED_PURGE)\
		NEST_CTRL(UUT_PRESENCE)\
		NEST_CTRL(UUT_IN_POSITION)

#define ENUM_FIELDS_COUNTERS\
		CTRL(UI,NONE)\
		EXEC_CTRL(ST_PASS)\
		EXEC_CTRL(ST_FAIL)\
		EXEC_CTRL(LT_PASS)\
		EXEC_CTRL(LT_FAIL)\
		EXEC_CTRL(IDLE_TIME)\
		NEST_CTRL(NEST_PASS)\
		NEST_CTRL(NEST_FAIL)

#define ENUM_FIELDS_TEXT\
		CTRL(UI,NONE)\
		EXEC_CTRL(TESTINFO)\
		NEST_CTRL(SERIAL_NUMBER)\
		NEST_CTRL(LABEL_INSTRUCTIONS)\
		NEST_CTRL(LABEL_STATUS)\
		NEST_CTRL(LABEL_ROT_STATUS)\
		NEST_CTRL(NEST_TEST_TIME)\
		NEST_CTRL(CYCLE_CNT)\
		NEST_CTRL(NEST_S2S_TIME)
		
#define ENUM_FIELDS_TAB\
		CTRL(UI,NONE)\
		EXEC_CTRL(TAB_NESTS)

#define ENUM_FIELDS_CVI_COLORS\
		CTRL(VAL,BLACK)\
		CTRL(VAL,BLUE)\
		CTRL(VAL,GREEN)\
		CTRL(VAL,RED)\
		CTRL(VAL,YELLOW)\
		CTRL(VAL,WHITE)

#define ENUM_FIELDS_POPUPS\
		CTRL(POPUP,ARXERROR)\
		CTRL(POPUP,ARXWARNING)

#define ENUM_FIELDS_MSGS\
	MsgKeys

#define ALL_ENUMS\
	int numSubs = 0;\
	int subSize = 128;\
	char subNames[20][128] = {0};\
	int subVals[20] = {0};\
	ENUM(ARX_UI_LED,ENUM_FIELDS_LEDS)\
	ENUM(ARX_UI_COUNTER,ENUM_FIELDS_COUNTERS)\
	ENUM(ARX_UI_TEXT,ENUM_FIELDS_TEXT)\
	ENUM(ARX_UI_TAB,ENUM_FIELDS_TAB)\
	ENUM(ARX_UI_COLOR,ENUM_FIELDS_CVI_COLORS)\
	ENUM(ARX_UI_POPUP,ENUM_FIELDS_POPUPS)\
	ENUM(ARX_UI_MSG,ENUM_FIELDS_MSGS)
//!@}
//! \cond
/// REGION END

/// REGION START Definition Macros
//! \endcond
//!@{
/***************************************************************************//*!
* \brief Helper definitions to quickly define elements from different panels
*******************************************************************************/
#define EXEC_CTRL(ctrl)\
	CTRL(EXEC_PANEL,ctrl)

#define NEST_CTRL(ctrl)\
	CTRL(NEST_PANEL,ctrl)

#define CTRL(prefix,ctrl)\
	strcpy (subNames[numSubs],#ctrl);\
	subVals[numSubs++] = prefix##_##ctrl;
//!@}
//! \cond
/// REGION END

/// REGION START Insert Macros
//! \endcond
/***************************************************************************//*!
* \brief Calls TSUtils_CreateCustomType from TSUtils_LIB to create all the enums
* 	defined above in DefineNewTypesHereThenCopyElsewhere.seq
* 
* \sa ArxUtil_UpdateStructsToTS
*******************************************************************************/
#define ENUM(enumName,enumFields)\
	numSubs = 0;\
	memset (subNames, 0, sizeof(subNames));\
	memset (subVals, 0, sizeof(subVals));\
	enumFields;\
	errChk( TSUtils_CreateCustomType(gMainWindow.engine, TS_PropValType_Enum, 0, seqPath, #enumName, numSubs, subSize, (char**)subNames, subVals, errMsg));
//!@}
//! \cond
/// REGION END
//! \endcond
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TSTypeInsertMacros_H__ */
