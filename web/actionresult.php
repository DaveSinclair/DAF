<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  actionresult.php
//
//  This file is part of the DAF user interface PHP code.  This file deals with actions relating to actionresult objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                  Action to be carried out
//
//  object=actioncluster  action=show                                 display all actionclusters
//  object=actioncluster  action=show  ID=                            display an individual actioncluster
// 
//  object=actioncluster  action=show    operation=create             insert a new actioncluster in the DAF actioncluster table
//                              name=... comments=....                with characteristics as specified by
//                              clusterID=.... type=....              the POST'd variables
//  object=actioncluster  action=show    operation=modify  ID=...     modify an existing actioncluster record in the 
//                              name=... comments=....                DAF actioncluster table
//                              clusterID=.... type=....
//  object=actioncluster  action=show    operation=delete  ID=...     delete an existing record in the DAF actioncluster
//                                                                    table
//
//  object=actioncluster  action=create                               present a form that allows the user to specify
//                                                                    the characteristics of a new actioncluster that is to be
//                                                                    added to the DAF actioncluster table
//  object=actioncluster  action=modify  ID=...                        present a form that allows the user to modify
//                                                                    the characteristics of an existing actioncluster in the
//                                                                    DAF actioncluster table


  //  object= actionresult  action=show  ID= name=
  
  //  object= actionresult  action=create 
  //  object= actionresult  action=show    operation=create ScenarioresultID, Testcase, Invocation, Pass, Loglocation, Comments
    
  //  object= actionresult  action=modify  name=...
  //  object= actionresult  action=show    operation=modify  ID=...  name=...

  //  object= actionresult action=show    operation=delete  ID=...

//
//---------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   actionresult
//
// Inputs:     $gp
//             $current_project
//             $current_phase
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a actionresult object.   The allowed actions are to create, modify or delete the 
// actionresult record in the actionresult table.
//
//----------------------------------------------------------------------------------------------------------------------

function actionresult($gp, $current_project, $current_phase) {

   global $db_server;

   if ($gp['object'] !== "actionresult") {
     add_error_message("DAFE0001: actionresult: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "actionresult";
   }
   
   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "actionresult");

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
                                        
    // ScenarioResultID, Actiontype, Stepnumber, State, Testcase, Invocation, Pass, Goodactions, Failedactions, Start, End, Loglocation, Comments    
    
         $query = "INSERT INTO " .$tablename . " (ScenarioresultID, Actiontype, Stepnumber, Status, Testcase, Invocation, " . 
                                               "Failedactions, Start, End, Comments) " .
                  "VALUES('" . $gp['scenarioresultID'] . "','" . 
                               $gp['actiontype'] . "','" .
                               $gp['stepnumber'] . "','" .
                               $gp['status'] . "','" .
                               $gp['testcase'] . "','" .
                               $gp['invocation'] . "','" .
                               $gp['pass'] . "','" .
                               $gp['start'] . "','" .
                               $gp['end'] . "','" .
                               $gp['comments'] . "')";              
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         if (! am_i_admin()) {
         
            add_error_message("DAFE0096: Only an admin user can modify an Action Result");
      
         } else {
         
            $query = "UPDATE " . $tablename . " SET Pass='" . $gp['pass'] . "', " .
                                              "Comments='" . $gp['comments'] . "'  " .                                             
                     "WHERE ID='" . $gp['ID'] . "'";   
            do_mysql_query($query, $db_server, 1);
            
            revise_scenarioresult($gp['scenarioresultID'], $status_db);
         }
         
         $gp['action'] = 'show';      
         $gp['ID'] = null;
         
      } else if ($gp['operation'] === "delete") {

         if (! am_i_admin()) {
         
            add_error_message("DAFE0097: Only an admin user can delete an Action Result");
      
         } else {
         
            $query = "DELETE FROM " . $tablename . " where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            
            revise_scenarioresult($gp['scenarioresultID'], $status_db);
           
         } 
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_actionresult($current_project, $current_phase);
    
   } else if ($gp['action'] === "modify") {

       modify_actionresult($gp['ID'], $status_db);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_actionresults1($status_db);
                    
      } else {
 
         show_detailed_actionresult($gp['ID'], $status_db);
      }
   
   } else if ($gp['action'] === "getstore") {
            
      query_actionresult_store($current_project, $current_phase);

   } else {
    
      report_bad_parms("actionresult");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_actionresult
//
// Inputs:     $current_project, $current_phase   
//
// Outputs:    Post's a list of variables that describe the characteristics of the actionresult that is to be created 
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new actionresult that is to be added to the DAF node
// table.   The routine posts
//
// status.php?object=node?action=show&operation=create&name=.............<<<<<< 
//
//----------------------------------------------------------------------------------------------------------------------

function create_actionresult($current_project, $current_phase) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $scenarioresult_tablename = determine_status_table_name($status_db, "scenarioresult");  /* <<<<<<<<<<<<<< this routine is broken !!!!!! */

   echo '<div dojoType="dijit.form.Form" id="create_actionresult" jsId="create_actionresult" encType="multipart/form-data" action="index.php" method="post">';
   echo <<<_END
      <script type="dojo/method" event="onSubmit">
        if (! this.validate()) {
            alert('Form cannot be submitted as it contains one or more invalid fields');
            return false;
        }
        return true;
    </script>
_END;

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Create an Action Result</div></caption>\n";

   echo '<tr><th>ScenarioResult</th><td><select name="scenarioresultID" />' . "\n";
   selectorID($scenarioresult_tablename, "Name", null);
   echo "</select></td></tr>\n"; 
   echo '<tr><th>Action</th><td><select name="actiontype" />' . "\n";
   selectorfield("actiontype", "Type", "Type", null);
   echo "</select></td></tr>\n";        
   echo '<tr><th>Testcase</th><td><select name="testcase" />' . "\n";
   selectorname("testcase", "Name", null);
   echo "</select></td></tr>\n";
   echo '<tr><th>Stepnumber</th>';
   echo '<td><input type="text" name="stepnumber" size = "' . DAF_ACTIONRESULT_STEPNUMBER_LEN . '" maxlength="' . DAF_ACTIONRESULT_STEPNUMBER_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONRESULT_STEPNUMBER_LEN . 'em;" regExp="\d+" required="true" trim="true" promptMessage="Enter a step number" ' . 
        'invalidMessage="Invalid stepnumber - must be a decimal number)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Status</th>';
   echo '<td><input type="text" name="status" size = "' . DAF_ACTIONRESULT_STATUS_LEN . '" maxlength="' . DAF_ACTIONRESULT_STATUS_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONRESULT_STATUS_LEN . 'em;" regExp="\w+" required="true" trim="true" promptMessage="Enter a valid status" ' . 
        'invalidMessage="Invalid status - must be one of (' . DAF_VALID_ACTION_STATUS . ')"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Invocation</th>';
   echo '<td><input type="text" name="invocation" size = "' . DAF_ACTIONRESULT_INVOCATION_LEN . '" maxlength="' . DAF_ACTIONRESULT_INVOCATION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONRESULT_INVOCATION_LEN . 'em;" required="true" trim="true" promptMessage="Enter a valid invocation"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Pass</th><td>';
   echo '<input type="text\" name="pass" size = "' . DAF_ACTIONRESULT_PASS_LEN . '" maxlength="' . DAF_ACTIONRESULT_PASS_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONRESULT_PASS_LEN . 'em;" regExp="\d{1,3}" required="true" trim="true" promptMessage="Enter a pass rate between 0 and 100" ' .   /* incomplete validation */
        'invalidMessage="Invalid stepnumber - must be 0 to 100)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Start</th>';
   echo '<td><input type="text" name="start" size = "' . DAF_ACTIONRESULT_START_LEN . '" maxlength="' . DAF_ACTIONRESULT_START_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONRESULT_START_LEN . 'em;" regExp="\d\d\d\d\/\d\d\/\d\d\s\d\d:\d\d:\d\d" required="true" trim="true" promptMessage="Enter a YYYY/MM/DD hh:mm:ss" ' .  
        'invalidMessage="Invalid stepnumber - must be YYYY/MM/DD hh:mm:ss"/>';
   echo "</td></tr>\n";
   echo '<tr><th>End</th>';
   echo '<td><input type="text" name="end" size = "' . DAF_ACTIONRESULT_END_LEN . '" maxlength="' . DAF_ACTIONRESULT_END_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONRESULT_END_LEN . 'em;" regExp="\d\d\d\d\/\d\d\/\d\d\s\d\d:\d\d:\d\d" required="true" trim="true" promptMessage="Enter a YYYY/MM/DD hh:mm:ss" ' .  
        'invalidMessage="Invalid stepnumber - must be YYYY/MM/DD hh:mm:ss"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Comments</th>';
   echo '<td><input type="text" name="comments" size = "' . DAF_ACTIONRESULT_COMMENTS_LEN . '" maxlength="' . DAF_ACTIONRESULT_COMMENTS_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:50em;" promptMessage="Enter a comment"/>';  
   echo "</td></tr>\n";
   echo "</table>\n";
 
   cellbutton(null, "actionresult_create", "Create Action Result", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"actionresult\" />\n";
   cellbutton(null, "actionresult_cancel", "Cancel", "index.php?object=actionresult&action=show", null);

   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_actionresults
//
// Inputs:     $status_db         the name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all the nodes in the DAF node table.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=node?action=create&object=actionresults             to create a new actionresult
// status.php?object=node?action=modify&object=actionresults&ID=...      to modify an existing actoinresult
// status.php?object=node?action=delete&object=actionresults&ID=...      to delete an existing actionresult
//
// The date range of the actionresults that are being displaying can be changed by the user submitting one of a number of
// forms which result in the following URLs to be posted
//
// status.php?object=actionresult?action=show&startviewdate=...&endviewdate=...&actionresultviewrequest 
// status.php?object=actionresult?action=show&actionresult_Ndays=N&actionresultviewNdaysrequest
// status.php?object=actionresult?action=show&actionresultviewallrequest 
//
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_actionresults($status_db) {

   global $actionresult_startviewdate, $actionresult_endviewdate, $actionresult_startviewtime, $actionresult_endviewtime, $actionresult_Ndays;  

   $tablename = determine_status_table_name($status_db, "actionresult");   
    
   $query = "SELECT ID, ScenarioresultID, Actiontype, Stepnumber, Status, Testcase, Invocation, Pass, " . 
            "Start, End, Comments FROM " . $tablename . " WHERE Start >= '" . $actionresult_startviewdate . " " .  $actionresult_startviewtime .  
            "' AND End <= '" . $actionresult_endviewdate . " " .  $actionresult_endviewtime . "' ORDER BY Start desc";   
   
   $actionresult = mysql_query($query);
    
   if (! $actionresult) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $lookup = lookuptable($tablename, "ID", "Jobname");
   $fieldnames = array("ID", "Scenario Result ID", "Action Type", "Step number", "Status", "Testcase", "Invocation", "Pass",  
                       "Start", "End", "Comments");

   $num_rows   = mysql_num_rows($actionresult);
   $num_fields = mysql_num_fields($actionresult);
   
         echo '<form style="float:left;" action="index.php" method="post">' . "\n";
         
         echo '<div style="float:left;">';
         echo 'Start view date: <input dojoType="dijit.form.DateTextBox" name=startviewdate value="' . $actionresult_startviewdate . '" />' . "\n";
         echo '</div>';

         echo '<div style="float:left;">';
         echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=startviewtime value="' . $actionresult_startviewtime . '" />' . "\n";
         echo '</div>';
         
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp; End view date: <input dojoType="dijit.form.DateTextBox" name=endviewdate value ="' . $actionresult_endviewdate . '" />' . "\n";
         echo '</div>'; 

         echo '<div style="float:left;">';
         echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=endviewtime value="' . $actionresult_endviewtime . '" />' . "\n";
         echo '</div>';
         
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp;';
         cellbutton(null, "actionresult_refreshview", "Refresh View", null, "submit");
         echo '<input type="hidden" name="action" value="show" />' . "\n";
         echo '<input type="hidden" name="object" value="actionresult" />' . "\n";
         echo '<input type="hidden" name="actionresultviewrequest" value="actionresultviewrequest" />' . "\n";
         echo '</div>';
         echo '</form>' . "\n";
         
         echo '<form style="float:left;" action="index.php" method="post">' . "\n";
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp;';

         echo ' N days: <input type="text" name="actionresult_Ndays" value="' . $actionresult_Ndays . '" />' . "\n";
         echo '&nbsp;&nbsp;&nbsp;';
         cellbutton(null, "actionresult_Ndays", "View N days", null, "submit");
         echo '<input type="hidden" name="actionresultviewNdaysrequest" value="actionresultviewNdaysrequest" />' . "\n";
         echo '<input type="hidden" name="action" value="show" />' . "\n";
         echo '<input type="hidden" name="object" value="actionresult" />' . "\n";
         echo '</div>';       
         echo '</form>' . "\n";       

         echo '<form action="index.php" method="post">' . "\n";
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp;';
         cellbutton(null, "actionresult_viewalldays", "View all", null, "submit");
         echo '<input type="hidden" name="actionresultviewallrequest" value="actionresultviewallrequest" />' . "\n";
         echo '<input type="hidden" name="action" value="show" />' . "\n";
         echo '<input type="hidden" name="object" value="actionresult" />' . "\n";
         echo '</div>';       
         echo '</form>' . "\n";
         
         echo '<div style="clear:both">';    

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Action/Testcase Results</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr><th>show</th><th>modify</th><th>delete</th>';
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldindex[mysql_field_name($actionresult, $i)] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $scenarioresultindex = $fieldindex['ScenarioresultID'];
   $passindex = $fieldindex['Pass'];
 
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($actionresult);
       if ($row[$passindex] == 100) {
          echo '<tr>';
          $tdclass = "";
          $smd_tdclass = 'class = "td_smd"';
       } else {
          echo '<tr class="fail">';
          $tdclass = 'class="fail"';
          $smd_tdclass = 'class = "td_smd_fail"';
       }
       $ID = $row[$fieldindex['ID']];
       $scenarioresultID = $row[$fieldindex['ScenarioresultID']];
       echo "<td $smd_tdclass>\n";
       cellbutton(null, "actionresult_show_$j", "show", "index.php?action=show&object=actionresult&ID=$ID");
       echo "</td>\n";
       echo "<td $smd_tdclass>\n";
       cellbutton(null, "actionresult_modify_$j", "modify", "index.php?action=modify&object=actionresult&ID=$ID");
       echo "</td>\n";
       echo "<td $smd_tdclass>\n";
       // cellbutton(null, "actionresult_delete_$j", "delete", "index.php?action=show&operation=delete&object=actionresult&scenarioresultID=$scenarioresultID&ID=$ID");
       button_with_confirm("actionoutresult_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=actionresult&scenarioresultID=$scenarioresultID&ID=$ID", 
                           "index.php?action=show&object=actionresult");
       echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {       
          if ($i == $scenarioresultindex) {
            if (isset($lookup[$row[$i]])) {
               echo '<td ' . $tdclass . '><a href="index.php?action=show&object=scenarioresult&ID='  . $scenarioresultID . '">' . $lookup[$scenarioresultID] . '</td>';
            } else {
               echo "<td $tdclass> not allocated </td>";
            }
          } else {
            echo "<td $tdclass>" . $row[$i] . '</td>';
          }
       }   
       echo '</tr>';
       echo "\n";

   }
      
  echo '</table>';
  echo '</div>';
  
  echo '</form>' . "\n"; 
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_actionresults1
//
// Inputs:     $status_db         the name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all the nodes in the DAF node table.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=node?action=create&object=actionresults             to create a new actionresult
// status.php?object=node?action=modify&object=actionresults&ID=...      to modify an existing actoinresult
// status.php?object=node?action=delete&object=actionresults&ID=...      to delete an existing actionresult
//
// The date range of the actionresults that are being displaying can be changed by the user submitting one of a number of
// forms which result in the following URLs to be posted
//
// status.php?object=actionresult?action=show&startviewdate=...&endviewdate=...&actionresultviewrequest 
// status.php?object=actionresult?action=show&actionresult_Ndays=N&actionresultviewNdaysrequest
// status.php?object=actionresult?action=show&actionresultviewallrequest 
//
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_actionresults1($status_db) {

   global $actionresult_startviewdate, $actionresult_endviewdate, $actionresult_startviewtime, $actionresult_endviewtime, $actionresult_Ndays;  

   $tablename = determine_status_table_name($status_db, "actionresult");   
    
         echo '<form style="float:left;" action="index.php" method="post">' . "\n";
         
         echo '<div style="float:left;">';
         echo 'Start view date: <input dojoType="dijit.form.DateTextBox" name=startviewdate value="' . $actionresult_startviewdate . '" />' . "\n";
         echo '</div>';

         echo '<div style="float:left;">';
         echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=startviewtime value="' . $actionresult_startviewtime . '" />' . "\n";
         echo '</div>';
         
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp; End view date: <input dojoType="dijit.form.DateTextBox" name=endviewdate value ="' . $actionresult_endviewdate . '" />' . "\n";
         echo '</div>'; 

         echo '<div style="float:left;">';
         echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=endviewtime value="' . $actionresult_endviewtime . '" />' . "\n";
         echo '</div>';
         
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp;';
         cellbutton(null, "actionresult_refreshview", "Refresh View", null, "submit");
         echo '<input type="hidden" name="action" value="show" />' . "\n";
         echo '<input type="hidden" name="object" value="actionresult" />' . "\n";
         echo '<input type="hidden" name="actionresultviewrequest" value="actionresultviewrequest" />' . "\n";
         echo '</div>';
         echo '</form>' . "\n";
         
         echo '<form style="float:left;" action="index.php" method="post">' . "\n";
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp;';

         echo ' N days: <input type="text" name="actionresult_Ndays" value="' . $actionresult_Ndays . '" />' . "\n";
         echo '&nbsp;&nbsp;&nbsp;';
         cellbutton(null, "actionresult_Ndays", "View N days", null, "submit");
         echo '<input type="hidden" name="actionresultviewNdaysrequest" value="actionresultviewNdaysrequest" />' . "\n";
         echo '<input type="hidden" name="action" value="show" />' . "\n";
         echo '<input type="hidden" name="object" value="actionresult" />' . "\n";
         echo '</div>';       
         echo '</form>' . "\n";       

         echo '<form action="index.php" method="post">' . "\n";
         echo '<div style="float:left;">';
         echo '&nbsp;&nbsp;&nbsp;';
         cellbutton(null, "actionresult_viewalldays", "View all", null, "submit");
         echo '<input type="hidden" name="actionresultviewallrequest" value="actionresultviewallrequest" />' . "\n";
         echo '<input type="hidden" name="action" value="show" />' . "\n";
         echo '<input type="hidden" name="object" value="actionresult" />' . "\n";
         echo '</div>';       
         echo '</form>' . "\n";
         
         echo '<div style="clear:both">';    
         echo '</div>';
 
echo <<<_END

   <link rel="stylesheet" href="./dojo/dojox/grid/resources/Grid.css" media="screen">
   <link rel="stylesheet" href="./dojo/dojox/grid/resources/claroGrid.css" media="screen">

   <div style="width:100%; height:600px;">   
   <div id="ActionresultStatus" ></div>
   </div>

   
   <script type="text/javascript">
      dojo.require("dojo.parser");
      dojo.require("dojox.grid.DataGrid");
      dojo.require("dojo.data.ObjectStore");

      dojo.ready(function() {

          // objstore = dojo.data.ItemFileReadStore({url: "index.php?object=actionresult&action=getstore"});
         
         dojo.require("dojo.store.JsonRest");
_END;

         echo "         myStore = new dojo.store.JsonRest({target:\"rest/" . $status_db . "/actionresult/\"});";
  
echo <<<_END
         
         var grid = new dojox.grid.DataGrid({    // use the adapter to give the grid a Dojo Data compliant interface to the store    
                            // store: dataStore = objstore,
                            store: dataStore = dojo.data.ObjectStore({objectStore: myStore}),
                            escapeHTMLInData: false,          /* <<<<<<<< is this safe (cross site attacks) */
                            columnReordering: true,
                            defaultcell: {width:"8em", styles:" border-style: solid;border-width: 1px; border-color:red; background-color: #B7DBFF;"},
                            structure: [ {name:"ID", field:"ID", width:"3em", styles:"text-align:center;border-width: 1px; border-color:red;"},        
                                         {name:"Scenario Result ID", field:"ScenarioresultID", width:"8em", styles:"text-align:center;"},        
                                         {name:"Action Type", field:"Actiontype", width:"8em"},        
                                         {name:"Step Number", field:"Stepnumber", width:"5em", styles:"text-align:center;"},        
                                         {name:"Status", field:"Status", width:"8em"},  
                                         {name:"Hostname", field:"Hostname", width:"8em"},
                                         {name:"Testcase", field:"Testcase", width:"8em"},
                                         {name:"Invocation", field:"Invocation", width:"32em"},
                                         {name:"Pass", field:"Pass", width:"4em", styles:"text-align:center;"},
                                         {name:"Start", field:"Start", width:"6em", styles:"text-align:center;"},
                                         {name:"End", field:"End", width:"6em", styles:"text-align:center;"},
                                         {name:"Comments", field:"Comments", width:"16em"}
                                       ]}, 
                            "ActionresultStatus");
                            

         grid.startup();
         
      });
      
   </script>
_END;

}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_actionresult
//
// Inputs:     ID                the ID of the row in the DAF cluster table containing the actionresult whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed fields of an actionresult rentry
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_actionresult($ID, $status_db) {

   $tablename = determine_status_table_name($status_db, "actionresult");

   $query = "SELECT ID, ScenarioResultID, Actiontype, Stepnumber, Status, Testcase, Invocation, Pass, " . 
            "Start, End, Comments FROM " . $tablename . " where ID = '"  . $ID . "'";

   $actionresult = mysql_query($query);

   if (! $actionresult) die ("Database access failed for query=$query: " . mysql_error());
   
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $lookup = lookuptable($tablename, "ID", "Jobname");
   $fieldnames = array("ID", "ScenarioResultID", "Action Type", "Stepnunber", "Status", "Testcase", "Invocation", "Pass",  
                       "Start", "End", "Comments");

   $num_rows   = mysql_num_rows($actionresult);   // should only be one row
   $num_fields = mysql_num_fields($actionresult);
   
   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Action/Testcase Results</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   $scenarioresultindex = $fieldindex['ScenarioResultID'];   
   
   $actionresult_row = mysql_fetch_row($actionresult);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($actionresult, $i) . '</th>';
      if ($i == $scenarioresultindex) {
         if (isset($lookup[$actionresult_row[$i]])) {
            echo '<td>'  . $lookup[$actionresult_row[$i]] . '</td>';
         } else {
            echo '<td> not allocated </td>';
         }
      } else {        
         echo '<td>' . $actionresult_row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';   
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_actionresult
//
// Inputs:     none   
//
// Outputs:    Post's a list of variables that describe the characteristics of the node that is to be modified 
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a node that is already in the DAF node
// table.   The routine posts
//
// status.php?object=node?action=show&operation=modify&object=actionresult&clusterID=...&.<morestuff>
//
//----------------------------------------------------------------------------------------------------------------------

function modify_actionresult($ID, $status_db) {

    // ScenarioResultID, Actiontype, Stepnumber, Status, Hostname, Testcase, Invocation, Pass, Start, End, Comments  
     
   $actionresult_tablename = determine_status_table_name($status_db, "actionresult");
    
   $query = "SELECT ID, ScenarioResultID, Actiontype, Stepnumber, Status, Testcase, Invocation, Pass," . 
            "Start, End, Comments FROM " . $actionresult_tablename . " where ID = '"  . $ID . "'";

   $actionresult = mysql_query($query);

   if (! $actionresult) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($actionresult);   // should only be one row
   $num_fields = mysql_num_fields($actionresult);
   
   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Action/Testcase Results</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[mysql_field_name($actionresult, $i)] = $i;
   }
   $IDindex = $fieldindex['ID'];   
   $scenarioresultindex = $fieldindex['ScenarioResultID'];   
   $actiontypeindex = $fieldindex['Actiontype'];
   $stepindex = $fieldindex['Stepnumber'];
   $statusindex = $fieldindex['Status'];
   $testcaseindex = $fieldindex['Testcase'];
   $invocationindex = $fieldindex['Invocation'];
   $passindex = $fieldindex['Pass'];
   $startindex = $fieldindex['Start'];
   $endindex = $fieldindex['End'];
   $commentsindex = $fieldindex['Comments'];
  
   $actionresult_row = mysql_fetch_row($actionresult);
 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($actionresult, $i);
      if ($i == $IDindex) {
         echo '<th>ID</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $scenarioresultindex) {
         echo '<th>Scenario Result ID</th>';
         echo "<td><input type=\"text\" name=\"scenarioresultID\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $actiontypeindex) {
         echo '<th>Action Type</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $stepindex) {
         echo '<th>Step</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $statusindex) {
         echo '<th>Status</th>';
         echo '<td><select name="status" />' . "\n";
         selectoractionstatus($actionresult_row[$i]);
         echo "</select></td>\n";
      } else if ($i == $testcaseindex) {
         echo '<th>Testcase</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $invocationindex) {
         echo '<th>Invocation</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" style=\"width:90%\" /></td>\n";
      } else if ($i == $passindex) {
         echo '<th>Pass</th>';
         echo "<td><input type=\"text\" name=\"pass\" value=\"" . $actionresult_row[$i] . "\" /></td>\n";
      } else if ($i == $startindex) {
         echo '<th>Start</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $endindex) {
         echo '<th>End</th>';
         echo "<td><input type=\"text\" value=\"" . $actionresult_row[$i] . "\" readonly=\"true\" /></td>\n";
      } else if ($i == $commentsindex) {
         echo '<th>Comments</th>';
         echo "<td><input type=\"text\" name=\"comments\" value=\"" . $actionresult_row[$i] . "\" style=\"width:90%\" /></td>\n";
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $actionresult_row[$i] . '" readonly="true" /></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton("div", "actionresult_modify", "Modify Action/Testcase Result", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="object"    value="actionresult" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   cellbutton(null, "actionresult_cancel", "Cancel", "index.php?object=actionresult&action=show&ID=$ID", null);

   echo '</form>' . "\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_actionresult_store
//
// Inputs:     $current_project
//             $current_phase
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a JSON store array object for the actionresult records in the specified project and phase.
//
//  <<<< cannot have a null project or phase here 
//---------------------------------------------------------------------------------------------------------------------

function query_actionresult_store($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "actionresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }
   
   $query = "SELECT ID, ScenarioresultID, Actiontype, Stepnumber, Status, Hostname, Testcase, Invocation, Pass, Start, End, Comments FROM " . $tablename;
   $result = mysql_query($query);
    
   if (! $result)
      die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   echo '{identifier: "ID",  label: "ID", items: [ ';

   if ($num_rows > 0) {

      for ($j = 0; $j < $num_rows; $j++) {

          if ($j != 0) {
             echo ',';
          }
     
          echo "{";

          $row = mysql_fetch_row($result);
           
          for ($i = 0; $i < $num_fields; $i++) { 
          
             if ($i != 0) {
                echo ', ';
             }
     
             echo '"' . mysql_field_name($result, $i) . '"' . ': ';
             echo '"' . $row[$i] . '"';
             
          }   
          echo '}';
      }

   }
   
   echo '] }';

   exit;  /* ???<<<<<<<< should we use exit here */
   
}


?>