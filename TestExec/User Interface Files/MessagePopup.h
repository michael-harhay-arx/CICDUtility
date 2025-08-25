/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ABOUTBOX                         1
#define  ABOUTBOX_OK_BTN                  2       /* control type: command, callback function: AboutBoxOKCallback */
#define  ABOUTBOX_TEXTMSG_4               3       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_COPYRIGHT               4       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_LICENSE                 5       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_ENGINE_VERSION          6       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_USER_INT_MSG            7       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_NI_LOGO                 8       /* control type: picture, callback function: (none) */
#define  ABOUTBOX_VERSION                 9       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_COMP_NAME               10      /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_ARX_LOGO                11      /* control type: picture, callback function: (none) */

#define  POPUP_MSG                        2
#define  POPUP_MSG_TEXTBOX                2       /* control type: textBox, callback function: (none) */
#define  POPUP_MSG_DISMISS                3       /* control type: command, callback function: ArxCB_Dismiss */
#define  POPUP_MSG_EXIT                   4       /* control type: command, callback function: ArxCB_Exit */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AboutBoxOKCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ArxCB_Dismiss(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ArxCB_Exit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif