<?php 

  //  object= testlevel  action=show 
  //  object= testlevel  action=show  ID= 
  
  //  object= testlevel  action=show    operation=create            name=... comments=.... clusterid=.... type=....
  //  object= testlevel  action=show    operation=modify  ID=...    name=... comments=.... clusterid=.... type=....
  //  object= testlevel  action=show    operation=delete  ID=... 

  //  object= testlevel  action=show    operation=createtestlevelmember             
  //  object= testlevel  action=show    operation=deletetestlevelmember  ID=...    testlevelmemberid=....

  //  object= testlevel  action=createtestleveltmember  testlevelID=...          

  //  object= objecttype  action=show 
  //  object= objecttype  action=show  ID= 

  //  object= objecttype  action=show operation=create  parentID=... name=.... comments=...
  //  object= objecttype  action=show operation=modify  ID=... parentID=... name=.... comments=...
  //  object= objecttype  action=show operation=delete  ID= name=
  
  //  object= objecttype  action=create
  //  object= objecttype  action=modify ID=...

  //  object= object  action=show  objecttype=...
  //  object= object  action=show  objecttype=... ID= 

  //  object= object  action=show objecttype=... operation=create  parentID=... name=.... description=...
  //  object= object  action=show objecttype=... operation=modify  ID=... parentID=... name=.... description=...
  //  object= object  action=show objecttype=... operation=delete  ID= name=
  
  //  object= object  action=create objecttype=... 
  //  object= object  action=modify objecttype=... ID=...
  
  
//---------------------------------------------------------------------------------------------------------------------
//
// Function:   testlevel
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

function testlevel($gp) {

  global $db_server;

  if ($gp['object'] !== "testlevel") {
     add_error_message("DAFE0001: testlevel: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "testlevel";
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         if (count_rows("daf.testlevel", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0064: Cannot create testlevel " . $gp['name'] . " because a testlevel of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.testlevel (Name, Description) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['description'] . "')";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf.operation", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0065: Cannot modify testcase name to " . $gp['name'] . " because a testcase of that name already exists");
            
         } else {

            $query = "UPDATE daf.testlevel SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . 
                     "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';     
         $gp['ID'] = NULL;
         
      } else if ($gp['operation'] === "createtestlevelmember") {
      
         if (count_rows("daf.testlevelmember", 
                         "TestlevelID = '" . $gp['testlevelid'] . "' AND LevelinstanceID = '" . $gp['levelinstanceid'] . "'") > 0) {
         
            add_error_message("DAFE0066: Cannot create that Test Level Member because an identicial member already exists in the Test Level");
         
         } else {
      
            $query = "INSERT INTO daf.testlevelmember (TestlevelID, LevelinstanceID) " .
                     "VALUES('" . $gp['testlevelid'] . "','" . $gp['levelinstanceid'] . "')";              
            do_mysql_query($query, $db_server, 1);
         }
           
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
        
      } else if ($gp['operation'] === "deletetestlevelmember") {      
  
         $query = "delete FROM daf.testlevelmember WHERE ID='" . $gp['testlevelmemberid'] . "'";              
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';  
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "delete") {
      
         if (count_rows("daf.testlevelmember", "TestlevelID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0067: Cannot delete that Testlevel because it still contains one or more Testlevelmembers");
         
         } else {

            $query = "DELETE FROM daf.testlevel where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            
         }
          
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }
   
   if ($gp['action'] === "createtestlevelmember") {
   
      create_testlevelmember($gp['ID']);
   
   } else if ($gp['action'] === "create") {
   
       create_testlevel();
    
   } else if ($gp['action'] === "modify") {

       modify_testlevel($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_testlevels();
           
      } else {
 
         show_detailed_testlevel($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("testlevel");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_testlevels
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

function show_testlevels() {
    
   $query = "SELECT * FROM daf.testlevel";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Test Levels</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "create_testlevel", "Create Test Level", "index.php?action=create&object=testlevel");
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
          cellbutton(null, "testlevel_show_$j", "show", "index.php?action=show&object=testlevel&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          cellbutton(null, "testlevel_modify_$j", "modify", "index.php?action=modify&object=testlevel&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "testlevel_delete_$j", "delete", "index.php?action=show&operation=delete&object=testlevel&ID=$ID");
          button_with_confirm("testlevel_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=testlevel&ID=$ID", 
                          "index.php?action=show&object=testlevel");
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
// Function:   create_testlevel
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

function create_testlevel() {

   echo '<div dojoType="dijit.form.Form" id="create_testlevel" jsId="create_testlevel" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a Test Level</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size=' . DAF_TESTLEVEL_LEN . '" maxlength="' . DAF_TESTLEVEL_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:' . DAF_ENVIRONMENT_NAME_LEN . 'em" regExp="[A-Za-z0-9_]{1,' . DAF_TESTLEVEL_LEN . '}" required="true" trim="true" promptMessage="Enter a Test Level name" ' . 
        'invalidMessage="Invalid Test Level name (must be 1 to ' . DAF_TESTLEVEL_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th><td>';
   echo '<input type="text" name="description" size=' . DAF_TESTLEVEL_DESCRIPTION_LEN . '" maxlength="' . DAF_TESTLEVEL_DESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_TESTLEVEL_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Test Level description" ' . 
        'invalidMessage="Invalid Test Level description (must be 1 to ' . DAF_TESTLEVEL_DESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";
   
   cellbutton(null, "create_testlevel", "Create Test Level", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"testlevel\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "testlevel_cancel", "Cancel", "index.php?object=testlevel&action=show", null);
   
   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_testlevel
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

function show_detailed_testlevel($ID) {

   $query = "SELECT * FROM daf.testlevel where ID = '" . $ID . "'";
   $testlevel = mysql_query($query);

   if (! $testlevel) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testlevel);   // should only be one row
   $num_fields = mysql_num_fields($testlevel);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Test Level</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testlevel, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $testlevel_row = mysql_fetch_row($testlevel);
   $TestlevelID = $testlevel_row[$fieldindex['ID']]; 

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($testlevel, $i) . '</th>';
      echo '<td>' . $testlevel_row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';   

   /* $query = "SELECT testlevelmember.ID, testlevelmember.TestlevelID, testlevelmember.LevelinstanceID FROM daf.testlevelmember " . 
            "WHERE daf.testlevelmember.testlevelID = '" . $ID . "'"; */
            
   $query = "SELECT testlevelmember.ID, testlevelmember.TestlevelID, objecttype.Name as 'Parent Object Type', " . 
             "testlevelmember.LevelinstanceID as 'Level Instance' FROM daf.testlevelmember " . 
            "INNER JOIN daf.levelinstance ON testlevelmember.levelinstanceID = levelinstance.ID " .
            "INNER JOIN daf.level ON levelinstance.levelID = level.ID " .
            "INNER JOIN daf.objecttype ON level.objecttypeID = objecttype.ID " .
            "WHERE testlevelmember.TestlevelID = '" . $ID . "'"; 

   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<form action="index.php" method="post">' . "\n";

   echo "<table class=\"tab1\">\n";
   echo "<caption class=\"cap1\"><div>Test Level Members</div></caption>\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th> &nbsp; </th>\n";
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      if ($fieldname == "TestlevelID") {
         /* do nothing */
      } else if ($fieldname == 'Level Instance') {
         echo "<th>Level Instance (Value)</th>\n";
      } else {
         echo "<th>$fieldname</th>\n";
      }
   }
   echo '</tr>';
   
   $testlevelIDindex = $fieldindex['TestlevelID']; 
   $testlevellookup = lookuptable("daf.testlevel", "ID", "Name"); 
   $levelinstanceIDindex = $fieldindex['Level Instance']; 
   $levelinstancelookup = lookuplevelinstancetable("daf.levelinstance", "ID");   
   
   for ($i = 0; $i<$num_rows; $i++) {
   
      $row = mysql_fetch_row($result);
      $testlevelID = $row[$fieldindex['TestlevelID']]; 
      $testlevelmemberID =  $row[$fieldindex['ID']];
      echo '<tr>';
       echo "<td class=td_smd>";
      button_with_confirm("testlevelmember_delete_$i", "delete", "delete", "index.php?action=show&object=testlevel&operation=deletetestlevelmember&ID=$ID&testlevelmemberid=$testlevelmemberID", 
                          "index.php?action=show&object=testlevel&ID=$ID");
      echo '</td>';

      for ($j = 0; $j < $num_fields; $j++) {
         if ($j == $testlevelIDindex) {
           /*  echo '<td>'  . $testlevellookup[$row[$j]] . '</td>'; */
         } else  if ($j == $levelinstanceIDindex) {
            echo '<td>'  . $levelinstancelookup[$row[$j]] . '</td>';
         } else {         
            echo '<td>'  . $row[$j] . '</td>' . "\n";
         }
      }
      echo '</tr>';
      
   }
   echo '</table>';
   echo '</form>';
   
   echo '<form action="index.php" method="post">' . "\n";
   cellbutton("div", "testlevelmember_add", "Add Test Level Member", null, "submit");
   echo '<input type="hidden" name="action" value="createtestlevelmember" />' . "\n";
   echo '<input type="hidden" name="object" value="testlevel" />' . "\n";
   echo '<input type="hidden" name="ID" value="' . $ID . '" />' . "\n";
   echo '</form>' . "\n"; 
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_testlevel
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

function modify_testlevel($ID) {

   $query = "SELECT ID, Name, Description FROM daf.testlevel where ID = '" . $ID . "'";
   $testlevel = mysql_query($query);

   if (! $testlevel) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testlevel);   // should only be one row
   $num_fields = mysql_num_fields($testlevel);

   echo '<div dojoType="dijit.form.Form" id="create_testlevel" jsId="create_testlevel" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Testlevels</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testlevel, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   
   $row = mysql_fetch_row($testlevel);
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testlevel, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size=' . DAF_TESTLEVEL_LEN . '" maxlength="' . DAF_TESTLEVEL_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_TESTLEVEL_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_TESTLEVEL_LEN . '}" required="true" trim="true" promptMessage="Enter a Test Level name" ' . 
              'invalidMessage="Invalid Test Level name (must be 1 to ' . DAF_TESTLEVEL_LEN . ' alphanumeric characters)"/>';
      } else if ($fieldname == "Description") {
         echo '<input type="text" name="description" value="' . $row[$i] . '" size=' . DAF_TESTLEVEL_DESCRIPTION_LEN . '" maxlength="' . DAF_TESTLEVEL_DESCRIPTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_TESTLEVEL_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Test Level description" ' . 
              'invalidMessage="Invalid Test Level description (must be 1 to ' . DAF_TESTLEVEL_DESCRIPTION_LEN . ' characters)"/>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton("div", "testlevel_modify", "Modify Test Level", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="object"    value="testlevel" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "testlevel_cancel", "Cancel", "index.php?object=testlevel&action=show&ID=$ID", null);
   
   echo "</div>\n";  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_testlevelmember
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

function create_testlevelmember($testlevelID) {

   echo '<div dojoType="dijit.form.Form" id="create_testlevelmember" jsId="create_testlevelmember" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Add a Test Level Member</div></caption>\n";
   
   echo "<tr><th>Level Instance</th><td><select name=\"levelinstanceid\"/>\n";
   selectorIDforTestLevelInstance();
   echo "</select></td><tr>\n";
   echo "</table>\n";  
      
   cellbutton(null, "testlevel_addmember", "Add Test Level Member", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"testlevel\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"createtestlevelmember\"/>\n";
   echo "<input type=\"hidden\" name=\"testlevelid\" value=\"" . $testlevelID . "\"/>\n";
   cellbutton(null, "testlevel_cancel", "Cancel", "index.php?object=testlevel&action=show&ID=$testlevelID", null);

   echo "</div>\n";
     
}

/* why is there no modify_testlevelmember ???? <<<<<<<<<<<< */

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   level
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

function level($gp) {

  global $db_server;

  if ($gp['object'] !== "level") {
     add_error_message("DAFE0001: level: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "level";
  }

  if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         if (count_rows("daf.level", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0068: Cannot create level " . $gp['name'] . " because a level of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.level (Name, Description, ObjecttypeID, CollectortypeID, ProxycollectorsetID) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['description']  . "','" . $gp['objecttypeid'] . "','" . $gp['collectortypeid'] . "','" . $gp['proxycollectorsetid'] . "')";              
            do_mysql_query($query, $db_server, 1);
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         if ((count_rows("daf.level", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0069: Cannot modify Level name to " . $gp['name'] . " because a Level of that name already exists");
            
         } else {
         
            $query = "UPDATE daf.level SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . "', CollectortypeID='" . $gp['collectortypeid'] .
                     "', ProxycollectorsetID='" . $gp['proxycollectorsetid'] . "', ObjecttypeID='" . $gp['objecttypeid'] . 
                     "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';      
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "delete") {
      
         if (count_rows("daf.levelinstance", "LevelID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0070: Cannot delete that Level because it still contains one or more Level Instances");
   
         } else {  

            $query = "DELETE FROM daf.level where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            
         }

         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_level();
    
   } else if ($gp['action'] === "modify") {

       modify_level($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_levels();
           
      } else {
 
         show_detailed_level($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("testlevel");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_levels
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

function show_levels() {
    
   $query = "SELECT * FROM daf.level";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Levels</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "create_level", "Create Level", "index.php?action=create&object=level");
   echo "</th>\n";

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      if ($fieldname == 'ObjecttypeID') {
          echo '<th>Associated Object Type</th>';
      } else if ($fieldname == 'CollectortypeID') {
          echo '<th>Collector Type</th>';
      } else if ($fieldname == 'ProxycollectorsetID') {
          echo '<th>Proxy Collector Set</th>';
      } else {
          echo '<th>' . $fieldname . '</th>';
      }
   }
   echo '</tr>';
   echo "\n";
   
   $objecttypeIDindex        = $fieldindex['ObjecttypeID']; 
   $objectlookup             = lookuptable("daf.objecttype", "ID", "Name");
   $collectortypeIDindex     = $fieldindex['CollectortypeID']; 
   $collectortypelookup      = lookuptable("daf.collectortype", "ID", "Name");
   $proxycollectorsetIDindex = $fieldindex['ProxycollectorsetID']; 
   $proxycollectorsetlookup  = lookuptable("daf.collectorset", "ID", "Name");
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
          echo "<td class=td_smd>";
          cellbutton(null, "level_show_$j", "show", "index.php?action=show&object=level&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          cellbutton(null, "level_modify_$j", "modify", "index.php?action=modify&object=level&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "level_delete_$j", "delete", "index.php?action=show&operation=delete&object=level&ID=$ID");
          button_with_confirm("level_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=level&ID=$ID", 
                              "index.php?action=show&object=level");
          echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $objecttypeIDindex) {
             echo '<td>'  . $objectlookup[$row[$i]] . '</td>';
          } else if ($i == $collectortypeIDindex) {
             echo '<td>'  . $collectortypelookup[$row[$i]] . '</td>';
          } else if ($i == $proxycollectorsetIDindex) {
             echo '<td>'  . $proxycollectorsetlookup[$row[$i]] . '</td>';
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
// Function:   create_level
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

function create_level() {

   echo '<div dojoType="dijit.form.Form" id="create_level" jsId="create_level" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a Level</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size=' . DAF_LEVEL_LEN . '" maxlength="' . DAF_LEVEL_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LEVEL_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_LEVEL_LEN . '}" required="true" trim="true" promptMessage="Enter a Level name" ' . 
        'invalidMessage="Invalid Test Level name (must be 1 to ' . DAF_LEVEL_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description"  size=' . DAF_LEVEL_DESCRIPTION_LEN . '" maxlength="' . DAF_LEVEL_DESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_LEVEL_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Level description" ' . 
        'invalidMessage="Invalid Test Level description (must be 1 to ' . DAF_LEVEL_DESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Associated Object Type</th><td><select name=\"objecttypeid\"/>\n";
   selectorID("daf.objecttype", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Collector Type</th><td><select name=\"collectortypeid\"/>\n";
   selectorID("daf.collectortype", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Proxy Collector Set</th><td><select name=\"proxycollectorsetid\"/>\n";
   selectorID("daf.collectorset", "Name");
   echo "</select></td></tr>\n";
   echo "</table>\n";
 
   cellbutton(null, "create_level", "Create Level", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"level\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "level_cancel", "Cancel", "index.php?object=level&action=show", null);

  echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_level
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

function show_detailed_level($ID) {

   $query = "SELECT * FROM daf.level where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Levels</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $row = mysql_fetch_row($result);
   $levelID = $row[$fieldindex['ID']];
   $collectortypeIDindex = $fieldindex['CollectortypeID']; 
   $collectortypelookup  = lookuptable("daf.collectortype", "ID", "Name");
   $objecttypeIDindex = $fieldindex['ObjecttypeID']; 
   $objecttypelookup  = lookuptable("daf.objecttype", "ID", "Name");
   $proxycollectorsetIDindex = $fieldindex['ProxycollectorsetID']; 
   $proxycollectorsetlookup  = lookuptable("daf.collectorset", "ID", "Name");

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $collectortypeIDindex) {
         echo '<th>Collector Type</th>';     
         echo '<td>'  . $collectortypelookup[$row[$i]] . '</td>';
      } else if ($i == $objecttypeIDindex) {
         echo '<th>Associated Object Type</th>';     
         echo '<td>'  . $objecttypelookup[$row[$i]] . '</td>';
      } else if ($i == $proxycollectorsetIDindex) {
         echo '<th>Proxy Collector Set</th>';     
         echo '<td>'  . $proxycollectorsetlookup[$row[$i]] . '</td>';
      } else {
         echo '<th>' . mysql_field_name($result, $i) . '</th>';
         echo '<td>'  . $row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_level
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

function modify_level($ID) {

   $query = "SELECT ID, Name, Description, ObjecttypeID, CollectortypeID, ProxycollectorsetID FROM daf.level where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_testlevel" jsId="create_testlevel" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify Level</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $collectortypeIDindex = $fieldindex['CollectortypeID']; 
   $objecttypeIDindex = $fieldindex['ObjecttypeID']; 
   $proxycollectorsetIDindex = $fieldindex['ProxycollectorsetID']; 
   
   $row = mysql_fetch_row($result);
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($i == $objecttypeIDindex) {
         echo '<th>Associated Object Type</th>';
         echo "<td><select name=\"objecttypeid\"/>\n";
         selectorID("daf.objecttype", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if ($i == $collectortypeIDindex) {
         echo '<th>Collector Type</th>';
         echo "<td><select name=\"collectortypeid\"/>\n";
         selectorID("daf.collectortype", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if ($i == $proxycollectorsetIDindex) {
         echo '<th>Proxy Collector Set</th>'; 
         echo "<td><select name=\"proxycollectorsetid\"/>\n";
         selectorID("daf.collectorset", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size=' . DAF_LEVEL_LEN . '" maxlength="' . DAF_LEVEL_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LEVEL_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_LEVEL_LEN . '}" required="true" trim="true" promptMessage="Enter a Level name" ' . 
        'invalidMessage="Invalid Test Level name (must be 1 to ' . DAF_LEVEL_LEN . ' alphanumeric characters)"/>';
      } else if ($fieldname == "Description") {
         echo '<th>Description</th>';
         echo '<td><input type="text" name="description" value="' . $row[$i] . '" size=' . DAF_LEVEL_DESCRIPTION_LEN . '" maxlength="' . DAF_LEVEL_DESCRIPTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_LEVEL_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Level description" ' . 
              'invalidMessage="Invalid Test Level description (must be 1 to ' . DAF_LEVEL_DESCRIPTION_LEN . ' characters)"/>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton("div", "modify_level", "Modify Level", null, "submit");

   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="object"    value="level" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "level_cancel", "Cancel", "index.php?object=level&action=show&ID=$ID", null);

   echo '</div>' . "\n"; 
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   levelinstance
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

function levelinstance($gp) {

   global $db_server;

   if ($gp['object'] !== "levelinstance") {
      add_error_message("DAFE0001: levelinstance: internal error - bad object - " . $gp['object']);
      $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "levelinstance";
   }
 
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         if (count_rows("daf.levelinstance", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0071: Cannot create levelinstance " . $gp['name'] . " because a levelinstance of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.levelinstance (LevelID, Value) " .
                     "VALUES('" . $gp['levelid'] . "','" . $gp['value'] . "')";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         $query = "UPDATE daf.levelinstance SET LevelID='" . $gp['levelid'] . "', Value='" . $gp['value'] .
                  "' WHERE ID='" . $gp['ID'] . "'";              
         do_mysql_query($query, $db_server, 1);
            
         $gp['action'] = 'show';      

      } else if ($gp['operation'] === "delete") {
      
         if (count_rows("daf.levelinstance", "LevelID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0072: Cannot delete that Level because it still contains one or more Levelinstances");
   
         } else { 

            $query = "DELETE FROM daf.levelinstance where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            
         }

         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_levelinstance();
    
   } else if ($gp['action'] === "modify") {

       modify_levelinstance($gp['ID']);
        
   } else if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_levelinstances();
 
      } else {
 
         show_detailed_levelinstance($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("levelinstance");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_levelinstances
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

function show_levelinstances() {
    
   $query = "SELECT * FROM daf.levelinstance";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Level Instances</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "create_levelinstance", "Create Level Instance", "index.php?action=create&object=levelinstance");
   echo "</th>\n";

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      if ($fieldname == 'LevelID') {
         echo '<th>Level</th>';
      } else {
         echo '<th>' . $fieldname . '</th>';
      }       
   }
   echo '</tr>';
   echo "\n";
   $levelIDindex = $fieldindex['LevelID']; 
   $levellookup = lookuptable("daf.level", "ID", "Name");
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
          echo "<td class=td_smd>";
          cellbutton(null, "levelinstance_show_$j", "show", "index.php?action=show&object=levelinstance&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          cellbutton(null, "levelinstance_modify_$j", "modify", "index.php?action=modify&object=levelinstance&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "levelinstance_delete_$j", "delete", "index.php?action=show&operation=delete&object=levelinstance&ID=$ID");
          button_with_confirm("levelinstance_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=levelinstance&ID=$ID", 
                              "index.php?action=show&object=level&ID=$ID");
          echo "</td>\n";

       for ($i = 0; $i < $num_fields; $i++) {
         if ($i == $levelIDindex) {
            echo '<td>'  . $levellookup[$row[$i]] . '</td>';
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
// Function:   create_levelinstance
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

function create_levelinstance() {

   echo '<div dojoType="dijit.form.Form" id="create_levelinstance" jsId="create_levelinstance" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a Level Instance</div></caption>\n";

   echo "<tr><th>Level</th><td><select name=\"levelid\"/>\n";
   selectorID("daf.level", "Name");
   echo "</select></td></tr>\n";
   echo '<tr><th>Value</th>';
   echo '<td><input type="text" name="value" size=' . DAF_LEVELINSTANCE_VALUE_LEN . '" maxlength="' . DAF_LEVELINSTANCE_VALUE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LEVELINSTANCE_VALUE_LEN . 'em;" regExp=".{1,' . DAF_LEVELINSTANCE_VALUE_LEN . '}" required="true" trim="true" promptMessage="Enter Test Level Instance value" ' . 
        'invalidMessage="Invalid Level Instance value (must be 1 to ' . DAF_LEVELINSTANCE_VALUE_LEN . ' characters)"/>';
   echo "<td></tr>\n";
   echo "</table>";
   
   cellbutton(null, "levelinstance_create", "Create Level Instance", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"levelinstance\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "levelinstance_cancel", "Cancel", "index.php?object=levelinstance&action=show", null);

   echo "</div>\n";
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_levelinstance
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

function show_detailed_levelinstance($ID) {

   $query = "SELECT levelinstance.LevelID, levelinstance.Value, level.Name as 'Level Name', objecttype.Name as 'Parent Object Name' " .
            "FROM daf.levelinstance INNER JOIN daf.level ON daf.levelinstance.levelID=daf.level.ID " .
            "INNER JOIN daf.objecttype ON daf.level.ObjecttypeID = daf.objecttype.ID " . 
            "where levelinstance.ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Level Instance</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $levelID = $fieldindex['LevelID']; 
   $levellookup = lookuptable("daf.level", "ID", "Name"); 
   
   $row = mysql_fetch_row($result);

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $levelID) {
         echo '<th>Level</th>';     
         echo '<td>' . $levellookup[$row[$i]] . '</td>';
      } else { 
         echo '<th>' . mysql_field_name($result, $i) . '</th>';
         echo '<td>' . $row[$i] . '</td>';
      } 
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_levelinstance
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

function modify_levelinstance($ID) {

   $query = "SELECT ID, LevelID, Value FROM daf.levelinstance where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_level" jsId="create_level" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Level Instances</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $levelIDindex = $fieldindex['LevelID']; 
   $levellookup = lookuptable("daf.level", "ID", "Name");
   
   $row = mysql_fetch_row($result);
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($i == $levelIDindex) {
         echo '<th>Level</th>';
         echo "<td><select name=\"levelid\"/>\n";
         selectorID("daf.level", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if ($fieldname == "Value") {
         echo '<th>Value</th>';
         echo '<td><input type="text" name="value" value="' . $row[$i] . '" size=' . DAF_LEVELINSTANCE_VALUE_LEN . '" maxlength="' . DAF_LEVELINSTANCE_VALUE_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LEVELINSTANCE_VALUE_LEN . 'em;" regExp=".{1,' . DAF_LEVELINSTANCE_VALUE_LEN . '}" required="true" trim="true" promptMessage="Enter Test Level Instance value" ' . 
              'invalidMessage="Invalid Level Instance value (must be 1 to ' . DAF_LEVELINSTANCE_VALUE_LEN . ' characters)"/><td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton("div", "levelinstance_modify", "Modify Level Instance", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="object"    value="levelinstance" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   cellbutton(null, "levelinstance_cancel", "Cancel", "index.php?object=levelinstance&action=show&ID=$ID", null);

   echo '</form>' . "\n"; 
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   objecttype
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

function objecttype($gp) {

   global $db_server;

   if ($gp['object'] !== "objecttype") {
     add_error_message("DAFE0001: objecttype: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "objecttype";
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {

         if (count_rows("daf.objecttype", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0073: Cannot create object type " . $gp['name'] . " because an object type of that name already exists");
             
         } else {
      
            $query = "CREATE TABLE daf." . $gp['name'] .  " ( 
                                           ID int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
                                           ParentID int(11) NOT NULL,
                                           Name varchar(32) NOT NULL,
                                           Comments varchar(128) NOT NULL,
                                           KEY`Name` (`Name`) 
                                         ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1";
                                            
            if (do_mysql_query($query, $db_server, 1)) {
           
               $query = "INSERT INTO daf.objecttype (Name, Description, ParentID) " .
                        "VALUES('" . $gp['name'] . "','" . $gp['description'] . "','" . $gp['parentid'] . "')";              
               do_mysql_query($query, $db_server, 1);
            } 
         }   
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf.objecttype", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0074: Cannot modify Object Type to " . $gp['name'] . " because an Object Type with that name already exists");
            
         } else {

            $query = "UPDATE daf.objecttype SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . "', Parentid='" . $gp['parentid'] .
                     "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
         }
         
         $gp['action'] = 'show';      
         $gp['ID'] = NULL; 

      } else if ($gp['operation'] === "delete") {
      
         if (count_rows("daf." . $gp['name']) > 0) {
         
            add_error_message("DAFE0075: Cannot delete that object type because one or more objects of that type still exist");
            
         } else {

            $query = "DELETE FROM daf.objecttype where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            
            $query = "DROP TABLE `" . $gp['name'] . "`";
            do_mysql_query($query, $db_server, 1);

         }
            
         $gp['action'] = 'show';
         $gp['ID'] = NULL; 
  
      }
   }

   if ($gp['action'] === "create") {
   
       create_objecttype();
    
   } else if ($gp['action'] === "modify") {

       modify_objecttype($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_objecttypes();
           
      } else {
 
         show_detailed_objecttype($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("objecttype");
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_objecttypes
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

function show_objecttypes() {
    
   $query = "SELECT * FROM daf.objecttype";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Object Types</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=4>";
   cellbutton(null, "objecttype_create", "Create Object Type", "index.php?action=create&object=objecttype");
   echo "<th>\n";

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<th>' . $fieldname . '</th>';
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $parentIDindex = $fieldindex['ParentID'];
   $objecttypelookup = lookuptable("daf.objecttype", "ID", "Name");
   $objecttypelookup[0] = "teststand"; 
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']]; 

       echo "<td class=td_smd>";
       cellbutton(null, "object_show_$j", "show", "index.php?action=show&object=objecttype&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "object_modify_$j", "modify", "index.php?action=modify&object=objecttype&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       // cellbutton(null, "object_delete_$j", "delete", "index.php?action=show&operation=delete&object=objecttype&ID=$ID&name=$name");
       button_with_confirm("object_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=objecttype&ID=$ID&name=$name", 
                           "index.php?action=show&object=object");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "object_instance_$j", "instances", "index.php?action=show&object=object&objecttype=$name");
       echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $parentIDindex) {
             if (isset($objecttypelookup[$row[$i]])) {
                echo '<td>'  . $objecttypelookup[$row[$i]] . '</td>';
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
// Function:   create_objecttype
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

function create_objecttype() {

   echo '<div dojoType="dijit.form.Form" id="create_objecttype" jsId="create_objecttype" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create an Object Type</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_OBJECTTYPE_LEN . '" maxlength="' . DAF_OBJECTTYPE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECTTYPE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OBJECTTYPE_LEN . '}" required="true" trim="true" promptMessage="Enter name of Object Type" ' . 
        'invalidMessage="Invalid Object Type (must be 1 to ' . DAF_OBJECTTYPE_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size=' . DAF_OBJECTTYPE_DESCRIPTION_LEN . '" maxlength="' . DAF_OBJECTTYPE_DESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_OBJECTTYPE_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter Object Type description" ' . 
        'invalidMessage="Invalid Object Type description (must be 1 to ' . DAF_OBJECTTYPE_DESCRIPTION_LEN . ' characters)"/>';  
   echo "</td></tr>\n";
   echo "<tr><th>Parent Object Type</th><td><select name=\"parentid\"/>\n";
   echo "<option selected value=\"0\">teststand</option>\n";
   selectorID("daf.objecttype", "Name");
   echo "</select></td></tr>\n";
   echo "</table>\n";
   
   cellbutton("div", "objecttype_create", "Create Object Type", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"objecttype\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "objecttype_cancel", "Cancel", "index.php?object=objecttype&action=show", null);

   echo "</div>\n";
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_objecttype
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

function show_detailed_objecttype($ID) {

   $query = "SELECT * FROM daf.objecttype where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Object Type</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $parentIDindex       = $fieldindex['ParentID'];
   $objecttypelookup    = lookuptable("daf.objecttype", "ID", "Name"); 
   $objecttypelookup[0] = "teststand";
   
   $row = mysql_fetch_row($result);

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($result, $i) . '</th>';
      if ($i == $parentIDindex) {
         if (isset($objecttypelookup[$row[$i]])) {
            echo '<td>'  . $objecttypelookup[$row[$i]] . '</td>';
         } else {
            echo '<td> &nbsp; </td>';
         }
      } else {
         echo '<td>'  . $row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_objecttype
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

function modify_objecttype($ID) {

   $query = "SELECT ID, ParentID, Name, Description FROM daf.objecttype where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_level" jsId="create_level" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify Object Type</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $parentIDindex = $fieldindex['ParentID'];
   $nameindex = $fieldindex['Name'];
   
   $row = mysql_fetch_row($result);
   
   $originalname = $row[$nameindex];
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($i == $parentIDindex) {
         echo '<th>Parent Object Type</th>';
         echo "<td><select name=\"parentid\"/>\n";
         if ($row[$i] == 0) {
            echo "<option selected value=\"0\">teststand</option>\n";
         } else {
            echo "<option value=\"0\">teststand</option>\n";
         } 
         selectorID("daf.objecttype", "Name", $row[$i]);
         echo "</select></td>";  
      } else if ($fieldname == "Name") {
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size="' . DAF_OBJECTTYPE_LEN . '" maxlength="' . DAF_OBJECTTYPE_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECTTYPE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OBJECTTYPE_LEN . '}" required="true" trim="true" promptMessage="Enter name of Object Type" ' . 
              'invalidMessage="Invalid Object Type (must be 1 to ' . DAF_OBJECTTYPE_LEN . ' alphanumeric characters)"/>';
      } else if ($fieldname == "Description") {
         echo '<th>Description</th>';
         echo '<td><input type="text" name="description" value="' . $row[$i] . '" size=' . DAF_OBJECTTYPE_DESCRIPTION_LEN . '" maxlength="' . DAF_OBJECTTYPE_DESCRIPTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_OBJECTTYPE_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter Object Type description" ' . 
              'invalidMessage="Invalid Object Type description (must be 1 to ' . DAF_OBJECTTYPE_DESCRIPTION_LEN . ' characters)"/>';  
      }
      echo "</tr>\n";
   }
   echo '</table>';
   
   cellbutton("div", "objecttype_modify", "Modify Object Type", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="object"    value="objecttype" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "objecttype_cancel", "Cancel", "index.php?object=objecttype&action=show&ID=$ID", null);

   echo "</div>\n";
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   object
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

function object($gp) {

  global $db_server;

  if ($gp['object'] !== "object") {
     add_error_message("DAFE0001: object: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "object";
   }
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf." . $gp['objecttype'], "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0076: Cannot create object " . $gp['name'] . " of type " . $gp['objecttype'] . " because an object of that name and type already exists");
             
         } else {
      
            $fieldnames = get_object_field_names($gp['objecttype']);
         
            $fields = array("Name", "Comments", "ParentID");
            $values = array($gp['name'], $gp['comments'], $gp['parentid']);
         
            for ($i=4; $i<count($fieldnames); $i++) {
               array_push($fields, ucfirst(strtolower($fieldnames[$i])));
               array_push($values, $gp[strtolower($fieldnames[$i])]);         
            }
      
            $query = "INSERT INTO daf." . $gp['objecttype'] . " (" . join($fields, ",") . ") VALUES('" . join($values, "','") . "')";
            do_mysql_query($query, $db_server, 1);
            
         }
         
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf." . $gp['objecttype'], "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0077: Cannot modify Object name to " . $gp['name'] . " because an Object with that name already exists");
            
         } else {
      
            $fieldnames = get_object_field_names($gp['objecttype']);
            $fields = array("ParentID", "Name", "Comments");
            $values = array($gp['parentid'], $gp['name'], $gp['comments']);
         
            for ($i=4; $i<count($fieldnames); $i++) {
               array_push($fields, ucfirst(strtolower($fieldnames[$i])));
               array_push($values, $gp[strtolower($fieldnames[$i])]);         
            }

            $setstring = "";
            for ($i=0; $i<count($fields); $i++) {
               $setstring .= $fields[$i] . "='" . $values[$i] . "'";
               if ($i != (count($fields)-1)) {
                  $setstring .= ",";
               }
            }

            $query = "UPDATE daf." .  $gp['objecttype'] . " SET " . $setstring . " WHERE ID='" . $gp['ID'] . "'";              
            if (! do_mysql_query($query, $db_server, 1)) {
               return;
            }    
         
            if (isset($gp['attribute_id'])) {
         
               foreach ($gp['attribute_id'] as $index => $value) {            
                  $query = "UPDATE daf.objectattributevalue SET Varcharvalue='$value' WHERE ID='$index'";              
                  if (! do_mysql_query($query, $db_server)) {
                     return;
                  }                 
               }  
            }
                 
         }
         $gp['action'] = 'show';      

      } else if ($gp['operation'] === "delete") {
      
         /* if (count_rows("daf.objecttype", "ParentID = '" . $gp['ID'] . "'") > 0) {
         
            echo "<pre>\nCannot delete that object type because it contains one or more child object types\n</pre>\n";
         
            $gp['action'] = 'show';
            $gp['ID'] = NULL;             <<<<<<<<<< test needed here
            
          } else { */

            $query = "DELETE FROM daf." . $gp['objecttype'] . " where ID='" . $gp['ID'] . "'";    
            if (! do_mysql_query($query, $db_server)) {
               return;
            } 
            
            $gp['action'] = 'show';
            $gp['ID'] = NULL; 

        /* } */
             
      } else if ($gp['operation'] === "query") {

         show_query_objectresults($gp);
         return;
         
      }

   }
    
   if ($gp['action'] === "create") {
   
       create_object($gp['objecttype']);
    
   } else if ($gp['action'] === "modify") {

       modify_object($gp['objecttype'], $gp['ID']);
        
   } else if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_objects($gp['objecttype']);
         
      } else {
 
         show_detailed_object($gp['objecttype'], $gp['ID']);
      }
               
   } else if ($gp['action'] === "query") {
   
      query_object();  
   
   } else {
    
      report_bad_parms("object");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_objects
//
// Inputs:     $objecttype
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// All objects must have the following 4 initial fields: ID, ParentID, Name, Comments - fields after that are user 
// added fields of which there may be 0 or more. 
//
//----------------------------------------------------------------------------------------------------------------------

function show_objects($objecttype) {

   $query = "SELECT * FROM daf." . $objecttype;
   $result = mysql_query($query);
   if (! $result) 
      die ("show_objects: Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);
   
   $parentobjecttype = determine_parent_object_type($objecttype);
   
   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>" . capitalise_and_pluralise_object($objecttype) . "</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo '<th> &nbsp; </th>';
   echo '<th> &nbsp; </th>';
   echo '<th> &nbsp; </th>';

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      if ($fieldname == 'ParentID') {
         echo '<th>Parent ' . $parentobjecttype . '</th>';
      } else {
         echo '<th>' . $fieldname . '</th>';
      }
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $parentIDindex = $fieldindex['ParentID'];
   $parentobjectlookup = lookuptable("daf." . $parentobjecttype, "ID", "Name"); 
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']]; 
          echo "<td class=td_smd>";
          cellbutton(null, "objecttype_show_$j", "show", "index.php?action=show&object=object&objecttype=" . $objecttype . "&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          cellbutton(null, "objecttype_modify_$j", "modify", "index.php?action=modify&object=object&objecttype=" . $objecttype . "&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "objecttype_delete_$j", "delete", "index.php?action=show&operation=delete&object=object&objecttype=" . $objecttype . "&ID=$ID&name=$name");
          button_with_confirm("objecttype_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=object&objecttype=" . $objecttype . "&ID=$ID&name=$name", 
                              "index.php?action=show&object=object&objecttype=" . $objecttype);
          echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $parentIDindex) {
             if (isset($parentobjectlookup[$row[$i]])) {
                echo '<td>' . $parentobjectlookup[$row[$i]] . '</td>';
             } else {
                echo '<td> &nbsp;' . $parentobjectlookup[$row[$i]] . ' </td>';
             }
          } else {
             echo '<td>' . $row[$i] . '</td>';
          }
       }  
       
       echo '</tr>';
       echo "\n";

   }
   echo '</table>';
   
   $objecttypeID = get_field_from_row("daf.objecttype", "ID", "Name='" . $objecttype . "'");           

   echo '<div class="floatleft">';
   echo '<form action="index.php" method="post">' . "\n";
   cellbutton(null, "objecttype_modify", "Create New " . ucfirst($objecttype) . " Object", null, "submit");
   echo '<input type="hidden" name="action" value="create" />' . "\n";
   echo '<input type="hidden" name="object" value="object" />' . "\n";
   echo '<input type="hidden" name="objecttype" value="' . $objecttype . '" />' . "\n";
   echo '</form>' . "\n"; 
   echo '</div>'; 
        
   echo '<div class="floatleft">';
   echo '<form action="index.php" method="post">' . "\n";
   cellbutton(null, "objecttype_addattribute", "Add New Attribute", null, "submit");
   echo '<input type="hidden" name="action" value="create" />' . "\n";
   echo '<input type="hidden" name="object" value="objectattributetype" />' . "\n";
   echo '<input type="hidden" name="objecttypeID" value="' . $objecttypeID . '" />' . "\n";
   echo '</form>' . "\n"; 
   echo '</div>'; 
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_object
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

function create_object($objecttype) {

   $objecttypeID      = get_field_from_row("daf.objecttype", "ID", "Name='$objecttype'");
   $parentobjecttype  = determine_parent_object_type($objecttype);
   $fieldnames        = get_object_field_names($objecttype);   
   $fieldtypes        = get_field_types("daf." . $objecttype);

   echo '<div dojoType="dijit.form.Form" id="create_object" jsId="create_object" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a " . ucfirst($objecttype) . "</div></caption>\n";

   echo "<tr><th>Name</th>";
   echo '<td><input type="text" name="name" size=' . DAF_OBJECT_LEN . '" maxlength="' . DAF_OBJECT_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECT_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OBJECT_LEN . '}" required="true" trim="true" promptMessage="Enter name of Object" ' . 
        'invalidMessage="Invalid Objec Type (must be 1 to ' . DAF_OBJECT_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Comments</th><td><input type="text" name="comments" size=' . DAF_OBJECT_COMMENTS_LEN . '" maxlength="' . DAF_OBJECT_COMMENTS_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%;" regExp=".{1,' . DAF_OBJECT_COMMENTS_LEN . '}" required="true" trim="true" promptMessage="Enter Object comments" ' . 
        'invalidMessage="Invalid Object comments (must be 1 to ' . DAF_OBJECT_COMMENTS_LEN . ' characters)"/>';
   echo "<td></tr>\n";
   echo "<tr><th>Object Type</th><td>" . $objecttype . "<td></tr>\n";
   echo "<tr><th>Parent " . $parentobjecttype . "</th><td><select name=\"parentid\">\n";
   selectorID("daf." . $parentobjecttype, "Name");                               /* <<<<, will vary according to objecttype */
   echo "</select></td></tr>\n";
   
   for ($i=4; $i<count($fieldnames); $i++) {
      if ($fieldtypes[$fieldnames[$i]] == "var") {
         echo "<tr><th>" . $fieldnames[$i] . "</th><td><input type=\"text\" name=\"" . strtolower($fieldnames[$i]) . "\"/><td></tr>\n";
      } else if ($fieldtypes[$fieldnames[$i]] == "enu") {
         echo "<tr><th>" . $fieldnames[$i] . "</th><td>";
         selector_for_enum("daf", $objecttype, $fieldnames[$i], strtolower($fieldnames[$i]));
         echo "</td><tr>\n";
      } else {
         /* problem <<<< */
      }
   }
   
   echo "</table>";

   cellbutton(null, "object_create", "Create " . ucfirst($objecttype), null, "submit");
   echo "<input type=\"hidden\" name=\"action\"       value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"       value=\"object\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"    value=\"create\"/>\n";
   echo "<input type=\"hidden\" name=\"objecttype\"   value=\"" . $objecttype . "\"/>\n";
   echo "<input type=\"hidden\" name=\"objecttypeid\" value=\"" . $objecttypeID . "\"/>\n";                      
   cellbutton(null, "object_cancel", "Cancel", "index.php?object=object&action=show&objecttype=$objecttype&objecttypeID=$objecttypeID", null);
   echo "</div>\n";   

   echo '<div class="floatleft">';                /* <<<<<<<<<<<<<<<< need to keep this button !!!!!!!!!!!!!!! ?? */
   objecttypes_button();   
   echo "</div>\n";
   
   echo '<div class="floatleft">';                /* <<<<<<<<<<<<<<<< need to keep this button !!!!!!!!!!!!!!! ?? */
   objects_button($objecttype);   
   echo "</div>\n";   
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_object
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

function show_detailed_object($objecttype, $ID) {

   $query = "SELECT * FROM daf." . $objecttype . " where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);
   
   $parentobjecttype = determine_parent_object_type($objecttype);
   
   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>" . ucfirst($objecttype) . "</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $parentIDindex = $fieldindex['ParentID'];
   $parentobjectlookup = lookuptable("daf." . $parentobjecttype, "ID", "Name"); 
   
   $row = mysql_fetch_row($result);
   $ID = $row[$fieldindex['ID']];

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $parentIDindex) {
         echo '<th>Parent ' . $parentobjecttype . '</th>';
         if (isset($parentobjectlookup[$row[$i]])) {
            echo '<td>'  . $parentobjectlookup[$row[$i]] . '</td>';
         } else {
            echo '<td> &nbsp; </td>';
         }
      } else {
         echo '<th>' . mysql_field_name($result, $i) . '</th>';
         echo '<td>'  . $row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   
   echo '</table>';
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_object
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

function modify_object($objecttype, $ID) {

   $query = "SELECT * FROM daf." . $objecttype . " where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);
   
   $parentobjecttype = determine_parent_object_type($objecttype);
   $fieldnames       = get_object_field_names($objecttype);
   
   echo '<div dojoType="dijit.form.Form" id="create_level" jsId="create_level" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify Object - " . ucfirst($objecttype) . "</div></caption>\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $parentIDindex = $fieldindex['ParentID'];
   $parentobjectlookup = lookuptable("daf." . $parentobjecttype, "ID", "Name");
   
   $objecttypeID = lookupID("objecttype", $objecttype);
   $objectattribute_types = get_objectattribute_types($objecttypeID);
//   eg  get_objectattribute_types("clusters)  might return   
//             [Model=>["Type"=>"varchar", Varcharwidth=>"32", Enumvalues=>""],
//             [Jff=>["Type"=>"enum", Varcharwidth=>"", Enumvalues=>"'fred','bill','jim'"]]

   $row = mysql_fetch_row($result);
 
   for ($i = 0; $i < $num_fields; $i++) {
   
      $fieldname = mysql_field_name($result, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == 'ParentID') {
         echo '<th>Parent ' . $parentobjecttype . '</th>';
         echo "<td><select name=\"parentid\"/>\n";
         selectorID("daf." . $parentobjecttype, "Name", $row[$i]);
         echo "</select></td>";  
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo "<th>Name</th>";
         echo '<td><input type="text" value="' . $row[$i] . '" name="' . strtolower($fieldname) . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECT_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OBJECT_LEN . '}" required="true" trim="true" promptMessage="Enter Object Name" ' . 
              'invalidMessage="Invalid Object Name (must be 1 to ' . DAF_OBJECT_LEN . ' alphanumeric characters)"/></td>';
      } else if ($fieldname == "Comments") {
         echo "<th>Comments</th>";
         echo '<td><input type="text" value="' . $row[$i] . '" name="' . strtolower($fieldname) . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:100%;" regExp=".{1,' . DAF_OBJECT_COMMENTS_LEN . '}" required="true" trim="true" promptMessage="Enter Object comments" ' . 
              'invalidMessage="Invalid Object comments (must be 1 to ' . DAF_OBJECT_COMMENTS_LEN . ' characters)"/></td>';
      } else if ($objectattribute_types[$fieldname]['Type'] == "varchar") {
         $vw = $objectattribute_types[$fieldname]['Varcharwidth'];
         echo "<th>" . $fieldname . "</th>";
         echo '<td><input type="text" value="' . $row[$i] . '" name="' . strtolower($fieldname) . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . $vw . 'em;" regExp=".{1,' . $vw . '}" required="true" trim="true" promptMessage="Enter value for Object ' . $fieldname  . '" ' . 
              'invalidMessage="Invalid Object ' . $fieldname . ' value (must be 1 to ' . $vw . ' characters)"/></td>';
      } else if ($objectattribute_types[$fieldname]['Type'] == "enum") {
         echo "<th>" . $fieldname . "</th><td>";
         selector_for_enum("daf", $objecttype, mysql_field_name($result, $i), strtolower(mysql_field_name($result, $i)));
         echo "</td>";
      } else {
         /* problem <<<< */    echo "XXXX: $fieldname ";
      }

      echo "</tr>\n";
      
   }
   
   echo '</table>';
   
   cellbutton("div", "objecttype_modify", "Modify " . ucfirst($objecttype), null, "submit");
   echo '<input type="hidden" name="action"       value="show" />' . "\n";
   echo '<input type="hidden" name="operation"    value="modify" />' . "\n";
   echo '<input type="hidden" name="object"       value="object" />' . "\n";
   echo '<input type="hidden" name="ID"           value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="objecttype"   value="' . $objecttype . '" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname . '" />' . "\n";
   cellbutton(null, "objecttype_cancel", "Cancel", "index.php?object=objecttype&action=show&ID=$ID", null);
   
   echo "<div>\n";  
   /* <<<<<<<<<<<<< <<<<<<<<<<<<<<<< <<<<<<<<<<<<<<<<<<< should we be freeing the $result variable ????? */
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_object
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// 
//
//
//---------------------------------------------------------------------------------------------------------------------
 
function query_object() {

echo <<<_END
   <script>
      dojo.ready(function() {    
         selectObjectAttributes('objecttypesel')        
       });
   </script>
_END;

   $objecttype = "cluster";  /* <<<<<<<<<<<< */

   $fieldnames = get_object_field_names($objecttype);    /* this returns  Array = ("ID", "ParentID", "Name", "Comments, ....); */
                                                         /* all objects have ID, ParentID, Name and Comments as attributes, and may */
                                                         /* have further user added attributes  */

   echo "<form action=\"index.php\" method=\"post\">\n";
   
   echo '<table id="queryobjecttableid" >';
   echo "<caption class=\"cap1\"><div>Query Objects</div></caption>\n";
   
   echo "<tr>";
   cellbutton("th", "object_query", "Query Object", null, "submit");
   echo "<th>Field</th><th>Operator</th><th>Value</th></tr>\n";

   echo "<tr><td></td><td>Object Type</td>"; 
   echo "<td><input type=\"text\" value=\"=\" readonly=\"true\" /></td>\n";
   
   echo "<td><select id=\"objecttypesel\" name=\"objecttypeID\" onchange=\"selectObjectAttributes('objecttypesel')\"/>\n";
   selectorID("daf.objecttype", "Name");
   echo "</select></td></tr>\n";

   echo "<tr><td></td>";
   echo "<td>Object Name</td>";
   echo "<td>";
   selectorOPvarchar(0, "=");                             // <<<< field size below 
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[0]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[0]\" value=\"Name\"/></td></tr>\n";
   
   echo "<tr><td>";
   selectorANDOR(1);
   echo "</td><td>Comments</td>";
   echo "<td>";
   selectorOPvarchar(1, "=");
   echo "</td>";
   echo "<td><input type=\"text\" name=\"value[1]\" size = \"" . DAF_SCENARIORESULT_NAME_LEN . "\" maxlength=\"" . DAF_SCENARIORESULT_NAME_LEN . "\" />";
   echo "<input type=\"hidden\" name=\"field[1]\" value=\"Comments\"/></td></tr>\n"; 

   echo "</table>\n";

   echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"object\"    value=\"object\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"query\" />\n";
   
   echo "</form>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_query_objectresults
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

function show_query_objectresults($gp) {

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
   
   $objecttypenamelookup   = lookuptable("daf.objecttype", "ID", "Name");
   $objecttypename         = $objecttypenamelookup[$gp['objecttypeID']];
   $parent_chain           = determine_parent_chain($objecttypename);
   $parent_object_type     = $parent_chain[count($parent_chain)-2];
   $parentobjectnamelookup = lookuptable("daf." . $parent_object_type, "ID", "Name");
   
   $objecttable = "daf." . $objecttypename;
   
   $query = "SELECT * FROM $objecttable $where";  
   
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Objects - $query</div></caption>\n";

   $fieldindex = array();
          
   echo '<tr>';
   for ($i = 0; $i < $num_fields; $i++) {
       $fieldname = mysql_field_name($result, $i);
        $fieldindex[$fieldname] = $i;
       if ($fieldname == "ParentID") {
          echo '<th>Parent Object Type</th>';
          echo '<th>Parent Object Name</th>';
       } else { 
          echo '<th>' . $fieldname . '</th>';
       }
   }
   echo '</tr>' . "\n";
   
   $parentidindex   = $fieldindex['ParentID'];
   $nameindex       = $fieldindex['Name'];

   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
                        
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $nameindex ) {
             echo '<td class=td_smd><a href=index.php?action=show&object=' . "cluster" . '&ID=' . $ID . '>' .  $row[$i] . '</a></td>';
          } else if ($i == $parentidindex ) {
             echo '<td>' . $parent_object_type . '</td>';
             echo '<td class=td_smd><a href=index.php?action=show&object=' . $parent_object_type . '&ID=' . $row[$i] . '>' .  $parentobjectnamelookup[$row[$i]] . '</a></td>';
          } else {                       
             echo '<td>'  . $row[$i] . '</td>';
          }
       }   
       echo "</tr>\n";

    }
    echo '</table>';
         
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   objectattributetype
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

function objectattributetype($gp) {

  global $db_server;

  if ($gp['object'] !== "objectattributetype") {
     add_error_message("DAFE0001: objectattributetype: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "objectattributetype";
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.objectattributetype", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0078: Cannot create object attribute type " . $gp['name'] . " because an object attribute type of that name already exists");
             
         } else {

            $query = "INSERT INTO daf.objectattributetype (Name, ObjecttypeID, Type, Varcharwidth, Enumvalues) " .
                     "VALUES('" . ucfirst(strtolower($gp['name'])) . "','" . $gp['objecttypeid'] . "','" . $gp['type'] . "','" . $gp['varcharwidth'] . "','" . $gp['enumvalues'] . "')";              
            if (do_mysql_query($query, $db_server, 1)) {
         
               $objecttypename = get_field_from_row("daf.objecttype", "Name", "ID = " . $gp['objecttypeid']);

               $query = null;         
               if ($gp['type'] == "varchar") {
                  $query = "ALTER TABLE daf." . $objecttypename . " ADD " . ucfirst(strtolower($gp['name'])) . " VARCHAR(" . $gp['varcharwidth'] . ") NOT NULL";
               } else if ($gp['type'] == "enum") {
                  $query = "ALTER TABLE daf." . $objecttypename . " ADD " . ucfirst(strtolower($gp['name'])) . " ENUM(" . stripslashes($gp['enumvalues']) . ") NOT NULL"; 
                  /* <<< do we need to use stripslashes elsewhere ?? <<<<< see above */
               } else {
                  add_error_message("DAFE0079: unknown field type = " . $gp['type']);
               }
            
               if (isset($query)) {
                  do_mysql_query($query, $db_server);
               } 
            }
            
         }   
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
            
         if ((count_rows("daf." . $gp['objectattributetype'], "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0080: Cannot modify Object name to " . $gp['name'] . " because an Object with that name already exists");
      
         } else { 
         
            $query = "UPDATE daf.objectattributetype SET Name='" . ucfirst(strtolower($gp['name'])) . "', ObjecttypeID='" . $gp['objecttypeid'] . "', Type='" . $gp['type'] .
                     "', Varcharwidth='" . $gp['varcharwidth'] . "', Enumvalues='" . $gp['enumvalues'] . "' WHERE ID='" . $gp['ID'] . "'";              
                      
            if (do_mysql_query($query, $db_server, 1)) {
        
               $objecttypename = get_field_from_row("daf.objecttype", "Name", "ID = " . $gp['objecttypeid']);
     
               if ($gp['type'] == "varchar") {
                  $query = "ALTER TABLE daf." . $objecttypename . " CHANGE " . ucfirst(strtolower($gp['name'])) . " " . ucfirst(strtolower($gp['name'])) . " VARCHAR(" . $gp['varcharwidth'] . ") NOT NULL";
               } else if ($gp['type'] == "enum") {
                  $query = "ALTER TABLE daf." . $objecttypename . " CHANGE " . ucfirst(strtolower($gp['name'])) . " " . ucfirst(strtolower($gp['name'])) . " ENUM(" . stripslashes($gp['enumvalues']) . ") NOT NULL"; 
                  /* <<< do we need to use stripslashes elsewhere ?? <<<<< see above */
               } else {
                  add_error_message("DAFE0079: Unknown field type = " . $gp['type']);
                  return;
               }
       
               if (! do_mysql_query($query, $db_server, 1)) {
                  return;                                                              /* do we need to delete the entry we just made in daf.objectattributetype <<<<< ? */
               }
               
            }
             
         }

         $gp['action'] = 'show';      
         $gp['ID'] = null;      

      } else if ($gp['operation'] === "delete") {
      
          /*  if (count_rows("daf." . $gp['name']) > 0) {
         
               echo "<pre>\nCannot delete that object type because one or more objects of that type still exist\n</pre>\n";
           
               $gp['action'] = 'show';
               $gp['ID'] = NULL; 
            
             } else {   */  /* <<<<<<<<<  what check is needed here *?  */
             
             $query = "ALTER TABLE daf." . $gp['objecttypename'] . " DROP " . $gp['name'];
             if (do_mysql_query($query, $db_server, 1)) {
             
                $query = "DELETE FROM daf.objectattributetype where ID='" . $gp['ID'] . "'";    
                do_mysql_query($query, $db_server);
             }
             
             $gp['action'] = 'show';
             $gp['ID'] = NULL;
               
         /*   }  */
  
      }
   }

   if ($gp['action'] === "create") {
   
       if (isset($gp['objecttypeID'])) {
          create_objectattributetype($gp['objecttypeID']);
       } else {
          create_objectattributetype();
       }
   
   } else if ($gp['action'] === "modify") {

       modify_objectattributetype($gp['ID']);
        
   } else if ($gp['action'] === "show") {
   
      if (! isset($gp['ID'])) {
      
         show_objectattributetypes();
 
      } else {
 
         show_detailed_objectattributetype($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("objectattributetype");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_objectattributetypes
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

function show_objectattributetypes() {

   $query = "SELECT * FROM daf.objectattributetype";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Object Attribute Types</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "objectattributetype_create", "Create Object Attribute Type", "index.php?action=create&object=objectattributetype");
   echo "</th>\n";

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      if ($fieldname == 'ObjecttypeID') {
         echo '<th>Object Type</th>';
      } else if ($fieldname == 'Varcharwidth') {
         echo '<th>Varchar width</th>';
      } else if ($fieldname == 'Enumvalues') {
         echo '<th>Enum values</th>';    
      } else {
         echo '<th>' . $fieldname . '</th>';
      }
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $objecttypeIDindex   = $fieldindex['ObjecttypeID'];
   $objecttypelookup    = lookuptable("daf.objecttype", "ID", "Name");
   $objecttypelookup[0] = "teststand";
   $typeindex           = $fieldindex['Type'];
   $varcharwidthindex   = $fieldindex['Varcharwidth'];
   $enumvaluesindex     = $fieldindex['Enumvalues'];
 
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       
       $type = $row[$typeindex];

       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']]; 
       $objecttypename = $objecttypelookup[ $row[ $fieldindex['ObjecttypeID']]];
       
          echo "<td class=td_smd>";
          cellbutton(null, "objectattributetypes_show_$j", "show", "index.php?action=show&object=objectattributetype&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          cellbutton(null, "objectattributetypes_modify_$j", "modify", "index.php?action=modify&object=objectattributetype&ID=$ID");
          echo "</td>\n";
          echo "<td class=td_smd>";
          // cellbutton(null, "objectattributetypes_delete_$j", "delete", "index.php?action=show&operation=delete&object=objectattributetype&ID=$ID&name=$name&objecttypename=$objecttypename");
          button_with_confirm("objectattributetypes_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=objectattributetype&ID=$ID&name=$name&objecttypename=$objecttypename", 
                              "index.php?action=show&objectattribytetype");
          echo "</td>\n";
       
       for ($i = 0; $i < $num_fields; $i++) {
          if ($i == $objecttypeIDindex) {
             if (isset($objecttypelookup[$row[$i]])) {
                echo '<td>'  . $objecttypelookup[$row[$i]] . '</td>';
             } else {
                echo '<td> &nbsp; </td>';
             }                
          } else if ($i == $varcharwidthindex) {
             if ($type == 'varchar') {
                echo '<td>' . $row[$i] . '</td>';
             } else {
                echo '<td> &nbsp; </td>';
             }
          } else if ($i == $enumvaluesindex) {
             if ($type == 'enum') {
                echo '<td>' . $row[$i] . '</td>';
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
// Function:   create_objectattributetype
//
// Inputs:     $objecttype
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//---------------------------------------------------------------------------------------------------------------------

function create_objectattributetype($objecttypeID = NULL) {

echo <<<_END
   <script>
      dojo.ready(function() {    
         showVarcharOrEnum('objectattributetype1')        
       });
   </script>
_END;

   echo '<div dojoType="dijit.form.Form" id="create_objectattributetype" jsId="create_objectattributetype" encType="multipart/form-data" action="index.php" method="post">';
   echo <<<_END
      <script type="dojo/method" event="onSubmit">
        if (! this.validate()) {
            alert('Form cannot be submitted as it contains one or more invalid fields');
            return false;
        }
        return true;
    </script>
_END;

   echo '<table id="createobjectattributetableid" >';
   echo "<caption class=\"cap1\"><div>Create an Object Attribute Type</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size=' . DAF_OBJECTATTRIBUTETYPE_LEN . '" maxlength="' . DAF_OBJECTATTRIBUTETYPE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECTATTRIBUTETYPE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OBJECTATTRIBUTETYPE_LEN . '}" required="true" trim="true" promptMessage="Enter name of Object Attribute Type" ' . 
        'invalidMessage="Invalid Attribute Object Type (must be 1 to ' . DAF_OBJECTATTRIBUTETYPE_LEN . ' alphanumeric characters)"/></td>';
   echo "</td></tr>\n";
   echo "<tr><th>Object Type</th><td><select name=\"objecttypeid\"/>\n";
   selectorID("daf.objecttype", "Name", $objecttypeID);
   echo "</select></td></tr>\n";
   echo "<tr><th>Type</th><td><select id=\"objectattributetype1\" name=\"type\" onchange=\"showVarcharOrEnum('objectattributetype1')\" />\n";
   echo "<option selected value=\"varchar\">varchar</option>\n";
   echo "<option          value=\"enum\">enum</option>\n";
   echo "</select></td></tr>\n";
   echo '<tr><th id="val1">Varchar width</th>';
   echo '<td id="td1"><input id="inp1" type="text" name="varcharwidth" /></td>';
 //  echo '<td><input id="inp1" type="text" name="varcharwidth" ' .
  //      'dojoType="dijit.form.ValidationTextBox" regExp="\d+" required="true" trim="true" promptMessage="Enter varchar field with for Object Attribute Type" ' . 
   //     'invalidMessage="Invalid Object Attribute Varchar width (must be a decimal number)"/></td>';
   echo "</tr>\n";

   echo "</table>\n";\
   
   cellbutton(null, "objectattributetype_create", "Create Object Attribute Type", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"objectattributetype\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "objectattributetype_cancel", "Cancel", "index.php?object=objectattributetype&action=show", null);

   echo "</div>\n";
      
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_objectattributetype
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

function show_detailed_objectattributetype($ID) {

   $query = "SELECT * FROM daf.objectattributetype where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Object Attribute Type</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $objecttypeIDindex   = $fieldindex['ObjecttypeID'];
   $objecttypelookup    = lookuptable("daf.objecttype", "ID", "Name"); 
   $objecttypelookup[0] = "teststand";
   $typeindex           = $fieldindex['Type'];
   $varcharwidthindex   = $fieldindex['Varcharwidth'];
   $enumvaluesindex     = $fieldindex['Enumvalues'];
   
   $row = mysql_fetch_row($result);
   
   $type = $row[$typeindex];

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<tr>';
      if ($i == $objecttypeIDindex) {
          echo '<th>Object Type</th>';
         if (isset($objecttypelookup[$row[$i]])) {
            echo '<td>'  . $objecttypelookup[$row[$i]] . '</td>';
         } else {
            echo '<td> &nbsp; </td>';
         }
      } else if ($i == $varcharwidthindex) {
          echo '<th>Varchar width</th>';
          if ($type == 'varchar') {
             echo '<td>' . $row[$i] . '</td>';
          } else {
             echo '<td> &nbsp; </td>';
          }
      } else if ($i == $enumvaluesindex) {
          echo '<th>Enum values</th>';
          if ($type == 'enum') {
             echo '<td>' . $row[$i] . '</td>';
          } else {
             echo '<td> &nbsp; </td>';
          }
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>'  . $row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_objectattributetype
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

function modify_objectattributetype($ID) {

   $query = "SELECT ID, Name, ObjecttypeID, Type, Varcharwidth, Enumvalues FROM daf.objectattributetype where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="create_level" jsId="create_level" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify Object Attribute Type</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex           = $fieldindex['ID'];
   $objecttypeIDindex = $fieldindex['ObjecttypeID'];
   $typeindex         = $fieldindex['Type'];
   $varcharwidthindex = $fieldindex['Varcharwidth'];
   $enumvaluesindex   = $fieldindex['Enumvalues'];
   
   $row = mysql_fetch_row($result);
   
   $type = $row[$typeindex];
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size=' . DAF_OBJECTATTRIBUTETYPE_LEN . '" maxlength="' . DAF_OBJECTATTRIBUTETYPE_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECTATTRIBUTETYPE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OBJECTATTRIBUTETYPE_LEN . '}" required="true" trim="true" promptMessage="Enter the Object Attribute Type name" ' . 
              'invalidMessage="Invalid Attribute Object Type (must be a 1 to ' . DAF_OBJECTATTRIBUTETYPE_LEN . ' alphanumeric characters)"/></td>';
      } else if ($i == $objecttypeIDindex) {
         echo '<th>Object Type</th>';
         echo "<td><select name=\"objecttypeid\"/>\n";
         selectorID("daf.objecttype", "Name", $row[$i]);
         echo "</select></td>";  
      } else if ($i == $typeindex) {
         echo "<th>Type</th>";
         echo "<td><input type=\"text\" value=\"" . $row[$i] . "\" readonly=\"true\" </td>\n";
         echo '</tr>';
         if ($row[$i] == "varchar") {
            echo "<tr><th>Varchar width</th>";
            echo '<td><input type="text" name="varcharwidth" value="' . $row[$varcharwidthindex] . '" size="10" maxlength="10" ' .
                 'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECTATTRIBUTETYPE_VARCHARWIDTH_LEN . 'em;" regExp="\d{1,' . DAF_OBJECTATTRIBUTETYPE_VARCHARWIDTH_LEN . '}" required="true" trim="true" promptMessage="Enter character width of Object Attribute Type" ' . 
                 'invalidMessage="Invalid Attribute Object Type (must be a decimal number greater than 0)"/></td>' . "\n";
         } else {         
            echo "<tr><th>Enum values</th>";
            echo '<td><input type="text" name="enumvalues" value="' . $row[$ienumvaluesindex] . '" size=' . DAF_OBJECTATTRIBUTETYPE_ENUMVALUES_LEN . '" maxlength="' . DAF_OBJECTATTRIBUTETYPE_ENUMVALUES_LEN . '" ' .
                 'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OBJECTATTRIBUTETYPE_ENUMVALUES_LEN . 'em;" regExp="(\'.+\',)*\'.+\'?" required="true" trim="true" promptMessage="Enter list of comma separated, quote delimited values, eg \'red\',\'green\',\'blue\'" ' . 
                 'invalidMessage="Invalid Attribute Object Type (must be 1 to ' . DAF_OBJECTATTRIBUTETYPE_ENUMVALUES_LEN . ' characters in a list of comma separated, quote delimited values, eg \'red\',\'green\',\'blue\')"/></td>' . "\n";
         }
      }
      echo "</tr>\n";

   }
   echo '</table>';
   
   cellbutton("div", "objectattributetype_modify", "Modify Object Attribute Type", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="object"    value="objectattributetype" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="hidden" name="originalname" value="' . $originalname .'" />' . "\n";
   cellbutton(null, "objectattributetype_cancel", "Cancel", "index.php?object=objectattributetype&action=show&ID=$ID", null);

   echo "</div>\n";
}


   

 //---------------------------------------------------------------------------------------------------------------------
//
// Function:   get_object_field_names
//
// Inputs:     $objecttype
//
// Outputs:    none
//
// Returns:    an array of the field names for that object  
//
// Description:
//
// All objects must have the following 4 initial fields: ID, ParentID, Name, Comments - fields after that are user 
// added fields of which there may be 0 or more. 
//
//  Eg    get_object_field_names("cluster") might return
//
//  Array = ("ID", "ParentID", "Name", "Comments, "Model", "Serial");
//
// The SHOW COLUMNS command returns data in the following format:
//
// +----------+--------------+------+-----+---------+----------------+
// | Field    | Type         | Null | Key | Default | Extra          |
// +----------+--------------+------+-----+---------+----------------+
// | ID       | int(11)      | NO   | PRI | NULL    | auto_increment |
// | ParentID | int(11)      | NO   |     | NULL    |                |
// | Name     | varchar(32)  | NO   | MUL | NULL    |                |
// | Comments | varchar(128) | NO   |     | NULL    |                |
// | Model    | varchar(32)  | NO   |     | NULL    |                |
// | Serial   | varchar(32)  | NO   |     | NULL    |                |
// +----------+--------------+------+-----+---------+----------------+
//
//
//---------------------------------------------------------------------------------------------------------------------

function get_object_field_names($objecttype) {

   $query = "SHOW COLUMNS FROM daf." . $objecttype;
   $result = mysql_query($query);

   if (! $result) {
      add_error_message("Database access failed for query=$query: " . mysql_error());
      return "";
   }

   $num_rows   = mysql_num_rows($result);   
   $num_fields = mysql_num_fields($result);
   
   $fieldnames = array();
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       array_push($fieldnames, $row[0]);
   }
   
   return $fieldnames;

}


?>