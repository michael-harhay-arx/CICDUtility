//! \cond
/// REGION START Main Page
//! \endcond
/***************************************************************************//*!
* \mainpage Introduction
* 
* \tableofcontents
* 
* \section ind_purpose Purpose
* \if TESTSTAND
* This document is meant for users of the Arxtron Sequencer Software along with
* 	NI TestStand.
* \else
* This document is meant for users of the Arxtron Sequencer Software along with
* 	LabWindows CVI.
* \endif
* 
* It is intended to demonstrate how it can be used and configured, 
* 	where to find specific features, and what indicators/outputs mean.
*******************************************************************************/
//! \cond
/// REGION END

/// REGION START Folder Structure
/* PROJECTADD
[ProjectNumber]
*/
//! \endcond
/***************************************************************************//*!
* \page folder Folder Structure
* 
* <pre>
* Root (C:\Arxtron)
*   > [ProjectNumber]
*       > \ref config "Configuration"
*       > \ref gui "GUI"
*           > Win32
*               > TestExec.exe (Sequencer executable)
*       > Source
*           > CombinedDebugPanel
*           > TestSteps
*   > \ref log "Logs"
*       > CSVLog
*       > DBLog
*       > INILog
* </pre>
*******************************************************************************/
//! \cond
/// REGION END

/// REGION START User Interface
/* PROJECTADD
[Customer]
*/
//! \endcond
/***************************************************************************//*!
* \page gui User Interface
* 
*************************************
* \section gui_main Main Interface
*************************************
* \addindex GUI
* \addindex User Interface
* 
* This is the main interface of the Arxtron Sequencer.  All GUI elements are set
* 	to dynamically rearrange depending on the window size.
* \if TESTSTAND
* 
* \elseif CVIONLY
* The default window size and location is based on the \ref config_seq "Sequencer configuration file".
* \endif
* 
* The GUI does have a non-configurable minimum size to prevent elements from becoming illegible.
* \anchor guiimgtop
* \image html gui_main.png "Main User Interface" 
* \image latex gui_main.png "Main User Interface" 
* 
*************************************
* \subsection gui_main_info 1. Test Program Info
*************************************
* \addindex Test Program Info
* This is a table meant to display various parameters about the current test.
* 	Typically this table indicates parameters from the selected \ref config_recipe "recipe",
* 	but can be customized to any static or even dynamic information.
* 
*************************************
* \subsection gui_main_glbcnt 2. Global Counters
*************************************
* \addindex Global Counters
* The counters here keep track of total number of pass and fail units, as well as
* 	the yield percentage across all nests.  Short Term counters only count number
* 	of units since the software was started, Long term counters keeps the entire
* 	historical count.
* 
* The IDLE Time timer increments when there are no nests actively testing, and is
* 	reset when any testing activity is detected.
* 
*************************************
* \subsection gui_main_led 3. LEDs
*************************************
* \addindex LEDs (Panel)
* <table>
* <caption id="gui_main_led_tbl">GUI LEDs</caption>
* <tr><th>LED<th>Usage
* <tr><td>Purge
* <td>ON: \ref gui_menu_exec "Purging Mode active"
* <tr><td>Trace Status
* <td>ON (Green): Traceability enabled and active\n
* ON (Red): Traceability disabled or failed to initialize
* <tr><td>Reject Chute Full
* <td>ON: Reject Chute full sensor or condition is active
* \if TECHDOC
* <tr><td>Capability
* <td>ON: \ref gui_modes_cap "Capability Mode" active
* </table>
* \addindex Capability
* \else
* </table>
* \endif
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \subsection gui_main_nestled 4. Nest LEDs
*************************************
* \addindex LEDs (Nest)
* <table>
* <caption id="gui_main_nestled_tbl">Nest LEDs</caption>
* <tr><th>LED<th>Usage
* <tr><td>Presence
* <td>ON: DUT presence sensor active
* <tr><td>Position
* <td>ON: DUT placement is acceptable
* </table>
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \subsection gui_main_sn 5. Serial Number
*************************************
* Displays the serial number of the current DUT.
* 
*************************************
* \subsection gui_main_nestcnt 6. Nest Timers and Counters
*************************************
* \addindex Nest Counters
* \addindex Nest Timers
* <table>
* <caption id="gui_main_nestcnt_tbl">Nest Timers and Counters</caption>
* <tr><th>Timer/Counter<th>Usage
* <tr><td>Cycle CNT
* <td>Number of test cycles ran by the nest.  This could be set to only count during actuation to be used for maintenance tracking.
* <tr><td>Test Time
* <td>The time it took to execute the test sequence.  This typically excludes handling time.
* <tr><td>S2S Time
* <td>Start-to-start time.  The time between the starting of the previous test sequence and the current test sequence.  This time should equal Test Time + Handling Time + Nest Idle Time.
* <tr><td>Nest Pass/Fail Counters
* <td>Similar to the \ref gui_main_glbcnt "global counters", by default, this is a long term counter.
* </table>
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \subsection gui_main_result 7. Test Result Display
*************************************
* \addindex Test Result Display
* This displays the list of test results from the test sequence.  The tests displayed
* 	depends on the \ref config_teststeps "TestDebugLevel", \ref config_system "DebugLevel",
* 	and \ref config_system "DebugLevelWithUser" settings.
* 
* By default, the result contains the following information.
* <pre>
* TestNumber  TestName                      PASS/FAIL     TestValue             TestTime
* </pre>
* This line of text spans 900 pixels by default.  If the display width is shorter
* 	than this, double clicking on this area will toggle the result section's width expansion.
* 
* The resulting text colour also gives a quick indication of the status of the test.
* <table>
* <caption id="gui_main_result_tbl">Test Result Colours</caption>
* <tr><th>Colour<th>Meaning
* <tr><td>Green<td>Pass
* <tr><td>Red<td>Fail
* <tr><td>Blue<td>\ref config_teststeps_tbl "Report"
* <tr><td>Grey<td>\ref config_teststeps_tbl "Skipped"
* </table>
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \subsection gui_main_notes 8. Additional Result Display
*************************************
* \addindex Test Result Display
* This section is reserved for additional test details and is only populated
* 	at the highest debug level by default.  Typically, this is used for displaying
* 	additional test results, limits, or error messages.
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \subsection gui_main_status 9. Nest Status Bar
*************************************
* \addindex Nest Status Bar
* This section displays the general status of the nest with both text and color indicators.
* <table>
* <caption id="gui_main_status_tbl">Nest Status Colors</caption>
* <tr><th>Colour<th>Meaning
* <tr><td>Blue<td>Idle
* <tr><td>Yellow<td>Testing
* <tr><td>Green<td>Pass
* <tr><td>Red<td>Fail
* </table>
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \subsection gui_main_instr 10. Nest Instruction Text
*************************************
* \addindex Nest Instruction Text
* This section displays instructions for the operator to follow.
* 
* \ref guiimgtop "To Top"
* 
*************************************
* \section gui_menu Menus
*************************************
* \addindex Menu Bar
*************************************
* \subsection gui_menu_file FILE
*************************************
* <table>
* <caption id="gui_menu_file_tbl">File Menu</caption>
* <tr><th>Menu<th>Function
* \if TESTSTAND
* 
* \else
* <tr><td>Login<td>Launches the \ref gui_panels_user "User Login" panel.
* <tr><td>Logout<td>Logs out of the current user.
* \endif
* </table>
* 
* \if TESTSTAND
*************************************
* \subsection gui_menu_edit Edit
*************************************
* \endif
* 
*************************************
* \subsection gui_menu_exec Execute
*************************************
* These menu options are disabled for the operator user by default, and is only accessible with a \ref config_user "higher level account".
* 
* <table>
* <caption id="gui_menu_exec_tbl">Execute Menu</caption>
* <tr><th>Menu<th>Function
* <tr><td>Reload Recipe<td>Change the current recipe via \ref gui_panels_conf_recipe "Recipe Selection" panel.
* <tr><td>Golden Positive<td>Starts the \ref gui_modes_gold "Golden Positive" test.
* <tr><td>Golden Negative<td>Starts the \ref gui_modes_gold "Golden Negative" test.
* <tr><td>Reinject Mode<td>Toggles \ref gui_modes_reinject "Reinject Mode".
* <tr><td>Reset Statistics<td>Resets all pass/fail counters.  Does not include Cycle CNT.
* <tr><td>Toggle Traceability<td>Toggles the traceability setting.
* This toggle is temporary and does not affect the default setting in \ref config_system "SystemConfig".
* </table>
* 
*************************************
* \subsection gui_menu_debug Debug
*************************************
* These menu options are disabled for the operator user by default, and is only accessible with a \ref config_user "higher level account".
* 
* <table>
* <caption id="gui_menu_debug_tbl">Debug Menu</caption>
* <tr><th>Menu<th>Function
* \if TECHDOC
* <tr><td>Capability Mode<td>Toggles \ref gui_modes_cap "Capability Mode".
* \endif
* <tr><td>Watch View<td>
* <tr><td>Debug Panel<td>Launches the \ref gui_panels_debug "Combined Debug Panel".
* </table>
* 
*************************************
* \subsection gui_menu_conf Configure
*************************************
* These menu options are disabled for the operator user by default, and is only accessible with a \ref config_user "higher level account".
* 
* <table>
* <caption id="gui_menu_conf_tbl">Configure Menu</caption>
* <tr><th>Menu<th>Function
* \if TESTSTAND
* <tr><td>Edit Recipe/System Config/Station Config<td>Launches the \ref gui_panels_conf "Configuration Editor" for the selected file.
* \else
* <tr><td>Edit Recipe/System Config/Station Config/Test Sequence<td>Launches the \ref gui_panels_conf "Configuration Editor" for the selected file.
* \endif
* </table>
* 
* \if 0
* Currently these menus are empty, add them when they have things to document
*************************************
* \subsection gui_menu_tools Tools
*************************************
*************************************
* \subsection gui_menu_help HELP
*************************************
*************************************
* \subsection gui_menu_proj Project Specific Menu
*************************************
* \endif
* 
*************************************
* \section gui_panels Other Panels
*************************************
*************************************
* \if 0
* PROJECTADD Screenshots and explanations
* \endif
* \subsection gui_panels_debug Combined Debug Panel
*************************************
* \addindex Combined Debug Panel
* Insert screenshots and explanation here.
* 
*************************************
* \subsection gui_panels_user User Login
*************************************
* \addindex User Login
* \if TESTSTAND
* 
* \else
* Logs into an existing user specified in \ref config_user "UserFile".
* \endif
* 
* \image html gui_panels_user.png "User Login"
* \image latex gui_panels_user.png "User Login" width=3.5in
*************************************
* \subsection gui_panels_conf Configuration Editor
*************************************
* \addindex Configuration Editor
* \image html gui_panels_conf.png "Configuration Editor"
* \image latex gui_panels_conf.png "Configuration Editor"
* 
* Can be launched using the \ref gui_menu_conf_tbl "Edit" menu option
* 	within the sequencer software.
* 
* All table cells are considered text boxes and can be edited as such.
* 	Buttons at the bottom allows for adding or removing fields and
* 	entries.
* 
* \addindex Configuration Backups
* Clicking OK will create a back up of the original file
* 	and save the new changes.  Clicking Save As will let you choose
* 	a different location to save the changes and the original file will
* 	stay the same.
* 
* Most configuration files will be using this same editor.
* 
*************************************
* \subsubsection gui_panels_conf_recipe Recipe Selection
*************************************
* \addindex Recipe Selection
* \image html gui_panels_conf_recipe.png "Recipe Selection"
* \image latex gui_panels_conf_recipe.png "Recipe Selection"
* 
* The recipe selection panel is a read-only version of the 
* 	\ref gui_panels_conf "Configuration Editor".  This loads
* 	predefined recipes for the software to run based on 
* 	a \ref config_recipe "local recipe file".
* 
*************************************
* \section gui_modes Operation Modes
* \if 0
* PROJECTADD Legend for how each mode is indicated
* \endif
*************************************
* Different operational modes for the software will be indicated
* 	either by \ref gui_main_nestled "Nest LEDs", \ref gui_main_instr "Nest Instruction Text",
* 	or changing of background colors.
* 
*************************************
* \subsection gui_modes_gold Golden Positive/Negative Test
*************************************
* \addindex Golden Positive Test
* \addindex Golden Negative Test
* This mode starts the execution of a Golden Positive or Golden Negative test.
* 	This mode will only run once by default and will switch back to
* 	normal operation if the test passes.  If the test fails, it will
* 	shut down the software.
* 
*************************************
* \subsection gui_modes_reinject Reinject Mode
*************************************
* \addindex Reinject Mode
* Typically, the tester will reject previously tested parts.  This mode
* 	will skip over the logic that checks for this in order to test
* 	the part again.
* 
* \if TECHDOC
*************************************
* \subsection gui_modes_cap Capability Mode
*************************************
* \addindex Capability Mode
* This mode is typically reserved for Arxtron employee use only.
* 	This mode is used for capability testing the station
* 	before or during the quality control stage of the project.
* \endif
* 
*******************************************************************************/
//! \cond
/// REGION END

/// REGION START Configurations
//! \endcond
/***************************************************************************//*!
* \page config Configurations
* This section of the document goes over everything found in the \ref folder "Configuration" folder.
* 
*************************************
* \section config_exes Executable Tools
* \if 0
* PROJECTADD comment on whether the code needs to be recompiled when certain configs change
* \endif
*************************************
* \addindex Executable Tools
* \addindex INI Editor
* \addindex User Editor
* <table>
* <caption id="config_exes_tbl">Executable Tools</caption>
* <tr><th>Name<th>Purpose<th>Usage
* <tr><td>ConvertINIToHeaderTool
* <td>This is used to generate .h files that include a struct that reflects sections of
* 	the configuration file.  This is a quick way to include the configuration file
* 	structure to be used in the code.  This typically applies to
* 	\ref config_system, \ref config_station, and \ref config_recipe.
* <td>Drag the configuration file on to the executable, a .h file of the same name will be created.
* <tr><td>IniEditor
* <td>This launches the gui_panels_conf "Configuration Editor".  Note that this editor will only
* 	work with ini files that include a Template section and all subsequent sections use the exact same format.
* <td>Drag the ini file on to the executable, the editor will launch.  A password may be required.
* <tr><td>UserEditor
* <td>This is used to edit user name, password, and admin level settings.
* 	Users can only modify other users that are below their current admin level.
* <td>Launch the software and login with the appropriate user.
* </table>
* 
* 
* \if CVIONLY
*************************************
* \section config_seq Sequencer
*************************************
* \addindex GUI Size
* \addindex GUI Position
* This configuration file sets the default GUI size, location, and number of parallel executions
* 	(nests).  The actual layout of the nests will need to be changed within the software source
* 	code.  Typically, the number of nests and their layout is set prior to delivery.
* \endif
* 
*************************************
* \section config_system StationConfig
*************************************
* This configuration file is only used when there are multiple PCs within the same station, and
* 	is used to store settings that are common across all PCs.
* 
*************************************
* \section config_station SystemConfig
* \if 0
* PROJECTADD table explaining project specific parameters and further expand on existing parameters if applicable
* \endif
*************************************
* This configuration file is used to store various project parameters.
* 
* \addindex Disabling Nests
* \addindex Debug Level
* <table>
* <caption id="config_station_tbl">SystemConfig Parameters</caption>
* <tr><th>Parameter<th>Usage
* <tr><td>NestEnable
* <td>1 = Nest is enabled\n 0 = Nest is disabled\n Nests are listed in order from left to right,
* 	where nest 1 is the left most number.
* <tr><td>DebugLevel
* <td>0-7\n This parameter determines how verbose the GUI output is.
* <tr><td>DebugLevelWithUser
* <td>Y/N\n If this setting is enabled (Y), then the DebugLevel is instead set
* 	based on the user logged in.
* </table>
* 
*************************************
* \section config_recipe Recipe
* \if 0
* PROJECTADD Detail explanation of each parameter
* \endif
*************************************
* This file typically stores test parameters relating to a certain type or
* 	line of products.
* 
*************************************
* \section config_teststeps TestSteps
*************************************
* This file contains each possible test that can be performed on the DUT.
* \addindex Test Steps Configuration
* \addindex Test Type: Report
* \addindex Test Retries
* \addindex Test Parameters
* \addindex Test Limits
* 
* <table>
* <caption id="config_teststeps_tbl">TestSteps.ini Fields</caption>
* <tr><th>Field<th>Explanation<th>Comment
* <tr><td>Section Name
* <td>[SectionName]\n The name of the test displayed on the GUI.
* <td>
* <tr><td>TestFuncName
* <td>The function within the TestSteps source code to call.
* <td>
* <tr><td>TestSkip
* <td>0 = Not skipped\n 1 = Skipped
* <td>
* <tr><td>TestType
* <td>1 = Test\n 2 = Report
* <td>A test set to report will never be considered a fail regardless of the test results.
* 	This is used so that the test continues running if it fails and TestAbort is set, or to
* 	gather information on fail data.
* <tr><td>TestAbort
* <td>0 = Continue test sequence when this test fails.\n 1 = Stop the test sequence when this test fails.
* <td>Regardless of the setting, a failed test will cause the test sequence to be considered failed.
* <tr><td>TestRepeat
* <td>n = Maximum number of times this test will retry when it receives a failed result.
* <td>Default 0, if the number is set to above 0, then only the latest attempt of the test result
* 	will be recorded.  If the number of repeats is reached and the test still fails, then
* 	the test will be considered a fail.
* <tr><td>TestParam
* <td>Key/value pair of parameters to pass into the test function.  Formatted as
* 	Key = Value.  Multiple parameters are separated by the character `.
* <td>The value is read as a raw string regardless of content, convertions are done within
* 	the source code for the test function.
* <tr><td>TestLimMin
* <td>Minimum numeric limit compared against the test result.
* <td>
* <tr><td>TestLimMax
* <td>Maximum numeric limit compared against the test result OR exact string match.
* <td>This is the default behavior when comparing strings, and can be changed
* 	depending on project requirements.
* <tr><td>TestUnit
* <td>The unit for the test value.  This is typically only used in logs.
* <td>
* <tr><td>TestDebugLevel
* <td>This corresponds to the \ref config_station "DebugLevel" set either
* 	directly from the configuration or based on the current user login.
* 	The results of this test will only be displayed when the current
* 	software DebugLevel is greater than this number.
* <td>
* </table>
* 
* \if CVIONLY
*************************************
* \section config_user UserFile
*************************************
* This stores encrypted user data used for \ref gui_panels_user "User Login".
* \endif
*******************************************************************************/
//! \cond
/// REGION END

/// REGION START Errors/Warnings
//! \endcond
/***************************************************************************//*!
* \page ew Errors and Warnings
* \if 0
* PROJECTADD Add a table of possible errors, error messages, causes and how to recover from
* 	each. If multiple errors/warnings have the same recovery step, group them together.
* 	If a group of errors can be generalized (eg. Motor errors:) then do that instead
* 	of listing each individual error, or point them towards the manual for that device
* 	if its available. Basically, use discretion and cover important/common ones, don't
* 	make this section overwhelmingly large.
* \endif
*******************************************************************************/
//! \cond
/// REGION END

/// REGION START Logs
//! \endcond
/***************************************************************************//*!
* \page log Logs
* \if 0
* PROJECTADD Logs are typically fairly customized based on the customer, but
* 	perhaps add something about default logs available in the future?
* \endif
*******************************************************************************/
//! \cond
/// REGION END
//! \endcond