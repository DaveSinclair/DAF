<?php 

  //  object= testcase  action=show 
  //  object= testcase  action=show  ID= 
  
  //  object= testcase  action=show    operation=create            name=... description=...
  //  object= testcase  action=show    operation=modify  ID=...    name=... description=...
  //  object= testcase  action=show    operation=delete  ID=... 

  //  object= tescase  action=create 
  //  object= tescase  action=modify  ID=...
  
  
//---------------------------------------------------------------------------------------------------------------------
//
// Function:   testcase
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function testcase($gp) {

  global $db_server;

  if ($gp['object'] !== "testcase") {
     add_error_message("DAFE0001: testcase: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "testcase";
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.testcase", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0061: Cannot create testcase " . $gp['name'] . " because a testcase of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.testcase (Name, Description) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['description'] . "')";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         if ((count_rows("daf.testcase", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0062: Cannot modify testcase name to " . $gp['name'] . " because a testcase of that name already exists");
         
         } else {

            $query = "UPDATE daf.testcase SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . 
                     "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';      
         $gp['ID'] = NULL;

      } else if ($gp['operation'] === "delete") {
       
          if (count_rows("daf.action", "TestcaseID = '" . $gp['ID'] . "'") > 0) {
         
            add_error_message("DAFE0063: Cannot delete that because it is still used in one or more scenarios");
         
         } else {

            $query = "DELETE FROM daf.testcase where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
         }   
 
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_testcase();
    
   } else if ($gp['action'] === "modify") {

       modify_testcase($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_testcases();
           
      } else {
 
         show_detailed_testcase($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("testcase");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_testcases
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

function show_testcases() {
    
   $query = "SELECT * FROM daf.testcase";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Testcases</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "testcase_create", "Create Testcase", "index.php?action=create&object=testcase");
   echo "</th>\n";

   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      echo '<th>' . $fieldname . '</th>';
   }
   echo '</tr>';
   echo "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       
       echo "<td class=td_smd>\n";
       cellbutton(null, "testcase_show_$j", "show", "index.php?action=show&object=testcase&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>\n";
       cellbutton(null, "testcase_modify_$j", "modify", "index.php?action=modify&object=testcase&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>\n";
       // cellbutton(null, "testcase_delete_$j", "delete", "index.php?action=show&operation=delete&object=testcase&ID=$ID");
       button_with_confirm("testcase_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=testcase&ID=$ID", 
                          "index.php?action=show&object=testcase");
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
// Function:   create_testcase
//
// Inputs:     $gp
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function create_testcase() {

   echo '<div dojoType="dijit.form.Form" id="create_testcase" jsId="create_testcase" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create an Testcase</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size=' . DAF_TESTCASE_LEN . '" maxlength="' . DAF_TESTCASE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:' . DAF_TESTCASE_LEN . 'em" regExp="[A-Za-z0-9_\/\$:;.\@\*\\\s]{1,' . DAF_TESTCASE_LEN . '}" required="true" trim="true" promptMessage="Enter a Testcase name" ' . 
        'invalidMessage="Invalid Testcase name (must be 1 to ' . DAF_TESTCASE_LEN . ' alphanumeric characters, including \/\$:;.\@\*\\)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size=' . DAF_TESTCASE_DESCRIPTION_LEN . '" maxlength="' . DAF_TESTCASE_DESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:50em" regExp=".{1,' . DAF_TESTCASE_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Testcase description" ' . 
        'invalidMessage="Invalid Testcase description (must be 1 to ' . DAF_TESTCASE_DESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";
   
   cellbutton("div", "testcase_create", "Create Testcase", null, "submit");
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="create" />' . "\n";
   echo '<input type="hidden" name="object"    value="testcase" />' . "\n";
   cellbutton(null, "testcase_cancel", "Cancel", "index.php?object=testcase&action=show", null);

   echo "</div>\n";
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_testcase
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_testcase($ID) {

   $query = "SELECT * FROM daf.testcase where ID = '" . $ID . "'";
   $testcase = mysql_query($query);

   if (! $testcase) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testcase);   // should only be one row
   $num_fields = mysql_num_fields($testcase);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Testcase</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testcase, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $testcase_row = mysql_fetch_row($testcase);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($testcase, $i) . '</th>';
      echo '<td>'  . $testcase_row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   $query = "SELECT DISTINCT scenario.ID, scenario.Name, action.Stepnumber FROM daf.action INNER JOIN daf.scenario ON action.ScenarioID = scenario.ID WHERE action.TestcaseID = '" . $ID . "'";
   $testcase = mysql_query($query);

   if (! $testcase) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testcase);   // should only be one row
   $num_fields = mysql_num_fields($testcase);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenarios</div></caption>";
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
// Function:   modify_testcase
//
// Inputs:     $ID
//
// Outputs:    none
//
// Returns:
//
// Description:
//
//----------------------------------------------------------------------------------------------------------------------

function modify_testcase($ID) {

   $query = "SELECT ID, Name, Description FROM daf.testcase where ID = '" . $ID . "'";
   $testcase = mysql_query($query);

   if (! $testcase) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testcase);   // should only be one row
   $num_fields = mysql_num_fields($testcase);

   echo '<div dojoType="dijit.form.Form" id="create_testcase" jsId="create_testcase" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Testcases</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testcase, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   
   $row = mysql_fetch_row($testcase);
   
   $ID = $row[$IDindex];
 
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($testcase, $i);
      echo '<tr>';
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size=' . DAF_TESTCASE_LEN . '" maxlength="' . DAF_TESTCASE_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:' . DAF_TESTCASE_LEN . 'em" regExp="[A-Za-z0-9__\/\$:;.\@\*\\\s]{1,' . DAF_TESTCASE_LEN . '}" required="true" trim="true" promptMessage="Enter a Testcase name" ' . 
              'invalidMessage="Invalid Testcase name (must be 1 to ' . DAF_TESTCASE_LEN . ' alphanumeric characters, including _\/\$:;.\@\*\\\s)"/></td>';
      } else if ($fieldname == "Description") {
         echo '<th>Description</th>';
         echo '<td><input type="text" name="description" value="' . $row[$i] . '" size=' . DAF_TESTCASE_DESCRIPTION_LEN . '" maxlength="' . DAF_TESTCASE_DESCRIPTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:50em" regExp=".{1,' . DAF_TESTCASE_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Testcase description" ' . 
              'invalidMessage="Invalid Testcase description (must be 1 to ' . DAF_TESTCASE_DESCRIPTION_LEN . ' characters)"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton("div", "testcase_create", "Modify Testcase", null, "submit");

   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="testcase" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID . '" />' . "\n";
   echo '<input type="hidden" name="originalname"    value="' . $originalname . '" />' . "\n";
   cellbutton(null, "testcase_cancel", "Cancel", "index.php?object=testcase&action=show&ID=$ID", null);

   echo '</div>' . "\n";
   
}

?>