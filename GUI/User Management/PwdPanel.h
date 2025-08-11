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

#define  PWD_PANEL                        1       /* callback function: UserManagementCB_Panel */
#define  PWD_PANEL_PASSWORD               2       /* control type: string, callback function: UserManagementCB_Password */
#define  PWD_PANEL_OK                     3       /* control type: command, callback function: UserManagementCB_Confirm */
#define  PWD_PANEL_CANCEL                 4       /* control type: command, callback function: UserManagementCB_Cancel */
#define  PWD_PANEL_USERNAME               5       /* control type: ring, callback function: (none) */
#define  PWD_PANEL_ERRMSG                 6       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK UserManagementCB_Cancel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_Confirm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_Panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_Password(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif