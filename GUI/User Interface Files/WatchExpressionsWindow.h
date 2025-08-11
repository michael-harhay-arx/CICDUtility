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

#define  WATCHPANEL                       1       /* callback function: MainWatchExpressionCallBack */
#define  WATCHPANEL_WRITETABLE            2       /* control type: table, callback function: (none) */
#define  WATCHPANEL_WATCHTABLE            3       /* control type: table, callback function: (none) */
#define  WATCHPANEL_ENABLEGETVAR          4       /* control type: radioButton, callback function: StartWatchExpressionsCheckBox */
#define  WATCHPANEL_SETVALUE              5       /* control type: command, callback function: SetTestStandVariableVal */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK MainWatchExpressionCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetTestStandVariableVal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartWatchExpressionsCheckBox(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif