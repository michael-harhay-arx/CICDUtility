/***************************************************************************//*!
* \file ProjectSpecificGUI_LIB.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 5/28/2021 5:52:06 PM
* \brief Use this project to define UI elements/functions that are specific to
* 	the project being developed. The base UI should only contain functions usable
* 	in all projects.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include <userint.h>
#include <ansi_c.h>
#include <wtypes.h>
#include <utility.h>
#include "ArxtronToolslib.h"
#include "ProjectSpecificGUI_LIB.h"
#include "UIUtility.h"
#include "UIDefinitions.h"

//==============================================================================
// Constants

//==============================================================================
// Types

/***************************************************************************//*!
* \brief Builds Standard Callback Data to be assigned to menu item
* 
* Callback Data Structure:  SequenceFileVarName;VarType;VarValue.
*	When menu item is pressed callback data will be parsed and put into
*	VarSet global array in TestExec which then modifies TestStand Variable value.
*
*	Most of the time boolean flag will be set in teststand to let teststand know
*	that a menu item has been pressed but data can also be made with type NUM and 
*	STR.
*
* \param [in] menuitemID		menu item handle used to create const string to store callback data
* \param [in] varname			teststand variable name ex. FileGlobals.ProjectSpecific
* \param [in] vartype			teststand variable type ex. VARTYPE_BOOL
* \param [in] varval			teststand variable value ex. VARIANT_TRUE
*******************************************************************************/
#define BuildCallBackMessage(menuitemID, varname, vartype, varval)\
	const char CD##menuitemID[128] = {0};\
	if((vartype == VARTYPE_BOOL)|| (vartype == VARTYPE_NUM))\
	{\
		sprintf(CD##menuitemID, "%s;%d;%d",varname,vartype,varval);\
	} else if(vartype == VARTYPE_STR)\
	{\
		sprintf(CD##menuitemID, "%s;%d;%s",varname,vartype,varval);\
	}

/***************************************************************************//*!
* \brief Creates new project specific menu item
*
* \param [in] menuID			menu handle for menu in which item is created
* \param [in] menuitemID		menu item handle
* \param [in] beforemenuitemID	Menu item ID above which to insert the new item, pass 0
									to place at bottom of list
* \param [in] ctrlname			display text on menu item
* \param [in] varname			teststand variable name ex. FileGlobals.ProjectSpecific
* \param [in] vartype			teststand variable type ex. VARTYPE_BOOL
* \param [in] varval			teststand variable value ex. VARIANT_TRUE
* \param [in] disable			enable (0)/disable (1) menu item
*******************************************************************************/
#define NewPrjMenuItem(menuID, menuitemID, beforemenuitemID, ctrlname, varname, vartype, varval, disable)\
	BuildCallBackMessage(menuitemID, varname, vartype, varval);\
	menuitemID = NewMenuItem(glbPRJMenuBar, menuID, ctrlname, beforemenuitemID?beforemenuitemID:-1, 0, *CallBack, (void*) CD##menuitemID);\
	SetMenuBarAttribute(glbPRJMenuBar, menuitemID, ATTR_DIMMED, disable);

/***************************************************************************//*!
* \brief Creates new project specific menu
*
* \param [in] menuID			menu handle for menu in which item is created
* \param [in] ctrlname			display text on menu
* \param [in] beforemenuID		Menu ID above which to insert the new item, pass 0
									to place at bottom of list
* \param [in] disable			enable (0)/disable (1) menu item
*******************************************************************************/
#define NewPRJMenu(menuID, ctrlname, beforemenuID, disable)\
	if(menuID) {DiscardMenu(glbPRJMenuBar, menuID); menuID = 0;}\
	menuID = NewMenu(glbPRJMenuBar, ctrlname, beforemenuID?beforemenuID:-1);\
	SetMenuBarAttribute(glbPRJMenuBar, menuID, ATTR_DIMMED, disable);
//==============================================================================
// Static global variables

//TestExec Menu Handles
static int glbPRJMenuEdit = 0;
static int glbPRJMenuExecute = 0;
static int glbPRJMenuConfigure = 0;
static int glbPRJMenuTools = 0;
static int glbPRJMenuDebug = 0;
static int glbPRJMenuBar = 0;

//Project Specific Menu Handles
static int glbPrjMenuID = 0;
static int glbPrjMenuItemID = 0;
//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Custom UIs
//! \endcond

/***************************************************************************//*!
* \brief Add project specific menus to the UI
*
* project specific menus can be made with NewPRJMenu macro. project specific menu
*	items can be made with NewPrjMenuItem macro. Whenever project specific menu item 
*	is pressed ArxCB_SendProjectSpecificMenuStatus is called and callback data is parsed
*	and pushed into VarSet global array in TestExec.
*
* \param [in] CallBack			pass pointer to ArxCB_SendProjectSpecificMenuStatus from TestExec
* \param [in] IsAdmin			IsAdmin flag to enable/disable menus and menu items
* \param [in] BeforeMenu		(optional) Menu ID above which to insert the new item
* \param [in] BeforeMenuItem	(optional) Menu item ID above which to insert the new item
*******************************************************************************/
int PRJGUI_AddProjectMenus (void CVICALLBACK (*CallBack)(int, int, void *, int), int IsAdmin, OPT int BeforeMenu, OPT int BeforeMenuItem)
{
	int error = -99999;
	char errmsg[ERRLEN] = {0};
	error = 0;
	
	/* Example of how to create project specific menus and menu items */
	NewPRJMenu(glbPrjMenuID, "Project Specific Menu", 0, !IsAdmin);
	
	/* Menu Items can also be created for Test Exec Default Menus including:
		Edit, Execute, Configure, Tools, Debug
	*/
	NewPrjMenuItem(glbPrjMenuID, glbPrjMenuItemID, 0, "Project Specific Menu Item", "FileGlobals.ProjectSpecific", VARTYPE_BOOL, VARIANT_TRUE, !IsAdmin);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Get Default Menu Control IDs (assumption that these default menus will never change)
* Default Menu Control IDs: MenuBar, MenuEdit, MenuExecute, MenuDebug, MenuConfigure, MenuTools
*******************************************************************************/
void PRJGUI_GetDefaultMenus (int MenuBar, int MenuEdit, int MenuExec, int MenuDebug, int MenuConfig, int MenuTools)
{
	glbPRJMenuBar = MenuBar;
	glbPRJMenuEdit = MenuEdit;
	glbPRJMenuExecute = MenuExec;
	glbPRJMenuDebug = MenuDebug;
	glbPRJMenuConfigure = MenuConfig;
	glbPRJMenuTools =  MenuTools;
}

/***************************************************************************//*!
* \brief Add project specific controls to the UI
*******************************************************************************/
int PRJGUI_AddProjectGUIControls (int ParentPanel)
{
	int error = -99999;
	char errmsg[ERRLEN] = {0};
	error = 0;
	
	int screenHeight = 0, screenWidth = 0;
	int panelLeft = 0, panelWidth = 0;
	
	//NewCtrl (ParentPanel,CTRL_SQUARE_COMMAND_BUTTON,"Label",top,left);
	//SetCtrlAttribute (ParentPanel,ID,ATTR_CALLBACK_FUNCTION_POINTER,fn);
	
Error:
	if (error) error = ARXERR_CUSTOM_UI_FAIL;
	return error;
}

/***************************************************************************//*!
* \brief De-initialize any custom panels created
*******************************************************************************/
void PRJGUI_CloseCustomPanels (void)
{
	
}
//! \cond
/// REGION END

/// REGION START Custom UI Callbacks
//! \endcond
/***************************************************************************//*!
* \brief Custom menu callbacks
*******************************************************************************/
int CVICALLBACK MenuCallback (int menuBar, int menuItem, void *callbackData, int panel)
{
	
	
	return 0;
}

/***************************************************************************//*!
* \brief Custom control callbacks
*******************************************************************************/
int CVICALLBACK ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	
	
	return 0;
}
//! \cond
/// REGION END

/// REGION START Public UI Functions
//! \endcond

//! \cond
/// REGION END
//! \endcond
