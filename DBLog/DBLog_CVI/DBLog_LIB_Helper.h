//==============================================================================
//
// Title:		DBLog_LIB_Helper.h
// Purpose:		A short description of the interface.
//
// Created on:	11/12/2024 at 3:16:03 PM by Timothius Prajogi.
// Copyright:	Arxtron Technologies Inc.. All Rights Reserved.
//
//==============================================================================

#ifndef __DBLog_LIB_Helper_H__
#define __DBLog_LIB_Helper_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "DBLog_LIB_Definitions.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

void SQLFreeResults (QueryResult *Result);
int SQLReadCallback (void* data, int argc, char** argv, char** az_col_name);

void UnescapeString (const char *Input, char *Output);
void ReplaceTokenInString (IN char *Token, IN char *ReplaceWith, IN char *InputStr, char *OutputStr);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __DBLog_LIB_Helper_H__ */
