<?php
//---------------------------------------------------------------------------------------------------------------------
//   myphp.php
//
//   Contains general purpose and utility routines
//
//   array() = set_field_widths()
//   document()
//   end_document()
//   head()
//   end_head()
//   body()
//   end_body()
//   show_table($tablename, $title, $where = NULL, $orderby = NULL, $show = NULL, $modify = NULL, $delete = NULL, $links = array())
//   show_table_element($tablename, $name, $title)
//   home_button()
//   testlevels_button()
//   teststands_button()
//   scenarios_button()
//   testcases_button()
//   testlevels_button()
//   levels_button()
//   levelinstances_button()
//   objecttypes_button()
//   objects_button()
//   objectattributetypes_button()
//   hosts_button()
//   hosts_button()
//   environments_button() 
//   actionresults_button()
//   collectortypes_button()
//   collectorvalues_button()
//   collectorsets_button() 
//   collectorsetmembers_button()
//   projects_button()
//   phases_button()
//   users_button()
//   maillists_button()
//   actionoutcome_button()
//   active_workrequests_button()
//   active_workqueues_button()
//   query_scenarioresults_button()
//   query_objects_button()
//   backup_button()
//   mysql_fatal_error($msg)
//   array($post) = get_post($var)
//   array($get) = get_get($var)
//   array($params) = getparms()
//   $ID = lookupID($tablename, $name)
//   $value = lookupfield($tablename, $fieldname, $selectingfieldname, $value)
//   $levinstances = lookuplevelinstancetable()
//   selectorID($tablename, $fieldname, $id = NULL) {
//   selectorname($tablename, $fieldname, $name)
//   selectorfield($tablename, $byfieldname, $selectedfield, $selectedvalue)
//   selectorIDforDuration($current_value)
//   selectorIDforUserName($ID = NULL)
//   selectorIDforUserEmail($email = NULL)
//   selectorOP($index, $value="")
//   selectorOPnum($index, $value="")
//   selectorOPvarchar($index, $value="")
//   selectorOPenum($index, $value="")
//   selectorANDOR($index, $value="")
//   selectorENUMScenariostate($index)
//   selectoractionstatus($value)
//   array($values) = get_hostselectorvalues()
//   array($names) = get_hostselectortypes()
//   selectorIDforhostselectortype($current_value)
//   array() = lookuptable($tablename, $indexfieldname, $lookupfieldname)
//   array() = lookuptableforusernames($tablename, $indexfieldname)
//   subtable($fields, $fieldnames, $tables, $where, $title, $objecttype, $orderby = NULL)
//   $num_rows = count_rows($tablename, $where)
//   array($offline, $online) = get_hosts_for_stand($standname)
//   $names = get_names_of_objects($tablename, $where = NULL)
//   $objects = get_fields_in_objects($tablename, $fieldname, $where)
//   array($offline, $online) = get_teststands_online_status()
//   $htmlentities = mysql_entities_fix_string($string)
//   $string = mysql_fix_string($string)
//   $encrypted = encrypt_password($password)
//   $names = get_names_of_phases_in_project($projectname)
//   $hms = hms($seconds)
//   determine_parent_object_type($objecttype)
//   determine_child_object_type($objecttype)
//   determine_objecttype_attribute_names_and_ids($objecttype)
//   determine_objecttype_attributes($objecttype)
//   capitalise_and_pluralise_object($objecttype)
//   determine_parent_chain($objecttype)
//   find_objects_in_teststand($teststandID, $objecttype)
//   get_field_from_row($tablename, $fieldname, $where)
//   get_enum_fields($database, $tablename, $columnname)
//   selector_for_enum($database, $tablename, $columnname, $selectfieldname, $enumvalue = NULL)
//   rc = get_backup_filenames($tablename = NULL, $completion_message, &$filenames) 
//   selector_on_array($array, $selected_value = NULL)
//   cellbutton($id, $label, $windowlocation) 
//   button_with_confirm($id, $label, $verb, $gowindowlocation, $nogowindowlocation)
//   restartdafserver(&$completion_message)
//   restartdafwebserver(&$completion_message)
//   restartdafsqlserver(&$completion_message)
//   checkdafserver(&$completion_message)
//   report_bad_parms($routinename = "unknown")
//   do_mysql_query($query, $db_server = null, $auditme = 0);
//

//   
//---------------------------------------------------------------------------------------------------------------------

$debug = "";


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   set_field_widths
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    noting 
//
// Description:
//
// Sets up the constants (mostly field widths).   These are super global so this routine only needs to be called once
//
// Must be kept in step with the entries in daf_protocol.x
//
//----------------------------------------------------------------------------------------------------------------------

function set_field_widths() {

   define("MAX_SQLSERVERNAME_LEN", 64);
   define("MAX_SQLUSERNAME_LEN", 64);
   define("MAX_SQLPASSWORD_LEN", 64);
   define("MAX_SQLDATABASENAME_LEN",  64);

   define("MAX_MSG_LEN",512);
   define("MAX_HOSTNAME_LEN", 64);
   define("MAX_PATHNAME_LEN",128);
   define("MAX_FILEHANDLE_LEN",64);
   define("MAX_SYSTEM_CMD_LEN",1024);
   define("MAX_CMD_LEN", 1024);
   define("MAX_CMD_OUTPUT_LINE_LEN", 1024);
   define("MAX_IDENT_LEN", 32);

   define("DAF_DATABASE_NAME_LEN", 64);          /* max length of a database name in MYSQL */
   define("DAF_TABLE_NAME_LEN", 64);             /* max length of a database table name in MYSQL */
   define("DAF_PROJECT_LEN", 32);
   define("DAF_PROJECTDESCRIPTION_LEN", 256);
   define("DAF_PHASE_LEN", 32);
   define("DAF_PHASEDESCRIPTION_LEN", 256);
   define("DAF_REQUESTTYPE_LEN", 32);
   define("DAF_SCENARIO_LEN", 48);
   define("DAF_STEPDESCRIPTION_LEN", 128);
   define("DAF_AUTHOR_LEN", 32);
   define("DAF_ACTIONTYPE_LEN", 32);
   define("DAF_LOGFILENAME_LEN", 64);
   define("DAF_STATE_LEN", 24);
   define("DAF_STATUS_LEN", 32);
   define("DAF_TESTCASE_LEN", 64);
   define("DAF_TESTCASE_DESCRIPTION_LEN", 64);
   define("DAF_TESTSTAND_LEN", 32);
   define("DAF_TESTSTAND_COMMENTS_LEN", 128);
   define("DAF_INVOCATION_LEN", 1024);
   define("DAF_DURATION_LEN", 24);
   define("DAF_LOGLOCATION_LEN", 128);
   define("DAF_LOGDIRECTORY_LEN", 128);
   define("DAF_COMMENTS_LEN", 256);
   define("DAF_PARAMETERS_LEN", 256);
   
   define ("DAF_SCENARIORESULT_NAME_LEN", 32);
   define ("DAF_SCENARIORESULT_PASS_LEN", 3);
   define ("DAF_SCENARIORESULT_START_LEN", 24);
   define ("DAF_SCENARIORESULT_END_LEN", 24);
   define ("DAF_SCENARIORESULT_TESTSTAND_LEN", 32);
   define ("DAF_SCENARIORESULT_TESTER_LEN", 32);
   define ("DAF_SCENARIORESULT_LOGLOCATION_LEN", 128);
   define ("DAF_SCENARIORESULT_COMMENTS_LEN", 256);
   
   define ("DAF_ACTIONRESULT_NAME_LEN", 32);
   define ("DAF_ACTIONRESULT_STATUS_LEN", 32);
   define ("DAF_ACTIONRESULT_INVOCATION_LEN", 1024);
   define ("DAF_ACTIONRESULT_PASS_LEN", 3);
   define ("DAF_ACTIONRESULT_START_LEN", 24);
   define ("DAF_ACTIONRESULT_END_LEN", 24);
   define ("DAF_ACTIONRESULT_COMMENTS_LEN", 256);

   define ("DAF_CLUSTER_NAME_LEN", 32);
   define ("DAF_CLUSTER_COMMENTS_LEN", 128);
   
   define ("DAF_NODE_NAME_LEN", 32);
   define ("DAF_NODE_MODEL_LEN", 16);
   define ("DAF_NODE_SERIAL_LEN", 16);
   define ("DAF_NODE_COMMENTS_LEN", 128);

   define("DAF_JOBNAME_LEN", 48);
   define("DAF_LEVEL_LEN", 32);
   define("DAF_LEVEL_DESCRIPTION_LEN", 128);
   define("DAF_TESTER_LEN", 32);            /* <<<< should this be 64 ??? */

   define("DAF_TESTLEVEL_LEN", 32);
   define("DAF_TESTLEVEL_DESCRIPTION_LEN", 128);

   define("DAF_HOSTSELECTORTYPE_NAME_LEN", 16);   /* is this used <<<< */
   define("DAF_HOSTSELECTORVALUE_NAME_LEN", 32);
   define("DAF_ACTIONDESCRIPTION_LEN", 64);
   define("DAF_HOST_NAME_LEN", 32);
   define("DAF_HOST_TYPE_LEN", 16);
   define("DAF_HOST_MODEL_LEN", 16);
   define("DAF_HOST_SERIAL_LEN", 16);
   define("DAF_HOST_HOSTSELECTORVALUE_LEN", 32);
   define("DAF_HOST_COMMENTS_LEN", 128);
   define("DAF_HOST_AGENTSTATUS_LEN", 16);
   define("DAF_COLLECTORVALUE_LEN", 256);
   define("DAF_COMPARATOR_LEN", 3);
   define("DAF_COLLECTORTYPE_NAME_LEN", 32);
   define("DAF_COLLECTORTYPE_OSTYPE_LEN", 32);
   define("DAF_COLLECTORTYPE_INVOCATION_LEN", 256);
   define("DAF_ENVIRONMENT_NAME_LEN", 32);
   define("DAF_ENVIRONMENT_DESCRIPTION_LEN", 128);
   define("DAF_ENVIRONMENTVALUE_NAME_LEN", 32);
   define("DAF_ENVIRONMENTVALUE_VALUE_LEN", 255);
   define("DAF_ENVIRONMENTVALUE_LEN", 255);
   define("DAF_PATHNAME_LEN", 128);
   define("DAF_FILENAME_LEN", 64);
   define("DAF_COLLECTORSET_NAME_LEN", 32);
   define("DAF_COLLECTORSET_DESCRIPTION_LEN", 128);
   define("DAF_MAX_NUM_COLLECTOR_TYPES", 20);
   define("DAF_OUTCOMEACTION_NAME_LEN", 32);

   define("DAF_ACTIONONPASS_LEN", 24);
   define("DAF_ACTIONONFAIL_LEN", 24);
   define("DAF_ERRORACTION_LEN", 24);
   define("DAF_NOTIFICATIONONPASS_LEN", 24);
   define("DAF_NOTIFICATIONONFAIL_LEN", 24);
   
   define("DAF_OUTCOMEACTION_LEN", 32);
   
   define("DAF_MAILLIST_LEN", 32);
   define("DAF_MAILLIST_DESCRIPTION_LEN", 128);

   define("DAF_USER_FIRSTNAME_LEN", 32);
   define("DAF_USER_LASTNAME_LEN", 32);
   define("DAF_USER_AREA_LEN", 32);
   define("DAF_USER_EMAIL_LEN", 64);
   define("DAF_USER_PASSWORD_LEN", 32);
   define ("DAF_VALID_ACTION_STATUS", "None,Canceled_due_to_error_in_step,Completed,No_available_host,No_remote_host_connect,Clnt_call_failed,Prepare_cmd_failed,Execute_cmd_failed");

   define("DAF_LEVELINSTANCE_VALUE_LEN", 256);
   define("DAF_OBJECTTYPE_LEN", 32);
   define("DAF_OBJECTTYPE_DESCRIPTION_LEN", 128);
   define("DAF_OBJECT_LEN", 32);
   define("DAF_OBJECT_COMMENTS_LEN", 256);
   define("DAF_OBJECTATTRIBUTETYPE_LEN", 32);
   define("DAF_OBJECTATTRIBUTETYPE_VARCHARWIDTH_LEN", 6);
   define("DAF_OBJECTATTRIBUTETYPE_ENUMVALUES_LEN", 256);
   

   define("DAF_LICENCE_LEN", 40);
   define("DAF_LICENCE_PATHNAME", "/opt/daf/daf_licence_file");


}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   document
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function document() {

echo <<<_END
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<link href="css/status.css" rel="stylesheet" type="text/css">
<link rel="stylesheet" type="text/css" href="dojo/dijit/themes/claro/claro.css"/>
_END;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   end_document
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function end_document() {

   echo "</html>\n";

}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   head
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function head() {

echo <<<_END

  <head>
	    <style type="text/css">
            body, html { font-family:helvetica,arial,sans-serif; font-size:90%; }
        </style>

		<title>DAF Test Automation</title>

		<!-- load dojo and provide config via data attribute -->
		<script src="dojo/dojo/dojo.js" type="text/javascript" djConfig="parseOnLoad: true"></script>
		<script>
             dojo.require("dijit.MenuBar");
             dojo.require("dijit.PopupMenuBarItem");
             dojo.require("dijit.Menu");
             dojo.require("dijit.MenuItem");
             dojo.require("dijit.PopupMenuItem");
             dojo.require("dijit.form.Button");
             dojo.require("dojo.behavior");
             dojo.require("dijit.form.DateTextBox");
             dojo.require("dijit.form.TimeTextBox");
             dojo.require("dijit.form.Form");
             dojo.require("dijit.form.Textarea");
             dojo.require("dijit.layout.BorderContainer");
		</script>

_END;

}
  
//---------------------------------------------------------------------------------------------------------------------
//
// Function:   end_head
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function end_head() {

   echo "</head>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   main_menu
//
// Inputs:     $current_project, 
//             $current_phase
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function main_menu($current_project = NULL, $current_phase = NULL, $current_logged_in_user = NULL) {

echo <<<_END

		<script>
           dojo.addOnLoad(_initMenuLoader);
           function _initMenuLoader() {
              var menuitem = dojo.byId("navMenu");
              menuitem.style.display = "block";
              menuitem = dojo.byId("projMenu");
              menuitem.style.display = "block";
           }
		</script>
		
        <div dojoType="dijit.MenuBar" id="navMenu" style="float:left; display:none">
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Projects
                </span>
                <div dojoType="dijit.Menu" id="projectMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=project&action=show'">
                        Projects
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=phase&action=show'">
                        Phases
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=project&action=selectcurrentprojectphase'">
                        Set Current Project/Phase
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Scenarios
                </span>
                <div dojoType="dijit.Menu" id="scenarioMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=scenario&action=show'">
                        Scenarios
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=testcase&action=show'">
                        Testcases
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=testlevel&action=show'">
                        Testlevels
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=hostselectortype&action=query'">
                        Host Selector Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=hostselectorvalue&action=show'">
                        Host Selector Values
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=outcomeaction&action=show'">
                        Outcome Actions
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Teststands
                </span>
                <div dojoType="dijit.Menu" id="teststandMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=teststand&action=show'">
                        Teststands
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=host&action=show'">
                        Hosts
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=objecttype&action=show'">
                        Object Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=objectattributetype&action=show'">
                        Object Attribute Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=object&action=query'">
                        Query Objects
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Collectors
                </span>
                <div dojoType="dijit.Menu" id="collectorMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=collectortype&action=show'">
                        Collector Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=collectorvalue&action=show'">
                        Collector Values
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=collectorset&action=show'">
                        Collector Sets
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location='index.php?object=collectorsetmember&action=show'">
                        Collector Set Members
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Environments
                </span>
                <div dojoType="dijit.Menu" id="environmentMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=environment&action=show'">
                        Environments
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Levels
                </span>
                <div dojoType="dijit.Menu" id="levelMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=level&action=show'">
                        Levels
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=levelinstance&action=show'">
                        Level Instances
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Users
                </span>
                <div dojoType="dijit.Menu" id="userMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=user&action=show'">
                        Users
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=maillist&action=show'">
                        Mail Lists
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    What is running?
                </span>
                <div dojoType="dijit.Menu" id="runningMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=active_workrequest&action=query'">
                        Active Scenarios
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=active_workqueue&action=query'">
                        Workqueue items
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Results
                </span>
                <div dojoType="dijit.Menu" id="resultMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=scenarioresult&action=show'">
                        Scenario Results
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=scenarioresult&action=query'">
                        Query Scenario Results
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=actionresult&action=show'">
                        Testcase Action Results
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=archive_workrequest&action=show'">
                        Archived Work Requests
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=archive_workqueue&action=show'">
                        Archived Work Queue
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Admin
                </span>
                <div dojoType="dijit.Menu" id="adminMenu" style="display:none">
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=backup&action=show'">
                        Backup and Restore
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=dafserver&action=show'">
                        DAF Server administration
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=auditlog&action=show'">
                        Show Audit Log
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=licence&action=show'">
                        Show Licence Information
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location='index.php?object=licence&action=update'">
                        Update Licence Information
                    </div>
                </div>
            </div>
         </div>

_END;

   echo '<div dojoType="dijit.MenuBar" id="projMenu" style="display:none" dir="rtl">' . "\n";

   if (isset($current_logged_in_user)) {
   
echo <<<_END
     
         <div dojoType="dijit.MenuBarItem" id="LogoutMenu">
             <div dojoType="dijit.MenuItem" onclick="window.location = 'index.php?action=logout'">
                 Logout
             </div>
         </div>
         <div dojoType="dijit.MenuBarItem">|</div>

_END;

   } 

      echo "<div dojoType=\"dijit.MenuBarItem\">\n";
         if (isset($current_project) && isset($current_phase)) {
            echo "<span>Project: $current_project &nbsp; &nbsp; Phase: $current_phase</span>\n";
         } else {
            // must have something in the menubar, otherwise it appears as a thinner bar than normal
            echo "<span>&nbsp;</span>\n";
         }
      echo "</div>\n";
      
      echo "<div dojoType=\"dijit.MenuBarItem\">|</div>\n";
    
      echo "<div dojoType=\"dijit.MenuBarItem\">\n";
         if (isset($current_logged_in_user)) {
            echo "<span>User: $current_logged_in_user &nbsp; &nbsp; &nbsp;</span>";
         }       
      echo "</div>\n";
      
   echo "</div>\n";
   

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   add_error_message
//
// Inputs:     none 
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function add_error_message($message) {        /* what happens if there is a ' in the message string ??  or a \n? <<<<<<<< */

   echo "<script>\n";
   echo "   dojo.ready(function() {\n"; 
   echo "    errorarea = dojo.byId('errorarea');\n";  
   echo "	 errorarea.innerHTML += '" . str_replace("\n", "<br>", str_replace("'", "\\'", $message)) .  "' + '<br>';\n";
   echo "       });\n";
   echo "</script>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   add_info_message
//
// Inputs:     none 
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function add_info_message($message) {        /* what happens if there is a ' in the message string  or a \n ??  <<<<<<<< */

   echo "<script>\n";
   echo "   dojo.ready(function() {\n"; 
   echo "    infoarea = dojo.byId('infoarea');\n";  
   echo "	 infoarea.innerHTML += '" . str_replace("\n", "<br>", str_replace("'", "\\'", $message)) .  "' + '<br>';\n";
   echo "       });\n";
   echo "</script>\n";

}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   body
//
// Inputs:     $current_project, 
//             $current_phase
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function body($current_project = NULL, $current_phase = NULL, $current_logged_in_user = NULL) {

  echo "<body class=\"claro\">\n";
    
  main_menu($current_project, $current_phase, $current_logged_in_user);

  /* add_info_message("this is an info message");
  add_error_message("this is an error message");
  add_info_message("this is an info message");
  add_error_message("this is an error message"); */

  echo "<div id=\"errorarea\" class=\"errorarea\"></div>";
  echo "<div id=\"infoarea\" class=\"infoarea\"></div>";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   end_body
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function end_body() {

   echo "</body>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_table
//
// Inputs:     $tablename
//             $title
//             $where
//             $orderby
//             $show
//             $modify
//             $delete
//             $links
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function show_table($tablename, $title, $where = NULL, $orderby = NULL, $show = NULL, $modify = NULL, $delete = NULL, 
    $links = array()) {
    
   $query = "SELECT * FROM $tablename";
   if (! is_null($where)) {
      $query = $query . " WHERE $where";
   }
   if (! is_null($orderby)) {
      $query = $query . " ORDER BY $orderby";
   }
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>$title</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   if (! is_null($show)) {
      echo '<th> &nbsp; </th>';
   }
   if (! is_null($modify)) {
      echo '<th> &nbsp; </th>';
   }
   if (! is_null($delete)) {
      echo '<th> &nbsp; </th>';
   }
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<th>' . $fieldname . '</th>';
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       if (! is_null($show)) {
          echo "<td class=td_smd><a href=index.php?action=show&object=$tablename&ID=$ID>show</a></td>";
       }
       if (! is_null($modify)) {
          echo "<td class=td_smd><a href=index.php?action=modify&object=$tablename&ID=$ID>modify</a></td>";
       }
       if (! is_null($delete)) {
          echo "<td class=td_smd><a href=index.php?action=show&operation=delete&object=$tablename&ID=$ID>delete</a></td>";
       }       
       
       for ($i = 0; $i < $num_fields; $i++) {
          if (isset($links[mysql_field_name($result, $i)])) {
             echo '<td>'  . '<a href="' . $links[mysql_field_name($result, $i)] . $row[$i] . '">' .  $row[$i] . '</a></td>';
          } else {           
             echo '<td>'  . $row[$i] . '</td>';
          }
       }   
       echo '</tr>';
       echo "\n";

   }
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_table_element
//
// Inputs:     $tablename
//             $name
//             $title
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function show_table_element($tablename, $name, $title) {

   $query = "SELECT * FROM $tablename where Name = '" . $name . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>$title - $name</div></caption>";
   echo "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
         $row[$j] = mysql_fetch_row($result);
   }

 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($result, $i) . '</th>';
      for ($j = 0; $j < $num_rows; $j++) {
          echo '<td>'  . $row[$j][$i] . '</td>';      }   
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
}



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   home_button, scenarios_button etc etc
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function home_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Home" />
</form>
_END;

}

function scenarios_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Scenarios" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="scenario" />
</form>
_END;

}

function scenariosets_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Scenario Sets" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="scenarioset" />
</form>
_END;

}

function testcases_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Testcases" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="testcase" />
</form>
_END;

}

function testlevels_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Test Levels" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="testlevel" />
</form>
_END;

}

function levels_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Levels" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="level" />
</form>
_END;

}

function levelinstances_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Level Instances" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="levelinstance" />
</form>
_END;

}

function objecttypes_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Object Types" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="objecttype" />
</form>
_END;

}

function objects_button($objecttype) {

echo "<form action=\"index.php\" method=\"post\">\n";
echo "<input type=\"submit\" name=\"button\" value=\"" . capitalise_and_pluralise_object($objecttype) . "\" />\n";   /* <<<<<<< plural and capitalise */
echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
echo "<input type=\"hidden\" name=\"object\" value=\"object\" />\n";
echo "<input type=\"hidden\" name=\"objecttype\" value=\"" . $objecttype . "\" />\n";
echo "</form>\n";

}

function objectattributetypes_button() {

echo "<form action=\"index.php\" method=\"post\">\n";
echo "<input type=\"submit\" name=\"button\" value=\"Object Attributes\" />\n";  
echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
echo "<input type=\"hidden\" name=\"object\" value=\"objectattributetype\" />\n";
echo "</form>\n";

}


function teststands_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Test Stands" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="teststand" />
</form>
_END;

}

function hosts_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Hosts" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="host" />
</form>
_END;

}

function scenarioresults_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Scenario Results" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="scenarioresult" />
</form>
_END;

}

function environments_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Environments" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="environment" />
</form>
_END;

}

function actionresults_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Action/Testcase Results" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="actionresult" />
</form>
_END;

}

function collectortypes_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Collector Types" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="collectortype" />
</form>
_END;

}

function collectorvalues_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Collector Values" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="collectorvalue" />
</form>
_END;

}

function collectorsets_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Collector Sets" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="collectorset" />
</form>
_END;

}

function collectorsetmembers_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Collector Set Members" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="collectorsetmember" />
</form>
_END;

}

function projects_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Projects" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="project" />
</form>
_END;

}

function phases_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Phases" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="phase" />
</form>
_END;

}

function users_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Users" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="user" />
</form>
_END;

}

function actionoutcomes_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Outcome Actions" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="outcomeaction" />
</form>
_END;

}

function maillists_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Mail lists" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="maillist" />
</form>
_END;

}

function active_workrequests_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Active Workrequests" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="active_workrequest" />
</form>
_END;

}

function active_workqueues_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Active Workqueue" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="active_workqueue" />
</form>
_END;

}

function query_scenarioresult_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Query Scenario Results" />
<input type="hidden" name=action value="query" />
<input type="hidden" name=object value="scenarioresult" />
</form>
_END;

}

function query_object_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Query Objects" />
<input type="hidden" name=action value="query" />
<input type="hidden" name=object value="object" />
</form>
_END;

}

function backup_button() {

echo <<<_END
<form action="index.php" method="post">
<input type="submit" name=button value="Backups" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="backup" />
</form>
_END;

}





//---------------------------------------------------------------------------------------------------------------------
//
// Function:   mysql_fatal_error
//
// Inputs:     $msg
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function mysql_fatal_error($msg) {
   $msg2 = msql_error();
   echo <<<_END
   Could not complete task - error was:
   <p>$msg: $msg2
   </p>
   Please inform <a href="mailto:admin@server.com">.
_END;
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_post
//
// Inputs:     $var
//
// Outputs:    none
//
// Returns:    a list of the variables inthe $_POST array, escaped so that the variables are valid html
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_post($var) {
  return mysql_real_escape_string($_POST[$var]);
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_get
//
// Inputs:     $var
//
// Outputs:    none
//
// Returns:    a list of the variables inthe $_GET array, escaped so that the variables are valid html
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_get($var) {
  return mysql_real_escape_string($_GET[$var]);
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_parms
//
// Inputs:     $var
//
// Outputs:    none
//
// Returns:    a list of the GET/POST variables escaped so that the variables are valid html
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function getparms() {

  $gp = array('action' => "none", 'object' => "none");
  
  foreach ($_GET as $key => $value) {
     $gp[$key] = mysql_real_escape_string($value);
  }
  
  foreach ($_POST as $key => $value) {
     if (is_array($value)) {
        foreach ($value as $key1 => $value1) {
           if (is_array($value1)) {
              foreach ($value1 as $key2 => $value2) {
                 $gp[$key][$key1][$key2] = mysql_real_escape_string($value2);
              }
           } else {
              $gp[$key][$key1] = mysql_real_escape_string($value1);
           }
        }
     } else {
        $gp[$key] = mysql_real_escape_string($value);
     }
  }

  return $gp;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   lookupID
//
// Inputs:     $tablename
//             $name
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function lookupID($tablename, $name) {

   $query = "SELECT ID FROM $tablename where Name = '" . $name . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   $row = mysql_fetch_row($result);
   
   return $row[0];
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   lookupfield
//
// Inputs:     $tablename
//             $fieldname
//             $selectingfieldname
//             $value
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

// to lookup the ScenarioresultID of the testlevelrecord object which has Name field set to $testlevel use
// $ScenarioresultID = lookupfield("testlevelrecord", "ScenarioresultID", "Name", $testlevel);

function lookupfield($tablename, $fieldname, $selectingfieldname, $value) {

   $query = "SELECT $fieldname FROM $tablename where $selectingfieldname = '" . $value . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);     // we expect only one row - is this an issue?
  
   $row = mysql_fetch_row($result);
   
   return $row[0];
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorID
//
// Inputs:     $tablename
//             $fieldname
//             $id
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorID($tablename, $fieldname, $id = NULL) {

  $query = "SELECT ID," . $fieldname . " FROM $tablename ORDER BY " . $fieldname;
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result);
  
  if (! is_null($id)) {
  
     for ($i = 0; $i < $num_rows; $i++) {
        $row = mysql_fetch_row($result);
        if ($id == $row[0]) {
           echo "<option selected value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
        } else {
           echo "<option value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
        }
     }
     
  } else {
  
        for ($i = 0; $i < $num_rows; $i++) {
        $row = mysql_fetch_row($result);
        echo "<option value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
     }
  
  }  
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorname
//
// Inputs:     $tablename
//             $fieldname
//             $name
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorname($tablename, $fieldname, $name) {

  $query = "SELECT Name," . $fieldname . " FROM $tablename ORDER BY " . $fieldname;
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows   = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result);
  
  if (isset($name)) {
  
     for ($i = 0; $i < $num_rows; $i++) {
        $row = mysql_fetch_row($result);
        if ($name == $row[0]) {
           echo "<option selected value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
        } else {
           echo "<option value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
        }
     }
     
  } else {
  
        for ($i = 0; $i < $num_rows; $i++) {
        $row = mysql_fetch_row($result);
        echo "<option value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
     }
  
  }  
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorname
//
// Inputs:     $tablename
//             $byfieldname
//             $selectedfield
//             $selectedvalue
//             $additionalfields    (array, optional) 
//             $additionalvalues    (array, must be specified if $additionalfields is present)
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//      selectorfield("outcomeaction", "Name", "ID", 23);
//
//      producs a selection list  
//                name1
//                name2
//                name3
//      with the name that has ID=23 selected
//
//----------------------------------------------------------------------------------------------------------------------

function selectorfield($tablename, $byfieldname, $selectedfield, $selectedvalue, $additionalfields = null, $additionvalues = null) {

  $query = "SELECT " . $selectedfield . " ," . $byfieldname . " FROM $tablename ORDER BY " . $byfieldname;
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows   = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result);
  
  
  if (isset($additionalfields)) {
  
     for ($i = 0; $i < count($additionfields); $i++) {     
        echo "<option value=\"" . $additionvalues[$i] . "\">" . $additionfields[$i] . "</option>\n";
     }
     
  }
  
  if (isset($selectedvalue)) {
  
     for ($i = 0; $i < $num_rows; $i++) {
        $row = mysql_fetch_row($result);
        if ($selectedvalue == $row[0]) {
           echo "<option selected value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
        } else {
           echo "<option value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
        }
     }
     
  } else {
  
        for ($i = 0; $i < $num_rows; $i++) {
        $row = mysql_fetch_row($result);
        echo "<option value=\"" . $row[0] . "\">" . $row[1] . "</option>\n";
     }
  
  }  
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorIDforDuration
//
// Inputs:     $current_value
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorIDforDuration($current_value) {

   $durationvalues = array("Rununtilcomplete","RunforNminutes","TimeoutafterNminutes");
   
   if (isset($current_value)) {

      for ($j = 0; $j < count($durationvalues); $j++) {
         if ($durationvalues[$j] == $current_value) {
            echo "<option selected value=\"" . $durationvalues[$j] . "\">" . $durationvalues[$j] . "</option>\n";
         } else {
            echo "<option value=\"" . $durationvalues[$j] . "\">" . $durationvalues[$j] . "</option>\n";
         }
      }
      
   } else {
   
      for ($j = 0; $j < count($durationvalues); $j++) {
         echo "<option value=\"" . $durationvalues[$j] . "\">" . $durationvalues[$j] . "</option>\n";
      }
        
   }
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorIDforUserName
//
// Inputs:     $current_value
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorIDforUserName($ID = NULL) {

  $query = "SELECT ID, Firstname, Lastname FROM daf.user ORDER BY Lastname";;
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows   = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result); 
  
  
  for ($i = 0; $i < $num_rows; $i++) {
  
     $row = mysql_fetch_row($result);
   
     $username = $row[1] . " " . $row[2];

     if (isset($ID)) {
        echo '<option' . ($row[0] == $ID ? ' selected' : '') . 'value="' . $row[0] . '"\>' . $username . "</option>\n";
     } else {
        echo '<option value="' . $row[0] . '"\>' . $username . "</option>\n";
     }
        
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorIDforUserEmail
//
// Inputs:     $email
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorIDforUserEmail($email = NULL) {

  $query = "SELECT ID, Firstname, Lastname, Email FROM daf.user ORDER BY Lastname";;
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows   = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result); 
  
  
  for ($i = 0; $i < $num_rows; $i++) {
  
     $row = mysql_fetch_row($result);
   
     $username = $row[1] . " " . $row[2];

     if (isset($ID)) {
        echo '<option' . ($row[3] == $email ? ' selected' : '') . 'value="' . $row[3] . '"\>' . $username . " (" . $row[3] . ")</option>\n";
     } else {
        echo '<option value="' . $row[3] . '"\>' . $username . " (" . $row[3] . ")</option>\n";
     }
        
   }
   
}




//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorIDforTestLevelInstance
//
// Inputs:     $current_value
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorIDforTestLevelInstance($ID = NULL) {

  $query = "SELECT levelinstance.ID, level.name, daf.levelinstance.value FROM daf.levelinstance INNER JOIN daf.level " .
           "ON daf.levelinstance.levelID = daf.level.ID ORDER BY daf.level.name";
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows   = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result); 
  
  for ($i = 0; $i < $num_rows; $i++) {
  
     $row = mysql_fetch_row($result);
   
     $level_instance_description = $row[1] . " " . $row[2];

     if (isset($ID)) {
        echo '<option' . ($row[0] == $ID ? ' selected' : '') . 'value="' . $row[0] . '"\>' . $level_instance_description . "</option>\n";
     } else {
        echo '<option value="' . $row[0] . '"\>' . $level_instance_description . "</option>\n";
     }
        
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorOP
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------


function selectorOP($index, $value="") {

   echo "<select name=\"comparator[" . $index . "]\" />\n";
   echo '<option value="LIKE"' . ($value == "LIKE" ? " selected" : "") . ">LIKE</option>\n";
   echo '<option value="LIKEPC"' . ($value == "LIKEPC" ? " selected" : "") . ">LIKE %..%</option>\n";
   echo '<option value="NOTLIKE"' . ($value == "NOTLIKE" ? " selected" : "") . ">NOT LIKE</option>\n";
   echo '<option value="REGEXP"' . ($value == "REGEXP" ? " selected" : "") . ">REGEXP</option>\n";
   echo '<option value="NOTREGEXP"' . ($value == "NOTREGEXP" ? " selected" : "") . ">NOT REGEXP</option>\n";
   echo '<option value="ISNULL"' . ($value == "ISNULL" ? " selected" : "") . ">IS NULL</option>\n";
   echo '<option value="ISNOTNULL"' . ($value == "ISNOTNULL" ? " selected" : "") . ">IS NOT NULL</option>\n";
   echo '<option value="ISEMPTY"' . ($value == "ISEMPTY" ? " selected" : "") . ">='' (is empty string)</option>\n";
   echo '<option value="ISNOTEMPTY"' . ($value == "ISNOTEMPTY" ? " selected" : "") . ">!='' (is not an empty string)</option>\n";
   echo '<option value="="' . ($value == "=" ? " selected" : "") . ">=</option>\n";
   echo '<option value="<"' . ($value == "<" ? " selected" : "") . ">&lt;</option>\n";
   echo '<option value="<="' . ($value == "<=" ? " selected" : "") . ">&le;</option>\n";
   echo '<option value=">"' . ($value == ">" ? " selected" : "") . ">&gt;</option>\n";
   echo '<option value=">="' . ($value == ">=" ? " selected" : "") . ">&ge;</option>\n";
   echo '<option value="!="' . ($value == "!=" ? " selected" : "") . ">&ne;</option>\n";
   echo "</select>\n"; 
   
}  


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorOPnum
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorOPnum($index, $value="") {

   echo "<select name=\"comparator[" . $index . "]\" />\n";
   echo '<option value="LIKE"' . ($value == "LIKE" ? " selected" : "") . ">LIKE</option>\n";
   echo '<option value="NOTLIKE"' . ($value == "NOTLIKE" ? " selected" : "") . ">NOT LIKE</option>\n";
   echo '<option value="="' . ($value == "=" ? " selected" : "") . ">=</option>\n";
   echo '<option value="!="' . ($value == "!=" ? " selected" : "") . ">&ne;</option>\n";
   echo '<option value="<"' . ($value == "<" ? " selected" : "") . ">&lt;</option>\n";
   echo '<option value="<="' . ($value == "<=" ? " selected" : "") . ">&le;</option>\n";
   echo '<option value=">"' . ($value == ">" ? " selected" : "") . ">&gt;</option>\n";
   echo '<option value=">="' . ($value == ">=" ? " selected" : "") . ">&ge;</option>\n";
   echo '<option value="ISNULL"' . ($value == "ISNULL" ? " selected" : "") . ">IS NULL</option>\n";
   echo '<option value="ISNOTNULL"' . ($value == "ISNOTNULL" ? " selected" : "") . ">IS NOT NULL</option>\n";
   echo "</select>\n"; 
   
}  


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorOPvarchar
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorOPvarchar($index, $value="") {

   echo "<select name=\"comparator[" . $index . "]\" />\n";
   echo '<option value="LIKE"' . ($value == "LIKE" ? " selected" : "") . ">LIKE</option>\n";
   echo '<option value="LIKEPC"' . ($value == "LIKEPC" ? " selected" : "") . ">LIKE %..%</option>\n";
   echo '<option value="NOTLIKE"' . ($value == "NOTLIKE" ? " selected" : "") . ">NOT LIKE</option>\n";
   echo '<option value="REGEXP"' . ($value == "REGEXP" ? " selected" : "") . ">REGEXP</option>\n";
   echo '<option value="NOTREGEXP"' . ($value == "NOTREGEXP" ? " selected" : "") . ">NOT REGEXP</option>\n";
   echo '<option value="ISEMPTY"' . ($value == "ISEMPTY" ? " selected" : "") . ">='' (is empty string)</option>\n";
   echo '<option value="ISNOTEMPTY"' . ($value == "ISNOTEMPTY" ? " selected" : "") . ">!='' (is not an empty string)</option>\n";
   echo '<option value="="' . ($value == "=" ? " selected" : "") . ">=</option>\n";
   echo '<option value="!="' . ($value == "!=" ? " selected" : "") . ">&ne;</option>\n";
   echo '<option value="ISNULL"' . ($value == "ISNULL" ? " selected" : "") . ">IS NULL</option>\n";
   echo '<option value="ISNOTNULL"' . ($value == "ISNOTNULL" ? " selected" : "") . ">IS NOT NULL</option>\n";
   echo "</select>\n"; 
   
} 

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorOPenum
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorOPenum($index, $value="") {

   echo "<select name=\"comparator[" . $index . "]\" />\n";
   echo '<option value="="' . ($value == "=" ? " selected" : "") . ">=</option>\n";
   echo '<option value="!="' . ($value == "!=" ? " selected" : "") . ">&ne;</option>\n";
   echo '<option value="ISNULL"' . ($value == "ISNULL" ? " selected" : "") . ">IS NULL</option>\n";
   echo '<option value="ISNOTNULL"' . ($value == "ISNOTNULL" ? " selected" : "") . ">IS NOT NULL</option>\n";
   echo "</select>\n"; 
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorANDOR
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function selectorANDOR($index, $value="AND") {

   echo "<select name=\"and_or[" . $index . "]\" />\n";
   echo '<option value="AND"' . ($value == "AND" ? " selected" : "") . ">AND</option>\n";
   echo '<option value="OR"' . ($value == "OR" ? " selected" : "") . ">OR</option>\n";
   echo "</select>\n"; 
   
} 


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorENUMScenariostate
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------
 

function selectorENUMScenariostate($index) {           /* <<<<<<<< shouldn't this be determined automatically from a query ?  */

   echo "<select name=\"value[" . $index . "][]\" multiple=\"multiple\" size=\"3\">\n";
   echo '<option value="Notstarted">Notstarted</option>' . "\n";
   echo '<option value="Failedtestlevelvalidation">Failedtestlevelvalidation</option>' . "\n";
   echo '<option value="Running">Running</option>' . "\n";
   echo '<option value="Canceled">Canceled</option>' . "\n";
   echo '<option value="Completed">Completed</option>' . "\n"; 
   echo "</select>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectoractionstatus
//
// Inputs:     $index
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------
 

function selectoractionstatus($value = 'xxxx') {           /* <<<<<<<< shouldn't this be determined automatically from a query ?  */

   $a = array("None", "Canceled_due_to_error_in_step", "Completed", "No_available_host", "No_remote_host_connect", "Clnt_call_failed",
              "Prepare_cmd_failed", "Execute_cmd_failed");
              
   foreach ($a as $choice) {
      echo '<option ' . ( $value == $choice ? "selected" : "" ) . ' value="' . $choice . '">' . $choice . "</option>\n";
   }

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   lookuplevelinstancetable
//
// Inputs:     none
//
// Outputs:    $lookup
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function lookuplevelinstancetable() {

  $query = "SELECT levelinstance.ID, level.name, daf.levelinstance.value FROM daf.levelinstance INNER JOIN daf.level " .
           "ON daf.levelinstance.levelID = daf.level.ID ORDER BY daf.level.name";
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $num_rows   = mysql_num_rows($result);
  $num_fields = mysql_num_fields($result);
  
  $lookup = array(); 
  
  for ($i = 0; $i < $num_rows; $i++) {
  
     $row = mysql_fetch_row($result);
   
     $lookup[$row[0]] = $row[1] . " (" . $row[2] . ")";
        
   }
   
   return $lookup;
   
}

//----------------------------------------------------------------------------------------------------------------------

function get_username_from_ID($id) {

  $query = "SELECT Firstname, Lastname FROM user WHERE ID = '" . $id . "'";
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $row = mysql_fetch_row($result);
  return  $row[0] . " " . $row[1];
  
}



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_hostselectorvalues
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_hostselectorvalues() {

   $query = "SELECT DISTINCT Value FROM daf.hostselectorvalue ORDER by Value";
   $hostselectorvalues = mysql_query($query);
   if (! $query) die ("Database access failed for query=$query: " . mysql_error());
   
   $values = array();
   
   for ($i=0; $i<mysql_num_rows($hostselectorvalues); $i++) {
     $row = mysql_fetch_row($hostselectorvalues);
     $values[$i] = $row[0];
   }
   
   return $values;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorIDforhostselectorvalue
//
// Inputs:     $current_value
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

function selectorIDforhostselectorvalue($current_value=NULL) {

   $hostselectorvalues = get_hostselectorvalues();
   
   if (isset($current_value)) {

      for ($j = 0; $j < count($hostselectorvalues); $j++) {
         if ($hostselectorvalues[$j] == $current_value) {
            echo "<option selected value=\"" . $hostselectorvalues[$j] . "\">" . $hostselectorvalues[$j] . "</option>\n";
         } else {
            echo "<option value=\"" . $hostselectorvalues[$j] . "\">" . $hostselectorvalues[$j] . "</option>\n";
         }
      }
      
   } else {
   
      for ($j = 0; $j < count($hostselectorvalues); $j++) {
         echo "<option value=\"" . $hostselectorvalues[$j] . "\">" . $hostselectorvalues[$j] . "</option>\n";
      }
        
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_hostselectortypes
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

function get_hostselectortypes() {

   $query = "SELECT Name FROM daf.hostselectortype ORDER by Name";
   $hostselectornames = mysql_query($query);
   if (! $query) die ("Database access failed for query=$query: " . mysql_error());
   
   $names = array();
   
   for ($i=0; $i<mysql_num_rows($hostselectornames); $i++) {
     $row = mysql_fetch_row($hostselectornames);
     $names[$i] = $row[0];
   }
   
   return $names;
   
}  

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selectorIDforhostselectortype
//
// Inputs:     $current_value
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

function selectorIDforhostselectortype($current_value = null) {

   $hostselectortypes = get_hostselectortypes();
   
   if (isset($current_value)) {

      for ($j = 0; $j < count($hostselectortypes); $j++) {
         if ($hostselectortypes[$j] == $current_value) {
            echo "<option selected value=\"" . $hostselectortypes[$j] . "\">" . $hostselectortypes[$j] . "</option>\n";
         } else {
            echo "<option value=\"" . $hostselectortypes[$j] . "\">" . $hostselectortypes[$j] . "</option>\n";
         }
      }
      
   } else {
   
      for ($j = 0; $j < count($hostselectortypes); $j++) {
         echo "<option value=\"" . $hostselectortypes[$j] . "\">" . $hostselectortypes[$j] . "</option>\n";
      }
        
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   lookuptable
//
// Inputs:     $tablename
//             $indexfieldname
//             $lookupfieldname
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

// $r = lookuptable("teststand", "ID", "Name")  creates an array $r in which $r[23] is the name of the teststand that has ID 23
function lookuptable($tablename, $indexfieldname, $lookupfieldname) {

  $query = "SELECT $indexfieldname, $lookupfieldname FROM $tablename ORDER BY " . $indexfieldname;
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $r = array();
  $num_rows = mysql_num_rows($result);
  for ($i = 0; $i < $num_rows; $i++) {
     $row = mysql_fetch_row($result);
     $r[$row[0]] = $row[1];  
  }  
  
  return $r;
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   lookuptableforusernames
//
// Inputs:     $indexfieldname
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

function lookuptableforusernames($tablename, $indexfieldname) {

  $query = "SELECT $indexfieldname, Firstname, Lastname FROM $tablename ORDER BY Lastname";
  $result = mysql_query($query);

  if (! $result) die ("Database access failed for query=$query: " . mysql_error());
  
  $r = array();
  $num_rows = mysql_num_rows($result);
  for ($i = 0; $i < $num_rows; $i++) {
     $row = mysql_fetch_row($result);
     $r[$row[0]] = $row[1] . " " . $row[2];  
  }  
  
  return $r;
    
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   subtable
//
// Inputs:     $fields
//             $fieldnames
//             $tables
//             $where
//             $title
//             $objecttype
//             $orderby
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

function subtable($fields, $fieldnames, $tables, $where, $title, $objecttype, $orderby = NULL) {

   $query = "SELECT " . $fields . " FROM " . $tables . " WHERE " . $where;
   if (! is_null($orderby)) {
      $query = $query . " ORDER BY $orderby";
   }
   $qresult = mysql_query($query);
   
   if (! $qresult) die ("Database access failed for query=$query: " . mysql_error());
   
   echo '<table class="tab1">';

   $num_rows   = mysql_num_rows($qresult);
   $num_fields = mysql_num_fields($qresult);
   
   for ($i = 0; $i < count($fieldnames); $i++) {
      if ('ID' === $fieldnames[$i]) {
         $IDindex = $i;
         break;
      }
   }
   
   if (!isset($IDindex)) {
      echo "subtable: cannot find 'ID' in fieldnames - cannot show subtable\n";
      return;
   }
   
   echo '<tr>';
   echo '<th colspan=2>' . $title . '</th>';
   for ($j = 0; $j < $num_fields; $j++) {
      echo '<th>' . $fieldnames[$j] . '</th>';
   }  
   echo '</tr>';
   
   for ($i = 0; $i < $num_rows; $i++) {
      $row = mysql_fetch_row($qresult);
      echo '<tr>'; 
      $ID = $row[$IDindex];
      echo "<td class=td_smd><a href=index.php?action=modify&object=$objecttype&ID=$ID>modify</a></td>";
      echo "<td class=td_smd><a href=index.php?action=show&operation=delete&object=$objecttype&ID=$ID>delete</a></td>";
      for ($j = 0; $j < $num_fields; $j++) {       
         echo '<td>'  . $row[$j] . '</td>';       
      }
      echo '</tr>';
      echo "\n";
   }


   echo '</table>';

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   count_rows
//
// Inputs:     $tablename
//             $where
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------  

function count_rows($tablename, $where = NULL) {

   if (isset($where)) {
      $query = "SELECT * FROM $tablename WHERE $where";
   } else {
      $query = "SELECT * FROM $tablename";
   }

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   return mysql_num_rows($result);
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_hosts_for_stand
//
// Inputs:     $standname
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------
 
function get_hosts_for_stand($standname) {

   $query = "SELECT * FROM daf.host WHERE TeststandID = (SELECT ID FROM teststand WHERE Name = '$standname')"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   $offline = array();
   $online  = array();
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   for ($i = 0; $i < $num_rows; $i++) {
     $row = mysql_fetch_row($result);
     if ($row[$fieldindex['Agentstatus']] == 'Online') {
         array_push($online, $row[$fieldindex['Name']]);
     } else {
         array_push($offline, $row[$fieldindex['Name']]);
     }
   }
   
   return array($offline, $online);
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_names_of_objects
//
// Inputs:     $tablename
//             $where
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_names_of_objects($tablename, $where = NULL) {

   if ($where == NULL) {
   
      $query = "SELECT * FROM $tablename"; 
   
   } else {

      $query = "SELECT * FROM $tablename WHERE $where"; 
      
   }

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   $objects = array();
      
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   for ($i = 0; $i < $num_rows; $i++) {
     $row = mysql_fetch_row($result);
     array_push($objects, $row[$fieldindex['Name']]);
   }
   
   return $objects;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_fields_in_objects
//
// Inputs:     $tablename
//             $fieldname
//             $where
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_fields_in_objects($tablename, $fieldname, $where) {

   // $fieldname must be a single field name

   $query = "SELECT $fieldname FROM $tablename WHERE $where"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   $objects = array();
      
   for ($i = 0; $i < $num_rows; $i++) {
     $row = mysql_fetch_row($result);
     array_push($objects, $row[0]);
   }
   
   return $objects;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_teststands_online_status
//
// Inputs:     
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_teststands_online_status() {

   $query = "select teststand.Name as teststandName, host.Name as hostName, Agentstatus from daf.host, teststand where teststand.ID = host.TeststandID oRDER by teststand.Name"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   $offline = array();
   $online  = array();
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   for ($i = 0; $i < $num_rows; $i++) {
     $row = mysql_fetch_row($result);
     $teststandname = $row[$fieldindex['teststandName']];
     if (! isset($online[$teststandname])) {
        $online[$teststandname] = 0;
        $offline[$teststandname] = 0;
     }
     if ($row[$fieldindex['Agentstatus']] == 'Online') {
         $online[$teststandname]++;
     } else {
         $offline[$teststandname]++;
     }
   }
   
   return array($offline, $online);
   
}

function mysql_entities_fix_string($string) {
  return htmlentities(mysql_fix_string($string));
}

function mysql_fix_string($string) {
   if (get_magic_quotes_gpc()) $string = stripslashes($string);
   return mysql_real_escape_string($string);
}  

function encrypt_password($password) {

  $salt1 = "qm&e*";
  $salt2 = "p2!@";
  $token = md5("$salt1$password$salt2");
      
  return $token;
 
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_names_of_phases_in_project
//
// Inputs:     $projectname
//             
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function get_names_of_phases_in_project($projectname) {

   $query = "SELECT phase.Name FROM daf.project INNER JOIN phase on project.ID = phase.projectID"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);    
   $num_fields = mysql_num_fields($result);
   
   $names = array();
   
   for ($i = 0; $i < $num_rows; $i++) {
      $row = mysql_fetch_row($result);
      array_push($names, $row[0]);
   }
   
   return $names;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   hms
//
// Inputs:     $seconds
//           
// Outputs:    none
//
// Returns:    a string the form  "N hours, M minutes, S seconds"
//
// Description:  Converts a number of seconds into a human readable duration of hours, minutes and seconds, eg
//
//   hms(3601)   returns  "1 hour, 0 minutes, 1 second"
//   hms(100)    returns  "1 minute, 40 seconds"
//   
//----------------------------------------------------------------------------------------------------------------------

function hms($seconds = 0) {

   $numsecs  = $seconds % 60;
   $nummins  = intval(($seconds/60)) % 60;
   $numhours = intval(($seconds/3600)) % 24;
   $numdays  = intval(($seconds/216000));
   
   $ts = "";
   
   if ($numhours > 1) {
      $ts = $numdays . " days, ";
   } else if ($ts == 1) {
      $ts = $numdays . " day, ";
   }
   
   if ($numhours > 1) {
      $ts = $ts . $numhours . " hours, ";
   } else if ($ts == 1) {
      $ts = $ts . $numhours . " hour, ";
   } else if (($numhours == 0) && ($numdays > 0)) {
      $ts =  $ts . $numhours . " hours, ";
   }


   if ($nummins > 1) {
      $ts =  $ts . $nummins . " minutes, ";
   } else if ($ts == 1) {
      $ts = $ts . $nummins . " minute, ";
   } else if (($nummins == 0) && ($numhours > 0)) {
      $ts =  $ts . $nummins . " minutes, ";
   }
 
   if ($numsecs > 1) {
      $ts =  $ts . $numsecs . " seconds";
   } else if ($ts == 1) {
      $ts = $ts . $numsecs . " second";
   } else if (($numsecs == 0) && ($nummins > 0)) {
      $ts =  $ts . $numsecs . " seconds";
   }

   return($ts);

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   determine_parent_object_type
//
// Inputs:     $objecttype
//           
// Outputs:    none
//
// Returns:    a name of the parent object type
//
// Description:  Works out the parent object type for the specified object type eg
//
//   determine_parent_object_type("cluster")   returns  "teststand"
//
//   the object type "teststand" is a special case as this is the top of the object tree and therefore has no parent,
//   in this case an empty string is returned   
//----------------------------------------------------------------------------------------------------------------------

function determine_parent_object_type($objecttype) {

   if ($objecttype == "teststand") {
      return "";
   }
   
   $query = "SELECT t1.ParentID, t2.Name FROM daf.objecttype as t1 INNER JOIN daf.objecttype as t2 ON t1.ParentID = t2.ID WHERE t1.Name = '" . $objecttype . "'"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);    /* should be 1 */
   $row = mysql_fetch_row($result);
   if ($row[0] == 0) {
      return "teststand";
   } else {
      return $row[1];
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   determine_child_object_types
//
// Inputs:     $objecttype
//           
// Outputs:    none
//
// Returns:    a list of the child object types
//
// Description:  Works out the child object types for the specified object type eg
//
//   determine_child_object_type("teststand")   might return  "cluster", "switch"
//
//   if an obbjecttype does not have any children then an empty array is returned  
// 
//----------------------------------------------------------------------------------------------------------------------

function determine_child_object_types($objecttype) {

   if ($objecttype == "teststand") {   
      $query = "SELECT Name FROM daf.objecttype  WHERE parentID = '0'"; 
   } else {
      $query = "SELECT t1.Name FROM daf.objecttype as t1 INNER JOIN daf.objecttype as t2 ON t1.ParentID = t2.ID WHERE t2.Name = '" . $objecttype . "'";
   }
   $result = mysql_query($query);
   
   $childs = array();
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);    /* should be 1 */
   if ($num_rows == 0) {
      /* do nothing */
   } else {
      for ($i = 0; $i < $num_rows; $i++) {
         $row = mysql_fetch_row($result);
         array_push($childs, $row[0]);
      }
   }
   return $childs;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   determine_objecttype_attributes
//
// Inputs:     $objecttype
//           
// Outputs:    none
//
// Returns:    the list of attribute names that belong to this objecttype
//
// Description:  Works out the user assigned attributes that belong to a particular object type eg
//
//   determine_objecttype_attributes("cluster")   might return  ("serialnumber", "location", "type")
//
//   the object type "teststand" is a special case as it cannot be given user attributes
//----------------------------------------------------------------------------------------------------------------------

function determine_objecttype_attributes($objecttype) {

   if ($objecttype == "teststand") {
      return array();
   }
   
   $query = "SELECT objectattributetype.Name FROM daf.objectattributetype INNER JOIN daf.objecttype ON objectattributetype.objecttypeID = objecttype.ID  " . 
            "WHERE objecttype.Name = '" . $objecttype . "'"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);
   $attributenames = array();
   
   for ($i=0; $i<$num_rows; $i++) {  
      $row = mysql_fetch_row($result);
      array_push($attributenames, $row[0]);
   }
   
   return $attributenames;
   
}



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   determine_objecttype_attribute_names_and_ids
//
// Inputs:     $objecttype
//           
// Outputs:    none
//
// Returns:    an array of two arrays - the first is list of attribute names that belong to this objecttype, the
//             second is the list of attributetype IDs that belong tihs to this objecttype
//
// Description:  Works out the user assigned attributes that belong to a particular object type eg
//
//   determine_objecttype_attribute_names_and_ids("cluster")   might return  (("serialnumber", "location", "type"), (1,2,3))
//
//   the object type "teststand" is a special case as it cannot be given user attributes
//----------------------------------------------------------------------------------------------------------------------

function determine_objecttype_attribute_names_and_ids($objecttype) {

   if ($objecttype == "teststand") {
      return array();
   }
   
   $query = "SELECT objectattributetype.ID, objectattributetype.Name FROM daf.objectattributetype INNER JOIN daf.objecttype ON objectattributetype.objecttypeID = objecttype.ID  " . 
            "WHERE objecttype.Name = '" . $objecttype . "'"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);
   $attributeids   = array();
   $attributenames = array();
   
   for ($i=0; $i<$num_rows; $i++) {  
      $row = mysql_fetch_row($result);
      array_push($attributenames, $row[0]);
      array_push($attributenames, $row[1]);
   }
   
   return array($attributeids, $attributenames);
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   capitalise_and_pluralise_object
//
// Inputs:     $objecttype
//           
// Outputs:    none
//
// Returns:    a capitalised, plural for the specified object type
//
// Description:  Works out the plural of the name of the bject type and capitalises it
//
//   eg  capitalise_and_pluralise_object("cluster")   returns  "Clusters"
//
//----------------------------------------------------------------------------------------------------------------------

function capitalise_and_pluralise_object($objectname) {
 
   if (substr($objectname, -1) == 's') {
     $objectname .= "es";
   } else if (substr($objectname, -1) == 'y') {
     $objectname = substr($objectname, 0, count($objectname)-2) . "ies";
   } else if (substr($objectname, -1) == 'x') {
     $objectname .= "es";
   } else {
     $objectname .= "s";
   }
   return(ucfirst($objectname));
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   determine_parent_chain
//
// Inputs:     $objecttype
//           
// Outputs:    none
//
// Returns:    a list of the names of the objects that are parents of the specified objecttype plus the objecttype
//             itself
//
// Description: 
//
//   eg  determine_parent_chain("powersupply")  might return   ("teststand", "cluster", "node", "powersupply")
//
//----------------------------------------------------------------------------------------------------------------------

function determine_parent_chain($objecttype) {

   $query = "SELECT ID, ParentID, Name FROM daf.objecttype"; 

   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);   
   
   $j = 0;
   $parents = array();
   
   echo "<pre>\n";
   
   for ($i=0; $i<$num_rows; $i++) {
      $row = mysql_fetch_row($result);
      $ID[$i]       = $row[0];
      $ParentID[$i] = $row[1];
      $Name[$i]     = $row[2];
      if ($row[2] == $objecttype) {
         $current_row = $i;
      }
      echo " ID " . $ID[$i]  . " ParentID " . $ParentID[$i] . " Name " . $Name[$i]  . "\n";
   }  
   echo "current_row " . $current_row . "\n";
   echo "</pre>\n";
   
   
   if (! isset($current_row)) {
      /* objecttype not found in daf.objecttype table */
      return array();
   } else {
   
      if ($ParentID[$current_row] == 0) {
         $parents = array($objecttype);
      } else {
         $parents = array($objecttype);
         while ($ParentID[$current_row] != 0) {
            for ($i=0; $i<$num_rows; $i++) {
               if ($ID[$i] == $ParentID[$current_row]) {
                  $current_row = $i;
                  array_push($parents, $Name[$current_row]);
                  $j++;
                  break;
               }
            }
         }   
      }      
   }
   
   array_push($parents, "teststand");
   
   /* now the parents[0], parents[1] etc are the objecttypes of the parents of the object we are interested in with 
      parents[0] being the furthest away in the tree from the teststand object and parents[N] being a child of the
      teststand so reverse the string so we can descend from the known teststand */
      
   $parents = array_reverse($parents);
      
   return $parents;
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   find_objects_in_teststand
//
// Inputs:     $teststandID
//             $objecttype
//           
// Outputs:    none
//
// Returns:    a list of the names of the objects of the specified objecttype in the specified teststand
//
// Description:  Finds any objects of type $objecttype that are in the specified teststand.   Note, since an object can be 
// a child of another object, this cannot be a simple search, as child of the child of the testcase does not directly know
// which teststand it is in, that fact has to be deduced from the teststand that its parent belongs to.
//
//   eg  find_objects_in_teststand(23, "clusters")  might return   ("cluster1", "cluster2") 
//
//----------------------------------------------------------------------------------------------------------------------

function find_objects_in_teststand($teststandID, $objecttype) { 

   $parents = determine_parent_chain($objecttype);
   
   if (count($parents) == 2) {
   
      $query = "SELECT Name FROM daf." . $objecttype . " WHERE parentID = '" .  $teststandID . "'";
   
   } else {
   
      $query = "SELECT ID FROM daf." . $parents[1] . " WHERE " . $parents[1] . ".parentID = '" . $teststandID . "'";
   
      for ($i=2; $i<count($parents) - 1; $i++) {   
         $query = "SELECT ID FROM daf." . $parents[$i] . " WHERE " . $parents[$i] . ".ID IN (" . $query . ")";     
      }
   
      $query = "SELECT Name FROM daf." . $parents[count($parents)-1] . " WHERE " . $parents[count($parents)-1] . ".ID IN (" . $query . ")"; 
   
   } 
   
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);   
   
   $names = array();
   
   for ($i = 0; $i<$num_rows; $i++) {
      $row = mysql_fetch_row($result);
      array_push($names, $row[0]);
   }
   
   return ($names);  
   
}

function get_field_from_row($tablename, $fieldname, $where) {

   $query = "SELECT $fieldname FROM $tablename WHERE $where";    
   $result = mysql_query($query);   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);    //* expected to be 1 not sure what to do if it is not
   
   $row = mysql_fetch_row($result);
   return $row[0];
   
}

function get_enum_fields($database, $tablename, $columnname) {
   
   $query = "SELECT COLUMN_TYPE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" . 
            $database . "' AND TABLE_NAME = '" . $tablename . "' AND COLUMN_NAME = '" . $columnname . "'";
            
   $result = mysql_query($query);   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);    //* expected to be 1 not sure what to do if it is not
   
   $row = mysql_fetch_row($result);
   
   // $row[0]  will be in form   enum('x','y','z')
   preg_match("/enum\('(.*)'\)/", $row[0], $matches);
   $enum_fields = explode("','", $matches[1]);

   return $enum_fields;
   
}

function selector_for_enum($database, $tablename, $columnname, $selectfieldname, $enumvalue = NULL) {

   $enumfields = get_enum_fields($database, $tablename, $columnname);
   
   echo "<select name=\"" . $selectfieldname . "\"/>\n";
   
   for ($i=0; $i<count($enumfields); $i++) {
     
     $selected = " ";
     if (! is_null($enumvalue)) {
        if ($enumvalue == $enumfields[$i]) {
           $selected = " selected ";
        }
     } 
     echo "<option" . $selected . "value=\"" . $enumfields[$i] . "\">" . $enumfields[$i] . "</option>\n";
        
  }
     
  echo "</select>\n";  

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_field_types
//
// Inputs:     $tablename
//           
// Outputs:    none
//
// Returns:    a array of the field types in that table, keyed by the fieldname . 
//             The field types are 3 letter abbreviations,  eg   var, enu, int   for varchar, enu, int  etc
//
// Description:  
//
//   eg  get_field_types("daf.clusters)  might return   (ID =>"int", ParentID=>"int", Name=>"var", Comments=>"var", Model=>"enu") 
//
//----------------------------------------------------------------------------------------------------------------------


function get_field_types($tablename) {

   $result = mysql_query("SHOW FIELDS FROM $tablename" );   /* what happens if it fails ? <<<<<<<< */

   $fieldtypes = array();
   while ($row = mysql_fetch_array($result)) { 
   
      $fieldtypes[$row['Field']] = substr($row['Type'], 0, 3); 
      
   } 

   return $fieldtypes;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_objectattribute_types
//
// Inputs:     $objecttypeID
//           
// Outputs:    none
//
// Returns:    a array of the object attribute field types in for that object, keyed by the fieldname . 
//
// Description:  
//
//   eg  get_objectattribute_types("clusters)  might return   
//             [Model=>["Type"=>"varchar", Varcharwidth=>"32", Enumvalues=>""],
//             [Jff=>["Type"=>"enum", Varcharwidth=>"", Enumvalues=>"'fred','bill','jim'"]]
//
//----------------------------------------------------------------------------------------------------------------------

function get_objectattribute_types($objecttypeID) {

   $query = "SELECT ID, Name, ObjecttypeID, Type, Varcharwidth, Enumvalues FROM daf.objectattributetype WHERE ObjecttypeID='" . $objecttypeID . "'"; 
            
   $result = mysql_query($query);   
   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
  
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $nameindex = $fieldindex['Name'];
   $typeindex = $fieldindex['Type'];
   $varcharwidthindex = $fieldindex['Varcharwidth'];
   $enumvaluesindex = $fieldindex['Enumvalues'];
   
   $types = array();    
   
   for ($i=0; $i<$num_rows; $i++) {

      $row = mysql_fetch_row($result);
      $name = $row[$nameindex];
      $types[$name]["Type"] = $row[$typeindex];
      $types[$name]["Varcharwidth"] = $row[$varcharwidthindex];
      $types[$name]["Enumvalues"] = $row[$enumvaluesindex];
   }  

   return($types);   

}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_backup_filenames
//
// Inputs:     $tablename
//           
// Outputs:    $filenames   a array of the backup files for the specified tablename (if this is NULL or omitted then 
//             all backups are returned
//             $completion_message
// 
// Returns:    TRUE if successful, FALSE if not
//
// Description:  
//
//    
//   format of the backupname is  /opt/lampp/htdocs/daf/backup/daf.YYYYMMDD.HHMMSS.sql
//                                /opt/lampp/htdocs/daf/backup/project.YYYYMMDD.HHMMSS.sql
//
//----------------------------------------------------------------------------------------------------------------------

function get_backup_filenames($tablename = NULL, $completion_message, &$filenames) {

   $rc = 0;
   $filenames = array();
  
   $backup_dir = "/opt/lampp/htdocs/daf/backup";
   exec("ls -t $backup_dir 2>&1", $output, $rc);

   if ($rc == 0) {
      foreach ($output as $filename) {
         if (isset($tablename)) {                 /* <<<< the following regexp does not look right....needs a leading /  */
           $num_matches = preg_match("$tablename" . '/\.(\d\d\d\d\d\d\d\d)\.(\d\d\d\d\d\d)\.sql/', $filename, $matches);
         } else {
           $num_matches = preg_match('/(\S+)\.(\d\d\d\d\d\d\d\d)\.(\d\d\d\d\d\d)\.sql/', $filename, $matches);
         }
         if ($num_matches > 0) {
           array_push($filenames, $filename);
         }
      }
      $rc = 1;
   } else {
      $completion_message = implode("\n", $output);
   }
   return $rc;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   selector_on_array    
//
// Inputs:     $array
//             $selected_value 
//           
// Outputs:    none                                                                                                   
// 
// Returns:    nothing                                
//
// Description:  
//
//----------------------------------------------------------------------------------------------------------------------

function selector_on_array($array, $selected_value = NULL) {

   for ($i = 0; $i < count($array); $i++) {
      if ($array[$i] == $selected_value) {
         echo "<option selected value=\"" . $array[$i] . "\">" . $array[$i] . "</option>\n";
      } else {
         echo "<option value=\"" . $array[$i] . "\">" . $array[$i] . "</option>\n";
      }
   }

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   cellbutton    
//
// Inputs:     $tagtype               'th' or 'td'  (to indicate cell type)
//             $id                    a unique id to be assigned to the button
//             $label                 the label to put on the button
//             $windowlocation        the window location that pushing the button will go to, eg
//                                    index.php?action=show&object=user&ID=23
//             $submit                
//             (Note: $windowlocation and $submit should never be defined at the same time, only one or other is required)
//           
// Outputs:    none                                                                                                   
// 
// Returns:    nothing                                
//
// Description:  
//
//----------------------------------------------------------------------------------------------------------------------

function cellbutton($tagtype, $id, $label, $windowlocation=null, $submit=null) {

       if (isset($tagtype)) {
          echo "<$tagtype>\n";
       }
       if (isset($submit)) {
          $type = "type:'submit'";
          if (isset($windowlocation)) {
             $type .= ", ";
          }
       } else {
          $type = "type:'button'";
          if (isset($windowlocation)) {
             $type .= ", ";
          }
       }
       if (isset($windowlocation)) {
          $goto = "onClick:function(){ window.location = '$windowlocation'}";
       } else {
          $goto = "";
       }
       echo "   <button id=\"$id\" data-dojo-type=\"dijit.form.Button\" data-dojo-props=\"$type $goto\">\n";
       echo "       $label\n";
       echo "   </button>\n";
       if (isset($tagtype)) {
          echo "</$tagtype>\n";
       }
       
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   button_with_confirm    
//
// Inputs:     $id                    an unique id to be assigned to the button
//             $label                 the label to put on the button - eg an action like 'Delete'
//             $verb                  the XXXX that will appear in the 'Are you sure you want to XXXX this item' message
//             $gowindowlocation      the window location that pushing the button will go to, if the user confirms
//                                    that the action is okay, eg index.php?action=delete&object=user&ID=23
//             $nogowindowlocation    the window location that pushing the button will go to if the user declines
//                                    to confirm the action, eg index.php?action=show&object=user&ID=23
//           
// Outputs:    none                                                                                                   
// 
// Returns:    nothing                                
//
// Description:  
//
//----------------------------------------------------------------------------------------------------------------------

function button_with_confirm($id, $label, $verb, $gowindowlocation, $nogowindowlocation) {

       echo "   <button id=\"$id\" data-dojo-type=\"dijit.form.Button\" data-dojo-props=\"type:'button', " . 
            "onClick:function(){ if (window.confirm('Are you sure you want to " . $verb . " this item?')) {window.location = '$gowindowlocation'} else {window.location = '$nogowindowlocation'} } \">\n";
       echo "       $label\n";
       echo "   </button>\n";
       
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   restartdafserver    
//
// Inputs:     none
//           
// Outputs:    $completion_message                                                                                                  
// 
// Returns:    the return code of the restart command, 0 is success, 1 is failure                                
//
// Description:  
//
// Restarst the DAF server
//
//----------------------------------------------------------------------------------------------------------------------

function restartdafserver(&$completion_message) {

   $restartcmd = "export LD_LIBRARY_PATH=/opt/lampp/lib:\$LD_LIBRARY_PATH; /opt/daf/daf -service stop start 2>&1";
   exec($restartcmd, $output, $rc);
   add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $restartcmd), $rc) ;
   
   if ($rc == 0) {    /* <<<<< */
     $completion_message = "DAF server was restarted";
   } else {
      $completion_message = "DAF server failed to restart - " . implode("\n", $output);
   }   
   
   return $rc;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   restartdafwebserver    
//
// Inputs:     none
//           
// Outputs:    $completion_message                                                                                                  
// 
// Returns:    the return code of the restart command, 0 is success, 1 is failure                                
//
// Description:  
//
// Restast the DAF Apache web server -  - this won't work as apache does not have root permissions
//
//----------------------------------------------------------------------------------------------------------------------

function restartdafwebserver(&$completion_message) {

   $stopcmd = "export LD_LIBRARY_PATH=/opt/lampp/lib:\$LD_LIBRARY_PATH; /opt/lampp/lampp stopapache 2>&1";
   exec($stopcmd, $output, $rc);
   add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $stopcmd), $rc) ;
   
   if ($rc == 0) {   
     $startcmd = "export LD_LIBRARY_PATH=/opt/lampp/lib:\$LD_LIBRARY_PATH; /opt/lampp/lampp startapache 2>&1 ";         /* <<<<<<<<<<<  wrong execution priviledges ?? */
     exec($startcmd, $output, $rc);
     add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $startcmd), $rc) ;
     if ($rc == 0) {           
       $completion_message = "DAF web server was restarted - " . implode("\n", $output);     
     } else {
       $completion_message = "DAF web server failed to start - " . implode("\n", $output);
     }
   } else {
      $completion_message = "DAF web server failed to stop - " . implode("\n", $output);
   }  
   
   return $rc;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   restartdafsqlserver    
//
// Inputs:     none
//           
// Outputs:    $completion_message                                                                                                  
// 
// Returns:    the return code of the restart command, 0 is success, 1 is failure                                
//
// Description:  
//
// Restart the DAF Mysql server - this won't work as apache does not have root permissions
//
//----------------------------------------------------------------------------------------------------------------------

function restartdafsqlserver(&$completion_message) {

   $stopcmd = "export LD_LIBRARY_PATH=/opt/lampp/lib:\$LD_LIBRARY_PATH; /opt/lampp/lampp stopmysql 2>&1";
   exec($stopcmd, $output, $rc);
   add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $stopcmd), $rc) ;
   
   if ($rc == 0) {   
     $startcmd = "export LD_LIBRARY_PATH=/opt/lampp/lib:\$LD_LIBRARY_PATH; /opt/lampp/lampp startmysql 2>&1 ";         /* <<<<<<<<<<<  wrong execution priviledges ?? */
     exec($startcmd, $output, $rc);
     add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $startcmd), $rc) ;
     if ($rc == 0) {           
       $completion_message = "DAF sql server was restarted";     
     } else {
       $completion_message = "DAF sql server failed to start - " . implode("\n", $output);
     }
   } else {
      $completion_message = "DAF sql server failed to stop - " . implode("\n", $output);
   }   
   
   return $rc;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   checkdafserver    
//
// Inputs:     none
//           
// Outputs:    $completion_message                                                                                                  
// 
// Returns:    0 is success, 1 is failure                                
//
// Description:  
//
// Checks that the DAF server is running on localhost - sends a query version to the DAF service on the local host
//
//----------------------------------------------------------------------------------------------------------------------

function checkdafserver(&$completion_message) {

   $queryversioncmd = "export LD_LIBRARY_PATH=/opt/lampp/lib:\$LD_LIBRARY_PATH; /opt/daf/daf -remote localhost -query version 2>&1";
   exec($queryversioncmd, $output, $rc);
   
   if ($rc == 0) {   
      $completion_message = "DAF server is running -\n" . implode("\n", $output) . "\n\n";
   } else {
      $completion_message = "DAF server does not appear to be running -\n" . implode("\n", $output) . "\n\n";
   }  
   
   $diskspace = disk_total_space("/opt/daf");
   $diskfreespace = disk_free_space("/opt/daf");
   $percentfree = ($diskspace - $diskfreespace) / $diskspace * 100;
   
   if ($percentfree < 5.0) {
      $completion_message .= "WARNING: /opt/daf filesystem has less than 5% free space (free bytes = $diskfreespace out of a total of $diskspace bytes)\n";
   } else {
      $completion_message .= "/opt/daf filesystem has " . sprintf("%.1f", $percentfree) . "% free space (free bytes = $diskfreespace out of a total of $diskspace bytes)\n";
   }

   return $rc;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   report_bad_parms  
//
// Inputs:     $routinename
//           
// Outputs:    none                                                                                                 
// 
// Returns:    nothing                                
//
// Description:  
//
// Adds an error message to the standard error area in the body, used when a routine cannot understand the $gp parameters
// it has been passed 
//
//----------------------------------------------------------------------------------------------------------------------

function report_bad_parms($routinename = "unknown") {

      add_error_message("DAF0003: $routinename: did not understand parameters");
      ob_start();
      print_r($gp);
      add_error_message(ob_get_contents());
      ob_end_clean();

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   do_mysql_query  
//
// Inputs:     $query
//             $db_server     optional
//             $auditme       1 if the query is to be recorded in the audit log
//           
// Outputs:    none                                                                                                 
// 
// Returns:    0 for failure,                              
//
// Description:  
//
// Performs a mysql query.   If the query fails, then 0 is returned.  If the query succeeds, 1 
//
//----------------------------------------------------------------------------------------------------------------------
function do_mysql_query($query, $db_server = null, $auditme = 0) {

   global $current_user_email;
   
   $rc = 1;

   if (isset($db_server)) {
      if (! mysql_query($query, $db_server)) {
         add_error_message("DAF0004: mysql query: $query failed - " . mysql_error());
         $rc = 0;
      }
   } else {
      if (! mysql_query($query)) {
         add_error_message("DAF0004: msyql query: $query failed - " . mysql_error());
         $rc = 0;
      } 
   }
   
   if ($auditme) {                                                                                             /* if $query already has \' in this then we have a problem <<<< */
      add_auditlog_entry($current_user_email, (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $query), mysql_errno()) ;
   }   

   
   return $rc;
   

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   do_mysql_query_id  
//
// Inputs:     $query
//             $db_server     optional
//             $auditme       1 if the query is to be recorded in the audit log
//           
// Outputs:    none                                                                                                 
// 
// Returns:    0 for failure,                              
//
// Description:  
//
// Performs a mysql query.   If the query fails, then 0 is returned.  If the query succeeds, the id of the row inserted is 
// returned.  If no ID is available, 0 is returned as if the query had failed 
//
//----------------------------------------------------------------------------------------------------------------------
function do_mysql_query_id($query, $db_server = null, $auditme = 0) {

   global $current_user_email;

   if (isset($db_server)) {
      if (! mysql_query($query, $db_server)) {
         add_error_message("DAF0005: do_mysql query_id: $query failed - " . mysql_error());
         $rc = 0;
      } else {
         $rc = mysql_insert_id();
      }
   } else {
      if (! mysql_query($query)) {
         add_error_message("DAF0005: do_msyql query_id: $query failed - " . mysql_error());
         $rc = 0;
      } else {
         $rc = mysql_insert_id();
      }
   }

   if ($auditme) {                                                                                             /* if $query already has \' in this then we have a problem <<<< */
                                                                                                               /* same applies elsewhere */
      add_auditlog_entry($current_user_email, (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $query), mysql_errno()) ;
   }   
   
   return $rc;

}


?>
