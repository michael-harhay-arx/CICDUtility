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

#define  MAINPANEL                        1       /* callback function: MainPanelCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  BAR                              1
#define  BAR_FILE                         2
#define  BAR_HELP                         3
#define  BAR_HELP_ABOUT                   4       /* callback function: AboutBoxMenuItemCallback */


     /* Callback Prototypes: */

void CVICALLBACK AboutBoxMenuItemCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif