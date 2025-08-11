/***************************************************************************//*!
* \todo
* * SetTSSeqVarStruct
* * About panel
* * Variable watch panel
* * main input flags
*******************************************************************************/
/***************************************************************************//*!
* \mainpage Main Page
* 
* \section changelog Change Log
* <table>
* <tr><th>Version<th>Author<th>Date<th>Changes
* <tr><td>0.0.1
* <td>Biye Chen
* <td>3/1/2021
* <td>
* <ul>
* <li>Initial release
* <li>Improvements from previous sequencer
* 	<ul>
* 	<li>Better code separation and more comments on each section
* 	<li>Separated UIR panels, less cluster
* 	<li>Reorganized uir layout
* 	<li>UI auto resize
* 	<li>Features debug mode (TestStand) and execution mode (CVI)
* 	<li>Removed all old/outdated code from past sequencers
* 	<li>Implemented new messaging system
* 	<li>Authorization based on TestStand users
* 	</ul>
* </ul>
* </table>
*******************************************************************************/
/***************************************************************************//*!
* \page gennotes General Notes
* 
* \section seqsetup Setup Steps Before using the Sequencer
* These steps setup the prerequisites for running the sequencer and must be done
* 	first!
* 
* <table>
* <tr><th>Action<th>Importance
* <tr>
* <td>Modify TestStand Search Path such that it includes the project's
* Arxtron Sequencer.seq and ProjectSpecificSequencer.seq
* <td>The search path is used to find these sequence files to obtain reference
* handles for them. The reference handles are used in order for the UI to
* interact with the sequence files.
* <tr>
* <td>Run TestStandConfiguration sequence in the OneTimeSetup.seq file
* <td>This sequence sets up various TestStand options optimized for this sequencer.
* 	<ul>
* 	<li>Creates a password for the administrator user (default: Admin)
* 	<li>Creates the operator user
* 	<li>Sets TestStand to use the auto login with Windows user option
* 	<li>Sets TestStand to use either Parallel or Batch model (see parameters)
* 	<li>Sets the RunTime Error Handler to use ShowDialog
* 	<li>Sets TestStand to use unique IDs to reference sequence steps
* 	<li>Sets up Interactive Execution options
* 	<li>Sets up default tracing options
* 	<li>Sets up timout options when closing the sequencer
* 	<li>Sets Automatic Conflict Resolution to never
* 	<li>Disables result recording
* 	<li>Sets the default adapter to CVI
* 	<li>Adds C:\Arxtron\Libraries to the search paths
* 	<li>Disables report generation
* 	<li>Copies custom icon for Sequencer sequences
* 	</ul>
* <tr>
* <td>Add \%TESTSTAND\%\Components\Models\TestStandModels\modelsupport2 to Windows
* system paths
* <td>This is required in order to use the modelsupport2.dll to load the
* **Number of Test Sockets** value directly from TestStand settings
* </table>
* 
* \section warnings Warnings
* <ul>
* <li>(2022/01/31) Do NOT include ArxtronToolsLib.h in this project. The tsErrChk macro conflicts
* with the one defined in tsutil.h. This was done coincidentally and future versions
* of ArxtronToolsLib.h should change its macro naming.
* <li>All handles obtained in the code using TS_ or TSUI_ functions should be
* released using CA_DiscardObjHandle. Unreleased handles will keep TestStand
* open in the background and undefined behavior may occur. As much as possible,
* keep handles local to the function that needs them. Global handles
* should be released in ApplicationMgr_OnExitApplication
* </ul>
*******************************************************************************/