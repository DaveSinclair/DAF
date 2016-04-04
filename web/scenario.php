<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  scenario.php
//
//  This file is part of the DAF user interface PHP code.  
//
// This file deals with actions relating to scenario, workrequest, workqueue and environment objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                  Action to be carried out
//
//  object=scenario  action=show                                      display all scenarios
//  object=scenario  action=show  ID=                                 display an individual scenario
// 
//  object=scenario  action=show    operation=create                  insert a new scenario in the DAF scenario table
//                              name=... comments=....                with characteristics as specified by
//                              clusterID=.... type=....              the POST'd variables
//  object=scenario  action=show    operation=modify  ID=...          modify an existing scenario record in the 
//                              name=... comments=....                DAF scenario table
//                              clusterID=.... type=....
//  object=scenario  action=show    operation=delete  ID=...          delete an existing record in the DAF scenario
//                                                                    table
//
//  object=scenario  action=create                                    present a form that allows the user to specify
//                                                                    the characteristics of a new scenario that is to be
//                                                                    added to the DAF scenario table
//  object=scenario  action=modify  ID=...                            present a form that allows the user to modify
//                                                                    the characteristics of an existing scenario in the
//                                                                    DAF scenario table
//

  //  object=scenario  action=show 
  //  object=scenario  action=show  ID=...
  
  //  object=scenario  action=show    operation=create   name=... author=.... createdon=...
  //  object=scenario  action=show    operation=modify   name=... author=....  ID=...
  //  object=scenario  action=show    operation=delete   ID=...

  //  object=scenario  action=show ID=N operation=insertstep where=N after|before  description=...
  //  object=scenario  action=show ID=N operation=createstep stepnumber=N description=...
  //  object=scenario  action=show ID=N operation=modifystep stepnumber=N description
  //  object=scenario  action=show ID=N operation=deletestep stepnumber=N stepID=N
  
  //  object=scenario  action=show operation=createaction ID=... stepnumber, description, actiontypeID, testcaseID, paramaters...
  //  object=scenario  action=show operation=copyaction   ID=... stepnumber, description, actiontypeID, testcaseID, paramaters...
  //  object=scenario  action=show operation=modifyaction ID=... stepnumber, description, actiontypeID, testcaseID, paramaters...
  //  object=scenario  action=show operation=deleteaction ID=... 
  
  
  //  object=scenario  action=launch jobname=.... ID=... 

  //  object=scenario  action=run ID=... 
  
  //  object=scenario  action=copyscenario ID=...  

  //  object=scenario  action=create 
  //  object=scenario  action=modify ID=...
  //  object=scenario  action=delete ID=...
  
  //  object=workrequest  action=show
  //  object=workrequest  action=cancel ID=...  scenarioresultID=...\
  
  //  object=workqueue  action=show
  //  object=workqueue  action=getjson 
  //  object=workqueue  action=cancel ID=... 
  
  
  //  object=environment  action=show 
  //  object=environment  action=show  ID=...
  
  //  object=environment  action=show    operation=create   name=... description=.... 
  //  object=environment  action=show    operation=modify   name=... description=....  ID=...
  //  object=environment  action=show    operation=delete   ID=...
  
  //  object=environment  action=createenvironmentvalue  environmentID=.... 
  //  object=environment  action=showscenarios  environmentID=.... environmentname=...


//---------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   scenario
//
// Inputs:     $gp            array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a scenario object.   The allowed actions are to create, modify or delete the 
// scenario record in the node table.
//
//----------------------------------------------------------------------------------------------------------------------

function scenario($gp, $current_project, $current_phase, $current_user_ID) {

  global $db_server;

  if ($gp['object'] !== "scenario") {
     add_error_message("DAFE0001: scenario: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "scenario";
   }
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
      /* <<<< need a check to see if a scenario of this name already exists - need this check for other object types too ! */
      
         if (count_rows("daf.scenario", "Name = '" . $gp['name'] . "'") > 0) {
          
             add_error_message("DAFE0039: Cannot create scenario " . $gp['name'] . " because a scenario of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.scenario (Name, Author, Createdon, Lastchanged, OutcomeactionID) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['author'] . "', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '" . $gp['outcomeactionID'] . "')";              
            do_mysql_query($query, $db_server, 1);
            
            add_info_message($query);
            
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         if ((count_rows("daf.scenario", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0040: Cannot modify scenario " . $gp['name'] . " because a scenario of that name already exists");
            
         } else {
             
            $query = "UPDATE daf.scenario SET Name='" . $gp['name'] . "', Author='" . $gp['author'] . "', OutcomeactionID='" . $gp['outcomeactionID'] . 
                                   "', Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';      
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.scenario where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         
         $query = "DELETE FROM daf.action where ScenarioID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);

         $gp['action'] = 'show';
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "createstep") {
   
         $query = "INSERT INTO daf.step (Stepnumber, Description, ScenarioID) " .
                  "VALUES('" . $gp['stepnumber'] . "','" . $gp['description'] . "','" . 
                  $gp['ID'] . "')";              
         if (do_mysql_query($query, $db_server, 1) != 0) {
            $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1);
         }

         $gp['action'] = 'show';
      
      } else if ($gp['operation'] === "insertstep") {
   
         if (! isset($gp['where'])) {
            add_error_message("DAFE0041: insertstep specified without specifying where");
         } else {
         
            if ($gp['where'] == 'before') {
               $newstepno = $gp['stepnumber'] + $gp['numsteps'];
            } else if ($gp['where'] == 'after') {
               $newstepno = $gp['stepnumber'] + 1 + $gp['numsteps'];
            }
            $query = "UPDATE daf.step SET Stepnumber = Stepnumber+" . $gp['numsteps'] . " WHERE ((ScenarioID = " . $gp['ID'] . ") AND (Stepnumber >= $newstepno))";
            
            if (do_mysql_query($query, $db_server, 1)) {
               for ($i=0; $i<$gp['numsteps']; $i++) {
                  if ($gp['where'] == 'before') {
                     $newstepno = $gp['stepnumber'] + $i;
                  } else if ($gp['where'] == 'after') {
                     $newstepno = $gp['stepnumber'] + 1 + $i;
                  } else {
                     /* problem <<<< */
                  }
                  $query = "INSERT INTO daf.step (Stepnumber, Description, ScenarioID) " .
                           "VALUES('" . $newstepno . "','" . $gp['description'] . "','" . 
                           $gp['ID'] . "')";              
                  do_mysql_query($query, $db_server, 1);
               }   
               $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
               do_mysql_query($query, $db_server, 1);
            }
         } 
         
         $gp['action'] = 'show';
         
      } else if ($gp['operation'] === "copystep") {
   
         if (! isset($gp['where'])) {
            add_error_message("DAFE0042: copystep specified without specifying where");
         } else {
 
            $deststepno = $gp['deststepnumber'];
            if ($gp['where'] == 'after') {
               $deststepno = $deststepno + 1;
            }
           
            $gap = $gp['numsteps'] * $gp['numdups'];
            $query = "UPDATE daf.step SET Stepnumber = Stepnumber+" . $gap . " WHERE ((ScenarioID = " . $gp['ID'] . ") AND (Stepnumber >= " . $deststepno . "))";
            do_mysql_query($query, $db_server, 1);
            $query = "UPDATE daf.action SET Stepnumber = Stepnumber+" . $gap . " WHERE ((ScenarioID = " . $gp['ID'] . ") AND (Stepnumber >= " . $deststepno . "))";
            do_mysql_query($query, $db_server, 1);

            if ($deststepno > $gp['stepnumber']) {
               $srcstepno = $gp['stepnumber'];
            } else {
               $srcstepno = $gp['stepnumber']+$gap;                 
            }
            
            for ($j=0; $j<$gp['numdups']; $j++) {
                  
               for ($i=0; $i<$gp['numsteps']; $i++) {
                  $query = "INSERT INTO daf.step (Stepnumber, Description, ScenarioID) " .
                           "VALUES('" . $deststepno . "', '" . $gp['description'] . "','" . $gp['ID'] . "')";  
                  do_mysql_query($query, $db_server, 1);   
                     
                  $query = "INSERT INTO daf.action (ScenarioID, Stepnumber, Description, ActiontypeID, Hostselectortype, Hostselectorvalue, CollectorsetID, EnvironmentID, TestcaseID, Parameters, Duration, Maxduration) " .
                           "SELECT '" . $gp['ID'] . "', '" . $deststepno . "', Description, ActiontypeID, Hostselectortype, Hostselectorvalue, CollectorsetID, EnvironmentID, TestcaseID, Parameters, Duration, Maxduration " . 
                           "FROM daf.action WHERE ScenarioID='" . $gp['ID'] . "' AND Stepnumber='" . ($srcstepno+$i) . "'";
                  do_mysql_query($query, $db_server, 1);

                  $deststepno++;
               }            
            } 
            $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1);
         }
         
         $gp['action'] = 'show';
   
      } else if ($gp['operation'] === "modifystep") {
    
         $query = "UPDATE daf.step SET Stepnumber='" . $gp['stepnumber'] . "', Description='" . $gp['description'] . 
                  "' WHERE ID='" . $gp['stepID'] . "'";              
         if (do_mysql_query($query, $db_server, 1) != 0) {
            $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1);
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "deletestep") {
   
         $query = "DELETE FROM daf.action where ScenarioID = " . $gp['ID'] . " AND Stepnumber = " . $gp['stepnumber'];   
         if (do_mysql_query($query, $db_server, 1)) {
   
            $query = "DELETE FROM daf.step where ID='" . $gp['stepID'] . "'";    
            if (do_mysql_query($query, $db_server, 1)) {
               $query = "UPDATE daf.step SET Stepnumber = Stepnumber-1 WHERE ((ScenarioID = " . $gp['ID'] . ") AND (Stepnumber > " . $gp['stepnumber'] . "))";
               do_mysql_query($query, $db_server, 1);  /* <<<<< rollback if this fails ???? */
               $query = "UPDATE daf.action SET Stepnumber = Stepnumber-1 WHERE ((ScenarioID = " . $gp['ID'] . ") AND (Stepnumber > " . $gp['stepnumber'] . "))";
               do_mysql_query($query, $db_server, 1);
               $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
               do_mysql_query($query, $db_server, 1);
            }
         } 

         $gp['action'] = 'show';
      
      } else if ($gp['operation'] === "createaction") {
   
         $query = "INSERT INTO daf.action (ScenarioID, Stepnumber, Description, ActiontypeID, Hostselectortype, Hostselectorvalue, CollectorsetID, EnvironmentID, TestcaseID, Parameters, Duration, Maxduration) " .
                  "VALUES('" . $gp['ID'] . "','" . $gp['stepnumber'] . "','" . $gp['description'] . "','" . $gp['actiontypeID'] . "','" .
                  $gp['hostselectortype'] . "','" . $gp['hostselectorvalue'] . "','" . $gp['collectorsetID'] . "','" . $gp['environmentID'] . "','" .
                  $gp['testcaseID'] . "','" . $gp['parameters'] . "','" . $gp['duration'] . "','" . $gp['maxduration'] . "')";              
         if (do_mysql_query($query, $db_server, 1) != 0) {
            $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1);
         }

         $gp['action'] = 'show';
      
      } else if ($gp['operation'] === "copyaction") {
  
         if (count_rows("daf.step", "ScenarioID = '" . $gp['ID'] . "' AND Stepnumber = '" . $gp['stepnumber'] . "'") == 0) {
         
            add_error_message("DAFE0043: Cannot add action to step " . $gp['stepnumber'] . " since that step number does not exist in this scenario");
           
         } else {
   
            $changed = 0;
            for ($i=0; $i < $gp['numdups']; $i++) {
               $query = "INSERT INTO daf.action (ScenarioID, Stepnumber, Description, ActiontypeID, Hostselectortype, Hostselectorvalue, CollectorsetID, EnvironmentID, TestcaseID, Parameters, Duration, Maxduration) " .
                        "VALUES('" . $gp['ID'] . "','" . $gp['stepnumber'] . "','" . $gp['description'] . "','" . $gp['actiontypeID'] . "','" .
                        $gp['hostselectortype'] . "','" . $gp['hostselectorvalue'] . "','" . $gp['collectorsetID'] . "','" . $gp['environmentID'] . "','" .
                        $gp['testcaseID'] . "','" . $gp['parameters'] . "','" . $gp['duration'] . "','" . $gp['maxduration'] . "')";              
               if (do_mysql_query($query, $db_server, 1)) {
                  $changed = 1;
               }
            }
            if ($changed) {
               $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
               do_mysql_query($query, $db_server, 1);
            }
         }
        
         $gp['action'] = 'show';
      
      } else if ($gp['operation'] === "modifyaction") {
   
         $query = "UPDATE daf.action SET Description='" . $gp['description'] . "', ActiontypeID = '" . $gp['actiontypeID'] . "'," .
                                  "Hostselectortype = '" . $gp['hostselectortype'] . "', Hostselectorvalue = '" . $gp['hostselectorvalue'] . "'," . 
                                  "CollectorsetID = '" . $gp['collectorsetID'] . "', EnvironmentID = '" . $gp['environmentID'] . "', TestcaseID = '" . $gp['testcaseID'] . "', Parameters = '" . $gp['parameters'] . "', " .
                                  "Duration = '" . $gp['duration'] . "', Maxduration = '" . $gp['maxduration'] . "' " . 
                                  "WHERE ID='" . $gp['actionID'] . "'";              
         if (do_mysql_query($query, $db_server, 1) != 0) {
            $query = "UPDATE daf.scenario SET Lastchanged=CURRENT_TIMESTAMP WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1);
         }
         $gp['action'] = 'show';
         
      } else if ($gp['operation'] === "deleteaction") {
   
         $query = "DELETE FROM daf.action where ID='" . $gp['actionID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';

      }
   
   }

   if ($gp['action'] === "create") {
   
       create_scenario();
    
   } else if ($gp['action'] === "modify") {

       modify_scenario($gp['ID']);
        
   } else if ($gp['action'] === "createstep") {
   
       create_step($gp['ID']);
    
   } else if ($gp['action'] === "modifystep") {

       modify_step($gp['ID'], $gp['stepID']);  
       
   } else if ($gp['action'] === "copystep") {

       copy_step($gp['ID'], $gp['stepnumber']);  
       
   } else if ($gp['action'] === "createaction") {

       create_action($gp['ID'], $gp['stepnumber']); 
       
   } else if ($gp['action'] === "copyaction") {

       copy_action($gp['ID'], $gp['actionID'], $gp['stepnumber']);  
       
   } else if ($gp['action'] === "modifyaction") {

       modify_action($gp['ID'], $gp['actionID']);  
   
   } else if ($gp['action'] === "copyscenario") {
    
       // copy the scenario 
       
       $new_scenario_name = "Copy_of_" . $gp['scenarioname'];
              
       if (count_rows("daf.scenario", "Name = '" . $new_scenario_name . "'") > 0) {
          
             add_error_message("DAFE0044: Cannot create scenario " . $new_scenario_name . " because a scenario of that name already exists");
             
       } else {
       
          $query = "INSERT into daf.scenario (Name, Author, Createdon, Lastchanged) SELECT '" . $new_scenario_name . "', Author, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP FROM daf.scenario WHERE ID = '" . $gp['ID'] . "'";
          if (($newscenarioID = do_mysql_query_id($query, $db_server, 1))) {
          
             // $newscenarioID = mysql_insert_id();     
       
             // copy the steps
       
             $query = "INSERT into daf.step (ScenarioID, Stepnumber, Description) SELECT '" . $newscenarioID . "', Stepnumber, Description FROM daf.step WHERE ScenarioID = '" . $gp['ID'] . "'";
             if (do_mysql_query($query, $db_server, 1)) {
        
                // copy the actions 
       
                $query = "INSERT into daf.action (ScenarioID, ActiontypeID, Description, Stepnumber, Hostselectortype, Hostselectorvalue, CollectorsetID, TestcaseID, " . 
                                          "Parameters, EnvironmentID, Duration, Maxduration) " . 
                                          "SELECT '" . $newscenarioID . "',  ActiontypeID, Description, Stepnumber, Hostselectortype, Hostselectorvalue, CollectorsetID, TestcaseID, " . 
                                          "Parameters, EnvironmentID, Duration, Maxduration FROM daf.action WHERE ScenarioID = '" . $gp['ID'] . "'";
                do_mysql_query($query, $db_server, 1);
                $gp['ID'] = $newscenarioID;

             } else {
                /* <<<< what about rollback if the insert failed ???? */
             }
          } else {
              /* <<<< what about rollback if the insert failed ???? */
          } 
       
       }
       show_detailed_scenario($gp['ID']);
          
   } else if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_scenarios();
           
      } else {
         /* show_table_element("scenario", $gp['name'], "Scenarios"); */
         show_detailed_scenario($gp['ID']);
      }
     
   } else if ($gp['action'] === "launch") {
   
      if (! is_licence_valid_and_current()) {
      
          add_error_message("DAFE0045: Cannot start a new scenario as there is no current licence for this DAF server");
          show_scenarios();
          
      } else {
   
         launch_scenario($gp['ID'], $gp['scenarioname'], $current_project, $current_phase, $current_user_ID);  
       
      } 
   
   } else if ($gp['action'] === "run") {
   
      run_scenario($gp, $current_project, $current_phase);
    
   } else {
    
      report_bad_parms("scenario");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_scenarios
//
// Inputs:     none   
//
// Outputs:    Post's a list of variables that describe the characteristics of the node that is to be created 
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new node that is to be added to the DAF node
// table.   The routine posts
//
// status.php?object=node?action=show&operation=create&name=...&comments=&clusterID=....&type=....  
//
//----------------------------------------------------------------------------------------------------------------------

function show_scenarios() {

   $query = "SELECT * FROM scenario";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenarios</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=4>";
   cellbutton(null, "scenario_create", "Create Scenario", "index.php?action=create&object=scenario");
   echo "</th>\n";
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<th>' . $fieldname . '</th>';
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $outcomeactionIDindex = $fieldindex['OutcomeactionID'];
   $outcomeactionlookup  = lookuptable("daf.outcomeaction", "ID", "Name");
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       $scenarioname = $row[$fieldindex['Name']];
       echo "<td class=td_smd>";
       cellbutton(null, "scenario_run_$j",    "run",    "index.php?action=launch&object=scenario&ID=$ID&scenarioname=$scenarioname");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "scenario_show_$j",   "show",   "index.php?action=show&object=scenario&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "scenario_modify_$j", "modify", "index.php?action=modify&object=scenario&ID=$ID");
       echo "</td>";
       echo "<td class=td_smd>";
       // cellbutton('td', "scenario_delete_$j", "delete", "index.php?action=show&operation=delete&object=scenario&ID=$ID");
       button_with_confirm("scenario_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=scenario&ID=$ID", 
                           "index.php?action=show&object=scenario");
       echo "</td>";

       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $outcomeactionIDindex) {
             if (isset($outcomeactionlookup[$row[$i]])) {
                echo '<td>'  . $outcomeactionlookup[$row[$i]] . '</td>'; 
                /* echo '<td>'  . $row[$i] . '</td>'; */
             } else {
                echo '<td> &nbsp; </td>';
             }
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
// Function:   create_scenario
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Posts:      name                    the name of the scenario that is to be created
//             author                  the name (in free format text) of the author of this scenario
//             product                 not used - not needed - ignore
//             operation               set to the value 'create'
//             object                  set to the value 'scenario'
//
// Description:
//
// Presents a form to the user to allow the user to specify the details of a new scenario and then POSTs the values
// that the user provided to the index.php script, so that the script can create a new row in the scenario table.
//
//--------------------------------------------------------------------------------------------------------------------

function create_scenario() {

   echo '<div dojoType="dijit.form.Form" id="create_scenario" jsId="create_scenario" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Scenario</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size = "' . DAF_SCENARIO_LEN . '"  maxlength="' . DAF_SCENARIO_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_SCENARIO_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_SCENARIO_LEN . '}" required="true" trim="true" promptMessage="Enter a Scenario Name" ' . 
        'invalidMessage="Invalid Scenario Name (must be 1 to ' . DAF_SCENARIO_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Author</th><td>';
   echo '<input type="text" name="author" size = "' . DAF_AUTHOR_LEN . '"  maxlength="' . DAF_AUTHOR_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_AUTHOR_LEN . 'em;" regExp="[A-Za-z\s\']{1,' . DAF_AUTHOR_LEN . '}" required="true" trim="true" promptMessage="Enter a Name" ' . 
        'invalidMessage="Invalid Scenario Name (must be 1 to ' . DAF_AUTHOR_LEN . ' alphabetic characters including spaces or \')"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Outcomeaction</th><td><select name=\"outcomeactionID\"/>\n";
   selectorfield("outcomeaction", "Name", "ID", null);
   echo "</select></td></tr>\n";
   echo "</table>\n";
   
   cellbutton("div", "scenario_modify", "Create Scenario", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"create\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "scenario_cancel", "Cancel", "index.php?object=scenario&action=show", null);

   echo "</div>\n"; 
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_scenario
//
// Inputs:     $scenarioID           The ID of the row in the scenario in the scenario table that is to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a table showing all the details of a single scenario.
//
//--------------------------------------------------------------------------------------------------------------------

function show_detailed_scenario($scenarioID) {

   $query = "SELECT * FROM daf.scenario where ID = '" . $scenarioID . "'";
   $scenario = mysql_query($query);

   if (! $scenario) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($scenario);   // should only be one row
   $num_fields = mysql_num_fields($scenario);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenarios</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($scenario, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $scenario_row = mysql_fetch_row($scenario);
   $scenarioname = $scenario_row[$fieldindex['Name']];
   $outcomeactionIDindex = $fieldindex['OutcomeactionID'];
   $outcomeactionlookup  = lookuptable("daf.outcomeaction", "ID", "Name"); 

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $outcomeactionIDindex) {
         echo '<th>Outcome Action</th>';
         if (isset($outcomeactionlookup[$scenario_row[$i]])) {
            echo '<td>'  . $outcomeactionlookup[$scenario_row[$i]] . '</td>'; 
         } else {
            echo '<td> &nbsp; </td>';
         }
      } else {
         echo '<th>' . mysql_field_name($scenario, $i) . '</th>';
         echo '<td>' . $scenario_row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   // $r = lookuptable("teststand", "ID", "Name")  creates an array $r in which $r[23] is the name of the teststand that has ID 23
   $actiontypelookup   = lookuptable("daf.actiontype",   "ID", "Type");
   $testcaselookup     = lookuptable("daf.testcase",     "ID", "Name");
   $environmentlookup  = lookuptable("daf.environment",  "ID", "Name");
   $collectorsetlookup = lookuptable("daf.collectorset", "ID", "Name");
   
   $query = "SELECT step.ID, step.Stepnumber, step.Description ". 
            " FROM daf.step WHERE step.ScenarioID = '" . $scenarioID . 
            "' ORDER by Stepnumber";
   $steps = mysql_query($query);
       
   if (! $steps) die ("Database access failed for query=$query: " . mysql_error());
   
   $query = "SELECT ID, ActiontypeID, Stepnumber, Hostselectortype, Hostselectorvalue, CollectorsetID, EnvironmentID, TestcaseID, Parameters, Duration, Maxduration" . 
            " FROM daf.action WHERE action.ScenarioID = '" . $scenarioID . "'";
   $actions = mysql_query($query);
        
   if (! $actions) die ("Database access failed for query=$query: " . mysql_error());
   
   $stepqueryfieldnames   = array("ID", "Stepnumber", "Description");
   $stepfieldnames        = array("Stepnumber", "Description");
   $actionqueryfieldnames = array("ID", "ActiontypeID", "Stepnumber", "Hostselectortype", "Hostselectorvalue", "CollectorsetID", "EnvironmentID", "TestcaseID", "Parameters", "Duration", "Maxduration");
   $actionfieldnames      = array("ID", "Action", "Host Selector Type", "Host Selector Value", "Collectorset", "Environment", "Testcase", "Parameters", "Duration", "Duration Limit");
       
   $num_steprows              = mysql_num_rows($steps);
   $num_stepfields            = mysql_num_fields($steps);
   $num_displayedstepfields   = count($stepfieldnames);
   
   $num_actionrows            = mysql_num_rows($actions);
   $num_actionfields          = mysql_num_fields($actions);
   $num_displayedactionfields = count($actionfieldnames);

   
   $stepfieldindex = array();
   for ($i = 0; $i < $num_stepfields; $i++) {
      $fieldname = $stepqueryfieldnames[$i];
      $stepfieldindex[$fieldname] = $i;
   } 
   $stepIDindex          = $stepfieldindex['ID'];
   $stepdescriptionindex = $stepfieldindex['Description'];
   
   $actionfieldindex = array();
   for ($i = 0; $i < $num_actionfields; $i++) {
      $fieldname = $actionqueryfieldnames[$i];
      $actionfieldindex[$fieldname] = $i;
   }
   $actionIDindex          =  $actionfieldindex['ID'];
   $actionstepnumberindex  =  $actionfieldindex['Stepnumber'];
   $actiontypeIDindex      =  $actionfieldindex['ActiontypeID'];
   $testcaseIDindex        =  $actionfieldindex['TestcaseID'];
   $environmentIDindex     =  $actionfieldindex['EnvironmentID'];
   $hostselectortypeindex  =  $actionfieldindex['Hostselectortype'];
   $hostselectorvalueindex =  $actionfieldindex['Hostselectorvalue'];
   $collectorsetIDindex    =  $actionfieldindex['CollectorsetID'];
   $durationindex          =  $actionfieldindex['Duration'];
   $maxdurationindex       =  $actionfieldindex['Maxduration'];
   
   $actionrow = array();
   for ($i = 0; $i < $num_actionrows; $i++) {
      $actionrow[$i] = mysql_fetch_row($actions);
   }
   // var_dump($actionrow);
      
   echo '<table>' . "\n";

   //echo '<tr><th colspan=3>Steps...</th>';
   //for ($j = 0; $j < $num_displayedstepfields; $j++) {
   //    if ($j == $stepdescriptionindex) {
   //       echo "<th colspan=7>$stepfieldnames[$j]</th>";
   //    } else {
   //       echo "<th>$stepfieldnames[$j]</th>";
   //    }
   //}  
   //echo '</tr>' . "\n";
      
   echo '<tr>';
   echo '<th colspan=4>';
   echo '<div align="top" style="float:left;" display="inline">'; 
   cellbutton(null, "add_step", "Add Step", "index.php?action=createstep&object=scenario&ID=$scenarioID");
   echo "</div>\n";
   echo '<div align="top" style="float:left;" display="inline">'; 
   cellbutton(null, "copy_scenario", "Copy Scenario", "index.php?action=copyscenario&object=scenario&scenarioname=$scenarioname&ID=$scenarioID");
   echo "</div>\n";
   echo '<div align="top" style="float:left;" display="inline">'; 
   cellbutton(null, "run_scenario", "Run Scenario", "index.php?action=launch&object=scenario&ID=$scenarioID&scenarioname=$scenarioname");
   echo "</div>\n";
   echo '</th><th>Step</th>';
   for ($j = 0; $j < $num_displayedactionfields; $j++) {
      if ($j != $actionIDindex) {
         echo "<th>$actionfieldnames[$j]</th>";
      } else {
      
      }
   }  
   echo '</tr>';   
    
   for ($i = 0; $i < $num_steprows; $i++) {
   
      echo '<tr>'; 
      
      $row        = mysql_fetch_row($steps);
      $stepID     = $row[$stepfieldindex['ID']];
      $stepnumber = $row[$stepfieldindex['Stepnumber']];
      
      echo "<td class=td_smd>\n";
      cellbutton(null, "scenario_copy_step_$i", "copy step", "index.php?action=copystep&object=scenario&ID=$scenarioID&stepnumber=$stepnumber");
      echo "</td>\n";
      echo "<td class=td_smd>\n";
      cellbutton(null, "scenario_add_action_$i", "add action", "index.php?action=createaction&object=scenario&ID=$scenarioID&stepnumber=$stepnumber");
      echo "</td>\n";
      echo "<td class=td_smd>\n";
      cellbutton(null, "scenario_modify_step_$i", "modify", "index.php?action=modifystep&object=scenario&ID=$scenarioID&stepID=$stepID");
      echo "</td>\n";
      echo "<td class=td_smd>\n";
      // cellbutton(null, "scenario_delete_step_$i", "delete", "index.php?action=show&operation=deletestep&object=scenario&ID=$scenarioID&stepID=$stepID&stepnumber=$stepnumber");
      button_with_confirm("scenario_delete_step_$j", "delete", "delete", "index.php?action=show&operation=deletestep&object=scenario&ID=$scenarioID&stepID=$stepID&stepnumber=$stepnumber", 
                          "index.php?action=show&object=scenario&ID=$scenarioID");
      echo "</td>\n";

      for ($j = 0; $j < $num_stepfields; $j++) {
         if ($j == $stepIDindex) {
         } else if ($j == $stepdescriptionindex) {
            echo '<td colspan=9>'  . $row[$j] . '</td>';
         } else {
            echo '<td>'  . $row[$j] . '</td>';
         }     
      }
      echo '</tr>';
      echo "\n";
      
      for ($j = 0; $j < $num_actionrows; $j++) {
         if ($actionrow[$j][$actionstepnumberindex] == $stepnumber) {
            $actionID = $actionrow[$j][$actionfieldindex['ID']];
            echo '<tr>';
            echo "<td class=td_smd>\n";
            echo "</td>\n";
            echo "<td class=td_smd>\n";
            cellbutton(null, "scenario_copy_action_" . $i . "_" . "$j", "copy action", "index.php?action=copyaction&object=scenario&ID=$scenarioID&actionID=$actionID&stepnumber=$stepnumber");
            echo "</td>\n";
            echo "<td class=td_smd>\n";
            cellbutton(null, "scenario_modify_action_" . $i . "_" . "$j", "modify", "index.php?action=modifyaction&object=scenario&ID=$scenarioID&actionID=$actionID");
            echo "</td>\n";
            echo "<td class=td_smd>\n";
            // cellbutton(null, "scenario_delete_action_" . $i . "_" . "$j", "delete", "index.php?action=show&operation=deleteaction&object=scenario&ID=$scenarioID&actionID=$actionID");
            button_with_confirm("scenario_delete_action_$j", "delete", "delete", "index.php?action=show&operation=deleteaction&object=scenario&ID=$scenarioID&actionID=$actionID", 
                                "index.php?action=show&object=scenario&ID=$scenarioID");
            echo "</td>\n";
            echo "<td></td>\n";

            for ($k = 0; $k < $num_actionfields; $k++) {
                     
               if (($k == $actionstepnumberindex) || ($k == $actionIDindex)) {
                  
               } else if ($k == $actiontypeIDindex) {
                  echo '<td>'  . $actiontypelookup[$actionrow[$j][$k]] . '</td>';
               } else if ($k == $testcaseIDindex) {
                  echo '<td>'  . $testcaselookup[$actionrow[$j][$k]] . '</td>';
               } else if ($k == $environmentIDindex) {
                  if (isset($environmentlookup[$actionrow[$j][$k]])) {
                     echo '<td class=td_smd><a href=index.php?action=show&object=environment&ID=' . $actionrow[$j][$k] . ">" . $environmentlookup[$actionrow[$j][$k]] . '</a></td>';
                  } else {
                     echo '<td> &nbsp; </td>';
                  }
               } else if ($k == $collectorsetIDindex) {
                  if (isset($collectorsetlookup[$actionrow[$j][$k]])) {
                     echo '<td><a href="index.php?action=show&object=collectorset&ID=' . $actionrow[$j][$k] . '">'  . $collectorsetlookup[$actionrow[$j][$k]] . '</a></td>';
                  } else {
                     echo '<td> &nbsp; </td>';
                  }
               } else if (($k == $hostselectortypeindex) || ($k == $hostselectorvalueindex)) {
                  echo '<td>' . $actionrow[$j][$k] . '</td>';
               } else if ($k == $maxdurationindex) {
                   if ($actionrow[$j][$durationindex] == 'Rununtilcomplete') {
                      echo '<td> n/a </td>';
                   } else {
                      echo '<td>' . hms($actionrow[$j][$k]) . '</td>';
                   }

               } else {
                  echo '<td>'  . $actionrow[$j][$k] . '</td>';
               }  
                 
            }        
            echo '</tr>';
            echo "\n";
         }   
      }      
   }

   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_scenario
//
// Inputs:     $scenarioID           The ID of the row in the scenario in the scenario table that is to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Posts:      action                set to the value 'show'
//             operation             set to the value 'modify'
//             object                set to the value 'scenario'  
//             ID                    the ID of the scenario row that is being changed
//             other items           fields in the scenario row   
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function modify_scenario($scenarioID) {

   $query = "SELECT ID, Name, Author, OutcomeactionID, Createdon, Lastchanged FROM daf.scenario where ID = '" . $scenarioID . "'";
   $scenario = mysql_query($query);

   if (! $scenario) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($scenario);   // should only be one row
   $num_fields = mysql_num_fields($scenario);

   echo '<div dojoType="dijit.form.Form" id="modify_scenario" jsId="modify_scenario" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify a Scenario</div></caption>\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($scenario, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $row = mysql_fetch_row($scenario);
   $scenarioID = $row[$fieldindex['ID']];  
   $IDindex = $fieldindex['ID'];
   $outcomeactionIDindex = $fieldindex['OutcomeactionID'];

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($scenario, $i);
      echo '<tr>'; 
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($i == $outcomeactionIDindex) {
         echo "<th>Outcomeaction</th><td><select name=\"outcomeactionID\"/>\n";
         selectorfield("outcomeaction", "Name", "ID", isset($row[$outcomeactionIDindex]) ? $row[$outcomeactionIDindex] : NULL);
         echo "</select></td>\n";
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size = "' . DAF_SCENARIO_LEN . '"  maxlength="' . DAF_SCENARIO_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_SCENARIO_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_SCENARIO_LEN . '}" required="true" trim="true" promptMessage="Enter a Scenario Name" ' . 
              'invalidMessage="Invalid Scenario Name (must be 1 to ' . DAF_SCENARIO_LEN . ' alphanumeric characters)"/></td>';
      } else if ($fieldname == "Author") {
         echo '<th>Author</th>';
         echo '<td><input type="text" name="author" value="' . $row[$i] . '" size = "' . DAF_AUTHOR_LEN . '"  maxlength="' . DAF_AUTHOR_LEN . '" ' . 
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_AUTHOR_LEN . 'em;" regExp="[A-Za-z\s\']{1,' . DAF_AUTHOR_LEN . '}" required="true" trim="true" promptMessage="Enter a Name" ' . 
              'invalidMessage="Invalid Scenario Name (must be 1 to ' . DAF_AUTHOR_LEN . ' alphabetic characters including spaces or \')"/></td>';
      } else if ($fieldname == "Createdon") {
         echo '<th>Created On</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Lastchanged") {
         echo '<th>Last Changed</th>';
         echo '<td>' . $row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   
   echo '</table>';   
       
   cellbutton("div", "scenario_modify", "Modify Scenario", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo '<input type="hidden" name="ID" value="' . $scenarioID . "\"/>\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname . "\"/>\n";
   cellbutton(null, "scenario_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);

   echo '</div>';   
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_stepnumbers_in_scenario
//
// Inputs:     $scenarioID           The ID of the scenario that is to be examined
//
// Outputs:    none
//
// Returns:    an array containing a list of the steps that are present in the scenario,  eg (0,1,2,3)
//
// Description:
//
// Performs a query on the daf step table to determine the stepnumbers of the steps in the specified scenario.  If there
// are no steps in the scenario and empty array is returned
//
//--------------------------------------------------------------------------------------------------------------------

function get_stepnumbers_in_scenario($scenarioID) {

   $query = "SELECT Stepnumber FROM daf.step where scenarioID = '" . $scenarioID . "' ORDER BY Stepnumber";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $steps = array();
   $num_rows   = mysql_num_rows($result);  
   for ($i = 0; $i < $num_rows; $i++) {      
      $row = mysql_fetch_row($result);
      array_push($steps, $row[0]);
   }
   
   return $steps;
   
} 

 
//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_step
//
// Inputs:     $scenarioID
//
// Outputs:    none
//
// Posts:      stepnumber
//             operation                  
//
// Returns:    nothing
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function create_step($scenarioID) {

   $steps = get_stepnumbers_in_scenario($scenarioID);   

   echo '<div dojoType="dijit.form.Form" id="create_step" jsId="create_step" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a Scenario Step</div></caption>\n";
   
   if (count($steps) == 0) {
   
      echo "<tr><th colspan=2>Step number 1</th></tr>\n";
      echo "<tr><th>Description</th><td>";
      echo "<input type=\"text\" name=\"description\" value=\"Step X\" size = \"" . DAF_STEPDESCRIPTION_LEN . '"  maxlength="' . DAF_STEPDESCRIPTION_LEN . '" ' .  
           'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_STEPDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a description" ' . 
           'invalidMessage="Invalid Scenario Step description (must be 1 to ' . DAF_STEPDESCRIPTION_LEN . ' characters)"/></td></tr>';
      echo "</table>\n";
      echo "<input type=\"hidden\" name=\"stepnumber\" value=\"1\" />\n";
      echo "<input type=\"hidden\" name=\"operation\" value=\"createstep\" />\n";

   
   } else {
   
      echo "<tr><th>Where</th><td><select name=\"where\">\n";
      echo "<option value=\"before\">before</option>\n";
      echo "<option value=\"after\" selected=\"selected\">after</option>\n";
      echo "</select></td></tr>\n";
      echo "<tr><th>Stepnumber</th><td><select name=\"stepnumber\">\n";
      for ($i=0; $i<count($steps); $i++) {
         if ($i != (count($steps)-1)) {
            echo "<option value=\"" . $steps[$i] . "\">$steps[$i]</option>\n";
         } else {
            echo "<option value=\"" . $steps[$i] . "\" selected=\"selected\">$steps[$i]</option>\n";
         }
      }
      echo "</select></td></tr>\n";
      echo "<tr><th>Number of steps to create</th><td>";
      echo '<input type="text" name="numsteps" value="1" size="10" maxlength="10" ' .  
           'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp="\d{1,4}" required="true" trim="true" promptMessage="Enter the number of steps to be inserted" ' . 
           'invalidMessage="Invalid number of steps (must be 1 to 999)"/></td></tr>';
      echo "<tr><th>Description</th><td>";
      echo "<input type=\"text\" name=\"description\" value=\"Step X\" size = \"" . DAF_STEPDESCRIPTION_LEN . '"  maxlength="' . DAF_STEPDESCRIPTION_LEN . '" ' .  
           'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_STEPDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a description" ' . 
           'invalidMessage="Invalid Scenario Step description (must be 1 to ' . DAF_STEPDESCRIPTION_LEN . ' characters)"/></td></tr>';
      echo "</table>\n";
      echo "<input type=\"hidden\" name=\"operation\" value=\"insertstep\" />\n";

   }

   cellbutton(null, "step_create", "Create Step", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"scenario\" />\n";
   echo "<input type=\"hidden\" name=\"ID\"        value=\"$scenarioID\" />\n";
   cellbutton(null, "step_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);
  
   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_step
//
// Inputs:     $scenarioID
//             $stepID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function modify_step($scenarioID, $stepID) {

   $steps = get_stepnumbers_in_scenario($scenarioID);   

   $query = "SELECT Stepnumber, Description FROM daf.step where ID = '" . $stepID . "'";
   $step = mysql_query($query);

   if (! $step) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($step);   // should only be one row
   $num_fields = mysql_num_fields($step);
   
   $step_row = mysql_fetch_row($step);

   $stepnumber  = $step_row[0];
   $description = $step_row[1];  

   echo '<div dojoType="dijit.form.Form" id="modify_step" jsId="modify_step" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify a Step</div></caption>\n";
   
   echo "<tr><th>Stepnumber</th>\n";
   echo "<td><input type=\"text\" name=\"stepnumber\" value=\"" . $step_row[0] . "\" readonly=\"true\" /></td></tr>\n";
   echo "<tr><th>Description</th>\n";
   echo "<td><input type=\"text\" name=\"description\" value=\"" . $step_row[1] . '" size = "' . DAF_STEPDESCRIPTION_LEN . '"  maxlength="' . DAF_STEPDESCRIPTION_LEN . '" ' .  
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_STEPDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a description" ' . 
        'invalidMessage="Invalid Scenario Step description (must be 1 to ' . DAF_STEPDESCRIPTION_LEN . ' characters)"/></td></tr>';
   echo "</table>\n";
   
   cellbutton(null, "step_modify", "Modify Step", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"stepID\"    value=\"$stepID\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modifystep\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\"        value=\"$scenarioID\" />\n";
   cellbutton(null, "step_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);

   echo "</div>\n";
   
}



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   copy_step
//
// Inputs:     $scenarioID
//             $stepnumber 
//             
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function copy_step($scenarioID, $stepnumber) {

   $steps = get_stepnumbers_in_scenario($scenarioID);   

   $query = "SELECT Description FROM daf.step where ScenarioID = '" . $scenarioID . "' AND Stepnumber = '" . $stepnumber . "'";
   $action = mysql_query($query);

   if (! $action) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($action);   // should only be one row
   $num_fields = mysql_num_fields($action);
   
   $row = mysql_fetch_row($action);
 
   $description = $row[0];

   echo '<div dojoType="dijit.form.Form" id="copy_step" jsId="copy_step" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Copy Step(s) from Step $stepnumber</div></caption>\n";

   echo "<tr><th>Where</th><td><select name=\"where\">\n";
   echo "<option value=\"before\">before</option>\n";
   echo "<option value=\"after\" selected=\"selected\">after</option>\n";
   echo "</select></td></tr>\n";
   echo "<tr><th>Destination stepnumber</th><td><select name=\"deststepnumber\">\n";
      for ($i=0; $i<count($steps); $i++) {
         if ($i != (count($steps)-1)) {
            echo "<option value=\"" . $steps[$i] . "\">$steps[$i]</option>\n";
         } else {
            echo "<option value=\"" . $steps[$i] . "\" selected=\"selected\">$steps[$i]</option>\n";
         }
      }
   echo "</select></td></tr>\n";
   echo "<tr><th>Number of steps to copy</th><td>";
   echo '<input type="text" name="numsteps" value="1" size="10" maxlength="10" ' .  
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp="\d{1,4}" required="true" trim="true" promptMessage="Enter the number of steps to be copied" ' . 
        'invalidMessage="Invalid number of steps (must be 1 to 999)"/></td></tr>';
   echo "<tr><th>Number of times to duplicate steps</th><td>";
   echo '<input type="text" name="numdups" value="1" size="10" maxlength="10" ' .  
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp="\d{1,4}" required="true" trim="true" promptMessage="Enter the number of times the steps are to be copied" ' . 
        'invalidMessage="Invalid number of duplicates (must be 1 to 999)"/></td></tr>';
   echo '<tr><th>Description</th>';                /* what happens if description has a ' in it etc  here and elsewhere <<<<< */
   echo '<td><input type="text" name="description" value="Duplicate of ' . $description . '" size="' . DAF_ACTIONDESCRIPTION_LEN . '" maxlength="' . DAF_ACTIONDESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_ACTIONDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Description" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ACTIONDESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table></td></tr>\n";
   
   cellbutton(null, "action_copy", "Copy Step", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"     value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"     value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"  value=\"copystep\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\"         value=\"$scenarioID\" />\n";
   echo "<input type=\"hidden\" name=\"stepnumber\" value=\"$stepnumber\" />\n";
   cellbutton(null, "action_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);

   echo "</div>\n";
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_action
//
// Inputs:     $scenarioID
//             $stepID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function create_action($scenarioID, $stepnumber) {

   echo '<div dojoType="dijit.form.Form" id="create_action" jsId="create_action" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Action</div></caption>\n";
   
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size="' . DAF_ACTIONDESCRIPTION_LEN . '" maxlength="' . DAF_ACTIONDESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_ACTIONDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Description" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ACTIONDESCRIPTION_LEN . " characters)\"/></td></tr>\n";
   echo "<tr><th>Action</th><td><select name=\"actiontypeID\"/>\n";
   selectorID("actiontype", "Type");
   echo "</select></td></tr>\n";
   // echo "<tr><th>Host Selector</th><td><select id=\"hostSelector\" name=\"hostselectortype\" onchange=\"selectHostSelectorValues(this, 'hostselectorvalueId')\" />\n";
   echo "<tr><th>Host Selector</th><td><select id=\"hostSelector\" name=\"hostselectortype\" onchange=\"selectHostSelectorValues(this, 'hostselectortdId')\" />\n";
   selectorIDforhostselectortype();
   echo "</select></td></tr>\n";
   echo "<tr><th>Host Selector Value</th>";
   //<td><select name=\"hostselectorvalue\"/>\n";   
   //selectorIDforhostselectorvalue();
   //echo "</select></td></tr>\n";
   echo "<td id=\"hostselectortdId\">";
   //echo "<select id=\"hostselectorvalueId\" name=\"hostselectorvalue\"/>";
   //echo "<--- onchange on the Host Selector select will fill this in --->";
   //echo "</select>";
   echo "</td></tr>\n";

   echo "<tr><th>Collector Set</th><td><select name=\"collectorsetID\"/>\n";
   selectorID("collectorset", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Environment</th><td><select name=\"environmentID\"/>\n";
   selectorID("environment", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Testcase</th><td><select name=\"testcaseID\"/>\n";
   selectorID("testcase", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Parameters</th><td><textarea name=\"parameters\" dojoType=\"dijit.form.Textarea\"></textarea>\n";            /* <<<<<<<<<  validation needed */
   echo "<tr><th>Duration</th><td><select id=\"durationSelector\" name=\"duration\" onchange=\"selectDuration(this, 'maxDurationID')\" />\n";
   selectorIDforDuration("Rununtilcomplete");
   echo "</select></td></tr>\n";
   echo '<tr><th>Max Duration</th>';
   echo '<td id="maxDurationID">';
   /*  <input id="maxDurationID" type="text" name="maxduration" size=10 maxlength=10 ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: 10em;" regExp="\d+" required="true" trim="true" promptMessage="Enter a duration in seconds" ' . 
        'invalidMessage="Invalid duration (must be a decimal number > 0)"/> */
   echo '</td>';
   echo "</table>\n";

   cellbutton(null, "action_create", "Create Action", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"     value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"object\"     value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"stepnumber\" value=\"$stepnumber\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"  value=\"createaction\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\"         value=\"$scenarioID\"/>\n";
   cellbutton(null, "action_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);


   echo "</div>\n";
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_action
//
// Inputs:     $scenarioID
//             $stepID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function modify_action($scenarioID, $actionID) {

   $query = "SELECT ID, Stepnumber, Hostselectortype, Hostselectorvalue, CollectorsetID, ActiontypeID, EnvironmentID, TestcaseID, Parameters, Description, Duration, Maxduration " . 
            "FROM daf.action where ID = '" . $actionID . "'";
   $action = mysql_query($query);

   if (! $action) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($action);     // should only be one row
   $num_fields = mysql_num_fields($action);
   
   $row = mysql_fetch_row($action);
   
   echo '<div dojoType="dijit.form.Form" id="modify_action" jsId="modify_action" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify an Action</div></caption>\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($action, $i);
      $fieldindex[$fieldname] = $i;
   }
 
   $ID                = $row[0];
   $stepnumber        = $row[1];
   $hostselectortype  = $row[2];
   $hostselectorvalue = $row[3];
   $collectorsetID    = $row[4];
   $actiontypeID      = $row[5];
   $environmentID     = $row[6];
   $testcaseID        = $row[7];
   $parameters        = $row[8];
   $description       = $row[9];
   $duration          = $row[10];
   $maxduration       = $row[11];  

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($action, $i);
      
      if ($fieldname == "ID") {
         echo '<tr><th>ID</th>';
         echo '<td>' . $row[$i] . '</td></tr>';
      } else if ($fieldname == "Stepnumber") {
         echo '<tr><th>Step number</th>';
         echo "<td><input type=\"text\" name=\"stepnumber\" value=\"" . $row[$i]  . "\" readonly=\"true\" /></td></tr>\n";
      } else if ($fieldname == "Description") {
         echo '<tr><th>Description</th>';
         echo '<td><input type="text" name="description" value="' . $row[$i] . '" size = "' . DAF_ACTIONDESCRIPTION_LEN . '" maxlength="' . DAF_ACTIONDESCRIPTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ACTIONDESCRIPTION_LEN . 'em;" regExp=".{1,' . DAF_ACTIONDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Description" ' . 
              'invalidMessage="Invalid Description (must be 1 to ' . DAF_ACTIONDESCRIPTION_LEN . ' characters)"/>';
         echo "</td></tr>\n";
      } else if ($fieldname == "Hostselectortype") {
         echo '<tr><th>Host Selector Type</th>';
         echo "<td><select name=\"hostselectortype\"/>\n";
         selectorIDforhostselectortype($hostselectortype);
         echo "</select></td></tr>\n";
      } else if ($fieldname == "Hostselectorvalue") {
         echo '<tr><th>Host Selector Value</th>';
         echo "<td><select name=\"hostselectorvalue\" />\n";
         selectorIDforhostselectorvalue($hostselectorvalue);
         echo "</select></td></tr>\n"; 
      } else if ($fieldname == "CollectorsetID") {
         echo '<tr><th>Collector Set</th>';
         echo "<td><select name=\"collectorsetID\"/>\n";
         selectorID("collectorset", "Name", $collectorsetID);
         echo "</select></td></tr>\n";
      } else if ($fieldname == "ActiontypeID") {
         echo '<tr><th>Action Type</th>';
         echo "<td><select name=\"actiontypeID\"/>\n";
         selectorID("actiontype", "Type", $actiontypeID);
         echo "</select></td></tr>\n";
      } else if ($fieldname == "EnvironmentID") {
         echo '<tr><th>Environment</th>';
         echo "<td><select name=\"environmentID\"/>\n";
         selectorID("environment", "Name", $environmentID);
         echo "</select></td></tr>\n";
      } else if ($fieldname == "TestcaseID") {
         echo '<tr><th>Testcase</th>';
         echo "<td><select name=\"testcaseID\"/>\n";
         selectorID("testcase", "Name", $testcaseID);
         echo "</select></td></tr>\n";
      } else if ($fieldname == "Parameters") {
         echo '<tr><th>Parameters</th>';                            /* <<<<<<<<<  validation for a textarea needed, what happens if parameters has a ' in it */
         echo '<td><input type="text" name="parameters" value="' . $parameters . '" size = "' . DAF_PARAMETERS_LEN . '" maxlength="' . DAF_PARAMETERS_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_PARAMETERS_LEN . '}" trim="true" promptMessage="Enter the parameters for the testcase" ' . 
              'invalidMessage="Invalid parameters (must be 1 to ' . DAF_PARAMETERS_LEN . ' characters)"/>';
         echo "</td></tr>\n";
      } else if ($fieldname == "Duration") {
         echo '<tr><th>Duration</th>';
         echo "<td><select name=\"duration\"/>\n";
         selectorIDforDuration($duration);
         echo "</select></td></tr>\n";
      } else if ($fieldname == "Maxduration") {
         echo '<tr><th>Max Duration</th>';
         echo '<td><input type="text" name="maxduration" value="' . $maxduration . '" size = "10" maxlength="10" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: 10em;" regExp="\d+" required="true" trim="true" promptMessage="Enter a duration in seconds" ' . 
              'invalidMessage="Invalid duration (must be a decimal number > 0)"/>';
         echo "</td></tr>\n";
      }
   }
   echo "</table>\n";

   cellbutton(null, "action_modify", "Modify Action", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"     value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"     value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"  value=\"modifyaction\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\"         value=\"$scenarioID\" />\n";
   echo "<input type=\"hidden\" name=\"actionID\"   value=\"$actionID\" />\n";
   cellbutton(null, "action_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);

   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   copy_action
//
// Inputs:     $scenarioID
//             $actionID,,, 
//             
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function copy_action($scenarioID, $actionID) {

   $steps = get_stepnumbers_in_scenario($scenarioID);   

   $query = "SELECT Stepnumber, Hostselectortype, Hostselectorvalue, CollectorsetID, ActiontypeID, EnvironmentID, TestcaseID, Parameters, Description, Duration, Maxduration " . 
            "FROM daf.action where ID = '" . $actionID . "'";
   $action = mysql_query($query);

   if (! $action) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($action);   // should only be one row
   $num_fields = mysql_num_fields($action);
   
   $row = mysql_fetch_row($action);
 
   $stepnumber        = $row[0];
   $hostselectortype  = $row[1];
   $hostselectorvalue = $row[2];
   $collectorsetID    = $row[3];
   $actiontypeID      = $row[4];
   $environmentID     = $row[5];
   $testcaseID        = $row[6];
   $parameters        = $row[7];
   $description       = $row[8];
   $duration          = $row[9];
   $maxduration       = $row[10];  

   echo '<div dojoType="dijit.form.Form" id="copy_action" jsId="copy_action" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Copy Action</div></caption>\n";

   echo "<tr><th>Destination stepnumber</th><td><select name=\"stepnumber\">\n";
      for ($i=0; $i<count($steps); $i++) {
         if ($i != (count($steps)-1)) {
            echo "<option value=\"" . $steps[$i] . "\">$steps[$i]</option>\n";
         } else {
            echo "<option value=\"" . $steps[$i] . "\" selected=\"selected\">$steps[$i]</option>\n";
         }
      }
   echo "</select></td></tr>\n";
   echo "<tr><th>Number of times to duplicate action</th><td>";
   echo '<input type="text" name="numdups" value="1" size="10" maxlength="10" ' .  
        'dojoType="dijit.form.ValidationTextBox" style="width:10em" regExp="\d{1,4}" required="true" trim="true" promptMessage="Enter the number of times the action is to be copied" ' . 
        'invalidMessage="Invalid number of duplicates (must be 1 to 999)"/></td></tr>';
   echo '<tr><th>Description</th>';                /* what happens if description has a ' in it etc  here and elsewhere <<<<< */
   echo '<td><input type="text" name="description" value="' . $description . '" size="' . DAF_ACTIONDESCRIPTION_LEN . '" maxlength="' . DAF_ACTIONDESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:90%" regExp=".{1,' . DAF_ACTIONDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Description" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ACTIONDESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Action</th><td><select name=\"actiontypeID\"/>\n";
   selectorID("actiontype", "Type", $actiontypeID);
   echo "</select></td></tr>\n";
   echo "<tr><th>Host Selector Type</th><td><select name=\"hostselectortype\"/>\n";
   selectorIDforhostselectortype($hostselectortype);
   echo "</select></td></tr>\n";
   echo "<tr><th>Host Selector Value</th><td><select name=\"hostselectorvalue\" />\n";
   selectorIDforhostselectorvalue($hostselectorvalue);
   echo "</select></td></tr>\n"; 
   echo "<tr><th>Collector Set</th><td><select name=\"collectorsetID\"/>\n";
   selectorID("collectorset", "Name", $collectorsetID);
   echo "</select></td></tr>\n";
   echo "<tr><th>Environment</th><td><select name=\"environmentID\"/>\n";
   selectorID("environment", "Name", $environmentID);
   echo "</select></td></tr>\n";
   echo "<tr><th>Testcase</th><td><select name=\"testcaseID\"/>\n";
   selectorID("testcase", "Name", $testcaseID);
   echo "</select></td></tr>\n";
   echo "<tr><th>Parameters</th>";
   echo '<td><input type="text" name="parameters" value="' . $parameters . '" size = "' . DAF_PARAMETERS_LEN . '" maxlength="' . DAF_PARAMETERS_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PARAMETERS_LEN . 'em;" regExp=".{1,' . DAF_PARAMETERS_LEN . '}" required="true" trim="true" promptMessage="Enter the parameters for the testcase" ' . 
        'invalidMessage="Invalid parameters (must be 1 to ' . DAF_PARAMETERS_LEN . ' characters)"/>';
   echo "<tr><th>Duration</th><td><select name=\"duration\"/>\n";
   selectorIDforDuration($duration);
   echo "</select></td></tr>\n";
   echo "<tr><th>Max Duration</th>";
   echo '<td><input type="text" name="maxduration" value="' . $maxduration . '" size = "10" maxlength="10" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: 10em;" regExp="\d+" required="true" trim="true" promptMessage="Enter a duration in seconds" ' . 
        'invalidMessage="Invalid duration (must be a decimal number > 0)"/>';
   echo "</table></td></tr>\n";
   
   cellbutton(null, "action_copy", "Copy Action", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"     value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"     value=\"scenario\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"  value=\"copyaction\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\"         value=\"$scenarioID\" />\n";
   echo "<input type=\"hidden\" name=\"actionID\"   value=\"$actionID\" />\n";
   cellbutton(null, "action_cancel", "Cancel", "index.php?object=scenario&action=show&ID=$scenarioID", null);

   echo "</div>\n";
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   launch_scenario
//
// Inputs:     $scenarioID
//             $jobname
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function launch_scenario($scenarioID, $scenarioname, $current_project, $current_phase, $current_user_ID) {

   $jobname = make_legal_filename($scenarioname, 48);
   $scenariologfilename = "console_" . $jobname . ".out"; 
   
   $logdirectory = lookupfield("daf.phase", "Logdirectory", "Name", $current_phase);
   
   echo '<div dojoType="dijit.form.Form" id="launch_scenario" jsId="launch_scenario" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Launch a Scenario</div></caption>\n";
   
   echo "<tr><th>Project</th><td><input type=\"text\" value=\"" . $current_project . "\" readonly=\"true\" /></td></tr>\n";
   echo "<tr><th>Phase</th><td><input type=\"text\" value=\"" . $current_phase . "\" readonly=\"true\" /></td></tr>\n";
   echo '<tr><th>JobName</th>';
   echo '<td><input type="text" name="jobname" size="' . DAF_JOBNAME_LEN . '" maxlength="' . DAF_JOBNAME_LEN . '" value="' . $jobname . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_JOBNAME_LEN . 'em;" regExp=".{1,' . DAF_JOBNAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Jobname" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_JOBNAME_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Log location</th>';
   echo '<td><input type="text" name="logdirectory" size="' . DAF_LOGDIRECTORY_LEN . '" maxlength="' . DAF_LOGDIRECTORY_LEN . '" value="' . $logdirectory . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LOGDIRECTORY_LEN . 'em;" regExp=".{1,' . DAF_LOGDIRECTORY_LEN . '}" required="true" trim="true" promptMessage="Enter a Log directory" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_LOGDIRECTORY_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Scenario log file</th>';
   echo '<td><input type="text" name="scenariologfilename" size="' . DAF_LOGFILENAME_LEN  . '" maxlength="' . DAF_LOGFILENAME_LEN  . '" value="' . $scenariologfilename . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LOGFILENAME_LEN . 'em;" regExp=".{1,' . DAF_LOGFILENAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Scenario Log File name" ' . 
        'invalidMessage="Invalid Scenario Log File name (must be 1 to ' . DAF_LOGFILENAME_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Teststand</th><td><select name=\"teststand\"/>\n";
   selectorfield("teststand", "Name", "Name", null);
   echo "</select></td></tr>\n";
   echo "<tr><th>Test level</th><td><select name=\"testlevel\"/>\n";
   selectorfield("testlevel", "Name", "Name", null);
   echo "</select></td></tr>\n";
   //echo "<tr><th>Tester</th><td><select name=\"testerID\"/>\n";
   //selectorIDforUserName();
   //echo "</select></td></tr>\n";
   echo '<tr><th>Comments</th>';
   echo '<td><input type="text" name="comments" size="' . DAF_COMMENTS_LEN . '" maxlength="' . DAF_COMMENTS_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:100%;" regExp=".{1,' . DAF_COMMENTS_LEN . '}" promptMessage="Enter optional Comments" ' . 
        'invalidMessage="Invalid Comments (must be 1 to ' . DAF_COMMENTS_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";
   
   cellbutton(null, "scenario_run", "Run Scenario", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"       value=\"run\" /> \n";
   echo "<input type=\"hidden\" name=\"object\"       value=\"scenario\" /> \n";
   echo "<input type=\"hidden\" name=\"project\"      value=\"" . $current_project . "\" />\n";
   echo "<input type=\"hidden\" name=\"phase\"        value=\"" . $current_phase   . "\" />\n"; 
   echo "<input type=\"hidden\" name=\"scenarioname\" value=\"" . $scenarioname    . "\" />\n"; 
   echo "<input type=\"hidden\" name=\"testerID\" value=\"" . $current_user_ID . "\" />\n"; 
   echo "<input type=\"hidden\" name=\"scenarioID\"   value=\"$scenarioID\" />\n";
   cellbutton(null, "scenario_cancel", "Cancel", "index.php?object=scenario&action=show", null);
   
   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   make_legal_filename
//
// Inputs:     $name
//             $max_length
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function make_legal_filename($name, $max_length) {

   return substr(preg_replace("/[^a-zA-Z0-9]/","_", $name), 0, $max_length);
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   run_scenario
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function run_scenario($gp, $project, $phase) {

   global $db_server;
   
   /* -----------------------------------------------------------------------------------------*/
   /* Confirm that all agents for this teststand are online                                    */
   /* -----------------------------------------------------------------------------------------*/ 
   
   list($offline_hosts, $online_hosts) = get_hosts_for_stand($gp['teststand']);
   
   if (count($offline_hosts) > 0) {
      
      add_error_message("DAFE0046: There are offline hosts (" . implode(",", $offline_hosts) . ") in test stand " . $gp['teststand'] . " - cannot run scenario");
      return;
      
   }   
   
   $scenarioID    = $gp['scenarioID'];
   $teststand     = $gp['teststand'];
   $testlevel     = $gp['testlevel'];
   $testerID      = $gp['testerID']; 
   $testername    = get_username_from_ID($testerID);

   $fields = array();
   $values = array();

   /* -----------------------------------------------------------------------------------------*/
   /* create the scenarioresult for this run                                                   */
   /* -----------------------------------------------------------------------------------------*/ 
   
    array_push($fields, "Project");             array_push($values, $gp['project']); 
    array_push($fields, "Phase");               array_push($values, $gp['phase']); 
    array_push($fields, "Scenarioname");        array_push($values, $gp['scenarioname']);         
    array_push($fields, "Jobname");             array_push($values, $gp['jobname']); 
    array_push($fields, "Teststand");           array_push($values, $teststand);  
    array_push($fields, "Testlevel");           array_push($values, $testlevel); 
    array_push($fields, "Tester");              array_push($values, $testername); 
    array_push($fields, "Comments");            array_push($values, $gp['comments']); 
    array_push($fields, "State");               array_push($values, "Initialised");
    array_push($fields, "Scenariologfilename"); array_push($values, $gp['scenariologfilename']);
      
    $status_db  = determine_status_database_name($project, $phase);
    $tablename = determine_status_table_name($status_db, "scenarioresult");
    $query = "INSERT INTO " . $tablename . "(" . implode(",", $fields) .  ", Start ) " .
             "VALUES('" . implode("','", $values) . "', NOW())";             
           
    if (($scenarioresultID = do_mysql_query_id($query, $db_server, 1)) == 0) {
       return;
    }  
    
   /* -----------------------------------------------------------------------------------------*/
   /* create the testlevelrecord for this run                                                  */
   /* -----------------------------------------------------------------------------------------*/

   $testlevelID = lookupID("testlevel", $testlevel);
   
   $fields = array("ScenarioresultID", "Name");
   $values = array($scenarioresultID, $testlevel);
   
   $tablename = determine_status_table_name($status_db, "testlevelrecord");
   $query = "INSERT INTO $tablename (" . implode(",", $fields) .  ") " .
            "VALUES('" . implode("','", $values) . "')";             
   $testlevelrecordID = do_mysql_query_id($query, $db_server, 1);   /* what happens if this fails  ? <<<<< */
    
    /* -----------------------------------------------------------------------------------------*/
    /* create the teststandrecord for this run                                                  */
    /* -----------------------------------------------------------------------------------------*/

    $teststandID = lookupID("teststand", $teststand);   
   
    $fields = array("ScenarioresultID", "Name");
    $values = array($scenarioresultID, $teststand);
 
    $tablename = determine_status_table_name($status_db, "teststandrecord");  
    $query = "INSERT INTO $tablename (" . implode(",", $fields) .  ") " .
             "VALUES('" . implode("','", $values) . "')";             
    if (($teststandrecordID = do_mysql_query_id($query, $db_server, 1)) == 0) {
       return;
    } 
     
    // are there any clusters to record ?
  
   /* $query = "SELECT Name FROM cluster WHERE TeststandID = $teststandID";
    $result = mysql_query($query, $db_server);
    if (! $result) {
       echo "<br />select $query failed - " . mysql_error() . "<br />";
       return;
    } 
        
    $num_rows = mysql_num_rows($result);
  
    for ($i= 0; $i < $num_rows; $i++) {

       $row = mysql_fetch_row($result);

       $clustername = $row[0];
       $fields = array("ScenarioresultID", "Name", "Teststandname");
       $values = array($scenarioresultID, $clustername, $teststand);
       $tablename = determine_status_table_name($status_db, "clusterrecord");  
       $query = "INSERT INTO $tablename (" . implode(",", $fields) . ") " .
                                         "VALUES('" . implode("','", $values) . "')";             
       if (! mysql_query($query, $db_server)) {
          add_error_message("insert $query failed - " . mysql_error());
          return;
       }
       $clusterrecordID = mysql_insert_id(); */ /* <<<<<<<<< needs rewrite XXXXXXXXXXXXXX */
    
       // are there any nodes to record ?
  
 /*      $query = "SELECT Name, Type, Model, Serial FROM node WHERE clusterID = (SELECT ID FROM cluster where TeststandID = $teststandID)";
       $nodes = mysql_query($query, $db_server);
       if (! $result) {
          echo "<br />select $query failed - " . mysql_error() . "<br />";
          return;
       } 
        
       $num_node_rows = mysql_num_rows($nodes);
  
       for ($i = 0; $i < $num_node_rows; $i++) {

          $node_row = mysql_fetch_row($nodes);

          $Name   = $node_row[0];
          $Type   = $node_row[1];
          $Model  = $node_row[2];
          $Serial = $node_row[3];
          $fields = array("ScenarioresultID", "Clustername", "Name", "Type", "Model", "Serial");
          $values = array($scenarioresultID, $clustername, $Name, $Type, $Model, $Serial);
          $tablename = determine_status_table_name($status_db, "noderecord");
          $query = "INSERT INTO $tablename (" . implode(",", $fields) . ") " .
                                            "VALUES('" . implode("','", $values) . "')";             
          if (! mysql_query($query, $db_server)) {
             add_error_message("insert $query failed - " . mysql_error());
             return;
          }
          $noderecordID = mysql_insert_id();
       }   
    } */
    
   /* -----------------------------------------------------------------------------------------*/
   /* update the scenarioresult for this run                                                   */
   /* -----------------------------------------------------------------------------------------*/ 
   
    $num_actions_in_scenario = count_actions_in_scenario($scenarioID);
    
    $logdirectory = $gp['logdirectory'];
   
    $loglocation = $logdirectory . '/' . 'scenario_' . $scenarioresultID . '_' . make_legal_filename($gp['jobname'], 32);;
    $tablename = determine_status_table_name($status_db, "scenarioresult");
    $query = "UPDATE $tablename SET Loglocation = '$loglocation', TeststandrecordID = '$teststandrecordID', " . 
             "TestlevelrecordID = '$testlevelrecordID', Actionsinscenario = '$num_actions_in_scenario' WHERE ID='$scenarioresultID'";     
              
    if (! do_mysql_query($query, $db_server, 1)) {
       return;
    }      
    
    /* -----------------------------------------------------------------------------------------*/
    /* create the workrequest entry for this run                                                */
    /* -----------------------------------------------------------------------------------------*/
    
    $teststandID = lookupID("teststand", $teststand);
     
    $query = "INSERT INTO daf.workrequest(Project, Phase, RequestType, TesterID, Tablename1, ObjectID1, Tablename2, ObjectID2, Tablename3, ObjectID3, Tablename4, ObjectID4, Logdirectory, Scenariologfilename ) " .
                                      "VALUES(" . 
                                      "'" . $gp['project'] . "'," . 
                                      "'" . $gp['phase'] . "'," . 
                                      "'Scenario'," .
                                      "'" . $testerID . "'," .
                                      "'scenario'," .
                                      "'" . $scenarioID . "'," .
                                      "'scenarioresult'," .
                                      "'" . $scenarioresultID . "'," .
                                      "'testlevel'," .
                                      "'" . $testlevelID . "'," .
                                      "'teststand'," .
                                      "'" . $teststandID . "'," .
                                      "'" . $logdirectory . "'," .
                                      "'" . $gp['scenariologfilename'] . "')";

   if (! do_mysql_query($query, $db_server, 1)) {
      return;
   } 
   
   $gp['action'] = 'show';
   $gp['object'] = 'currently_running';  
   
   show_active_workrequest($project, $phase);
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   environment
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function environment($gp) {

   global $db_server;

   if ($gp['object'] !== "environment") {
      add_error_message("DAFE0001: environment: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "environment";
   }
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         if (count_rows("daf.environment", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0047: Cannot create environment " . $gp['name'] . " because an environment of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.environment (Name, Description) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['description'] . "')";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';
    
      } else if ($gp['operation'] === "modify") {
 
        if ((count_rows("daf.environment", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0048: Cannot modify environment " . $gp['name'] . " because an environment of that name already exists");
        
         } else {
   
            $query = "UPDATE daf.environment SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . 
                                       "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }
         
         $gp['action'] = 'show';
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "delete") {
      
        if (count_rows("daf.action", "EnvironmentID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0049: Cannot delete that environment because it is still used in one or more actions in scenarios");
         
            $gp['action'] = 'show';
            $gp['ID'] = NULL;
            
        } else if (count_rows("daf.environmentvalue", "EnvironmentID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0050: Cannot delete that environment because it still has one or more environment values set");
         
            $gp['action'] = 'show';
            $gp['ID'] = NULL;
   
        } else {
   
            $query = "DELETE FROM daf.environment where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            $gp['action'] = 'show';
            $gp['ID'] = NULL;
        }
         
      } else if ($gp['operation'] === "createenvironmentvalue") {
   
        if (count_rows("daf.environmentvalue", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0051: Cannot create environment value " . $gp['name'] . " because an create value of that name already exists");
        
        } else {
        
            $query = "INSERT INTO daf.environmentvalue (Name, Value, EnvironmentID) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['value'] .  "','" . $gp['environmentID'] . "')";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
    
      } else if ($gp['operation'] === "modifyenvironmentvalue") {
   
        if ((count_rows("daf.environmentvalue", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0053: Cannot modify environment value " . $gp['name'] . " because an environment value of that name already exists");
        
         } else {
         
            $query = "UPDATE daf.environmentvalue SET Name='" . $gp['name'] . "', Value='" . $gp['value'] . 
                     "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "deleteenvironmentvalue") {
   
         $query = "DELETE FROM daf.environmentvalue where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
         
      }
      
   }
    
   if ($gp['action'] === "create") {
   
      create_environment();
   
   } else if ($gp['action'] === "modify") {
   
      modify_environment($gp['ID']);
      
   } else if ($gp['action'] === "createenvironmentvalue") {
   
      create_environmentvalue($gp['environmentID']);
   
   } else if ($gp['action'] === "modifyenvironmentvalue") {
   
      modify_environmentvalue($gp['ID']);
     
   } else if ($gp['action'] === "showscenarios") {
      
      show_scenarios_using_this_environment($gp['environmentID'], $gp['environmentname']);
   
   } else if ($gp['action'] === "show") {
   
      if (isset($gp['ID'])) {
         
         show_environment($gp['ID']);
        
      } else {
            
         show_all_environments();
         
      }
   
   } else {
    
      report_bad_parms("environment");
   
   }   
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_environments
//
// Inputs:     
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_environments() {
    
   $query = "SELECT * FROM daf.environment";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Environments</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "create_environment", "Create Environment", "index.php?action=create&object=environment");
   echo "</th>\n";

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
          echo "<td class=td_smd>";
          cellbutton(null, "environment_show_$j", "show", "index.php?action=show&object=environment&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          cellbutton(null, "environment_modify_$j", "modify", "index.php?action=modify&object=environment&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "environment_delete_$j", "delete", "index.php?action=show&operation=delete&object=environment&ID=$ID");
          button_with_confirm("environment_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=environment&ID=$ID", 
                              "index.php?action=show&object=environment");
          echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          echo '<td>'  . $row[$i] . '</td>';
       }   
       echo '</tr>';
       echo "\n";

   }
   echo '</table>';
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_environment
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function create_environment() {

   echo '<div dojoType="dijit.form.Form" id="create_environment" jsId="create_environment" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Environment</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size = "' . DAF_ENVIRONMENT_NAME_LEN . '" maxlength="' . DAF_ENVIRONMENT_NAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:' . DAF_ENVIRONMENT_NAME_LEN . 'em" regExp=".{1,' . DAF_ENVIRONMENT_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter an Environment Name" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENT_NAME_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th><td><input type="text" name="description" size = "' . DAF_ENVIRONMENT_DESCRIPTION_LEN . '" maxlength="' . DAF_ENVIRONMENT_DESCRIPTION_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_ENVIRONMENT_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Description" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENT_DESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";   
   echo "</table>\n";

   cellbutton("div", "environment_create", "Create Environment", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"environment\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   cellbutton(null, "environment_cancel", "Cancel", "index.php?object=environment&action=show", null);

   echo "</form>\n";
   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_environment
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function show_environment($environmentID) {

   $query = "SELECT * FROM daf.environment where ID = '" . $environmentID . "'";
   $environment = mysql_query($query);

   if (! $environment) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($environment);   // should only be one row
   $num_fields = mysql_num_fields($environment);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="fullwidth">';
   echo "<caption class=\"cap1\"><div>Environment</div></caption>\n";
     
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($environment, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex    = $fieldindex['ID'];
   $Nameindex = $fieldindex['Name'];
   
   $environment_row = mysql_fetch_row($environment);
   /* $scenarioID = $scenario_row[$fieldindex['ID']]; */
   
   $environmentID   = $environment_row[$IDindex];
   $environmentname = $environment_row[$Nameindex];
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th class="fullwidth">' . mysql_field_name($environment, $i) . '</th>';
      echo '<td>' . $environment_row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   $query = "SELECT ID, Name, Value FROM environmentvalue WHERE environmentID = '" . $environmentID . "' ORDER by Name";
   $environmentvalue = mysql_query($query);
   
   echo '<table class="fullwidth">';

   $num_environmentvaluerows   = mysql_num_rows($environmentvalue);
   $num_environmentvaluefields = mysql_num_fields($environmentvalue);
       
   echo '<tr>';
   // echo '<th width="5%"> &nbsp; </th><th width="5%"> &nbsp; </th>';
   echo '<th colspan=2>';
   cellbutton(null, "environment_addvar", "Add Environment Variable", "index.php?action=createenvironmentvalue&object=environment&environmentID=$environmentID");
   echo '</th>';
   echo '<th width="5%">ID</th>';
   echo '<th width="25%">Environment Variable Name</th>';
   echo '<th width="60%">Environment Variable Value</th>';
   echo "</tr>\n";
         
   for ($i = 0; $i < $num_environmentvaluerows; $i++) {
   
      echo '<tr>'; 
      
      $row = mysql_fetch_row($environmentvalue);
      $environmentvalueID = $row[0];
    
      echo "<td class=td_smd>";
      cellbutton(null, "environment_modify_$i", "modify", "index.php?action=modifyenvironmentvalue&object=environment&ID=$environmentvalueID");
      echo "</td>\n";
      echo "<td class=td_smd>";
      // cellbutton(null, "environment_delete_$i", "delete", "index.php?action=show&operation=deleteenvironmentvalue&object=environment&ID=$environmentvalueID");
      button_with_confirm("environment_delete_$i", "delete", "delete", "index.php?action=show&operation=deleteenvironmentvalue&object=environment&ID=$environmentvalueID", 
                              "index.php?action=show&object=environment");
      echo "</td>\n";
      
      for ($j = 0; $j < $num_environmentvaluefields; $j++) {
         echo '<td>'  . $row[$j] . '</td>';
      }
      
      echo '</tr>' . "\n";
      
   }

   echo '</table>';
   
   echo '<form action="index.php" method="post">' . "\n";
   cellbutton("div", "environment_showscen", "Show scenarios that use this environment", null, "submit");
   echo '<input type="hidden" name="action" value="showscenarios" />' . "\n";
   echo '<input type="hidden" name="object" value="environment" />' . "\n";
   echo '<input type="hidden" name="environmentID" value="' . $environmentID .'" />' . "\n";
   echo '<input type="hidden" name="environmentname" value="' . $environmentname .'" />' . "\n";  
   echo '</form>' . "\n";
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_environment
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function modify_environment($ID) {

   $query = "SELECT ID, Name, Description FROM daf.environment where ID = '" . $ID . "'";
   $environment = mysql_query($query);

   if (! $environment) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($environment);   // should only be one row
   $num_fields = mysql_num_fields($environment);
   
   echo '<div dojoType="dijit.form.Form" id="modify_action" jsId="modify_action" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify Environment</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($environment, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
 
   $row = mysql_fetch_row($environment);

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($environment, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size = "' . DAF_ENVIRONMENT_NAME_LEN . '" maxlength="' . DAF_ENVIRONMENT_NAME_LEN . '" ' . 
              'dojoType="dijit.form.ValidationTextBox" style="width:' . DAF_ENVIRONMENT_NAME_LEN . 'em" regExp=".{1,' . DAF_ENVIRONMENT_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter an Environment Name" ' . 
              'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENT_NAME_LEN . ' characters)"/></td>';
      } else if ($fieldname == "Description") {
         echo '<tr><th>Description</th><td><input type="text" name="description" value="' . $row[$i] . '" size = "' . DAF_ENVIRONMENT_DESCRIPTION_LEN . '" maxlength="' . DAF_ENVIRONMENT_DESCRIPTION_LEN . '" ' . 
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_ENVIRONMENT_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Description" ' . 
              'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENT_DESCRIPTION_LEN . ' characters)"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
  
   cellbutton("div", "environment_modify", "Modify Environment", null, "submit");
   echo "<input type=\"hidden\"  name=\"action\"    value=\"show\"/>\n";
   echo "<input type=\"hidden\"  name=\"object\"    value=\"environment\"/>\n";
   echo "<input type=\"hidden\"  name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\"  name=\"ID\"        value=\"$ID\" />\n";
   echo "<input type=\"hidden\"  name=\"originalname\" value=\"$originalname\" />\n";
   cellbutton(null, "environment_cancel", "Cancel", "index.php?object=environment&action=show&ID=$ID", null);

   echo "</div\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_environmentvalue
//
// Inputs:     $environmentID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function create_environmentvalue($environmentID) {

   echo '<div dojoType="dijit.form.Form" id="create_environmentvalue" jsId="create_environmentvalue" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Environment Variable</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size = "' . DAF_ENVIRONMENTVALUE_NAME_LEN . '" maxlength="' . DAF_ENVIRONMENTVALUE_NAME_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ENVIRONMENTVALUE_NAME_LEN . 'em;" regExp=".{1,' . DAF_ENVIRONMENTVALUE_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter an Environment Value Name" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENTVALUE_NAME_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Value</th>';
   echo '<td><input type="text" name="value" size = "' . DAF_ENVIRONMENTVALUE_VALUE_LEN . '" maxlength="' . DAF_ENVIRONMENTVALUE_VALUE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ENVIRONMENTVALUE_VALUE_LEN . 'em;" regExp=".{1,' . DAF_ENVIRONMENTVALUE_VALUE_LEN . '}" required="true" trim="true" promptMessage="Enter an Environment Value" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENTVALUE_VALUE_LEN . ' characters)"/>';
   echo "</td></tr>\n"; 
   echo "<table>\n";

   cellbutton("div", "environment_createvar", "Create Environment Variable", null, "submit");
   echo "<input type=\"hidden\" name=\"object\" value=\"environment\" />\n";
   echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"createenvironmentvalue\" />\n";
   echo "<input type=\"hidden\" name=\"environmentID\" value=\"$environmentID\" />\n";
   cellbutton(null, "environment_createvar_cancel", "Cancel", "index.php?object=environment&action=show", null);
   
   echo "</form>\n";
   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_environmentvalue
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function modify_environmentvalue($ID) {

   $query = "SELECT Name, Value, EnvironmentID FROM daf.environmentvalue where ID = '" . $ID . "'";
   $environmentvalue = mysql_query($query);

   if (! $environmentvalue) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($environmentvalue);   // should only be one row
   $num_fields = mysql_num_fields($environmentvalue);
   
   $row = mysql_fetch_row($environmentvalue);

   $name     = $row[0];
   $value    = $row[1]; 
   $originalname = $name;

   echo <<<_END
   echo '<div dojoType="dijit.form.Form" id="modify_environmentvalue" jsId="modify_environmentvalue" encType="multipart/form-data" action="index.php" method="post">';
      <script type="dojo/method" event="onSubmit">
        if (! this.validate()) {
            alert('Form cannot be submitted as it contains one or more invalid fields');
            return false;
        }
        return true;
    </script>
_END;

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Modify Environment Variable</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" value="' . $name . '" size = "' . DAF_ENVIRONMENTVALUE_NAME_LEN . '" maxlength="' . DAF_ENVIRONMENTVALUE_NAME_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ENVIRONMENTVALUE_NAME_LEN . 'em;" regExp=".{1,' . DAF_ENVIRONMENTVALUE_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter an Environment Value Name" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENTVALUE_NAME_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Value</th>';
   echo '<td><input type="text" name="value" value="' . $value . '" size = "' . DAF_ENVIRONMENTVALUE_VALUE_LEN . '" maxlength="' . DAF_ENVIRONMENTVALUE_VALUE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ENVIRONMENTVALUE_VALUE_LEN . 'em;" regExp=".{1,' . DAF_ENVIRONMENTVALUE_VALUE_LEN . '}" required="true" trim="true" promptMessage="Enter an Environment Value" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENTVALUE_VALUE_LEN . ' characters)"/>';
   echo "</td></tr>\n"; 
   echo "<table>\n";

   cellbutton("div", "environment_modifyvar", "Modify Environment Variable", null, "submit");
   echo "<input type=\"hidden\"  name=\"action\"     value=\"show\"/>\n";
   echo "<input type=\"hidden\"  name=\"object\"     value=\"environment\"/>\n";
   echo "<input type=\"hidden\"  name=\"operation\"  value=\"modifyenvironmentvalue\"/>\n";
   echo "<input type=\"hidden\"  name=\"ID\"         value=\"$ID\" />\n";
   echo "<input type=\"hidden\"  name=\"originalname\" value=\"$originalname\" />\n";
   cellbutton(null, "environment_modifyvar_cancel", "Cancel", "index.php?object=environment&action=show&ID=$ID", null);

   echo "</div>\n";
   

}

function count_actions_in_scenario($scenarioID) {

   global $db_server;

   $query = "SELECT COUNT(*) FROM daf.action WHERE scenarioID = '$scenarioID'";
   
   $result = mysql_query($query, $db_server);
   
   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
              
   $num_rows = mysql_num_rows($result);   // should only be one row
   
   $row = mysql_fetch_row($result);
   
   return $row[0];
   
} 

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_scenarios_using_this_environment
//
// Inputs:     $environmentID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function show_scenarios_using_this_environment($environmentID, $environmentname) {

   $query = "SELECT DISTINCT scenario.ID, scenario.Name, action.Stepnumber FROM daf.action INNER JOIN scenario ON action.ScenarioID = scenario.ID " . 
            "WHERE action.EnvironmentID = '" . $environmentID . "' ORDER BY scenario.Name, action.Stepnumber";
   $testcase = mysql_query($query);

   if (! $testcase) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testcase);   // should only be one row
   $num_fields = mysql_num_fields($testcase);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenarios using environment $environmentname</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testcase, $i);
      echo '<th>' . $fieldname . '</th>';
      $fieldindex[$fieldname] = $i;      
   }
   echo "</tr>\n";
   
   $idindex   = $fieldindex['ID'];
   $nameindex = $fieldindex['Name'];
   
   for ($i= 0; $i < $num_rows; $i++) {
   
      $testcase_row = mysql_fetch_row($testcase);
      echo '<tr>';
      for ($j = 0; $j < $num_fields; $j++) { 
         if ($j == $nameindex) {
            echo "<td class=td_smd><a href=index.php?action=show&object=scenario&ID=$testcase_row[$idindex]>$testcase_row[$j]</a></td>";  
         } else { 
            echo '<td>'  . $testcase_row[$j] . '</td>';
         }
      } 
      echo "</tr>\n";
      
   }
   
   echo "</tr>\n";
   echo '</table>'; 
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   hostselectorvalue
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function hostselectorvalue($gp) {

   global $db_server;
   
   if ($gp['object'] !== "hostselectorvalue") {
      add_error_message("DAFE0001: hostselectorvalue: internal error - bad object - " . $gp['object']);
      $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "hostselectorvalue";
   }
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         $query = "INSERT INTO daf.hostselectorvalue(Value) VALUES('" . $gp['value'] . "')";              
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
    
      } else if ($gp['operation'] === "modify") {
   
         $query = "UPDATE daf.hostselectorvalue SET Value='" . $gp['value'] .  
                                    "' WHERE ID='" . $gp['ID'] . "'";              
         if (! do_mysql_query($query, $db_server, 1)) {
            return;
         }    
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "delete") {
      
        if (count_rows("daf.host", "Hostselectorvalue = '" . $gp['hostselectorvalue'] . "' AND Teststandprimary = '0'") > 0) {
         
            add_error_message("DAFE0054: Cannot delete that Host Selector Value because it is still used in one or more host definitions");
         
            $gp['action'] = 'show';
            $gp['ID'] = NULL;
            
        } else {
   
            $query = "DELETE FROM daf.hostselectorvalue where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            $gp['action'] = 'show';
            $gp['ID'] = NULL;
        }
      }
   }
    
   if ($gp['action'] === "create") {
   
      create_hostselectorvalue();
   
   } else if ($gp['action'] === "modify") {
   
      modify_hostselectorvalue($gp['ID']);
      
   } else if ($gp['action'] === "show") {
   
      show_all_hostselectorvalues();
   
   } else {
    
      report_bad_parms("hostselectorvalue");
   
   }   
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_hostselectorvalues
//
// Inputs:     
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_hostselectorvalues() {
    
   $query = "SELECT * FROM daf.hostselectorvalue";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Host Selector Values</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=2>";
   cellbutton(null, "create_hostselectorvalue", "Create New Host Selector Value", "index.php?action=create&object=hostselectorvalue");
   echo "</th>\n";

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
       $hostselectorvalue = $row[$fieldindex['Value']];
          echo "<td class=td_smd>";
          cellbutton(null, "hostselectorvalue_modify_$j", "modify", "index.php?action=modify&object=hostselectorvalue&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "hostselectorvalue_delete_$j", "delete", "index.php?action=show&operation=delete&object=hostselectorvalue&ID=$ID&hostselectorvalue=$hostselectorvalue");
          button_with_confirm("hostselectorvalue_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=hostselectorvalue&ID=$ID&hostselectorvalue=$hostselectorvalue", 
                              "index.php?action=show&object=hostselectorvalue");
          echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          echo '<td>'  . $row[$i] . '</td>';
       }   
       echo '</tr>';
       echo "\n";

   }
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_hostselectorvalue
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function create_hostselectorvalue() {

   echo '<div dojoType="dijit.form.Form" id="create_hostselectorvalue" jsId="create_hostselectorvalue" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Host Selector Value</div></caption>\n";
   
   echo '<tr><th>Value</th>';
   echo '<td><input type="text" name="value" size = "' . DAF_ENVIRONMENTVALUE_LEN . '" maxlength="' . DAF_ENVIRONMENTVALUE_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_ENVIRONMENTVALUE_LEN . 'em;" regExp=".{1,' . DAF_ENVIRONMENTVALUE_LEN . '}" required="true" trim="true" promptMessage="Enter a Host Selector Value" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_ENVIRONMENTVALUE_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";

   cellbutton("div", "hostselectorvalue_create", "Create Host Selector Value", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"hostselectorvalue\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   cellbutton(null, "hostselectorvalue_cancel", "Cancel", "index.php?object=hostselectorvalue&action=show", null);

   echo "</form>\n";
   echo "</div>\n";

}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_hostselectorvalue
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:    
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function modify_hostselectorvalue($ID) {

   $query = "SELECT ID, Value FROM daf.hostselectorvalue where ID = '" . $ID . "'";
   $hostselectorvalue = mysql_query($query);

   if (! $hostselectorvalue) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($hostselectorvalue);   // should only be one row
   $num_fields = mysql_num_fields($hostselectorvalue);
   
   echo '<div dojoType="dijit.form.Form" id="create_hostselectorvalue" jsId="create_hostselectorvalue" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify Host Selector Value</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($hostselectorvalue, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
 
   $row = mysql_fetch_row($hostselectorvalue);

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($hostselectorvalue, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Value") {
         echo '<th>Value</th>';
         echo '<td><input type="text" name="value" value="' . $row[$i] . '" size = "' . DAF_HOSTSELECTORVALUE_NAME_LEN . '" maxlength="' . DAF_HOSTSELECTORVALUE_NAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_HOSTSELECTORVALUE_NAME_LEN . 'em;" regExp=".{1,' . DAF_HOSTSELECTORVALUE_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Host Selector Value" ' . 
        'invalidMessage="Invalid Description (must be 1 to ' . DAF_HOSTSELECTORVALUE_NAME_LEN . ' characters)"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }

   echo '</table>';
  
   cellbutton("div", "hostselectorvalue_modify", "Modify Host Selector Value", null, "submit");
   echo "<input type=\"hidden\"  name=\"action\"    value=\"show\"/>\n";
   echo "<input type=\"hidden\"  name=\"object\"    value=\"hostselectorvalue\"/>\n";
   echo "<input type=\"hidden\"  name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\"  name=\"ID\"        value=\"$ID\" />\n";
   cellbutton(null, "hostselectorvalue_cancel", "Cancel", "index.php?object=hostselectorvalue&action=show&ID=$ID", null);

   echo '</div>';
}


?>
