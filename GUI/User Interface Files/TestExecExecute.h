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

#define  EXEC_PANEL                       1
#define  EXEC_PANEL_LONG_TERM             2       /* control type: string, callback function: (none) */
#define  EXEC_PANEL_SHORT_TERM            3       /* control type: string, callback function: (none) */
#define  EXEC_PANEL_TESTINFO              4       /* control type: table, callback function: (none) */
#define  EXEC_PANEL_LED_PURGE             5       /* control type: LED, callback function: (none) */
#define  EXEC_PANEL_LED_CAPABILITY        6       /* control type: LED, callback function: (none) */
#define  EXEC_PANEL_LED_TRACE             7       /* control type: LED, callback function: (none) */
#define  EXEC_PANEL_ITAC_TIMER            8       /* control type: timer, callback function: (none) */
#define  EXEC_PANEL_CNT_TIMER             9       /* control type: timer, callback function: (none) */
#define  EXEC_PANEL_LED_REJECT            10      /* control type: LED, callback function: (none) */
#define  EXEC_PANEL_Shift_Timer           11      /* control type: timer, callback function: (none) */
#define  EXEC_PANEL_ARX_LOGO2             12      /* control type: picture, callback function: (none) */
#define  EXEC_PANEL_IMG_LOGO              13      /* control type: picture, callback function: (none) */
#define  EXEC_PANEL_LT_YIELD              14      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_LT_FAIL               15      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_LT_PASS               16      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_IDLE_TIME             17      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_ST_YIELD              18      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_ST_FAIL               19      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_TAB_NESTS             20      /* control type: tab, callback function: (none) */
#define  EXEC_PANEL_ST_PASS               21      /* control type: string, callback function: (none) */
#define  EXEC_PANEL_IDLE_TIMER            22      /* control type: timer, callback function: ArxCB_IdleTimer */

#define  NEST_PANEL                       2
#define  NEST_PANEL_ADDITIONAL_RESULTS    2       /* control type: listBox, callback function: ArxCB_ExpandTestInfo */
#define  NEST_PANEL_DATA_LISTBOX          3       /* control type: listBox, callback function: ArxCB_ExpandTestInfo */
#define  NEST_PANEL_SERIAL_NUMBER         4       /* control type: string, callback function: (none) */
#define  NEST_PANEL_UUT_PRESENCE          5       /* control type: LED, callback function: (none) */
#define  NEST_PANEL_UUT_IN_POSITION       6       /* control type: LED, callback function: (none) */
#define  NEST_PANEL_NEST_FAIL             7       /* control type: string, callback function: (none) */
#define  NEST_PANEL_NEST_TEST_TIME        8       /* control type: string, callback function: (none) */
#define  NEST_PANEL_CYCLE_CNT             9       /* control type: string, callback function: (none) */
#define  NEST_PANEL_NEST_PASS             10      /* control type: string, callback function: (none) */
#define  NEST_PANEL_NEST_YIELD            11      /* control type: string, callback function: (none) */
#define  NEST_PANEL_LABEL_STATUS          12      /* control type: textMsg, callback function: (none) */
#define  NEST_PANEL_LABEL_INSTRUCTIONS    13      /* control type: textMsg, callback function: (none) */
#define  NEST_PANEL_LABEL_ROT_STATUS      14      /* control type: textMsg, callback function: (none) */
#define  NEST_PANEL_LABEL_ROT             15      /* control type: textMsg, callback function: (none) */
#define  NEST_PANEL_NEST_S2S_TIME         16      /* control type: string, callback function: (none) */


     /* Control Arrays: */

#define  GrpCtrlCounters                  1
#define  GrpCtrlCountersLT                2
#define  GrpCtrlCountersST                3
#define  GrpCtrlLEDs                      4
#define  GrpCtrlLogoLEDs                  5
#define  GrpCtrlLogos                     6
#define  GrpCtrlNestCounters              7
#define  GrpCtrlNestLEDs                  8
#define  GrpCtrlNestRotStatus             9
#define  GrpCtrlNestStatus                10
#define  GrpCtrlNestTestStatus            11
#define  GrpCtrlTimers                    12

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ArxCB_ExpandTestInfo(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ArxCB_IdleTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif