//==============================================================================
//
// Title:		DBLog_LIB_Helper.c
// Purpose:		A short description of the implementation.
//
// Created on:	11/12/2024 at 3:16:03 PM by Timothius Prajogi.
// Copyright:	Arxtron Technologies Inc.. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "toolbox.h"
#include <formatio.h>
#include "DBLog_LIB_Helper.h"
#include <ansi_c.h>

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***************************************************************************//*!
* \brief From SystemLog_LIB
*******************************************************************************/
void SQLFreeResults (QueryResult *Result)
{
	int numTotalResults = Result->recordCount * Result->resultCount;
	for (int i=0; i<numTotalResults; ++i)
		free(Result->values[i]);
	free (Result->values);
	Result->values = NULL;
	Result->recordCount = Result->resultCount = 0;
}

/***************************************************************************//*!
* \brief From SystemLog_LIB
*******************************************************************************/
int SQLReadCallback (void* data, int argc, char **argv, char **az_col_name)
{
	QueryResult *result = (QueryResult *)data;
	
	++result->recordCount;
	if (0==result->resultCount)
		result->resultCount = argc;
	
	result->values = realloc(result->values, result->recordCount * result->resultCount * sizeof(char*));
	for (int i=0; i<argc; ++i)
		result->values[(result->recordCount-1)*argc+i] = StrDup(argv[i]);
	
    return 0;
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
void UnescapeString (const char *Input, char *Output) 
{
    const char *p = Input;
    char *q = Output;
    
    while (*p != '\0') {
        if (*p == '\\') {
            p++;
            switch (*p) {
                case 't':
                    *q = '\t';
                    break;
                case 'n':
                    *q = '\n';
                    break;
                case 'r':
                    *q = '\r';
                    break;
                case '\\':
                    *q = '\\';
                    break;
                case '\"':
                    *q = '\"';
                    break;
                case '\'':
                    *q = '\'';
                    break;
                default:
                    *q = *p;  // Copy the character as-is if it's not a recognized escape
            }
        } else {
            *q = *p;
        }
        p++;
        q++;
    }
    *q = '\0';
}

/***************************************************************************//*!
* \brief Template for copy pasting
*******************************************************************************/
void ReplaceTokenInString (IN char *Token, IN char *ReplaceWith, IN char *InputStr, char *OutputStr)
{
	char *foundPattern = NULL;
	int tokenLen = strlen (Token);
	int replaceWithLen = strlen (ReplaceWith);
	
	strcpy (OutputStr, InputStr);
	foundPattern = strstr (OutputStr, Token);
	while (foundPattern)
	{
		int restLen = strlen(foundPattern+tokenLen);
		memmove (foundPattern+replaceWithLen, foundPattern+tokenLen, restLen+1);
		memcpy (foundPattern, ReplaceWith, replaceWithLen);		
		
		// Next
		foundPattern = strstr (foundPattern+replaceWithLen, Token);
	}
}
