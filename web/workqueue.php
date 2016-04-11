<?php 


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   workrequest
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

function workrequest($gp, $project, $phase) {

   global $db_server;

   if ($gp['object'] !== "workrequest") {
     add_error_message("DAFE0001: workrequest: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "workrequest";
   }
  
   if ($gp['action'] === "cancel") {
  
      $query = "UPDATE daf.workqueue SET Statemodifier = 'Tobecanceled', State = 'Pendingcompletion' where ScenarioresultID = '" . $gp['scenarioresultID'] . "'";
      do_mysql_query($query, $db_server, 1);

      $gp['action'] = 'show';
      $gp['ID'] = NULL;

      show_active_workrequest($project, $phase);

   } else if ($gp['action'] === "getjson") {
            
      query_active_workrequest($project, $phase);

   } else if ($gp['action'] === "show") {
            
      show_table("daf.workrequest", "Work Requests");

   } else {
    
      report_bad_parms("workrequest"); 
   
   }

}

// -----------------------------------------------------------------------------------------
//
// workqueue
//
// -----------------------------------------------------------------------------------------

function workqueue($gp, $project, $phase) {

   global $db_server;

   if ($gp['object'] !== "workqueue") {
      add_error_message("DAFE0001: workqueue: internal lerror - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "workqueue";
   }

   if ($gp['action'] === "cancel") {

      $query = "UPDATE daf.workqueue SET Statemodifier = 'Tobecanceled', State = 'Pendingcompletion' where ID = '" . $gp['ID'] . "'";              
      do_mysql_query($query, $db_server, 1);

  $x = $hhhh;

      $gp['action'] = 'show';
      $gp['ID'] = NULL;

      show_active_workqueue($project, $phase);

   } else if ($gp['action'] === "getjson") {

      query_active_workqueue($project, $phase);

   } else if ($gp['action'] === "show") {

      $gp['action'] = 'show';

   } else {

      report_bad_parms("workqueue");

   }

   show_table("daf.workqueue", "Work Queue");           /* <<<<<<<<<<<< need a specific table ? */

}


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

   <link rel="stylesheet" href="./dojo/dojox/grid/resources/Grid.css" media="screen">
   <link rel="stylesheet" href="./dojo/dojox/grid/resources/claroGrid.css" media="screen">

   <div style="width:100%; height:95%;">
   <div id="WhatIsRunningErrorStatus"></div>
   </div>
   <script type="text/javascript">
      dojo.require("dojo.parser");
      dojo.require("dojox.grid.DataGrid");
      dojo.require("dojo.data.ObjectStore");
      dojo.require("dojo.data.ItemFileWriteStore");
      var runningdata1 = {identifier: "ID", items: [{ID: 0, Scenario: "", "Test Level": "", "Scenario Result": "", "Action Type": "", "Action Result": "", "Step Number": "",
                         "Host": "", "Testcase": "", "State": "", "Pass": "", "Start": "", "End": "", "Maxduration": "", "Tag": "", "Log Location": ""}]};
      var objstore;
      
      runningdata1 = {identifier: "ID", items: []};

      dojo.ready(function() {

         objstore = dojo.data.ItemFileWriteStore({clearOnClose: true, data: runningdata1});
         
         xx(objstore);

         var grid = new dojox.grid.DataGrid({    // use the adapter to give the grid a Dojo Data compliant interface to the store    
		                    store: objstore,
		                    escapeHTMLInData: false,          /* <<<<<<<< is this safe (cross site attacks) */
		                    columnReordering: true,
		                    structure: [ {name:"Cancel", field:"Cancelit", width:"6em"},
		                                 {name:"Elapsed Secs", field:"Elapsedsecs", width:"4em", styles:"text-align:center;"},
                                         {name:"ID", field:"ID", width:"4em", styles:"text-align:center;"},        
		                                 {name:"Scenario", field:"Scenario", width:"8em"},        
		                                 {name:"Test Level", field:"Test Level", width:"8em"},  
		                                 {name:"Scenario Result", field:"Scenario Result", width:"8em"},  
		                                 {name:"Action Type", field:"Action Type", width:"8em"},  
		                                 {name:"Action Result", field:"Action Result", width:"8em"},  
		                                 {name:"Step Number", field:"Stepnumber", width:"4em", styles:"text-align:center;"},  
		                                 {name:"Host", field:"Hostname", width:"8em"},  
		                                 {name:"Testcase", field:"Testcase", width:"8em"},  
		                                 {name:"State", field:"StateModifier", width:"8em"},
		                                 {name:"Pass", field:"Pass", width:"6em", styles:"text-align:center;"},
		                                 {name:"Start", field:"Start", width:"6em", styles:"text-align:center;"},
		                                 {name:"End", field:"End", width:"6em", styles:"text-align:center;"},
		                                 {name:"Maxduration", field:"Maxduration", width:"6em", styles:"text-align:center;"},
		                                 {name:"Tag", field:"Tag", width:"4em", styles:"text-align:center;"},
		                                 {name:"Log Location", field:"Loglocation", width:"8em"}
		                               ]}, 
		                    "WhatIsRunningErrorStatus");
		                    

		 grid.startup();
         setInterval("xx(objstore)", 1000);
         
      });
      
      function xx(objstore) {
      
         dojo.xhrGet({
            url: "index.php?object=workqueue&action=getjson",
            load: function(runningdata, ioArgs) {
		       // copy the running data into the object store
		       
		       function saveDone(){alert("Done saving.");}
               function saveFailed(){alert("Save failed.");}
               
               var gotNames= function(items, request){
                  for (var i = 0; i < items.length; i++) {
                     console.log(items[i]);
                     objstore.deleteItem(items[i]);
                  }
                  //objstore.save({onComplete: saveDone, onError: saveFailed});
                  objstore.save();
               }
               
               var request = objstore.fetch({query: {ID:"*"}, queryOptions: {ignoreCase: true}, onComplete: gotNames});
		       
		       for (var i=0; i<runningdata.length; i++) {
                  console.log(runningdata[i]);
		          objstore.newItem(runningdata[i]);
		          console.log(objstore);

		       } 
            },
            error: function(errorMessage, ioArgs) {
		       //Look up the node we'll stick the error text under.
               var targetNode = dojo.byId("WhatIsRunningErrorStatus");
		       var message = "";
               switch (ioArgs.xhr.status) {
                  case 404:
                     // message = "The requested page was not found"; 
                     // this means there were no workqueue items 
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
               targetNode.innerHTML = ioArgs.xhr.status + " " + message + " " + errorMessage;
               console.log(ioArgs);
               
               console.log(ioArgs.xhr.status + " " + message + " " + errorMessage);  /* <<<<<<<<< do we need this alert */
            },
		    handleAs: 'json'
		 });
	  }  
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

   $fieldnames = array("ID", "Scenario", "Test Level", "Scenario Result", "Action Result", "Action Type", "Stepnumber", "Hostname", "State", "StateModifier", "Testcase", "Invocation",  
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

      echo '<table class="tab1">';
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
          echo "<td class=td_smd><a href=index.php?action=cancel&object=workqueue&ID=$ID>cancel</a></td>";
           
          for ($i = 0; $i < $num_fields; $i++) {       
             if ($i == $scenarioindex) {
                if (isset($lookupscenario[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=scenario&ID=' . $row[$i] . '">' . 
                        $lookupscenario[$row[$i]] . '</a></td>';
                } else {
                   echo '<td> $row[$i] </td>';
                }
             } else if ($i == $scenarioresultindex) {
                if (isset($lookupscenarioresult[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=scenarioresult&ID=' . $row[$i] . '">' . 
                        $lookupscenarioresult[$row[$i]] . '</a></td>';
                } else {
                   echo '<td> $row[$i] </td>';
                }
             } else if ($i == $testlevelindex) {
                if (isset($lookuptestlevel[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=testlevel&ID=' . $row[$i] . '">' . 
                        $lookuptestlevel[$row[$i]] . '</a></td>';
                } else {
                   echo '<td> $row[$i] </td>';
                }
             } else if ($i == $actionresultindex) {
                echo '<td><a href="index.php?action=show&object=actionresult&ID=' . $row[$i] . '">' . $row[$i] . '</a></td>';
             } else {
                echo '<td>'  . $row[$i] . '</td>';
             }
          }   
          echo '</tr>';
      }

      echo '</table>';
       
   } else {
   
       add_info_message("No scenarios are running at present");
       
   }
   
   echo <<<_END
   <form action="index.php" method="post">
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
// which case all the workqueue records are shown, or all the workqueue records in the specified project).
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
   //<div id="WhatIsRunningErrorStatus" style="height: 100px;"></div>  for reference
echo <<<_END

   <link rel="stylesheet" href="./dojo/dojox/grid/resources/Grid.css" media="screen">
   <link rel="stylesheet" href="./dojo/dojox/grid/resources/claroGrid.css" media="screen">

   <div style="width:100%; height:95%;">
   <div id="WhatIsRunningErrorStatus"></div>
   </div>
   <script type="text/javascript">
      dojo.require("dojo.parser");
      dojo.require("dojox.grid.DataGrid");
      dojo.require("dojo.data.ObjectStore");
      dojo.require("dojo.data.ItemFileWriteStore");

      var runningdata1 = {identifier: "ID", items: [{ID: 0, Project: "", "Phase": "", "Requesttype": "", "Tester": "", "State": "", "Tablename1": "",
                         "ObjectID1": "", "Tablename2": "", "ObjectID2": "", "Tablename3": "", "ObjectID3": "", "Tablename4": "", "ObjectID4": "", 
                         "Start": "", "End": "", "Logdirectory": "", "Scenariologfilename": ""}]};
      var objstore;
      
      runningdata1 = {identifier: "ID", items: []};

      dojo.ready(function() {

         objstore = dojo.data.ItemFileWriteStore({clearOnClose: true, data: runningdata1});
         
         xx(objstore);

         var grid = new dojox.grid.DataGrid({    // use the adapter to give the grid a Dojo Data compliant interface to the store    
		                    store: objstore,
		                    escapeHTMLInData: false,          /* <<<<<<<< is this safe (cross site attacks) */
		                    columnReordering: true, 
		                    structure: [ {name:"Cancel", field:"Cancelit", width:"8em"},
		                                 {name:"ID", field:"ID", width:"6em", styles:"text-align:center;"},        
		                                 {name:"Project", field:"Project", width:"8em"},        
		                                 {name:"Phase", field:"Phase", width:"8em"},  
		                                 {name:"Request Type", field:"Requesttype", width:"8em"},  
		                                 {name:"Tester", field:"Tester", width:"8em"},  
		                                 {name:"State", field:"State", width:"8em"},  
		                                 {name:"Tablename1", field:"Tablename1", width:"8em"},  
		                                 {name:"ObjectID1", field:"ObjectID1", width:"6em", styles:"text-align:center;"},  
		                                 {name:"Tablename2", field:"Tablename2", width:"8em"},  
		                                 {name:"ObjectID2", field:"ObjectID2", width:"6em", styles:"text-align:center;"},
		                                 {name:"Tablename3", field:"Tablename3", width:"8em"},
		                                 {name:"ObjectID3", field:"ObjectID3", width:"6em", styles:"text-align:center;"},
		                                 {name:"Tablename4", field:"Tablename4", width:"8em"},
		                                 {name:"ObjectID4", field:"ObjectID4", width:"6em", styles:"text-align:center;"},
				                         {name:"Description1", field:"Description1", width:"8em"},
		                                 {name:"Description1Type", field:"Description1Type", width:"6em", styles:"text-align:center;"},
		                                 {name:"Start", field:"Start", width:"6em", styles:"text-align:center;"},
		                                 {name:"End", field:"End", width:"6em", styles:"text-align:center;"},
		                                 {name:"Log directory", field:"Logdirectory", width:"8em"},
		                                 {name:"Scenario Log Filename", field:"Scenariologfilename", width:"8em"}
		                               ]}, 
		                    "WhatIsRunningErrorStatus");

		 grid.startup();

         setInterval("xx(objstore)", 1000);
      });
      
      function xx(objstore) {
      
         dojo.xhrGet({
            url: "index.php?object=workrequest&action=getjson",
            load: function(runningdata, ioArgs) {
		       // copy the running data into the object store
               
               var gotNames= function(items, request){
                  for (var i = 0; i < items.length; i++) {
                     console.log(items[i]);
                     objstore.deleteItem(items[i]);
                  }
                  objstore.save();
               }
               
               var request = objstore.fetch({query: {ID:"*"}, queryOptions: {ignoreCase: true}, onComplete: gotNames});
               
		       for (var i=0; i<runningdata.length; i++) {
                  console.log(runningdata[i]);
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
                     // message = "The requested page was not found"; 
                     // this means there were no workrequest items 
                     message = "There are no active workrequest items at present";
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
               targetNode.innerHTML = message + " " + errorMessage;
           },
		    handleAs: 'json'
		 });

	  }  
   </script>
_END;
    
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_active_workrequest1
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

function show_active_workrequest1($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }

   $fieldnames = array("ID", "Project", "Phase", "Requesttype", "TesterID", "State", "Tablename1", "ObjectID1", 
   		               "Tablename2", "ObjectID2", "Tablename3", "ObjectID3",   
                       "Tablename4", "ObjectID4", "Description1", "Description1Type",
   		               "Start", "End", "Logdirectory", "Scenariologfilename");
   $query = "SELECT ID, Project, Phase, Requesttype, TesterID, State, Tablename1, ObjectID1, Tablename2, ObjectID2, Tablename3, ObjectID3, Tablename4, ObjectID4, " .
            "Description1, Description1Type, Start, End, Logdirectory, Scenariologfilename FROM daf.workrequest "  . $where . " ORDER by Project, Phase";
   $result = mysql_query($query);
    
   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $lookupscenario       = lookuptable("daf.scenario", "ID", "Name");  
   $lookupscenarioresult = lookuptable($tablename, "ID", "Jobname");     /* <<<<<<<<<<<< different phases ??? */
   $lookuptestlevel      = lookuptable("daf.testlevel", "ID", "Name");
   $lookupteststand      = lookuptable("daf.teststand", "ID", "Name"); 
   $lookupusername       = lookuptableforusernames("daf.user", "ID");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   if ($num_rows > 0) {

      echo '<table class="tab1">';
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
          echo "<td class=td_smd><a href=index.php?action=cancel&object=workrequest&ID=$ID&scenarioresultID=$scenarioresultID>cancel</a></td>";
           
          for ($i = 0; $i < $num_fields; $i++) {
          
             if ($i == $testerIDindex) {
                if (isset($lookupusername[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=user&ID=' . $row[$i] . '">' . 
                        $lookupusername[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $scenarioIDindex) {
                if (isset($lookupscenario[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=scenario&ID=' . $row[$i] . '">' . 
                        $lookupscenario[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $scenarioresultIDindex) {
                if (isset($lookupscenarioresult[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=scenarioresult&ID=' . $row[$i] . '">' . 
                        $lookupscenarioresult[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $testlevelIDindex) {
                if (isset($lookuptestlevel[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=testlevel&ID=' . $row[$i] . '">' . 
                        $lookuptestlevel[$row[$i]] . '</a></td>';
                } else {
                   echo '<td>' .  $row[$i] . '</td>';
                }
             } else if ($i == $teststandIDindex) {
                if (isset($lookupteststand[$row[$i]])) {
                   echo '<td><a href="index.php?action=show&object=teststand&ID=' . $row[$i] . '">' . 
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
   
       add_info_message("No scenarios are running at present");
       
   }
   
   echo <<<_END
   <form action="index.php" method="post">
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

   $fieldnames = array("ID", "Scenario", "Test Level", "Scenario Result", "Action Result", "Action Type", "Stepnumber", "Hostname", "State", "StateModifier", "Testcase", "Invocation",  
                       "Pass", "Start", "End", "Maxduration", "Tag", "Loglocation");
   $query = "SELECT ID, ScenarioID, TestlevelID, ScenarioresultID, ActionResultID, Actiontype, Stepnumber, Hostname, State, StateModifier, Testcase, Invocation, Pass," . 
                      " Start, End, Maxduration, Tag, Loglocation FROM daf.workqueue " . $where . " ORDER by ScenarioID";
   $result = mysql_query($query);
    
   if (! $result)
      die ("Database access failed for query=$query: " . mysql_error());
   
   $lookupscenario       = lookuptable("daf.scenario", "ID", "Name");    // <<<<<<<<  this could be done using JOIN  more efficiently
   $lookupscenarioresult = lookuptable($tablename, "ID", "Jobname");    // <<<<<<<<  this could be done using JOIN  more efficiently
   $lookuptestlevel      = lookuptable("daf.testlevel", "ID", "Name");  

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   echo "[ ";

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
      $startindex          = $fieldindex['Start'];

      for ($j = 0; $j < $num_rows; $j++) {

          if ($j != 0) {
             echo ',';
          }
     
          echo "{";

          $row = mysql_fetch_row($result);
          $ID = $row[$IDindex];
          $start = $row[$startindex];
          date_default_timezone_set('UTC');               /* <<<<<<<<< */
          $elapsedsecs = strtotime("now") - strtotime($start);
          
          $cancelhtml = "<a href=index.php?action=cancel&object=workqueue&ID=$ID>cancel</a>";
          echo 'Cancelit: "' . $cancelhtml . '", ';
          echo 'Elapsedsecs: "' . $elapsedsecs . '"';         
           
          for ($i = 0; $i < $num_fields; $i++) { 
          
             echo ', ';
     
             if ($i == $scenarioindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupscenario[$row[$i]])) { 
                   // echo '"' . $lookupscenario[$row[$i]] . '"';
                   echo '"<a href=index.php?action=show&object=scenario&ID=' . $row[$i] . '>' . $lookupscenario[$row[$i]] . '</a>"';
                } else {
                   echo "\" \"";
                }
             } else if ($i == $scenarioresultindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupscenarioresult[$row[$i]])) {
                   // echo '"' . $lookupscenarioresult[$row[$i]] . '"';
                   echo '"<a href=index.php?action=show&object=scenarioresult&ID=' . $row[$i] . '>' . $lookupscenarioresult[$row[$i]] . '</a>"';
                } else {
                   echo "\" \"";
                }
             } else if ($i == $testlevelindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookuptestlevel[$row[$i]])) {
                   // echo '"' . $lookuptestlevel[$row[$i]] . '"';
                   echo '"<a href=index.php?action=show&object=testlevel&ID=' . $row[$i] . '>' . $lookuptestlevel[$row[$i]] . '</a>"';
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
   
   echo ']';

   exit;  /* ???<<<<<<<< should we use exit here */
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_active_workrequest
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
// Displays a JSON array object for the active workrequest records in the specified project and phase.  (Project or phase defaults to null in 
// which case all the workrequest records are shown, or all the workrequest records in the specified project).
//
//---------------------------------------------------------------------------------------------------------------------

function query_active_workrequest($current_project = NULL, $current_phase = NULL) {

   $status_db = determine_status_database_name($current_project, $current_phase);
   $tablename = determine_status_table_name($status_db, "scenarioresult");
   
   $where = '';
   if (isset($project) && isset($phase)) {
      $where = "WHERE Project='" . $project . "' AND Phase = '" . $phase . "'";
   } else if (isset($project)) {
      $where = "WHERE Project='" . $project . "'";
   }

   $fieldnames = array("ID", "Project", "Phase", "Requesttype", "Tester", "State", "Tablename1", "ObjectID1", 
   		               "Tablename2", "ObjectID2", "Tablename3", "ObjectID3", "Tablename4", "ObjectID4", 
   		               "Description1", "Description1Type", "Start", "End", "Logdirectory", "Scenariologfilename");
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
   
   echo "[ ";
   
   if ($num_rows > 0) {

      $fieldindex = array();
         for ($i = 0; $i < $num_fields; $i++) {
         $fieldindex[$fieldnames[$i]] = $i;
      }
    
      $IDindex               = $fieldindex['ID'];
      $testerIDindex         = $fieldindex['Tester'];
      $scenarioIDindex       = $fieldindex['ObjectID1'];
      $scenarioresultIDindex = $fieldindex['ObjectID2'];
      $testlevelIDindex      = $fieldindex['ObjectID3'];
      $teststandIDindex      = $fieldindex['ObjectID4'];

      for ($j = 0; $j < $num_rows; $j++) {
      
          if ($j != 0) {
             echo ',';
          }
     
          echo "{";

          $row = mysql_fetch_row($result);
          $ID = $row[$IDindex];
          $scenarioresultID= $row[$scenarioresultIDindex];
          
          $cancelhtml = "<a href=index.php?action=cancel&object=workrequest&ID=$ID&scenarioresultID=$scenarioresultID>cancel</a>";
          echo 'Cancelit: "' . $cancelhtml . '"';
          
          for ($i = 0; $i < $num_fields; $i++) {
          
             echo ", ";
          
             if ($i == $testerIDindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupusername[$row[$i]])) {
                   echo '"' . $lookupusername[$row[$i]] . '"';
                } else {
                   echo '"' . $row[$i] . '"';
                }
             } else if ($i == $scenarioIDindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupscenario[$row[$i]])) {
                   echo '"' . $lookupscenario[$row[$i]] . '"';
                } else {
                   echo '"' . $row[$i] . '"';
                }
             } else if ($i == $scenarioresultIDindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupscenarioresult[$row[$i]])) {
                  echo '"' . $lookupscenarioresult[$row[$i]] . '"';
                } else {
                   echo '"' . $row[$i] . '"';
                }
             } else if ($i == $testlevelIDindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookuptestlevel[$row[$i]])) {
                  echo '"' . $lookuptestlevel[$row[$i]] . '"';
                } else {
                   echo '"' . $row[$i] . '"';
                }
             } else if ($i == $teststandIDindex) {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                if (isset($lookupteststand[$row[$i]])) {
                   echo '"' . $lookupteststand[$row[$i]] . '"';
                } else {
                   echo '"' . $row[$i] . '"';
                }
             } else {
                echo '"' . $fieldnames[$i] . '"' . ': ';
                echo '"' .  $row[$i] . '"';
             }
          }   
          echo '}';

      }
       
   } 
   
   echo ']';

   exit;  /* ???<<<<<<<< should we use exit here */
   
}

?>
