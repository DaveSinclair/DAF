<?php 

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_active_workqueue
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
// Displays the active workqueue records in the specified project and phase.  (Project or phase defaults to null in 
// which case all the workqueue records are shown, or all the workqueue records in the specified project).
//
//---------------------------------------------------------------------------------------------------------------------

function show_active_workqueue($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }

echo <<<_END

   <link rel="stylesheet" href="http://ajax.googleapis.com/ajax/libs/dojo/1.6/dojox/grid/resources/claroGrid.css" media="screen">

   <div id="WhatIsRunningErrorStatus" style="height: 100px;"></div>
   <script type="text/javascript">
      dojo.require("dojo.parser");
      dojo.require("dijit.form.Button");
      dojo.require("dojox.grid.DataGrid");
      dojo.require("dojo.data.ObjectStore");
      dojo.require("dijit.form.DateTextBox");
      dojo.require("dojo.data.ItemFileWriteStore");
      var ixyz = 0;
      var dummy = 10;
      var runningdata = [ID: 0, Scenario: "", "Test Level": "", "Scenario Result": "", "Action Type": "", "Action Result": "", "Step Number": "",
                         "Host": "", "Testcase": "", "State": "", "Pass": "", "Start": "", "End": "", "Maxduration": "", "Tag": "", "Log Location": ""];
      var objstore;

      // 
      dojo.ready(function() {

         objstore = dojo.data.ItemFileWriteStore({clearOnClose: true, identifier: "ID", items: runningdata});

         var grid = new dojox.grid.DataGrid({    // use the adapter to give the grid a Dojo Data compliant interface to the store    
		                    store: objstore,
		                    structure: [ {name:"ID", field:"ID"},        
		                                 {name:"Scenario", field:"Scenario"},        
		                                 {name:"Test Level", field:"Test Level"},  
		                                 {name:"Scenario Result", field:"Scenario Result"},  
		                                 {name:"Action Type", field:"Action Type"},  
		                                 {name:"Action Result", field:"Action Result"},  
		                                 {name:"Step Number", field:"Stepnumber"},  
		                                 {name:"Host", field:"Hostname"},  
		                                 {name:"Testcase", field:"Testcase"},  
		                                 {name:"State", field:"StateModifier"},
		                                 {name:"Pass", field:"Pass"},
		                                 {name:"Start", field:"Start"},
		                                 {name:"End", field:"End"},
		                                 {name:"Maxduration", field:"Maxduration"},
		                                 {name:"Tag", field:"Tag"},
		                                 {name:"Log Location", field:"Loglocation"}
		                               ]}, 
		                    "WhatIsRunningErrorStatus");
		                    

		 grid.startup();

         //setInterval("yy(objstore)", 1000);
      });
      
    
      function xx(objstore) {
      
         dojo.xhrGet({
            url: "status.php?object=workqueue&action=getjson",
            load: function(runningdata, ioArgs) {
		       // copy the running data into the object store
		       
		       for (i=0; i<runningdata.length; i++) {
		       
		          console.log(dummy);
		          console.log(runningdata);
		          console.log(objstore);
		          
		          dojo.forEach(fetcheditems, function(item) {
		             if (item.ID == runningdata[i].ID) {
		                objstore.deleteItem(item);
		             }
		          
		          });

		          objstore.newItem(runningdata[i]);
		          console.log(objstore);

		       } 
            },
            error: function(errorMessage, ioArgs) {
		       //Look up the node we'll stick the text under.
               var targetNode = dojo.byId("WhatIsRunningErrorStatus");
		       var message = "";
               switch (ioArgs.xhr.status) {
                  case 404:
                     /* message = "The requested page was not found"; */
                     /* this means there were no workqueue items */
                     message = "There are no active workqueue items at present";
                     break;
                  case 500:
                     message = "The server reported an error.";
                     break;
                  case 407:
                     message = "You need to authenticate with a proxy.";
                     break;
                  default:
                     message = "Unknown error.";
               }
               targetNode.innerHTML = message + " " + errorMessage + " " + ixyz;
               ixyz++;
            },
		    handleAs: 'json'
		 });
	  }  */
   </script>
_END;
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_active_workqueue1
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
// Displays the active workqueue records in the specified project and phase.  (Project or phase defaults to null in 
// which case all the workqueue records are shown, or all the workqueue records in the specified project).
//
//---------------------------------------------------------------------------------------------------------------------

function show_active_workqueue1($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }

   $fieldnames = array("ID", "Scenario", "Test Level", "Scenario Result", "Action Result", "Action Type", "Stepnunber", "Hostname", "State", "StateModifier", "Testcase", "Invocation",  
                       "Pass", "Start", "End", "Maxduration", "Tag", "Loglocation");
   $query = "SELECT ID, ScenarioID, TestlevelID, ScenarioresultID, ActionResultID, Actiontype, Stepnumber, Hostname, State, StateModifier, Testcase, Invocation, Pass," . 
                      " Start, End, Maxduration, Tag, Loglocation FROM daf.workqueue " . $where . " ORDER by ScenarioID";
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookupscenario       = lookuptable("daf.scenario", "ID", "Name");    // <<<<<<<<  this could be done using JOIN  more efficiently
   $lookupscenarioresult = lookuptable($tablename, "ID", "Jobname");    // <<<<<<<<  this could be done using JOIN  more efficiently
   $lookuptestlevel      = lookuptable("daf.testlevel", "ID", "Name");  

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   if ($num_rows > 0) {

      echo '<table>';
      echo "<caption class=\"cap1\"><div>Currently active Workqueue Items</div></caption>";
      echo "\n";
  
      echo '<tr>';
      echo '<th> &nbsp; </th>';

      $fieldindex = array();
         for ($i = 0; $i < $num_fields; $i++) {
         echo '<th>' . $fieldnames[$i] . '</th>';
         $fieldindex[$fieldnames[$i]] = $i;
      }
      echo '</tr>'. "\n";
    
      $IDindex             = $fieldindex['ID'];
      $scenarioindex       = $fieldindex['Scenario'];
      $scenarioresultindex = $fieldindex['Scenario Result'];
      $actionresultindex   = $fieldindex['Action Result'];
      $testlevelindex      = $fieldindex['Test Level'];
 
      for ($j = 0; $j < $num_rows; $j++) {
          $row = mysql_fetch_row($result);
          echo '<tr>';
          $ID = $row[$IDindex];
          echo "<td class=td_smd><a href=status.php?action=cancel&object=workqueue&ID=$ID>cancel</a></td>";
           
          for ($i = 0; $i < $num_fields; $i++) {       
             if ($i == $scenarioindex) {
                if (isset($lookupscenario[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=scenario&ID=' . $row[$i] . '">' . 
                        $lookupscenario[$row[$i]] . '</a></td>';
                } else {
                   echo '<td> $row[$i] </td>';
                }
             } else if ($i == $scenarioresultindex) {
                if (isset($lookupscenarioresult[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=scenarioresult&ID=' . $row[$i] . '">' . 
                        $lookupscenarioresult[$row[$i]] . '</a></td>';
                } else {
                   echo '<td> $row[$i] </td>';
                }
             } else if ($i == $testlevelindex) {
                if (isset($lookuptestlevel[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=testlevel&ID=' . $row[$i] . '">' . 
                        $lookuptestlevel[$row[$i]] . '</a></td>';
                } else {
                   echo '<td> $row[$i] </td>';
                }
             } else if ($i == $actionresultindex) {
                echo '<td><a href="status.php?action=show&object=actionresult&ID=' . $row[$i] . '">' . $row[$i] . '</a></td>';
             } else {
                echo '<td>'  . $row[$i] . '</td>';
             }
          }   
          echo '</tr>';
      }

      echo '</table>';
       
   } else {
   
       echo "<p>No scenarios are running at present\n</p>";
       
   }
   
   echo <<<_END
   <form action="status.php" method="post">
   <input type="submit" name=button value="Refresh" />
   <input type="hidden" name=action value="show" />
   <input type="hidden" name=object value="active_workqueue" />
   </form>
_END;

   echo '<div style="float:left;" display="inline">';
   active_workrequests_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarioresults_button();
   echo '</div>'; 
  
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';    
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_active_workrequest
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
// Displays the active workrequest records in the specified project and phase.  (Project or phase defaults to null in 
// which case all the workrequest records are shown, or all the workrequest records in the specified project).
//
//---------------------------------------------------------------------------------------------------------------------

function show_active_workrequest($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }

   $fieldnames = array("ID", "Project", "Phase", "Requesttype", "TesterID", "State", "Tablename1", "ObjectID1", "Tablename2", "ObjectID2", "Tablename3", "ObjectID3",   
                       "Tablename4", "ObjectID4", "Start", "End", "Logdirectory", "Scenariologfilename");
   $query = "SELECT ID, Project, Phase, Requesttype, TesterID, State, Tablename1, ObjectID1, Tablename2, ObjectID2, Tablename3, ObjectID3, Tablename4, ObjectID4, " .
            "Start, End, Logdirectory, Scenariologfilename FROM daf.workrequest "  . $where . " ORDER by Project, Phase";
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookupscenario       = lookuptable("daf.scenario", "ID", "Name");  
   $lookupscenarioresult = lookuptable($tablename, "ID", "Jobname");     /* <<<<<<<<<<<< different phases ??? */
   $lookuptestlevel      = lookuptable("daf.testlevel", "ID", "Name");
   $lookupteststand      = lookuptable("daf.teststand", "ID", "Name"); 
   $lookupusername       = lookuptableforusernames("daf.user", "ID");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   if ($num_rows > 0) {

      echo '<table>';
      echo "<caption class=\"cap1\"><div>Currently active Work Request Items</div></caption>";
      echo "\n";
  
      echo '<tr>';
      echo '<th> &nbsp; </th>';

      $fieldindex = array();
         for ($i = 0; $i < $num_fields; $i++) {
         echo '<th>' . $fieldnames[$i] . '</th>';
         $fieldindex[$fieldnames[$i]] = $i;
      }
      echo '</tr>'. "\n";
    
      $IDindex               = $fieldindex['ID'];
      $testerIDindex         = $fieldindex['TesterID'];
      $scenarioIDindex       = $fieldindex['ObjectID1'];
      $scenarioresultIDindex = $fieldindex['ObjectID2'];
      $testlevelIDindex      = $fieldindex['ObjectID3'];
      $teststandIDindex      = $fieldindex['ObjectID4'];

      for ($j = 0; $j < $num_rows; $j++) {
          $row = mysql_fetch_row($result);
          echo '<tr>';
          $ID = $row[$IDindex];
          $scenarioresultID= $row[$scenarioresultIDindex];
          echo "<td class=td_smd><a href=status.php?action=cancel&object=workrequest&ID=$ID&scenarioresultID=$scenarioresultID>cancel</a></td>";
           
          for ($i = 0; $i < $num_fields; $i++) {
          
             if ($i == $testerIDindex) {
                if (isset($lookupusername[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=user&ID=' . $row[$i] . '">' . 
                        $lookupusername[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $scenarioIDindex) {
                if (isset($lookupscenario[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=scenario&ID=' . $row[$i] . '">' . 
                        $lookupscenario[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $scenarioresultIDindex) {
                if (isset($lookupscenarioresult[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=scenarioresult&ID=' . $row[$i] . '">' . 
                        $lookupscenarioresult[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $testlevelIDindex) {
                if (isset($lookuptestlevel[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=testlevel&ID=' . $row[$i] . '">' . 
                        $lookuptestlevel[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $teststandIDindex) {
                if (isset($lookupteststand[$row[$i]])) {
                   echo '<td><a href="status.php?action=show&object=teststand&ID=' . $row[$i] . '">' . 
                        $lookupteststand[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';                }
             } else {
                echo '<td>'  . $row[$i] . '</td>';
             }
          }   
          echo '</tr>';
      }

      echo '</table>';
       
   } else {
   
       echo "<p>No scenarios are running at present\n</p>";
       
   }
   
   echo <<<_END
   <form action="status.php" method="post">
   <input type="submit" name=button value="Refresh" />
   <input type="hidden" name=action value="show" />
   <input type="hidden" name=object value="active_workrequest" />
   </form>
_END;

   echo '<div style="float:left;" display="inline">';
   active_workqueues_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarioresults_button();
   echo '</div>'; 
  
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';    
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_active_workqueue
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
// Displays a JSON array object for the active workqueue records in the specified project and phase.  (Project or phase defaults to null in 
// which case all the workqueue records are shown, or all the workqueue records in the specified project).
//
//---------------------------------------------------------------------------------------------------------------------

function query_active_workqueue($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }

   $fieldnames = array("ID", "Scenario", "Test Level", "Scenario Result", "Action Result", "Action Type", "Stepnunber", "Hostname", "State", "StateModifier", "Testcase", "Invocation",  
                       "Pass", "Start", "End", "Maxduration", "Tag", "Loglocation");
   $query = "SELECT ID, ScenarioID, TestlevelID, ScenarioresultID, ActionResultID, Actiontype, Stepnumber, Hostname, State, StateModifier, Testcase, Invocation, Pass," . 
                      " Start, End, Maxduration, Tag, Loglocation FROM daf.workqueue " . $where . " ORDER by ScenarioID";
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookupscenario       = lookuptable("daf.scenario", "ID", "Name");    // <<<<<<<<  this could be done using JOIN  more efficiently
   $lookupscenarioresult = lookuptable($tablename, "ID", "Jobname");    // <<<<<<<<  this could be done using JOIN  more efficiently
   $lookuptestlevel      = lookuptable("daf.testlevel", "ID", "Name");  

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   echo "{ identifier: \"ID\",  label: \"Scenario\", [ ";
   
   if ($num_rows > 0) {

      $fieldindex = array();
         for ($i = 0; $i < $num_fields; $i++) {
         $fieldindex[$fieldnames[$i]] = $i;
      }
    
      $IDindex             = $fieldindex['ID'];
      $scenarioindex       = $fieldindex['Scenario'];
      $scenarioresultindex = $fieldindex['Scenario Result'];
      $actionresultindex   = $fieldindex['Action Result'];
      $testlevelindex      = $fieldindex['Test Level'];
 
      for ($j = 0; $j < $num_rows; $j++) {
  
         if ($j != 0) {
             echo ',';
          }
     
          echo "{";

          $row = mysql_fetch_row($result);
          $ID = $row[$IDindex];
           
          for ($i = 0; $i < $num_fields; $i++) {  
          
             if ($i != 0) {
                echo ',';
             }
     
             if ($i == $scenarioindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupscenario[$row[$i]])) { 
                   echo '"' . $lookupscenario[$row[$i]] . '"';
                } else {
                   echo "\" \"";
                }
             } else if ($i == $scenarioresultindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupscenarioresult[$row[$i]])) {
                   echo '"' . $lookupscenarioresult[$row[$i]] . '"';
                } else {
                   echo "\" \"";
                }
             } else if ($i == $testlevelindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookuptestlevel[$row[$i]])) {
                   echo '"' . $lookuptestlevel[$row[$i]] . '"';
                } else {
                   echo "\" \"";
                }
             } else {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                echo '"' . $row[$i] . '"';
             }
          }   
          echo '}';
      }

   }
   
   echo ']  }';
   
   exit;  /* ???<<<<<<<< should we use exit here */
   
}



  	/*		dojo.ready(function(){
				myStore = dojo.store.Cache(dojo.store.JsonRest({target:"rest/project/", idProperty:"ID"}), dojo.store.Memory());
				grid = new dojox.grid.DataGrid({
					store: dataStore = dojo.data.ObjectStore({objectStore: myStore}),
                                        rowsPerPage: 10,
                                        rowCount: 10,
					structure: [
						{name:"Name",     field:"Name",     width: "200px"},
						{name:"Description", field:"Description", width: "200px", editable: true}
					]
				}, "target-node-id"); // make sure you have a target HTML element with this id
				grid.startup();
				dojo.query("#save").onclick(function(){
					dataStore.save();
				});
			});   */

?>