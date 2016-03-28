<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  teststand.php
//
//  This file is part of the DAF user interface PHP code.  This file deals with actions relating to teststand objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                  Action to be carried out
//
//  object=teststand  action=show                                     display all teststands
//  object=teststand  action=show  ID=                                display an individual teststand
// 
//  object=teststand  action=show    operation=create                 insert a new teststand in the DAF teststand table
//                              name=... comments=....                with characteristics as specified by
//                              type=....  \                           the POST'd variables
//  object=teststand  action=show    operation=modify  ID=...         modify an existing teststand record in the 
//                              name=... comments=....                DAF teststand table
//                              type=....
//  object=teststand  action=show    operation=delete  ID=...         delete an existing record in the DAF teststand
//                                                                    table
//
//  object=teststand  action=create                                   present a form that allows the user to specify
//                                                                    the characteristics of a new teststand that is to be
//                                                                    added to the DAF teststand table
//  object=teststand  action=modify  ID=...                           present a form that allows the user to modify
//                                                                    the characteristics of an existing teststand in the
//                                                                    DAF teststand table
//



  //  object= teststand  action=show 
  //  object= teststand  action=show  ID= 
  
  //  object= teststand  action=show    operation=create            name=... comments=.... 
  //  object= teststand  action=show    operation=modify  ID=...    name=... comments=.... 
  //  object= teststand  action=show    operation=delete  ID=... 

  //  object= teststand  action=create 
  //  object= teststand  action=modify  ID=...
  
  
  //  object= host  action=show 
  //  object= host  action=show  ID= 
  
  //  object= host  action=show    operation=create            teststandid=... name=... type=... model=... serial=... hostselectorvalue=... teststandprimar=... comments=....
  //  object= host  action=show    operation=modify  ID=...    teststandid=... name=... type=... model=... serial=... hostselectorvalue=... teststandprimar=... comments=....
  //  object= host  action=show    operation=delete  ID=... 

  //  object= host  action=create 
  //  object= host  action=modify  ID=...

  //  object= collectortype  action=show 
  //  object= collectortype  action=show  ID= 
  
  //  object= collectorvalue  action=show 
  //  object= collectorvalue  action=show  ID= 
  
  
  //  object= collectorset  action=show 
  //  object= collectorset  action=show  ID= 
  
  //  object= collectorsetmember  action=show 

  
  //  object= collectorset  action=show    operation=create            name=... description=.... 
  //  object= collectorset  action=show    operation=modify  ID=...    name=... description=.... 
  //  object= collectorset  action=show    operation=delete  ID=... 
  
  //  object= collectorset  action=show    operation=createcollectorsetmember            collectorID=... requiredvalue=....  collectorsetID=... 
  //  object= collectorset  action=show    operation=modifycollectorsetmember  ID=...    collectorID=... requiredvalue=....  collectorsetID=... 
  //  object= collectorset  action=show    operation=deletecollectorsetmember  ID=... 

  //  object= collectorset  action=createcollectorsetmember            collectortypeID=... requiredvalue=....  collectorsetID=... 
  //  object= collectorset  action=modifycollectorsetmember  ID=...    collectortypeID=... requiredvalue=....  collectorsetID=... 
  //  object= collectorset  action=deletecollectorsetmember  ID=...    collectorsetID=...


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   teststand
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function teststand($gp) {

  global $db_server;

  if ($gp['object'] !== "teststand") {
     add_error_message("DAFE0001: teststand: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "teststand";
   }


   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {      
   
         if (count_rows("daf.teststand", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0081: Cannot create teststand " . $gp['name'] . " because a teststand of that name already exists");
         
         } else {
             
            $query = "INSERT INTO daf.teststand (Name, Comments) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['comments'] . "')";              
            do_mysql_query($query, $db_server, 1);
         
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         if ((count_rows("daf." . $gp['teststand'], "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0082: Cannot modify Test Stand name to " . $gp['name'] . " because an Test Stand with that name already exists");
      
         } else { 
         
            $query = "UPDATE daf.teststand SET Name='" . $gp['name'] . "', Comments='" . $gp['comments'] . 
                      "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
         }
         
         $gp['action'] = 'show';      
         $gp['ID'] = null;      

      } else if ($gp['operation'] === "delete") {
      
         if (count_rows("daf.host", "TeststandID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0083: Cannot delete that teststand because it is still contains one or more hosts");
         
         } else {

            $query = "DELETE FROM daf.teststand where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
         }   
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }


   if ($gp['action'] === "create") {
   
            add_error_message("action >>" . $gp['action'] . "<< operation >> " . $gp['operation'] . "<<");
       create_teststand();
    
   } else if ($gp['action'] === "modify") {

       modify_teststand($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_teststands();
 
      } else {
 
         show_detailed_teststand($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("teststand");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_teststands
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_teststands() {

   $query = "SELECT * FROM daf.teststand";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   list($offline_hosts, $online_hosts) = get_teststands_online_status();

   $num_rows  = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Teststands</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "teststand_create", "Create Teststand", "index.php?action=create&object=teststand");
   echo "</th>\n";
  
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<th>' . $fieldname . '</th>';
      $fieldindex[$fieldname] = $i;
   }
   echo '<th>Online hosts</th><th>Offline hosts</th>';
   echo '</tr>';
   echo "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID   = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']];
       echo "<td class=td_smd>\n";
       cellbutton(null, "teststand_show_$j", "show", "index.php?action=show&object=teststand&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>\n";
       cellbutton(null, "teststand_modify_$j", "modify", "index.php?action=modify&object=teststand&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>\n";
       // cellbutton(null, "teststand_delete_$j", "delete", "index.php?action=show&operation=delete&object=teststand&ID=$ID");
       button_with_confirm("teststand_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=teststand&ID=$ID", 
                           "index.php?action=show&object=teststand");
       echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          echo '<td>'  . $row[$i] . '</td>';
       }
       if (! isset($online_hosts[$name])) {
          echo '<td> &nbsp; </td>';
       } else {
          echo '<td>'  . $online_hosts[$name]  . '</td>';
       }
       if (! isset($offline_hosts[$name])) {
          echo '<td> &nbsp; </td>';
       } else {
          echo '<td>'  . $offline_hosts[$name]  . '</td>';
       }
       echo '</tr>';
       echo "\n";

   }
   echo '</table>' . "\n";
   
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_teststand
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//------------------------------------------------------------------------------------------------------------------

function create_teststand() {

   echo '<div dojoType="dijit.form.Form" id="create_teststand" jsId="create_teststand" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Test Stand</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_TESTSTAND_LEN . '" maxlength="' . DAF_TESTSTAND_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_TESTSTAND_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_TESTSTAND_LEN . '}" required="true" trim="true" promptMessage="Enter Teststand Name" ' . 
        'invalidMessage="Invalid Teststand name (must be 1 to ' . DAF_TESTSTAND_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Comments</th>';
   echo '<td><input type="text" name="comments" size="' . DAF_TESTSTAND_COMMENTS_LEN . '" maxlength="' . DAF_TESTSTAND_COMMENTS_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%;" regExp=".{1,' . DAF_TESTSTAND_COMMENTS_LEN . '}" required="true" trim="true" promptMessage="Enter Teststand Comments" ' . 
        'invalidMessage="Invalid Teststand name (must be 1 to ' . DAF_TESTSTAND_COMMENTS_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";
   
   cellbutton("div", "teststand_create", "Create Teststand", null, "submit");
   echo '<input type="hidden" name="action"    value="create" />' . "\n";
   echo '<input type="hidden" name="object"    value="teststand" />' . "\n";
   echo '<input type="hidden" name="operation" value="create" />' . "\n";
   cellbutton(null, "teststand_cancel", "Cancel", "index.php?object=teststand&action=show", null);

   echo "</div>\n";
    
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_teststand
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_detailed_teststand($ID) {

   $query = "SELECT * FROM daf.teststand where ID = '" . $ID . "'";
   $teststand = mysql_query($query);

   if (! $teststand) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($teststand);   // should only be one row
   $num_fields = mysql_num_fields($teststand);

   echo '<table class="fullwidth">';
   echo "<caption class=\"cap1\"><div>Teststand</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($teststand, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $teststand_row = mysql_fetch_row($teststand);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th class="fullwidth">' . mysql_field_name($teststand, $i) . '</th>';
      echo '<td>' . $teststand_row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   show_all_hosts($ID, "fullwidth");
   
  /* $query = "SELECT ID, Name, Type, Model, Serial, Hostselectorvalue, Teststandprimary, Comments, Agentstatus, Agentstatusdate FROM daf.host WHERE host.TeststandID = '" . $ID . "'";
            
   $testcase = mysql_query($query);

   if (! $testcase) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testcase);   // should only be one row
   $num_fields = mysql_num_fields($testcase);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Hosts</div></caption>";
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
            echo "<td class=td_smd><a href=index.php?action=show&object=host&ID=$testcase_row[$idindex]>$testcase_row[$j]</a></td>";  
         } else {       
            echo '<td>'  . $testcase_row[$j] . '</td>';
         }
      } 
      echo "</tr>\n";
      
   }

   echo '</table>';  */
   
   show_detailed_child_objects("teststand", $ID, 1);
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_child_objects
//
// Inputs:     $parentobjecttype
//             $ID
//             $recurse
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//  Displays tables of all the children of the parent objecttype.  Set $recurse to TRUE if you want to show
//  the children of the children etc
//
//---------------------------------------------------------------------------------------------------------------------

function show_detailed_child_objects($parentobjecttype, $ID, $recurse = NULL) {

   $childtypes = determine_child_object_types($parentobjecttype);
   $childIDs = array();
   
   foreach ($childtypes as $childtype) {
   
      $query = "SELECT * FROM daf." . $childtype . " where parentID = '" . $ID . "'";
      $result = mysql_query($query);

      if (! $result) 
         die ("Database access failed for query=$query: " . mysql_error());

      $num_rows   = mysql_num_rows($result);   
      $num_fields = mysql_num_fields($result);
      
      if ($num_rows > 0) {

         echo '<table class="fullwidth">';
         echo "<caption class=\"cap1\"><div>" . capitalise_and_pluralise_object($childtype) . "</div></caption>\n";
   
         $fieldindex = array();
         echo "<tr>";
         for ($i = 0; $i < $num_fields; $i++) {
            $fieldname = mysql_field_name($result, $i);
            $fieldindex[$fieldname] = $i;
            if ($fieldname == 'ID') {
              echo '<th width="5%">ID</th>';
            } else if ($fieldname == 'ParentID') {
               echo '<th width="10%">Parent</th>';
            } else if ($fieldname == 'Name') {
               echo '<th width="20%">Name</th>';
            } else {
               echo '<th>' . $fieldname . '</th>';
            }
         }
         echo "</tr>\n";
      
         $IDindex = $fieldindex['ID'];
         $parentIDindex = $fieldindex['ParentID'];
         $lookup = lookuptable("daf." . $parentobjecttype, "ID", "Name");
      
         $childIDs = array();
 
         for ($j = 0; $j < $num_rows; $j++) {
            $row = mysql_fetch_row($result);
            echo '<tr>';
            for ($i = 0; $i < $num_fields; $i++) {
               if ($i == $IDindex) {
                  array_push($childIDs, $row[$i]);
                  echo '<td>'  .  $row[$i] . '</td>';
               } else if ($i == $parentIDindex) {
                  echo '<td>'  . $lookup[$row[$i]] . '</td>';
               } else {
                  echo '<td>'  . $row[$i] . '</td>';
               }
            }
            echo '</tr>';
            echo "\n";
         }
         echo '</table>';
      }
 
      if (isset($recurse)) {
         foreach ($childIDs as $parentID) {
            show_detailed_child_objects($childtype, $parentID, $recurse);
         }
      }   
      
   }

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_teststand
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function modify_teststand($ID) {

   $query = "SELECT * FROM daf.teststand where ID = '" . $ID . "'";
   $teststand = mysql_query($query);

   if (! $teststand) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($teststand);   // should only be one row
   $num_fields = mysql_num_fields($teststand);

   echo '<div dojoType="dijit.form.Form" id="create_teststand" jsId="create_teststand" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Teststands</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($teststand, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $originalname = $row[$fieldindex['Name']];
   
   $teststand_row = mysql_fetch_row($teststand);
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($teststand, $i);
      echo '<tr>';
      if ($i == $IDindex) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>' . $teststand_row[$i] . '</td>';
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $teststand_row[$i] . '"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
 
   cellbutton("div", "teststand_create", "Modify Teststand", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="teststand" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "teststand_cancel", "Cancel", "index.php?object=teststand&action=show&ID=$ID", null);

   //echo '</form>' . "\n";
   echo "</div>\n";
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   host
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function host($gp) {

  global $db_server;

  if ($gp['object'] !== "host") {
     add_error_message("DAFE0001: host: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "host";
   }
   
  if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         $errorfound = 0;
         if (count_rows("daf.host", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0084: Cannot create host " . $gp['name'] . " because a host of that name already exists");
            $error_found = 1;
            $gp['action'] = 'show';

         } else {
               
            if (! isset($gp['teststandprimary'] )) {
               $gp['teststandprimary'] = 0;
            } else {
               // check to see if there is a host that is designated as the test stand primary - if there is then 
               // reject the attempt to create this host
               $primary = get_names_of_objects("host", "TeststandID = '" . $gp['teststandid'] . "' AND Teststandprimary = '1'");
               if (count($primary) > 0) {
                  $lookup = lookuptable("daf.teststand", "ID", "Name");
                  add_error_message("DAFE0085: There is already a primary host (" . implode(",", $primary) . ") in test stand "  . $lookup[$gp['teststandid']] . 
                                    " - cannot create a second one");
                  $errorfound = 1;
               }
            }
         }
         
         if (! $errorfound) {
            $query = "INSERT INTO daf.host (TeststandID, Name, Type, Model, Serial, Hostselectorvalue, Teststandprimary, Comments) " .
                     "VALUES('" . $gp['teststandid'] . "', '" . $gp['name'] . "','" . $gp['type'] . "','" . $gp['model'] . "','" . 
                      $gp['hostselectorvalue'] . "','" . $gp['teststandprimary'] . "','" . $gp['serial'] . "','" . $gp['comments'] . "')";              
            do_mysql_query($query, $db_server, 1);
            $gp['action'] = 'show';
         }

      } else if ($gp['operation'] === "modify") {
      
         $errorfound = 0;
         
         if ((count_rows("daf.host", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0086: Cannot modify Host name to " . $gp['name'] . " because a Host with that name already exists");
            $errorfound = 1;
      
         } else { 

            if (! isset($gp['teststandprimary'] )) {
                $gp['teststandprimary'] = 0;
            } else {
            // check to see if there is a host that is designated as the test stand primary - if there is then 
            // reject the attempt to modify this host
               $primary = get_names_of_objects("host", "TeststandID = '" . $gp['teststandid'] . "' AND Teststandprimary = '1'");
               if (count($primary) > 0) {
                  $lookup = lookuptable("daf.teststand", "ID", "Name");
                  add_error_message("DAFE0087: There is already a primary host (" . implode(",", $primary) . ") in test stand "  . $lookup[$gp['teststandid']] . 
                                    " - cannot create a second one");
                  $errorfound = 1;
               }
            } 
         }

         if (! $errorfound) {
            $query = "UPDATE daf.host SET TeststandID='" . $gp['teststandid'] . "', Name='" . $gp['name'] . "',  Type='" . $gp['type'] . 
                     "',  Model='" . $gp['model'] . "',  Serial='" . $gp['serial'] . "', Hostselectorvalue='" . $gp['hostselectorvalue'] . 
                     "', Teststandprimary='" . $gp['teststandprimary'] . "', Comments='" . $gp['comments'] . "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
         }    
         $gp['action'] = 'show';      
         $gp['ID'] = null;

      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.host where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_host();
    
   } else if ($gp['action'] === "modify") {

       modify_host($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_hosts("host", "Hosts");
           
      } else {
 
         show_detailed_host($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("host");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_host
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function create_host() {

   $hostselectorvalues = get_hostselectorvalues();

   echo '<div dojoType="dijit.form.Form" id="create_host" jsId="create_host" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Test Host</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_HOST_NAME_LEN . '" maxlength="' . DAF_HOST_NAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_HOST_NAME_LEN . 'em;" regExp=".{1,' . DAF_HOST_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter Host name" ' . 
        'invalidMessage="Invalid Host name (must be 1 to ' . DAF_HOST_NAME_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Type</th>';
   echo '<td><input type="text" name="type" size="' . DAF_HOST_TYPE_LEN . '" maxlength="' . DAF_HOST_TYPE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_HOST_TYPE_LEN . 'em;" regExp=".{1,' . DAF_HOST_TYPE_LEN . '}" required="true" trim="true" promptMessage="Enter Host Type" ' . 
        'invalidMessage="Invalid Host Type (must be 1 to ' . DAF_HOST_TYPE_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Model</th>';
   echo '<td><input type="text" name="model" size="' . DAF_HOST_MODEL_LEN . '" maxlength="' . DAF_HOST_MODEL_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_HOST_MODEL_LEN . 'em;" regExp=".{1,' . DAF_HOST_MODEL_LEN . '}" required="true" trim="true" promptMessage="Enter Host Model type" ' . 
        'invalidMessage="Invalid Host Model type (must be 1 to ' . DAF_HOST_MODEL_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Serial</th>';
   echo '<td><input type="text" name="serial" size="' . DAF_HOST_SERIAL_LEN . '" maxlength="'. DAF_HOST_SERIAL_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_HOST_SERIAL_LEN . 'em;" regExp=".{1,' . DAF_HOST_SERIAL_LEN . '}" required="true" trim="true" promptMessage="Enter Host Serial number" ' . 
        'invalidMessage="Invalid Host Serial number (must be 1 to ' . DAF_HOST_SERIAL_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Host Selector Value</th><td><select name="hostselectorvalue" />' . "\n";
   selectorIDforhostselectorvalue();
   echo "</select></td></tr>\n";         
   echo '<tr><th>Primary teststand host</th><td><input type="checkbox" name="teststandprimary" value="1" />' . "</td></tr>\n";     
   echo '<tr><th>Comments</th>';
   echo '<td><input type="text" name="comments" size = "' . DAF_HOST_COMMENTS_LEN . '" maxlength="' . DAF_HOST_COMMENTS_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%;" regExp=".{1,' . DAF_HOST_COMMENTS_LEN . '}" required="true" trim="true" promptMessage="Enter Host Comments" ' . 
        'invalidMessage="Invalid Host Comments (must be 1 to ' . DAF_HOST_COMMENTS_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Teststand</th><td><select name="teststandid" />' . "\n";
   selectorID("teststand", "Name");
   echo "</select></td></tr>\n";   
   echo "</table>\n";

   cellbutton("div", "teststand_create", "Create Host", null, "submit");
         
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"host\" />\n";
   cellbutton(null, "host_cancel", "Cancel", "index.php?object=host&action=show", null);

   echo "</div>\n";
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_hosts
//
// Inputs:     TeststandID    (optional)
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_all_hosts($TeststandID = NULL, $maxwidth = NULL) {

   if (isset($TeststandI)) {
      $query = "SELECT * FROM daf.host WHERE TeststandID = '" . $TeststandID . "'";
   } else {
      $query = "SELECT * FROM daf.host";
   }
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.teststand", "ID", "Name");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   if (isset($maxwidth)) {
     echo '<table class="fullwidth">';      
   } else {
      echo '<table class="tab1">';
   }
   echo "<caption class=\"cap1\"><div>Hosts</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "host_create", "Create Host", "index.php?action=create&object=host");
   echo "</th>\n";
   
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      if ($fieldname == "TeststandID") {
         echo '<th>Test Stand</th>';
      } else if ($fieldname == "Hostselectorvalue") {
         echo '<th>Host Selector Value</th>';
      } else if ($fieldname == "Teststandprimary") {
         echo '<th>Primary Host in Test Stand?</th>';
      } else if ($fieldname == "Agentstatus") {
         echo '<th>Agent Status</th>';
      } else if ($fieldname == "Agentstatusdate") {
         echo '<th>Agent Status Date</th>';
      } else {         
         echo '<th>' . $fieldname . '</th>';
      }
   }
  
   echo '<th>Collectors</th>';
   echo '</tr>';
   echo "\n";
   
   $teststandindex = $fieldindex['TeststandID'];
   $teststandprimaryindex = $fieldindex['Teststandprimary']; 
 
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       echo "<td class=td_smd>";
       cellbutton(null, "host_show_$j", "show", "index.php?action=show&object=host&ID=$ID");
       echo "</td>";
       echo "<td class=td_smd>";
       cellbutton(null, "host_modify_$j", "modify", "index.php?action=modify&object=host&ID=$ID");
       echo "</td>";
       echo "<td class=td_smd>";
       // cellbutton(null, "host_delete_$j", "delete", "index.php?action=show&operation=delete&object=host&ID=$ID");
       button_with_confirm("host_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=host&ID=$ID", 
                           "index.php?action=show&object=host");
       echo "</td>";
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $teststandindex) {
            if (isset($lookup[$row[$i]])) {
               echo '<td>' . $lookup[$row[$i]] . '</td>';
            } else {
               echo '<td> not allocated </td>';
            }
          } else if ($i == $teststandprimaryindex) {
            if ($row[$i]) {
               echo '<td>yes</td>';
            } else {
               echo '<td> &nbsp; </td>';
            }
          } else{
            echo '<td>'  . $row[$i] . '</td>';
          }
       }
       echo "<td class=td_smd>";
       cellbutton(null, "collectors_$ID", "View", "index.php?action=show&object=collectorvalue&ID=$ID");
       echo "</td></tr>\n";

   }
   echo '</table>';

   echo '</form>' . "\n"; 
         
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_host
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_detailed_host($ID) {

   $query = "SELECT * FROM daf.host where ID = '" . $ID . "'";
   $host = mysql_query($query);

   if (! $host) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($host);   // should only be one row
   $num_fields = mysql_num_fields($host);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Host</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($host, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $host_row = mysql_fetch_row($host);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $fieldindex['TeststandID']) {
         echo '<th>Test Stand</th>';
         echo '<td>'  . $host_row[$i] . '</td>';
      } else if ($i == $fieldindex['Hostselectorvalue']) {
         echo '<th>Host Selector Value</th>';
         echo '<td>'  . $host_row[$i] . '</td>';
      } else if ($i == $fieldindex['Teststandprimary']) {
         echo '<th>Primary Host in Test Stand?</th>';
         if ($host_row[$i] ) {
            echo '<td>yes</td>';
         } else {
            echo '<td>no</td>';
         }
      } else if ($i == $fieldindex['Agentstatus']) {
         echo '<th>Agent Status</th>';
         echo '<td>'  . $host_row[$i] . '</td>';
      } else {
         echo '<th>' . mysql_field_name($host, $i) . '</th>';
         echo '<td>'  . $host_row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_host
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function modify_host($ID) {

   $query = "SELECT * FROM daf.host where ID = '" . $ID . "'";
   $host = mysql_query($query);

   if (! $host) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($host);   // should only be one row
   $num_fields = mysql_num_fields($host);

   echo '<div dojoType="dijit.form.Form" id="create_teststand" jsId="create_teststand" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Hosts</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($host, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex                = $fieldindex['ID'];
   $teststandindex         = $fieldindex['TeststandID'];
   $hostselectorvalueindex = $fieldindex['Hostselectorvalue'];
   $teststandprimaryindex  = $fieldindex['Teststandprimary']; 
   
   $row = mysql_fetch_row($host);
   $originalname = $row[$fieldindex['Name']];
 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($host, $i);
      if ($i == $IDindex) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($i == $teststandindex) {
         echo '<th>Test Stand</th>';
         echo '<td><select name="teststandid" />' . "\n";
         selectorID("teststand", "Name", $row[$i]);
         echo '</select></td>' . "\n";
      } else if ($i == $hostselectorvalueindex) {
         echo '<th>Host Selector</th>';
         echo '<td><select name="hostselectorvalue" />' . "\n";
         selectorIDforhostselectorvalue($row[$i]);
         echo '</select></td>' . "\n"; 
      } else if ($i == $teststandprimaryindex) {
         if ($row[$i]) {
           echo  '<th>Primary host in teststand</th><td><input type="checkbox" checked name="teststandprimary" value="1" /></td>';    
         } else {
           echo  '<th>Primary host in teststand</th><td><input type="checkbox"         name="teststandprimary" value="1" /></td>';      
         }
      } else {
         if ($i == $fieldindex['Agentstatus']) {
            echo '<th>Agent Status</th>';
            echo '<td>' . $row[$i] . '</td>';
         } else if ($i == $fieldindex['Agentstatusdate']) {
            echo '<th>Agent Status Date</th>';
            echo '<td>' . $row[$i] . '</td>';
         } else {
            echo '<th>' . $fieldname . '</th>';
            echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $row[$i] . '"/></td>';
         }         
      }
      echo '</tr>';
      echo "\n";
   }

   echo '</table>';
   
   cellbutton("div", "host_modify", "Modify Host", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="host" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "host_cancel", "Cancel", "index.php?object=host&action=show&ID=$ID", null);

   echo "</div>\n";  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   collectortype
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function collectortype($gp) {

  global $db_server;

  if ($gp['object'] !== "collectortype") {
     add_error_message("DAFE0001: collectortype: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "collectortype";
   }
   
   if (isset($gp['operation'])) {

      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.collectortype", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0088: Cannot create collector type " . $gp['name'] . " because a collector type of that name already exists");

         } else {
         
            $query = "INSERT INTO daf.collectortype (Name, Builtin, OStype, Invocation) " .
                     "VALUES('" . $gp['name'] . "', '" . $gp['builtin'] . "','" . $gp['ostype'] . "','" . $gp['invocation'] . "')";
            if (! do_mysql_query($query, $db_server, 1)) {
               return;
            }   
         }
          
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf.collectortype", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0089: Cannot modify Collector Type name to " . $gp['name'] . " because a Collector Type with that name already exists");
      
         } else { 
         
            $query = "UPDATE daf.collectortype SET Name='" . $gp['name'] . "', Builtin='" . $gp['builtin'] . "',  Ostype='" . $gp['ostype'] . 
                     "',  Invocation='" . $gp['invocation'] . "' WHERE ID='" . $gp['ID'] . "'";              
            if (! do_mysql_query($query, $db_server, 1)) {
               return;
            }   
         }
          
         $gp['action'] = 'show';      

      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.collectortype where ID='" . $gp['ID'] . "'";    
         if (! do_mysql_query($query, $db_server, 1)) {
            return;
         }  
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }
   
   if ($gp['action'] === "create") {
   
       create_collectortype();
    
   } else if ($gp['action'] === "modify") {

       modify_collectortype($gp['ID']); 
        
   } else  if ($gp['action'] === "show") {

     show_collectortypes();
    
   } else {
    
      report_bad_parms("collectortype");
   
   }
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_collectortypes
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_collectortypes() {
 
   $query = "SELECT * FROM daf.collectortype ORDER BY ID";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Collector Types</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo '<th colspan=2>';
   cellbutton(null, "collector_create", "Create Collector", "index.php?action=create&object=collectortype");
   echo '</th>';

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<th>' . $fieldname . '</th>';
      $fieldindex[$fieldname] = $i;
   }
   echo "</tr>\n";
   
   $builtinindex = $fieldindex['Builtin'];
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       if ($row[$builtinindex] == 0) {
      
          echo "<td class=td_smd>";
          cellbutton(null, "collectortype_modify_$j", "modify", "index.php?action=modify&object=collectortype&ID=$ID");
          echo "</td>";
          echo "<td class=td_smd>";
          // cellbutton(null, "collectortype_delete_$j", "delete", "index.php?action=show&operation=delete&object=collectortype&ID=$ID");
          button_with_confirm("collectortype_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=collectortype&ID=$ID", 
                           "index.php?action=show&object=collectortype");
          echo "</td>";

       } else {
          echo "<td> &nbsp; </td>";
          echo "<td> &nbsp; </td>";
       }
       
       for ($i = 0; $i < $num_fields; $i++) {
          echo '<td>'  . $row[$i] . '</td>';
       }   
       echo "</tr>\n";

   }
   echo '</table>';

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_collectortype
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function create_collectortype() {

   echo '<div dojoType="dijit.form.Form" id="create_collectortype" jsId="create_collectortype" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Collector Type</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_COLLECTORTYPE_NAME_LEN . '" maxlength="' . DAF_COLLECTORTYPE_NAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_COLLECTORTYPE_NAME_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_COLLECTORTYPE_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter Collector Type name" ' . 
        'invalidMessage="Invalid Collector Type number (must be 1 to ' . DAF_COLLECTORTYPE_NAME_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Ostype</th>';
   echo '<td><input type="text" name="ostype" size="' . DAF_COLLECTORTYPE_OSTYPE_LEN . '" maxlength="' . DAF_COLLECTORTYPE_OSTYPE_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_COLLECTORTYPE_OSTYPE_LEN . 'em;" regExp=".{1,' . DAF_COLLECTORTYPE_OSTYPE_LEN . '}" required="true" trim="true" promptMessage="Enter Collector Type OS type" ' . 
        'invalidMessage="Invalid Collector Type OS type (must be 1 to ' . DAF_COLLECTORTYPE_OSTYPE_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Invocation</th>';
   echo '<td><input type="text" name="invocation" size="' . DAF_COLLECTORTYPE_INVOCATION_LEN . '" maxlength="' . DAF_COLLECTORTYPE_INVOCATION_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_COLLECTORTYPE_INVOCATION_LEN . 'em;" regExp=".{1,' . DAF_COLLECTORTYPE_INVOCATION_LEN . '}" required="true" trim="true" promptMessage="Enter Collector Type Invocation" ' . 
        'invalidMessage="Invalid Collector Type invocation (must be 1 to ' . DAF_COLLECTORTYPE_INVOCATION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "<table>\n";
   
   cellbutton("div", "collector_create", "Create Collector", null, "submit");
   echo "<input type=\"hidden\" name=\"builtin\" value=\"0\" />\n";
   echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"collectortype\" />\n";
   cellbutton(null, "collector_cancel", "Cancel", "index.php?object=collectortype&action=show", null);

   echo "</div>\n";
              
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_collectortype
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function modify_collectortype($ID) {

   $query = "SELECT * FROM daf.collectortype where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_teststand" jsId="create_teststand" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Collector Type</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $builtinindex = $fieldindex['Builtin'];
   
   $row = mysql_fetch_row($result);
   $originalname = $row[$fieldindex['Name']];
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if (($i == $IDindex)  || ($i == $builtinindex)) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $row[$i] . '"/></td>';
      }
      echo "</tr>\n";
   }
   echo "</table>\n";

   cellbutton("div", "collector_modify", "Modify Collector", null, "submit");
   echo "<input type=\"hidden\" name=\"builtin\"   value=\"0\" />\n";
   echo "<input type=\"hidden\" name=\"ID\"        value=\"" . $ID . "\" />\n";
   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"collectortype\" />\n";
   echo "<input type=\"hidden\" name=\"originalname\" value=\"" . $originalname . "\" />\n";
   cellbutton(null, "collector_cancel", "Cancel", "index.php?object=collectortype&action=show&ID=$ID", null);
   
   echo "</div>\n";
           
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   collectorvalue
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function collectorvalue($gp) {

  if ($gp['object'] !== "collectorvalue") {
     add_error_message("DAFE0001: collectorvalue: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "collectorvalue";
   }
   
  if ($gp['action'] === "show") {
  
     if (isset($gp['ID'])) {
        show_collector_values($gp['ID']);
     } else {
        show_collector_values();
     }

   } else {
    
      report_bad_parms("collectorvalue");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_collectorvalues
//
// Inputs:     $HostID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_collector_values($HostID = NULL) {
 
   $query = "SELECT collectorvalue.ID, host.Name as 'Hostname', collectortype.Name as 'Name', collectorvalue.Valuestatusdate, collectorvalue.Value FROM daf.collectorvalue INNER JOIN daf.host ON collectorvalue.HostID = host.ID " . 
            "INNER JOIN daf.collectortype ON collectorvalue.CollectortypeID = collectortype.ID";
   if (! is_null($HostID)) {
      $query = $query . " WHERE host.ID='$HostID'";
   } else {
        $query = $query . " ORDER BY Hostname";
   }
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Collector Values</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   
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
// Function:   collectorset
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

function collectorset($gp) {

   global $db_server;

   if ($gp['object'] !== "collectorset") {
      add_error_message("DAFE0001: collectorset: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "collectorset";
   }
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.collectorset", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0090: Cannot create collector set " . $gp['name'] . " because a collector set of that name already exists");

         } else {
         
            $query = "INSERT INTO daf.collectorset (Name, Description) " .
                     "VALUES('" . $gp['name'] . "', '" . $gp['description'] . "')"; 
            do_mysql_query($query, $db_server, 1);
            
         }
         
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf.collectorset", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0091: Cannot modify Collector Set name to " . $gp['name'] . " because a Collector Set with that name already exists");
      
         } else { 
         
            $query = "UPDATE daf.collectorset SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . 
                     "' WHERE ID='" . $gp['ID'] . "'"; 
            do_mysql_query($query, $db_server);
            
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;      

      } else if ($gp['operation'] === "createcollectorsetmember") {
      
         // check to see if there is a collectorsetmember in this collectorset for the requested collectortype - if there is then 
         // reject the attempt to create this collectorsetmember
         $exists = get_fields_in_objects("collectorsetmember", "ID", "CollectortypeID = '" . $gp['collectortypeID'] . "' AND CollectorsetID = '" . $gp['collectorsetID']  . "'");
         if (count($exists) > 0) {
            $lookupcollectorset = lookuptable("daf.collectorset", "ID", "Name");
            $lookupcollectortype = lookuptable("daf.collectortype", "ID", "Name");
            add_error_message("DAFE0092: There is already a collector set member for collector type " .  $lookupcollectortype[$gp['collectorsetID']]  . " in collectorset "  . 
                              $lookupcollectorset[$gp['collectorsetID']] . " - cannot create a second one");
         } else {
            $query = "INSERT INTO daf.collectorsetmember (CollectortypeID, Requiredvalue, Comparator, CollectorsetID) " .
                     "VALUES('" . $gp['collectortypeID'] . "', '" . $gp['requiredvalue'] . "', '"  . $gp['comparator'] . "', '" . $gp['collectorsetID'] . "')"; 
            do_mysql_query($query, $db_server, 1);
         }   
         $gp['action'] = 'show';
         $gp['ID'] = $gp['collectorsetID'];
  
      } else if ($gp['operation'] === "modifycollectorsetmember") {

         $query = "UPDATE daf.collectorsetmember SET CollectortypeID='" . $gp['collectortypeID'] . "', Requiredvalue='" . $gp['requiredvalue'] . "', Comparator='" . $gp['comparator'] .
                  "' WHERE ID='" . $gp['ID'] . "'"; 
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show'; 
         $gp['ID'] = $gp['collectorsetID'];     
  
      } else if ($gp['operation'] === "deletecollectorsetmember") {

         $query = "DELETE FROM daf.collectorsetmember where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
         $gp['ID'] = $gp['collectorsetID'];

      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.collectorset where ID='" . $gp['ID'] . "'";    
         if (! do_mysql_query($query, $db_server, 1)) {
            return;
         }  
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      } 
   
   }
   
   if ($gp['action'] === "createcollectorsetmember") {
   
      create_collectorsetmember($gp['collectorsetID']);
   
   } else if ($gp['action'] === "modifycollectorsetmember") {
   
      modify_collectorsetmember($gp['ID']);   
   
   } else if ($gp['action'] === "create") {
   
       create_collectorset();
    
   } else if ($gp['action'] === "modify") {

       modify_collectorset($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_collectorsets("collectorset", "Collector Sets");
           
      } else {
 
         show_detailed_collectorset($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("collectorset");
   
   }
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_collectorsetmember
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function collectorsetmember($gp) {

  global $db_server;

  if ($gp['object'] !== "collectorsetmember") {
     add_error_message("DAFE0001: collectorsetmember: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "collectorsetmember";
   }
   
  if ($gp['action'] === "show") {

    show_table("collectorsetmember", "Collector Set Members");
    
   } else {
    
      report_bad_parms("collectorsetmember");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_collectorset
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function create_collectorset() {

   $hostselectorvalues = get_hostselectorvalues();

   echo '<div dojoType="dijit.form.Form" id="create_collectorset" jsId="create_collectorset" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Collector Set</div></caption>\n";
   
   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_COLLECTORSET_NAME_LEN . '" maxlength="' . DAF_COLLECTORSET_NAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_COLLECTORSET_NAME_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_COLLECTORSET_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter Collector Set name" ' . 
        'invalidMessage="Invalid Collector Type Set name (must be 1 to ' . DAF_COLLECTORSET_NAME_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size="' . DAF_COLLECTORSET_DESCRIPTION_LEN . '" maxlength="' . DAF_COLLECTORSET_DESCRIPTION_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_COLLECTORSET_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter Collector Set description" ' . 
        'invalidMessage="Invalid Collector Set description (must be 1 to ' . DAF_COLLECTORSET_DESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";
   
   cellbutton("div", "collectorset_create", "Create Collector Set", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"collectorset\" />\n";
   cellbutton(null, "collectorset_cancel", "Cancel", "index.php?object=collectorset&action=show", null);

   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_collectorsets
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function show_all_collectorsets() {

   $query  = "SELECT * FROM daf.collectorset";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Collector Sets</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "collectorset_create", "Create Collector Set", "index.php?action=create&object=collectorset");
   echo "</th>\n";
   
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      echo '<th>' . $fieldname . '</th>';
   }
  
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       echo "<td class=td_smd>";
       cellbutton(null, "collectorset_show_$j", "show", "index.php?action=show&object=collectorset&ID=$ID");
       echo "</td>";
       echo "<td class=td_smd>";
       cellbutton(null, "collectorset_modify_$j", "modify", "index.php?action=modify&object=collectorset&ID=$ID");
       echo "</td>";
       echo "<td class=td_smd>";
       // cellbutton(null, "collectorset_delete_$j", "delete", "index.php?action=show&operation=delete&object=collectorset&ID=$ID");
       button_with_confirm("collectorset_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=collectorset&ID=$ID", 
                           "index.php?action=show&object=collectorset");
       echo "</td>";

       for ($i = 0; $i < $num_fields; $i++) {
          echo '<td>'  . $row[$i] . '</td>';
       }   
       echo '</tr>';
       echo "\n";

   }
   echo '</table>';
   
   echo '</form>' . "\n"; 
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_collectorset
//
// Inputs:     $collectorsetID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function show_detailed_collectorset($collectorsetID) {

   $query = "SELECT * FROM daf.collectorset where ID = '" . $collectorsetID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="fullwidth">';
   echo "<caption class=\"cap1\"><div>Collector Set</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $row = mysql_fetch_row($result);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th class="fullwidth">' . mysql_field_name($result, $i) . '</th>';
      echo '<td>'  . $row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   $query = "SELECT collectorsetmember.ID, collectortype.Name, collectorsetmember.Comparator, collectorsetmember.Requiredvalue FROM daf.collectorsetmember INNER JOIN daf.collectortype " . 
             "ON daf.collectorsetmember.collectortypeID=collectortype.ID where collectorsetID = '" . $collectorsetID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="fullwidth">';
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th> &nbsp; </th><th> &nbsp; </th>\n";
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      echo "<th>$fieldname</th>\n";
   }
   echo '</tr>';   
   
   for ($i = 0; $i<$num_rows; $i++) {
   
      $row = mysql_fetch_row($result);
      $collectorsetmemberID = $row[$fieldindex['ID']]; 

      echo '<tr>';
      echo "<td class=td_smd>";
      cellbutton(null, "collectorset_modifymember_$i", "modify", "index.php?action=modifycollectorsetmember&object=collectorset&ID=$collectorsetmemberID&collectorsetID=$collectorsetID");
      echo "</td>\n";
      echo "<td class=td_smd>";
      // cellbutton(null, "collectorset_deletemember_$i", "delete", "index.php?action=show&operation=deletecollectorsetmember&object=collectorset&ID=$collectorsetmemberID&collectorsetID=$collectorsetID");
      button_with_confirm("collectorset_deletemember_$j", "delete", "delete", "index.php?action=show&operation=deletecollectorsetmember&object=collectorset&ID=$collectorsetmemberID&collectorsetID=$collectorsetID", 
                           "index.php?action=show&object=collectorsetID&collectorsetID=$collectorsetID");
      echo "</td>\n";
      for ($j = 0; $j < $num_fields; $j++) {
         echo '<td>'  . $row[$j] . '</td>';
         echo "\n";
      }
      echo '</tr>';
      
   }
   echo '</table>';
   
   echo '<form action="index.php" method="post">' . "\n";
   cellbutton("div", "collectorset_addmember", "Add Collector Set Member", null, "submit");
   echo '<input type="hidden" name="action"         value="createcollectorsetmember" />' . "\n";
   echo '<input type="hidden" name="object"         value="collectorset" />' . "\n";
   echo '<input type="hidden" name="collectorsetID" value="' . $collectorsetID . '" />' . "\n";
   echo '</form>' . "\n"; 

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_collectorset
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function modify_collectorset($ID) {

   $query = "SELECT * FROM daf.collectorset where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_teststand" jsId="create_teststand" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Collector Sets</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
  
   $row = mysql_fetch_row($result);
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      echo '<th>' . $fieldname . '</th>';
      echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $row[$i] . '"/></td>';      
      echo "</tr>\n";
   }

   echo '</table>';

   cellbutton("div", "collectorset_modify", "Modify Collector Set", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="collectorset" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "collectorset_cancel", "Cancel", "index.php?object=collectorsete&action=show&ID=$ID", null);

   echo "</div>\n";
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_collectorsetmember
//
// Inputs:     $collectorsetID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function create_collectorsetmember($collectorsetID) {

   echo '<div dojoType="dijit.form.Form" id="create_collectorsetmember" jsId="create_collectorsetmember" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Collector Set</div></caption>\n";
   
   echo '<tr><th>Collector Type</th><td><select name="collectortypeID" />' . "\n";
   selectorID("collectortype", "Name");
   echo "</select></td></tr>\n";  
   echo '<tr><th>Comparator</th><td><select name="comparator" />' . "\n";
   echo '<option value="EQ">EQ (string equality)</option>' . "\n";
   echo '<option value="NE">NE (string non-equality)</option>' . "\n";
   echo '<option value="==">== (numerical equality)</option>' . "\n";
   echo '<option value="<">&lt; (numerical less than)</option>' . "\n";
   echo '<option value="<=">&le; (numerical less than or equal);</option>' . "\n";
   echo '<option value=">">&gt; (numberical greater than)</option>' . "\n";
   echo '<option value=">=">&ge; (numerical greater than or equal)</option>' . "\n";
   echo '<option value="!=">&ne; (numerical not equal)</option>' . "\n";
   echo "</select></td></tr>\n";     
   echo '<tr><th>Required value</th>';
   echo '<td><input type="text" name="requiredvalue" size="' . DAF_COLLECTORSET_NAME_LEN . '" maxlength="' . DAF_COLLECTORSET_NAME_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_COLLECTORSET_NAME_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_COLLECTORSET_NAME_LEN . '}" required="true" trim="true" promptMessage="Enter Collector Set Member name" ' . 
        'invalidMessage="Invalid Collector Set Member name (must be 1 to ' . DAF_COLLECTORSET_NAME_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";
   
   cellbutton("div", "collectorset_createmember", "Create Collector Set Member", null, "submit");
   echo "<input type=\"hidden\" name=\"collectorsetID\" value=\"$collectorsetID\"/>\n";
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"createcollectorsetmember\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"collectorset\"/>\n";
   cellbutton(null, "collectorset_cancel", "Cancel", "index.php?object=collectorset&action=show&collectorsetID=$collectorsetID", null);

   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_collectorsetmember
//
// Inputs:     $collectorsetmemberID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function modify_collectorsetmember($collectorsetmemberID) {

   $query = "SELECT * FROM collectorsetmember where ID = '" . $collectorsetmemberID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_teststand" jsId="create_teststand" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Collector Sets</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $collectortypeIDindex = $fieldindex['CollectortypeID'];
   $collectorsetIDindex = $fieldindex['CollectorsetID'];
   $collectorsetcomparatorindex = $fieldindex['Comparator'];
   
   $lookupcollectorset = lookuptable("collectorset", "ID", "Name");
  
   $row = mysql_fetch_row($result);
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if ($i == $IDindex) {
         echo '<th>ID</th>';
         echo '<td> ' . $row[$i] . '</td>';
      } else if ($i == $collectortypeIDindex) {  
         echo '<th>Collector Type</th>';       
         echo '<td><select name="collectortypeID"/>' . "\n";
         selectorID("collectortype", "Name", $row[$i]);
         echo '</select></td>' . "\n";
      } else if ($i == $collectorsetcomparatorindex) {
         echo '<th>Comparator</th>' . "\n";
         echo '<td><select name="comparator"/>' . "\n";
         echo '<option value="EQ">EQ (string equality)</option>' . "\n";
         echo '<option value="NE">NE (string non-equality)</option>' . "\n";
         echo '<option value="==">== (numerical equality)</option>' . "\n";
         echo '<option value="<">&lt; (numerical less than)</option>' . "\n";
         echo '<option value="<=">&le; (numerical less than or equal);</option>' . "\n";
         echo '<option value=">">&gt; (numberical greater than)</option>' . "\n";
         echo '<option value=">=">&ge; (numerical greater than or equal)</option>' . "\n";
         echo '<option value="!=">&ne; (numerical not equal)</option>' . "\n";
         echo '</select></td>' . "\n";
      } else if ($i == $collectorsetIDindex) {
         echo '<th>Collector Set</th>';
         $collectorsetID = $row[$i];
         echo '<td> ' . $lookupcollectorset[$row[$i]] . '</td>';
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $row[$i] . '"/></td>';
      }      
      echo "</tr>\n";
   }

   echo '</table>';
  
   cellbutton("div", "collectorset_modifymember", "Modify Collector Set Member", null, "submit");
   echo "<input type=\"hidden\" name=\"ID\"               value=\"$collectorsetmemberID\"/>\n";
   echo "<input type=\"hidden\" name=\"collectorsetID\"   value=\"$collectorsetID\"/>\n";
   echo "<input type=\"hidden\" name=\"action\"           value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"        value=\"modifycollectorsetmember\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"           value=\"collectorset\"/>\n";
   cellbutton(null, "collectorset_cancel", "Cancel", "index.php?object=collectorset&action=show&collectorsetID=$collectorsetID", null);
   
   echo "</div>\n";

}

?>
