<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  scenarioresult.php
//
//  This file is part of the DAF user interface PHP code.  This file deals with actions relating to scenarioresult objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                  Action to be carried out
//
//  object=scenarioresult  action=show                                display all scenarioresults
//  object=scenarioresult  action=show  ID=...                        display an individual scenarioresult
// 
//  object=scenarioresult  action=show    operation=create            insert a new scenarioresult in the DAF scenarioresult table
//                              name=... comments=....                with characteristics as specified by
//                              clusterID=.... type=....              the POST'd variables
//  object=scenarioresult  action=show    operation=modify  ID=...    modify an existing scenarioresult record in the 
//                              name=... comments=....                DAF scenarioresult table
//                              clusterID=.... type=....
//  object=scenarioresult  action=show    operation=delete  ID=...    delete an existing record in the DAF scenarioresult
//                                                                    table
//
//  object=scenarioresult  action=create                              present a form that allows the user to specify
//                                                                    the characteristics of a new scenarioresult that is to be
//                                                                    added to the DAF scenarioresult table
//  object=scenarioresult  action=modify  ID=...                      present a form that allows the user to modify
//                                                                    the characteristics of an existing scenarioresult in the
//                                                                    DAF scenarioresult table
//
//
//  object= scenarioresult  action=show  ID= name=
//  
//  object= scenarioresult  action=create 
//  object= scenarioresult  action=show    operation=create Name, Pass, Start, End, Teststand, Tester, Comments
//   
//  object= scenarioresult  action=modify  name=...
//  object= scenarioresult  action=show    operation=modify  ID=...  name=...
//
//  object= scenarioresult action=show    operation=delete  ID=...

//  object= scenarioresult action=show    operation=query  
 

//---------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   scenarioresult
//
// Inputs:     $gp                 array containing posted parameters (eg $gp['object'])
//             $current_project    the name of the project whose scenario results we are looking at
//             $phase              the phase of the project whose scenario results we are looking at
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a scenario result object.   The allowed actions are to create, modify or delete the 
// scenario result in the scenarioresult table.
//
//----------------------------------------------------------------------------------------------------------------------

function scenarioresult($gp, $current_project, $current_phase, $scenarioresult_startviewdate, $scenarioresult_endviewdate, 
                        $scenarioresult_startviewtime, $scenarioresult_endviewtime, $scenarioresult_Ndays) {
  
   global $db_server;

   if ($gp['object'] !== "scenarioresult") {
      add_error_message("DAFE0001: scenarioresult: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "scenarioresult";
   }
   
   if ($current_project == "none" || $current_phase == "none") {
   
     add_error_message("DAFE0055: Cannot show results for project $current_project phase $current_phase");
     return; 
   }
  
   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         $query = "INSERT INTO " . $tablename . " (Jobname, Pass, Start, End, Teststand, Tester, Loglocation, Comments) " .
                  "VALUES('" . $gp['jobname'] . "','" . 
                               $gp['pass'] . "','" .
                               $gp['start'] . "','" .
                               $gp['end'] . "','" .
                               $gp['teststand'] . "','" .                               
                               $gp['tester'] . "','" .
                               $gp['loglocation'] . "','" .
                               $gp['comments'] . "')";              
         do_mysql_query($query, $db_server, 1);
         
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
            
         if (! am_i_admin()) {
         
            add_error_message("DAFE0094: Only an admin user can modify a Scenario Result");
      
         } else {

            $query = "UPDATE " . $tablename . " SET Comments='"  . $gp['comments'] . "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }   
         $gp['action'] = 'show';      
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "delete") {
      
         if (! am_i_admin()) {
         
            add_error_message("DAFE0095: Only an admin user can delete a Scenario Result");
      
         } else {
         
            $actionresult_table_name = determine_status_table_name($status_db, "actionresult");
      
            if (count_rows($actionresult_table_name, "ScenarioresultID = '" . $gp['ID'] . "'") > 0) {
         
               add_error_message("DAFE0056: Cannot delete that because it still contains one or more action result records");
   
            } else {

               $query = "DELETE FROM " . $tablename . " where ID='" . $gp['ID'] . "'";    
               do_mysql_query($query, $db_server, 1);   
            }
            
         }
   
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
         
      } else if ($gp['operation'] === "query") {

         show_query_scenarioresults($gp);
         return;
         
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_scenarioresult();
    
   } else if ($gp['action'] === "modify") {

       modify_scenarioresult($gp['ID'], $status_db);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_scenarioresults1($status_db, $scenarioresult_startviewdate, $scenarioresult_endviewdate, 
                                  $scenarioresult_startviewtime, $scenarioresult_endviewtime, $scenarioresult_Ndays);
  
      } else {
 
         show_detailed_scenarioresult($gp['ID'], $status_db);
      }
   
   } else if ($gp['action'] === "query") {
   
       query_scenarioresult($current_project, $current_phase);
   
   } else if ($gp['action'] === "getstore") {
            
      query_scenarioresult_store($current_project, $current_phase);

   } else {
    
      report_bad_parms("scenarioresult");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_query_scenarioresults
//
// Inputs:     $gp         
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//  Displays a table of scenario results, one result per row.  
// 
//----------------------------------------------------------------------------------------------------------------------

function show_query_scenarioresults($gp) {

   $status_db = determine_status_database_name($gp['project'], $gp['phase']);
   $tablename = determine_status_table_name($status_db, "scenarioresult");

   $where = "";
   $fieldcount = 0;
   
   foreach($gp['value'] as $index => $value) {
   
      if ($value != "") {

         $field      = $gp['field'][$index];
         $comparator = $gp['comparator'][$index];
         if ($fieldcount == 0) {
            $where = "WHERE ";
            $and_or = "";
         } else {
             $and_or = " " . $gp['and_or'][$index];
         }
         
         if ($field == "Start") {
           $value .= " " . $gp['time1'];
         }
         if ($field == "End") {
           $value .= " " . $gp['time2'];
         }
         
         if ($comparator == "LIKE") {
            $where .= "$and_or $field LIKE '" . $value . "'";
         } else if ($comparator == "LIKEPC") {
            $where .= "$and_or $field LIKE '%" . $value . "%'";
         } else if ($comparator == "NOTLIKE") {
            $where .= "$and_or $field NOT LIKE '" . $value . "'";
         } else if ($comparator == "REGEXP") {
            $where .= "$and_or $field REGEXP '" . $value . "'";
         } else if ($comparator == "NOTREGEXP") {
            $where .= "$and_or $field NOT REGEXP '" . $value . "'";
         } else if ($comparator == "ISNULL") {
            $where .= "$and_or $field IS NULL";
         } else if ($comparator == "ISNOTNULL") {
            $where .= "$and_or $field IS NOT NULL";
         } else if ($comparator == "ISEMPTY") {
            $where .= "$and_or $field = ''";
         } else if ($comparator == "ISNOTEMPTY") {
            $where .= "$and_or $field != ''";
         } else if ($comparator == "=") {
            if (is_array($value)) {
               $where .= "$and_or $field IN ( '" . join("','", $value) . "' )";
            } else {
               $where .= "$and_or $field = '" . $value . "'";
            }
         } else if ($comparator == "<") {
            $where .= "$and_or $field < '" . $value . "'";
         } else if ($comparator == "<=") {
            $where .= "$and_or $field <= '" . $value . "'";
         } else if ($comparator == ">") {
            $where .= "$and_or $field > '" . $value . "'";
         } else if ($comparator == ">=") {
            $where .= "$and_or $field >= '" . $value . "'";
         } else if ($comparator == "!=") {
            if (is_array($value)) {
               $where .= "$and_or $field NOT IN ( '" . join("','", $value) . "' )";
            } else {
               $where .= "$and_or $field != '" . $value . "'";
            }
         } else {
            $where .= "$and_or $field $comparator $value";
         }
         $fieldcount++;
      }
   }
   
   $query = "SELECT * FROM " . $tablename . " $where";
   
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenario Results - ($query)</div></caption>\n";

   $fieldindex = array();
          
   echo '<tr>';
   for ($i = 0; $i < $num_fields; $i++) {
       $fieldname = mysql_field_name($result, $i);
       if ($fieldname == 'ID') {
          $IDindex = $i;
          echo '<th>ID</th>';
       } else if ($fieldname == 'Scenarioname') {
          echo '<th>Scenario Name</th>';
       } else  if ($fieldname == 'Jobname') {
          $jobnameindex = $i;
          echo '<th>Job Name</th>';
       } else if ($fieldname == 'Actionsinscenario') {
          echo '<th>Actions in Scenario</th>';
       } else if ($fieldname == 'Actionsattempted') {
          echo '<th>Actions attempted</th>';
       } else if ($fieldname == 'Actionscompleted') {
          echo '<th>Actions completed</th>';
       } else if ($fieldname == 'Actionspassed') {
          echo '<th>Actions passed</th>';
       } else if ($fieldname == 'Actionsfailed') {
          echo '<th>Actions failed</th>';
       } else if ($fieldname == 'Pass') {
          $passindex = $i;
          echo '<th>Pass</th>';
       } else if ($fieldname == 'Teststand') {
          $teststandindex = $i;
          echo '<th>Test Stand</th>';
       } else if ($fieldname == 'Testlevel') {
          $testlevelindex = $i;
          echo '<th>Test Level</th>';
       } else if ($fieldname == 'Loglocation') {
          echo '<th>Log location</th>';
          $loglocationindex = $i;
       } else if ($fieldname == 'Scenariologfilename') {
          $scenariologfilenameindex = $i;
          echo '<th>Scenario Log FileName</th>';
       } else if ($fieldname == 'TestlevelrecordID') {
          $testlevelrecordindex = $i;
       } else if ($fieldname == 'TeststandrecordID') {
          $teststandrecordindex = $i;
       } else {
          echo '<th>' . $fieldname . '</th>';
       }
       $fieldindex[$fieldname] = $i;
   }
   echo '</tr>' . "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       if ($row[$passindex] == 100) {
          echo '<tr>';
          $tdclass = "";
       } else {
          echo '<tr class="fail">';
          $tdclass = 'class="fail"';
       }
       $ID = $row[$IDindex];
                        
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $jobnameindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=scenarioresult&ID=" . $ID . '>' . $row[$i] . '</a></td>';
          } else if ($i == $teststandindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=teststandrecord&ID=" . $row[$teststandrecordindex] . '>' . $row[$i] . '</a></td>';
          } else if ($i == $teststandrecordindex ) {
             /* skip */
          } else if ($i == $testlevelindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=testlevelrecord&ID=" . $row[$testlevelrecordindex] . '>' . $row[$i] . '</a></td>';
          } else if ($i == $testlevelrecordindex ) {
             /* skip */
          } else if ($i == $loglocationindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=directory&dir=" . $row[$i] . '>' .  $row[$i] . '</a></td>';
          } else if ($i == $scenariologfilenameindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=path&path=" .  $row[$loglocationindex] . "/" . $row[$i] . '>' . $row[$i] . '</a></td>';
          } else                {           
             echo "<td $tdclass>"  . $row[$i] . '</td>';
          }
       }   
       echo '</tr>';
       echo "\n";

    }
    echo '</table>';
         
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_scenarioresults
//
// Inputs:     $status_db         The name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//  Displays a table of scenario results, one result per row.  

// The date range of the actionresults that are being displaying can be changed by the user submitting one of a number of
// forms which result in the following URLs to be posted
//
// status.php?object=scenarioresult?action=show&startviewdate=...&endviewdate=...&scenarioresultviewrequest 
// status.php?object=scenarioresult?action=show&scenarioresult_Ndays=N&scenarioresultviewNdaysrequest
// status.php?object=scenarioresult?action=show&scenarioresultviewallrequest 
//
// The routine may also post
//
// status.php?object=scenarioresult?action=show&object=scenarioresult&ID=...
// status.php?object=scenarioresult?action=show&object=teststandrecord&ID=...
// status.php?object=scenarioresult?action=show&object=testlevelrecord&ID=...
// status.php?object=scenarioresult?action=show&object=directory=...&dir=...
// status.php?object=scenarioresult?action=show&object=path=...&path=...
// 
//----------------------------------------------------------------------------------------------------------------------

function show_all_scenarioresults($status_db, $scenarioresult_startviewdate, $scenarioresult_endviewdate, 
                                  $scenarioresult_startviewtime, $scenarioresult_endviewtime, $scenarioresult_Ndays) {

   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   echo '<form style="float:left" action="index.php" method="post">' . "\n";
         
   echo '<div style="float:left">';
   echo 'Start view date: <input dojoType="dijit.form.DateTextBox" name=startviewdate value="' . $scenarioresult_startviewdate . '" />' . "\n";
   echo '</div>';
   
   echo '<div style="float:left">';
   echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=startviewtime value="' . $scenarioresult_startviewtime . '" />' . "\n";
   echo '</div>';
         
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp; End view date: <input dojoType="dijit.form.DateTextBox" name=endviewdate value="' . $scenarioresult_endviewdate . '" />' . "\n";
   echo '</div>'; 
   
   echo '<div style="float:left">';
   echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=endviewtime value="' . $scenarioresult_endviewtime . '" />' . "\n";
   echo '</div>'; 
         
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "scenarioresult_refreshview", "Refresh View", null, "submit");
   echo '<input type="hidden" name="scenarioresultviewrequest" value="scenarioresultviewrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioresult" />' . "\n";
   echo '</div>';
   echo '</form>' . "\n";

   echo '<form style="float:left" action="index.php" method="post">' . "\n";
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "scenarioresult_viewndays", "View N Days", null, "submit");
   echo '&nbsp;&nbsp;&nbsp; N days: <input type="text" name="scenarioresult_Ndays" size="5" value="' . $scenarioresult_Ndays . '" />' . "\n";
   echo '<input type="hidden" name="scenarioresultviewNdaysrequest" value="scenarioresultviewNdaysrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioresult" />' . "\n";
   echo '</div>';       
   echo '</form>' . "\n";       

   echo '<form action="index.php" method="post">' . "\n";
   echo '<div">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "scenarioresult_viewall", "View All", null, "submit");
   echo '<input type="hidden" name="scenarioresultviewallrequest" value="scenarioresultviewallrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioresult" />' . "\n";
   echo '</div>';       
   echo '</form>' . "\n";
         
   echo '<div style="clear:both" >';    
         
   $query = "SELECT * FROM " . $tablename . " WHERE Start >= '" . $scenarioresult_startviewdate . " " .  $scenarioresult_startviewtime . "' AND End <= '" . 
            $scenarioresult_endviewdate . " " .  $scenarioresult_endviewtime . "' ORDER BY scenarioresult.Start desc";
   
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenario Results</div></caption>";
   echo "\n";

   $fieldindex = array();
          
   echo '<tr>';
   echo '<th>show</th><th>modify</th><th>delete</th>';
   
   for ($i = 0; $i < $num_fields; $i++) {
       $fieldname = mysql_field_name($result, $i);
       if ($fieldname == 'ID') {
          $IDindex = $i;
          echo '<th>ID</th>';
       } else if ($fieldname == 'Scenarioname') {
          echo '<th>Scenario Name</th>';
       } else  if ($fieldname == 'Jobname') {
          $jobnameindex = $i;
          echo '<th>Job Name</th>';
       } else if ($fieldname == 'Actionsinscenario') {
          echo '<th>Actions in Scenario</th>';
       } else if ($fieldname == 'Actionsattempted') {
          echo '<th>Actions attempted</th>';
       } else if ($fieldname == 'Actionscompleted') {
          echo '<th>Actions completed</th>';
       } else if ($fieldname == 'Actionspassed') {
          echo '<th>Actions passed</th>';
       } else if ($fieldname == 'Actionsfailed') {
          echo '<th>Actions failed</th>';
       } else if ($fieldname == 'Pass') {
          $passindex = $i;
          echo '<th>Pass</th>';
       } else if ($fieldname == 'Teststand') {
          $teststandindex = $i;
          echo '<th>Test Stand</th>';
       } else if ($fieldname == 'Testlevel') {
          $testlevelindex = $i;
          echo '<th>Test Level</th>';
       } else if ($fieldname == 'Loglocation') {
          echo '<th>Log location</th>';
          $loglocationindex = $i;
       } else if ($fieldname == 'Scenariologfilename') {
          $scenariologfilenameindex = $i;
          echo '<th>Scenario Log FileName</th>';
       } else if ($fieldname == 'TestlevelrecordID') {
          $testlevelrecordindex = $i;
       } else if ($fieldname == 'TeststandrecordID') {
          $teststandrecordindex = $i;
       } else {
          echo '<th>' . $fieldname . '</th>';
       }
       $fieldindex[$fieldname] = $i;
   }
   echo '</tr>' . "\n";
              
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       if ($row[$passindex] == 100) {
          echo '<tr>';
          $tdclass = "";
       } else {
          echo '<tr class="fail">';
          $tdclass = 'class="fail"';
       }
       $ID = $row[$IDindex];
       /* $name = $row[$fieldindex['Name']]; */
       
       echo "<td $tdclass>\n";
       cellbutton(null, "scenarioresult_show_$j", "show", "index.php?action=show&object=scenarioresult&ID=$ID");
       echo "</td>\n";
       echo "<td $tdclass>\n";
       cellbutton(null, "scenarioresult_modify_$j", "modify", "index.php?action=modify&object=scenarioresult&ID=$ID");
       echo "</td>\n";
       echo "<td $tdclass>\n";
       // cellbutton(null, "scenarioresult_delete_$j", "delete", "index.php?action=show&operation=delete&object=scenarioresult&ID=$ID");
       button_with_confirm("scenarioresult_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=scenarioresult&ID=$ID", 
                           "index.php?action=show&object=scenarioresult");
       echo "</td>\n";

                        
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $jobnameindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=scenarioresult&ID=" . $ID . '>' .  $row[$i] . '</a></td>';
          } else if ($i == $teststandindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=teststandrecord&ID=" . $row[$teststandrecordindex] . '>' .  $row[$i] . '</a></td>';
          } else if ($i == $teststandrecordindex ) {
             /* skip */
          } else if ($i == $testlevelindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=testlevelrecord&ID=" . $row[$testlevelrecordindex] . '>' .  $row[$i] . '</a></td>';
          } else if ($i == $testlevelrecordindex ) {
             /* skip */
          } else if ($i == $loglocationindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=directory&dir=" . $row[$i] . '>' .  $row[$i] . '</a></td>';
          } else if ($i == $scenariologfilenameindex ) {
             echo "<td $tdclass><a href=index.php?action=show&object=path&path=" .  $row[$loglocationindex] . "/" . $row[$i] . '>' .  $row[$i] . '</a></td>';
          } else                {           
             echo "<td $tdclass>"  . $row[$i] . '</td>';
          }
       }   
       echo '</tr>';
       echo "\n";

    }
    echo '</table>';
   
    echo '</div>';       
             
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_scenarioresults1
//
// Inputs:     $status_db         The name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//  Displays a table of scenario results, one result per row.  

// The date range of the actionresults that are being displaying can be changed by the user submitting one of a number of
// forms which result in the following URLs to be posted
//
// status.php?object=scenarioresult?action=show&startviewdate=...&endviewdate=...&scenarioresultviewrequest 
// status.php?object=scenarioresult?action=show&scenarioresult_Ndays=N&scenarioresultviewNdaysrequest
// status.php?object=scenarioresult?action=show&scenarioresultviewallrequest 
//
// The routine may also post
//
// status.php?object=scenarioresult?action=show&object=scenarioresult&ID=...
// status.php?object=scenarioresult?action=show&object=teststandrecord&ID=...
// status.php?object=scenarioresult?action=show&object=testlevelrecord&ID=...
// status.php?object=scenarioresult?action=show&object=directory=...&dir=...
// status.php?object=scenarioresult?action=show&object=path=...&path=...
// 
//----------------------------------------------------------------------------------------------------------------------

function show_all_scenarioresults1($status_db, $scenarioresult_startviewdate, $scenarioresult_endviewdate, 
                                  $scenarioresult_startviewtime, $scenarioresult_endviewtime, $scenarioresult_Ndays) {

   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   echo '<form style="float:left" action="index.php" method="post">' . "\n";
         
   echo '<div style="float:left">';
   echo 'Start view date: <input dojoType="dijit.form.DateTextBox" name=startviewdate value="' . $scenarioresult_startviewdate . '" />' . "\n";
   echo '</div>';
   
   echo '<div style="float:left">';
   echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=startviewtime value="' . $scenarioresult_startviewtime . '" />' . "\n";
   echo '</div>';
         
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp; End view date: <input dojoType="dijit.form.DateTextBox" name=endviewdate value="' . $scenarioresult_endviewdate . '" />' . "\n";
   echo '</div>'; 
   
   echo '<div style="float:left">';
   echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=endviewtime value="' . $scenarioresult_endviewtime . '" />' . "\n";
   echo '</div>'; 
         
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "scenarioresult_refreshview", "Refresh View", null, "submit");
   echo '<input type="hidden" name="scenarioresultviewrequest" value="scenarioresultviewrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioresult" />' . "\n";
   echo '</div>';
   echo '</form>' . "\n";

   echo '<form style="float:left" action="index.php" method="post">' . "\n";
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "scenarioresult_viewndays", "View N Days", null, "submit");
   echo '&nbsp;&nbsp;&nbsp; N days: <input type="text" name="scenarioresult_Ndays" size="5" value="' . $scenarioresult_Ndays . '" />' . "\n";
   echo '<input type="hidden" name="scenarioresultviewNdaysrequest" value="scenarioresultviewNdaysrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioresult" />' . "\n";
   echo '</div>';       
   echo '</form>' . "\n";       

   echo '<form action="index.php" method="post">' . "\n";
   echo '<div">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "scenarioresult_viewall", "View All", null, "submit");
   echo '<input type="hidden" name="scenarioresultviewallrequest" value="scenarioresultviewallrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioresult" />' . "\n";
   echo '</div>';       
   echo '</form>' . "\n";
         
   echo '<div style="clear:both" >'; 
   echo '</div';
   
echo <<<_END

   <link rel="stylesheet" href="./dojo/dojox/grid/resources/Grid.css" media="screen">
   <link rel="stylesheet" href="./dojo/dojox/grid/resources/claroGrid.css" media="screen">

   <div id="ScenarioresultStatus" style="width:1024px; height:500px;"></div>
   <script type="text/javascript">
      dojo.require("dojo.parser");
      dojo.require("dojox.grid.DataGrid");
      dojo.require("dojo.data.ObjectStore");

      dojo.ready(function() {

         // objstore = dojo.data.ItemFileReadStore({url: "index.php?object=scenarioresult&action=getstore"});
         
         dojo.require("dojo.store.JsonRest");
_END;

         echo "         myStore = new dojo.store.JsonRest({target:\"rest/" . $status_db . "/scenarioresult/\"});";
  
echo <<<_END
         
         var grid = new dojox.grid.DataGrid({    // use the adapter to give the grid a Dojo Data compliant interface to the store    
		                    // store: dataStore = objstore,
                            store: dataStore = dojo.data.ObjectStore({objectStore: myStore}),
		                    escapeHTMLInData: false,          /* <<<<<<<< is this safe (cross site attacks) */
		                    columnReordering: true,
                            structure: [ {name:"ID", field:"ID", width:"3em", styles:"text-align:center;"},        
		                              /*   {name:"Project", field:"Project"},        
		                                 {name:"Phase", field:"Phase"}, */  
		                                 {name:"Scenario Name", field:"Scenarioname", width:"8em"},        
		                                 {name:"Job Name", field:"Jobname", width:"8em"},        
		                                 {name:"State", field:"State", width:"6em"},  
		                                 {name:"Actions in Scenario", field:"Actionsinscenario", width:"6em", styles:"text-align:center;"},  
		                                 {name:"Actions Attempted", field:"Actionsattempted", width:"6em", styles:"text-align:center;"},  
		                                 {name:"Actions Passed", field:"Actionspassed", width:"5em", styles:"text-align:center;"},  
		                                 {name:"Actions Failed", field:"Actionsfailed", width:"5em", styles:"text-align:center;"},  
		                                 {name:"Action Type", field:"Actiontype", width:"8em", styles:"text-align:center;"},  
		                                 {name:"Pass", field:"Pass", width:"6em", styles:"text-align:center;"},
		                                 {name:"Start", field:"Start", width:"6em", styles:"text-align:center;"},
		                                 {name:"End", field:"End", width:"6em", styles:"text-align:center;"},
		                                 {name:"Test Stand", field:"Teststand", width:"8em"},
		                                 {name:"Test Level", field:"Testlevel", width:"8em"},
		                                 {name:"Tester", field:"Tester", width:"8em"},
		                                 {name:"Log Location", field:"Loglocation", width:"8em"},
		                                 {name:"Scenario Log Filename", field:"Scenariologfilename", width:"8em"},
		                                 {name:"Comments", field:"Comments", width:"8em"}
		                               ]}, 
		                    "ScenarioresultStatus");
		                    

		 grid.startup();
         
      });
      
   </script>
_END;
             
}



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_scenarioresult
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//  Allows the user to manually enter a new row in the scenario result table (not sure how we select the
//  appropriate database table though for the project/phase)
//
//  It would be very unusual for the user to create their own scenario result - this should normally be done by
//  the DAF automation framework.   The routine may post
//
// status.php?object=scenarioresult?action=show&operation=create&name...&pass=...&start=...  and a lot more 
//
//---------------------------------------------------------------------------------------------------------------------
 
function create_scenarioresult() {

   echo '<div dojoType="dijit.form.Form" id="create_scenarioresult" jsId="create_scenarioresult" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a Scenario Result</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_SCENARIORESULT_NAME_LEN . '" maxlength="' . DAF_SCENARIORESULT_NAME_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_SCENARIORESULT_NAME_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_SCENARIORESULT_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Scenario Result Name" ' . 
        'invalidMessage="Invalid Scenario Result Name (must be 1 to ' . DAF_SCENARIORESULT_NAME_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>JobName</th>';
   echo '<td><input type="text" name="jobname" size="' . DAF_JOBNAME_LEN . '" maxlength="' . DAF_JOBNAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_JOBNAME_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_JOBNAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Scenario Result Name" ' . 
        'invalidMessage="Invalid Scenario Result Name (must be 1 to ' . DAF_JOBNAME_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Pass</th><td><input type=\"text\" name=\"pass\" size = \"" . DAF_SCENARIORESULT_PASS_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_PASS_LEN . "\" /></td></tr>\n";
   echo "<tr><th>Start</th><td><input type=\"text\" name=\"start\" size = \"" . DAF_SCENARIORESULT_START_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_START_LEN . "\" /></td></tr>\n";
   echo "<tr><th>End</th><td><input type=\"text\" name=\"end\" size = \"" . DAF_SCENARIORESULT_END_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_END_LEN . "\" /></td></tr>\n";
   echo "<tr><th>Teststand</th><td><input type=\"text\" name=\"teststand\" size = \"" . DAF_SCENARIORESULT_TESTSTAND_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_TESTSTAND_LEN . "\" /></td></tr>\n";
   echo "<tr><th>Tester</th><td><input type=\"text\" name=\"tester\" size = \"" . DAF_SCENARIORESULT_TESTER_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_TESTER_LEN . "\" /></td></tr>\n";
   echo "<tr><th>Loglocation</th><td><input type=\"text\" name=\"loglocation\" size = \"" . DAF_SCENARIORESULT_LOGLOCATION_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_LOGLOCATION_LEN . "\" /></td></tr>\n";
   echo "<tr><th>Comments</th><td><input type=\"text\" name=\"comments\" size = \"" . DAF_SCENARIORESULT_COMMENTS_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_COMMENTS_LEN . "\" /></td></tr>\n";
   echo "</table>\n";
   cellbutton(null, "scenarioresult_query", "Create Scenario Result", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"scenarioresult\" />\n";
   cellbutton(null, "scenarioresult_cancel", "Cancel", "index.php?object=scenarioresult&action=show", null);
   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//                                                            
// Function:   make_host_and_time_specific_pathname_prefix
//
// Inputs:     $pathdir      the initial part of the path,            eg  /tmp/daflogs
//             $hostname     the hostname,                            eg  suse11a
//             $identifier   an identifier, say the scenarioresultID  eg  123
//             $testcasename the relevant testcase name               eg  nsfst
//             $postfix      the last part of the made up pathname    eg  stdout
//
// Outputs:    none
//
// Returns:    a pathname, made up according to the description below, eg
//             /tmp/daflogs/suse11a_123_20110827.174740_nfst.stdout
//
// Description:
//                                                                                                       
//  Must be kept in step with the corresonding DAF routine in dafservice.c 
//
//  Creates a time stamped pathname, based on host, testcase and other parameters - this is intended
//  to be the path a to a file whose pathname is unique to this particular scenario or circumstance.                               
//                                                                                                        
//  The pathname that is formed is in the following format:                                               
//                                                                                                        
//  pathdir/hh..hh_YYYYMMDD_HHMMSS_tt.tt.xxxxx                                                          
//                                                                                                        
//  where hh..hh is the rightmost 16 chars of the host name running this test agent                       
//        ii..ii is the identifier in the parameters expressed as a numeric string                       
//        tt..tt is the rightmost 32 chars of the testcase                                                
//        YYYYYMMDD_HHMMSS is our standard timestamp  eg 20110826_14:30:0 for 2.30pm on 26 Aug 2011       
//        xxxxx is the postfix provided in the parameters, which should not exceed 8 chars                
//                                                                                                        
//  eg    /tmp/daflogs/suse11a_123_20110827.174740_nfst.stdout                                            
//                                                                                                        
//        any '/' or '\' present in the testcase are substituted with '_' 
//                                
//---------------------------------------------------------------------------------------------------------------------

function make_host_and_time_specific_pathname_prefix($pathdir, $hostname, $identifier, $testcasename, $postfix) {

   $tc = strtr($testcasename, "/", "_");
   $pathname = $pathdir . "/" . substr($hostname, -16) . "_" . $identifier . "_" . substr($tc, -16) . "." . $postfix;
                                                   
   return $pathname;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_scenarioresult
//
// Inputs:     $ID                the scenarioresult ID of interest
//             $status_db         the name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:    nothing
//
//  Displays a single scenario result, showing the detailed values for each scenarioresult field.  The routine might post
//
// status.php?object=scenarioresult?action=show&object=teststandrecord&ID=...
// status.php?object=scenarioresult?action=show&object=testlevelrecord&ID=...
// status.php?object=scenarioresult?action=show&object=directory=...&dir=...
// status.php?object=scenarioresult?action=show&object=path=...&path=...
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_scenarioresult($ID, $status_db) {

   $tablename = determine_status_table_name($status_db, "scenarioresult");
   $actionresult_table_name = determine_status_table_name($status_db, "actionresult");

   $query = "SELECT * FROM " . $tablename . " where ID = '" . $ID . "'";
   $scenarioresult = mysql_query($query);

   if (! $scenarioresult) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($scenarioresult);   // should only be one row
   $num_fields = mysql_num_fields($scenarioresult);

   echo '<table class="fullwidth">';
   echo "<caption class=\"cap1\"><div>Scenario result</div></caption>\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($scenarioresult, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $scenarioresult_row = mysql_fetch_row($scenarioresult);
   
   $scenarioresultID         = $scenarioresult_row[$fieldindex['ID']];
   $loglocationindex         = $fieldindex['Loglocation'];
   $scenariologfilenameindex = $fieldindex['Scenariologfilename']; 
   $teststandindex           = $fieldindex['Teststand'];
   $teststandrecordindex     = $fieldindex['TeststandrecordID'];
   $testlevelindex           = $fieldindex['Testlevel'];
   $testlevelrecordindex     = $fieldindex['TestlevelrecordID'];
   
   $teststandrecordID = $scenarioresult_row[$teststandrecordindex];
   $testlevelrecordID = $scenarioresult_row[$testlevelrecordindex]; 
    
   // var_dump($scenarioresult_row, $scenarioresultID, $fieldindex);

   for ($i = 0; $i < $num_fields; $i++) {
  
      if (($i != $teststandrecordindex) && ($i != $testlevelrecordindex)) {
         echo '<tr>';
         echo '<th class="fullwidth">' . mysql_field_name($scenarioresult, $i) . '</th>';
         if ($i == $loglocationindex) {
            echo '<td class=td_smd><a href=index.php?action=show&object=directory&dir=' . $scenarioresult_row[$i] . '>' .  $scenarioresult_row[$i] . '</a></td>';
         } else if ($i == $scenariologfilenameindex ) {
            echo '<td class=td_smd><a href=index.php?action=show&object=path&path=' .  $scenarioresult_row[$loglocationindex] . "/" . $scenarioresult_row[$i] . '>' .  $scenarioresult_row[$i] . '</a></td>';
         } else if ($i == $teststandindex ) {
            echo '<td class=td_smd><a href=index.php?action=show&object=teststandrecord&ID=' . $scenarioresult_row[$teststandrecordindex] . '>' .  $scenarioresult_row[$i] . '</a></td>';
         } else if ($i == $testlevelindex ) {
            echo '<td class=td_smd><a href=index.php?action=show&object=testlevelrecord&ID=' . $scenarioresult_row[$testlevelrecordindex] . '>' .  $scenarioresult_row[$i] . '</a></td>';
         } else {
            echo '<td>'  . $scenarioresult_row[$i] . '</td>';
         }
         echo '</tr>'. "\n";
      }
   }
   echo '</table>';
   
   /*
   subtable('actionresult.ID, ScenarioResultID, daf.actiontype, Stepnumber, Status, Hostname, Testcase, Invocation, actionresult.Pass, actionresult.Start, ' . 
            'actionresult.End, Loglocation, actionresult.Comments',
            array("ID", "Scenario Result ID", "Action Type", "Step Number", "Status", "Hostname", "Testcase", "Invocation", "Pass", "Start", "End", "Loglocation", "Comments"),
            'actionresult, scenarioresult',
            "actionresult.ScenarioResultID = $scenarioresultID AND actionresult.ScenarioResultID = scenarioresult.ID",
            "Action Results", "actionresult",
            "scenarioresult.Start");        */               
            
   $query = "SELECT actionresult.ID, ScenarioresultID, Actiontype, Stepnumber, Status, Hostname, Testcase, Invocation, actionresult.Pass, actionresult.Start, actionresult.End, " . 
            "Loglocation, actionresult.Comments FROM " . $actionresult_table_name . "," . $tablename . " WHERE actionresult.ScenarioResultID = $scenarioresultID AND actionresult.ScenarioResultID = scenarioresult.ID " .
            "ORDER BY scenarioresult.Start";
     
   $qresult = mysql_query($query);
   
   if (! $qresult) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   echo '<table class="tab1">';

   $num_rows   = mysql_num_rows($qresult);
   $num_fields = mysql_num_fields($qresult);   
   
   $fieldindex = array();
   $fieldnames = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldnames[$i] = mysql_field_name($qresult, $i);
      $fieldindex[$fieldnames[$i]] = $i;
   }
      
   $IDindex             = $fieldindex['ID'];
   $scenarioresultindex = $fieldindex['ScenarioresultID']; 
   $loglocationindex    = $fieldindex['Loglocation'];
      
   echo '<tr>';
   echo '<th colspan=5> Action results </th>';
   for ($j = 0; $j < $num_fields; $j++) {
      if (($j != $IDindex) && ($j != $scenarioresultindex) && ($j != $loglocationindex)) {
         echo '<th>' . $fieldnames[$j] . '</th>';
      }
   }  
   echo '</tr>';
   
   for ($i = 0; $i < $num_rows; $i++) {
      $row = mysql_fetch_row($qresult);
      $actionresultID = $row[$fieldindex['ID']];
      $testcasename   = $row[$fieldindex['Testcase']];
      $hostname       = $row[$fieldindex['Hostname']];
      $loglocation    = $row[$fieldindex['Loglocation']];
      $stdoutfile     = make_host_and_time_specific_pathname_prefix($loglocation, $hostname, $actionresultID, $testcasename, "stdout");
      $stderrfile     = make_host_and_time_specific_pathname_prefix($loglocation, $hostname, $actionresultID, $testcasename, "stderr");
      /* $joblogfile     = $loglocation . "/" . $hostname . "_console.log";  */    
      $joblogfile     = $loglocation . "/agent_log_" . $hostname . "_" . $actionresultID . ".log";     

      echo '<tr>'; 
      $ID = $row[$IDindex];
      echo "<td class=td_smd>";
      cellbutton(null, "actionresult_modify_$i", "modify", "index.php?action=modify&object=actionresult&ID=$ID", null);
      echo "</td>\n";
      echo "<td class=td_smd>";
      // cellbutton(null, "actionresult_delete_$i", "delete", "index.php?action=show&operation=delete&object=actionresult&ID=$ID", "submit");  
      button_with_confirm("actionresult_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=actionresult&ID=$ID", 
                          "index.php?action=show&object=actionresult");
      echo "</td>\n";
      echo "<td class=td_smd>";
      cellbutton(null, "scenarioresult_stdout_$i", "stdout", "index.php?action=show&object=path&path=$stdoutfile", "submit");
      echo "</td>\n";
      echo "<td class=td_smd>";
      cellbutton(null, "scenarioresult_stderr_$i", "stderr", "index.php?action=show&object=path&path=$stderrfile", "submit");
      echo "</td>\n";
      echo "<td class=td_smd>";
      cellbutton(null, "scenarioresult_jcl_$i", "job control log", "index.php?action=show&object=path&path=$joblogfile", "submit");
      echo "</td>\n";
      for ($j = 0; $j < $num_fields; $j++) {  
         if (($j != $IDindex) && ($j != $scenarioresultindex) && ($j != $loglocationindex)) {     
            echo '<td>'  . $row[$j] . '</td>';
         }       
      }
      echo '</tr>';
      echo "\n";
   }

   echo '</table>';
   
   if (isset($testlevelrecordID)) {
   
      $svccodelevelrecord_table_name = determine_status_table_name($status_db, "svccodelevelrecord");            /* <<<< OOOOOPPPSSSS <<<<<<<<<<< */
               
   } else {
   
      add_error_message("DAFE0057: There is no testlevel for this Scenario result");
   
   }    
            
   if (isset($teststandrecordID)) {
   
      $clusterrecord_table_name = determine_status_table_name($status_db, "clusterrecord"); 
               
   } else {
   
      add_error_message("DAFE0058: There is no teststand for this Scenario result");
   
   }            

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_scenarioresult
//
// Inputs:     $ID                the scenarioresult ID of interest
//             $status_db         the name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:
//
//  Presents a form to the user to allow the details of a particular scenarioresult to be modified by the user (note,
//  we only modify the scenarioresult record, we do not modify any actionresult records that are associated with this
//  particular scenario result).
//
//  // status.php?object=cluster?action=show&object=scenarioresult&ID=... and other fields
//
//----------------------------------------------------------------------------------------------------------------------

function modify_scenarioresult($ID, $status_db) {                  /* <<<<<<<<<<<< needs validation adding */

   $tablename = determine_status_table_name($status_db, "scenarioresult");

   $query = "SELECT * FROM " . $tablename . " where ID = '" . $ID . "'";
   $scenarioresult = mysql_query($query);

   if (! $scenarioresult) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($scenarioresult);   // should only be one row
   $num_fields = mysql_num_fields($scenarioresult);

   echo '<div dojoType="dijit.form.Form" id="create_scenarioresult" jsId="create_scenarioresult" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Scenario Results</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($scenarioresult, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $scenarioresult_row = mysql_fetch_row($scenarioresult);
   $commentsindex = $fieldindex['Comments'];
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($scenarioresult, $i);
      echo '<tr>';
      if ($i == $commentsindex) {
        echo '<th>Comments</th>';
        echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $scenarioresult_row[$i] . '" style="width:90%" /></td>';
      } else {
        echo '<th>' . $fieldname . '</th>';
        echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $scenarioresult_row[$i] . '" readonly="true" /></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton("div", "scenarioresult_query", "Modify Scenario Result", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="scenarioresult" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   cellbutton(null, "scenarioresult_cancel", "Cancel", "index.php?object=scenarioresult&action=show&ID=$ID", null);

   echo '</div>' . "\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   directory
//
// Inputs:     $gp          input parameters posted from a previous web page - only the following elements have
//                          any effect:
//                              $gp['object']    which must have the value 'directory'
//                              $gp['action']    which must have the value 'show'
//                              $gp['dir']       which contains the name of the directory that is to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Handles requests to do something with a name directory.   The allowed actions are to display
// scenario result in the scenarioresult table.
//
//----------------------------------------------------------------------------------------------------------------------

function directory($gp) {

  //  object= directory  action=show  dir=

  if ($gp['object'] !== "directory") {
     add_error_message("DAFE0001: directory: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "directory";
  }
  
  if ($gp['action'] === "show") {
 
      show_directory($gp['dir']);
    
  } else {
    
      report_bad_parms("directory");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_directory
//
// Inputs:     $dir                the name of the directory whose contents are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//  Displays the contents of a directory
//
//----------------------------------------------------------------------------------------------------------------------

function show_directory($dir) {

   $files = array();   

   if (is_dir($dir)) {
     if ($dh = opendir($dir)) {
        while (($file = readdir($dh)) !== false) {
           if (!is_dir($file)) {
              array_push($files, $file);
           }
        }
        closedir($dh);
     }
   } 
   
   sort($files);
   foreach ($files as $key => $val) {
      echo '<br><a href="?action=show&object=path&path=' . $dir . '/' . $val . '">' . $val . '</a>' . "\n";    /* need to html escape ???  <<<<<<< */
   }

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   path
//
// Inputs:     $gp          input parameters posted from a previous web page - only the following elements have
//                          any effect:
//                              $gp['object']    which must have the value 'path'
//                              $gp['action']    which must have the value 'show'
//                              $gp['path']      which contains the pathname of the file that is to be shown
//                              $gp['offset']
//
// Outputs:    none
//
// Returns:    nothing
//
// Handles requests to do something with a particular file directory.   The only currently allowed action is to display the 
// contents of the file. 
//
//---------------------------------------------------------------------------------------------------------------------

function path($gp) {  

  //  object= directory  action=show  path=
  
  if ($gp['object'] !== "path") {
     add_error_message("DAFE0001: path: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "path";
}
  
  if ($gp['action'] === "show") {
 
      show_path($gp['path'], isset($gp['offset']) ? $gp['offset'] : 0 );
    
  } else {
    
      report_bad_parms("path");
   
  }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_path
//
// Inputs:     $path       the pathname of the file that is to be displayed
//             $offset     the offset position to start in the file at
//             $maxlen     the number of bytes to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//  Displays the contents of a file
//
//----------------------------------------------------------------------------------------------------------------------

function show_path($path, $offset = NULL, $maxlen = NULL) {

   if (file_exists($path)) {                              /* paging forward is line aligned but paging backwards is not ! <<<<<<<<<<< */
   
      $stat = stat($path);
      $filelen = $stat['size'];
      $pagesize = 100000;
      
      if ($filelen == 0) {
      
         echo "<p><pre>\n\n";
         echo "File $path is empty\n";
         echo "</p><pre>\n\n";
      
      } else {
      
         if (! isset($previous_offset)) {
            $previous_offset = 0;
         }
      
         if (! isset($offset)) {
            $offset = 0;
         } else  if ($offset < 0) {
            $offset = 0;
         }
         if (! isset($maxlen)) {
            $maxlen = $pagesize;
         }
         if ( ($offset + $maxlen - 1) > $filelen) {
            $maxlen = $filelen - $offset + 1;
         }
      
         if ( ($section = file_get_contents($path, NULL, NULL, $offset, $maxlen))) {
         
            $back_to_cr = $pagesize - 1;
            for ($i=(((strlen($section)-1000) < 0) ? 0 : (strlen($section)-1000)) ; $i<strlen($section); $i++) {
               if ($section[$i] == "\n") {
                  $back_to_cr = $i;
                  break;
               }
            }
            $next_offset = $offset + $back_to_cr + 1;
            $previous_offset = $offset - $pagesize;
            if ($previous_offset < 0) {
               $previous_offset = 0;
            }

            echo "<div>";        
         
            echo '<div style="float:left">';
            echo '<form action="index.php" method="post">' . "\n";
            cellbutton("div", "show_path2", "Previous Page", null, "submit");
            echo '<input type="hidden" name="action"    value="show" />' . "\n";
            echo '<input type="hidden" name="path"      value="' . $path . '"modify" />' . "\n";
            echo '<input type="hidden" name="object"    value="path" />' . "\n";
            echo '<input type="hidden" name="offset"    value="' . $previous_offset . '" />' . "\n";
            echo '</form>' . "\n";
            echo "</div>\n";
            echo "</div>\n";

            echo '<div style="float:left">';
            echo '<form action="index.php" method="post">' . "\n";
            cellbutton("div", "show_path1", "Next Page", null, "submit");
            echo '<input type="hidden" name="action"    value="show" />' . "\n";
            echo '<input type="hidden" name="path"      value="' . $path . '"modify" />' . "\n";
            echo '<input type="hidden" name="object"    value="path" />' . "\n";
            echo '<input type="hidden" name="offset"    value="' . $next_offset . '" />' . "\n";
            echo '</form>' . "\n";
            echo "</div>\n";
            
            echo '<div style="float:none">';
            if ($stat['size'] > ($offset+$maxlen)) {      
               echo "Showing $maxlen chars at offset $offset from $path, actual file size is " . $stat['size'] . " bytes\n";
            }
            echo "</div>";
 
            echo '<div style="float:none">';        
            echo "<p><pre>\n";
            echo $section;
            echo "\n</pre>\n";  
            echo "</div>";        
         
         } else {
      
             add_error_message("DAFE0059: Could not fetch data from $path at offset $offset for length $maxlen");
         }
      
      }
      
   } else {
   
      add_error_message("DAFE0060: File $path does not exist");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_scenarioresult
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
// 
//
// status.php?object=scenarioresult?action=show&operation=query&name...&pass=...&start=...  and a lot more 
//
//---------------------------------------------------------------------------------------------------------------------
 
function query_scenarioresult($current_project, $current_phase) {

   $i = 0;
   echo "<script>dojo.require(\"dijit.form.DateTextBox\")</script>\n";
   
   echo '<div dojoType="dijit.form.Form" id="create_scenarioresult" jsId="create_scenarioresult" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Query Scenario Result</div></caption>\n";
   
   echo "<tr>";
   cellbutton("th", "scenarioresult_query", "Query Scenario Result", NULL, "submit");
   echo "<th>Field</th><th>Operator</th><th>Value</th></tr>\n";

   echo "<tr><td></td><td>Project</td>";
   echo "<td><input type=\"text\" value=\"=\" readonly=\"true\" /></td>\n";
   // echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<td><select name=\"project\"/>\n";
   selectorname("daf.project", "Name", $current_project);
   echo "</select></td>\n";
   
   echo "<tr><td>";
   echo "</td><td>Phase</td>";
   echo "<td><input type=\"text\" value=\"=\" readonly=\"true\" /></td>\n";
   echo "<td><select name=\"phase\"/>\n";
   selectorname("daf.phase", "Name", $current_phase);
   echo "</select></td>\n";

   echo "<tr><td>";
   echo "</td><td>Scenario Name</td>";
   echo "<td>";
   selectorOPvarchar($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Scenarioname\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Job Name</td>";
   echo "<td>";
   selectorOPvarchar($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Jobname\"/></td></tr>\n";
   
   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>State</td>";
   echo "<td>";
   selectorOPenum($i, "=");
   echo "</td>";
   // echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<td>";
   selectorENUMScenariostate($i);
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"State\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Actions in Scenario</td>";
   echo "<td>";
   selectorOPnum($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Actionsinscenario\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Actions attempted</td>";
   echo "<td>";
   selectorOPnum($i, "=");                                         /* <<<<<<< wrong field width above and below */
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Actionsattempted\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Actions completed</td>";
   echo "<td>";
   selectorOPnum($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Actionscompleted\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Actions passed</td>";
   echo "<td>";
   selectorOPnum($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Actionspassed\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Actions failed</td>";
   echo "<td>";
   selectorOPnum($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Actionsfailed\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Pass</td>"; 
   echo "<td>";
   selectorOPnum($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_PASS_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_PASS_LEN . "\" /></td>";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Pass\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Start</td>";
   echo "<td>";
   selectorOPnum($i, ">=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" id=\"date1\" dojoType=\"dijit.form.DateTextBox\" />\n"; /*<<<< use todays date */
   echo '&nbsp; <input dojoType="dijit.form.TimeTextBox" name="time1" />' . "\n";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Start\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>End</td>";
   echo "<td>";
   selectorOPnum($i, "<=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" id=\"date2\" dojoType=\"dijit.form.DateTextBox\" />\n"; /*<<<< use todays date */
   echo '&nbsp; <input dojoType="dijit.form.TimeTextBox" name="time2" />' . "\n";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"End\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Teststand</td>";
   echo "<td>";
   selectorOPvarchar($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_TESTSTAND_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_TESTSTAND_LEN . "\" /></td>";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Teststand\"/></td></tr>\n";

   $i++;
   echo "<tr><td>";
   selectorANDOR($i);
   echo "</td><td>Tester</td>";
   echo "<td>";
   selectorOPvarchar($i, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[$i]\" size = \"" . DAF_SCENARIORESULT_TESTER_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_TESTER_LEN . "\" /></td>";
   echo "<input type=\"hidden\" name=\"field[$i]\" value=\"Tester\"/></td></tr>\n";
   
   echo "</table>\n";

   echo "<input type=\"hidden\" name=\"object\" value=\"scenarioresult\"/>\n";
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"query\"/>\n";
   
   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   revise_scenarioresult
//
// Inputs:     $ID                the scenarioresult ID of interest
//             $status_db         the name of the status database for the project/phase being considered
//
// Outputs:    none
//
// Returns:    nothing
//
//  Recalculates the 
//
//     Actions in Scenario	Actions attempted	Actions completed	Actions passed	Actions failed	Pass
//
//  based on the current actionresults that belong to this scenarioresult	
//
//----------------------------------------------------------------------------------------------------------------------

function revise_scenarioresult($ID, $status_db) {

   global $db_server;

   $scenarioresult_tablename = determine_status_table_name($status_db, "scenarioresult");
   $actionsinscenario = get_field_from_row($scenarioresult_tablename, "Actionsinscenario", "ID='$ID'");
   
   $actionresult_tablename = determine_status_table_name($status_db, "actionresult");

   $query = "SELECT Pass, Status FROM " . $actionresult_tablename . " where scenarioresultID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);  
   $num_fields = mysql_num_fields($result);
   
   $actionsattempted  = 0;
   $actionscompleted  = 0;
   $actionspassed     = 0;
   $actionsfailed     = 0;

   for ($i=0; $i<$num_rows; $i++) {

      $row = mysql_fetch_row($result);
      $pass   = $row[0];
      $actionstatus = $row[1];
   
      $actionsattempted++;
      if ($actionstatus == 'Completed') {
         $actionscompleted++;
      }
      if ($pass == 100) {
         $actionspassed++;
      } else {
         $actionsfailed++;
      }
      
   }
   
   if (isset($actionsinscenario)) {
      if ($actionsinscenario != 0) {
         $scenariopass = sprintf("%d", (100 * $actionspassed/$actionsinscenario));
      } else {
         $scenariopass = 0;
      }
   } else {
      $scenariopass = 0;
   }
   
   $actionsattempted  = 0;
   $actionscompleted  = 0;
   $actionspassed     = 0;
   $actionsfailed     = 0;

   $query = "UPDATE " . $scenarioresult_tablename . " SET  Pass='" . $scenariopass . "', " .
                                                          "Actionsattempted='" . $actionsattempted . "', " .
                                                          "Actionscompleted='" . $actionscompleted . "', " .
                                                          "Actionspassed='"    . $actionspassed . "', " .
                                                          "Actionsfailed='"    . $actionsfailed . "' " .
            " WHERE ID='" . $ID . "'";              
   do_mysql_query($query, $db_server, 1);

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_scenarioresult_store
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
// Displays a JSON store array object for the scenarioresult records in the specified project and phase.  
//
//  <<<< cannot have a null project or phase here 
//---------------------------------------------------------------------------------------------------------------------

function query_scenarioresult_store($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }
   
   /* $fieldnames = array("ID", "Project", "Phase", "Scenario Name", "Job Name", "State", "Actions in Scenario", "Actions Attempted", "Actions Passed", "Actions Failed", 
                       "Pass", "Start", "End", "Test Stand", "Test Level", "Tester", "Log Location", "Scenario Log Filename", "Comments"); */
   $query = "SELECT ID, Project, Phase, Scenarioname, Jobname, State, Actionsinscenario, Actionsattempted, Actionspassed, Actionsfailed, Pass," . 
                      " Start, End, Teststand, Testlevel, Tester, Loglocation, Scenariologfilename, Comments FROM " . $tablename . " ORDER by Start";
   $result = mysql_query($query);
    
   if (! $result)
      die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   echo '{identifier: "ID",  label: "Jobname", items: [ ';

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