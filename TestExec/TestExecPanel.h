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
#define  ABOUTBOX_TEXTMSG_5               4       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_VERSION                 5       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_LICENSE                 6       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_ENGINE_VERSION          7       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_PICTURE_2               8       /* control type: picture, callback function: (none) */
#define  ABOUTBOX_USER_INT_MSG            9       /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_VERSION_2               10      /* control type: textMsg, callback function: (none) */
#define  ABOUTBOX_VERSION_3               11      /* control type: textMsg, callback function: (none) */

#define  MAINPANEL                        2       /* callback function: MainPanelCallback */
#define  MAINPANEL_LISTBAR                2       /* control type: activeX, callback function: (none) */
#define  MAINPANEL_APPLICATIONMGR         3       /* control type: activeX, callback function: (none) */
#define  MAINPANEL_SEQUENCEFILEVIEWMGR    4       /* control type: activeX, callback function: (none) */
#define  MAINPANEL_EXECUTIONVIEWMGR       5       /* control type: activeX, callback function: (none) */
#define  MAINPANEL_STATUSBAR              6       /* control type: activeX, callback function: (none) */
#define  MAINPANEL_TAB                    7       /* control type: tab, callback function: TabControlCallback */

#define  SPLASH                           3
#define  SPLASH_PICTURE_2                 2       /* control type: picture, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_STEPLISTVIEW            2       /* control type: activeX, callback function: (none) */
#define  TABPANEL_VARIABLES               3       /* control type: activeX, callback function: (none) */
#define  TABPANEL_SEQUENCES               4       /* control type: activeX, callback function: (none) */
#define  TABPANEL_INSERTIONPALETTE        5       /* control type: activeX, callback function: (none) */
#define  TABPANEL_ENTRYPOINT1BTN          6       /* control type: activeX, callback function: (none) */
#define  TABPANEL_ENTRYPOINT2BTN          7       /* control type: activeX, callback function: (none) */
#define  TABPANEL_RUNSEQUENCEBTN          8       /* control type: activeX, callback function: (none) */
#define  TABPANEL_SEQUENCEFILELABEL       9       /* control type: activeX, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_2_STEPLISTVIEW          2       /* control type: activeX, callback function: (none) */
#define  TABPANEL_2_VARIABLES             3       /* control type: activeX, callback function: (none) */
#define  TABPANEL_2_CALLSTACK             4       /* control type: activeX, callback function: (none) */
#define  TABPANEL_2_THREADS               5       /* control type: activeX, callback function: (none) */
#define  TABPANEL_2_BREAKRESUMEBTN        6       /* control type: activeX, callback function: (none) */
#define  TABPANEL_2_TERMINATERESTARTBTN   7       /* control type: activeX, callback function: (none) */
#define  TABPANEL_2_EXECUTIONLABEL        8       /* control type: activeX, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_3_REPORTVIEW            2       /* control type: activeX, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  BAR                              1
#define  BAR_FILE                         2
#define  BAR_EDIT                         3
#define  BAR_EXECUTE                      4
#define  BAR_DEBUG                        5
#define  BAR_CONFIGURE                    6
#define  BAR_TOOLS                        7
#define  BAR_HELP                         8
#define  BAR_HELP_ABOUT                   9       /* callback function: AboutBoxMenuItemCallback */


     /* Callback Prototypes: */

void CVICALLBACK AboutBoxMenuItemCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK AboutBoxOKCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TabControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif