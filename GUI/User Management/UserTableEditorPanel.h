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

#define  TPWD_PANEL                       1
#define  TPWD_PANEL_PWD2                  2       /* control type: string, callback function: UserManagementCB_PwdField */
#define  TPWD_PANEL_PWD1                  3       /* control type: string, callback function: UserManagementCB_PwdField */
#define  TPWD_PANEL_ERRMSG                4       /* control type: textMsg, callback function: (none) */

#define  UET_PANEL                        2
#define  UET_PANEL_UE_DELETE_ROW          2       /* control type: command, callback function: UserManagementCB_DeleteUser */
#define  UET_PANEL_UE_OK                  3       /* control type: command, callback function: UserManagementCB_EXIT */
#define  UET_PANEL_UE_TABLE               4       /* control type: table, callback function: UserManagementCB_Table */
#define  UET_PANEL_UE_CANCEL              5       /* control type: command, callback function: UserManagementCB_EXIT */
#define  UET_PANEL_COL1                   6       /* control type: textMsg, callback function: (none) */
#define  UET_PANEL_ERRMSG                 7       /* control type: textMsg, callback function: (none) */
#define  UET_PANEL_COL2                   8       /* control type: textMsg, callback function: (none) */
#define  UET_PANEL_MIN_USR_LVL            9       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK UserManagementCB_DeleteUser(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_EXIT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_PwdField(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_Table(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif