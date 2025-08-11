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

#define  DEBUGPANEL                       1
#define  DEBUGPANEL_LISTBAR               2       /* control type: activeX, callback function: (none) */
#define  DEBUGPANEL_APPLICATIONMGR        3       /* control type: activeX, callback function: (none) */
#define  DEBUGPANEL_SEQUENCEFILEVIEWMGR   4       /* control type: activeX, callback function: (none) */
#define  DEBUGPANEL_EXECUTIONVIEWMGR      5       /* control type: activeX, callback function: (none) */
#define  DEBUGPANEL_TAB                   6       /* control type: tab, callback function: TabControlCallback */

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

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK TabControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif