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

#define  UE_PANEL                         1       /* callback function: UserManagementCB_EditPanel */
#define  UE_PANEL_SEL_USER                2       /* control type: ring, callback function: UserManagementCB_EditSelectUser */
#define  UE_PANEL_EDIT_USERNAME           3       /* control type: string, callback function: UserManagementCB_EditEnter */
#define  UE_PANEL_EDIT_PASSWORD           4       /* control type: string, callback function: UserManagementCB_EditEnter */
#define  UE_PANEL_CONFIRM_EDIT_PASSWORD   5       /* control type: string, callback function: UserManagementCB_EditEnter */
#define  UE_PANEL_EDIT_ADMIN_LEVEL        6       /* control type: numeric, callback function: UserManagementCB_EditEnter */
#define  UE_PANEL_OK                      7       /* control type: command, callback function: UserManagementCB_EditConfirm */
#define  UE_PANEL_CANCEL                  8       /* control type: command, callback function: UserManagementCB_EditCancel */
#define  UE_PANEL_ERRMSG                  9       /* control type: textMsg, callback function: (none) */
#define  UE_PANEL_DELETE                  10      /* control type: command, callback function: UserManagementCB_Delete */


     /* Control Arrays: */

#define  EDIT_FIELDS                      1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK UserManagementCB_Delete(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_EditCancel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_EditConfirm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_EditEnter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_EditPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserManagementCB_EditSelectUser(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif