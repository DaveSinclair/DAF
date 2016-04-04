<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  project.php
//
//  This file is part of the DAF user interface PHP code.  This file deals with actions relating to project, phase 
//  user, actionoutcome, maillist and scenarioset objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                Action to be carried out
//
//  object=project  action=show                                     display all projects
//  object=project  action=show  ID=                                display an individual project
// 
//  object=project  action=show    operation=create                 insert a new project in the DAF project table
//                              name=... description=....           with characteristics as specified by
//                                                                  the POST'd variables
//  object=project  action=show    operation=modify  ID=...         modify an existing project record in the 
//                              name=... description=....           DAF project table
//  object=project  action=show    operation=delete  ID=...         delete an existing record in the DAF project
//                                                                  table
//
//  object=project  action=create                                   present a form that allows the user to specify
//                                                                  the characteristics of a new project that is to be
//                                                                  added to the DAF project table
//  object=project  action=modify  ID=...                           present a form that allows the user to modify
//                                                                  the characteristics of an existing project in the
//                                                                  DAF project table
//
//  object=phase  action=show                                       display all phases
//  object=phase  action=show  ID=                                  display an individual phase
// 
//  object=phase  action=show    operation=create                   insert a new phase in the DAF project table
//                              name=... description=....           with characteristics as specified by
//                              projectID=....                      the POST'd variables
//  object=phase  action=show    operation=modify  ID=...           modify an existing phase record in the 
//                              name=... description=....           DAF project table
//                              projectID=.... 
//  object=phase  action=show    operation=delete  ID=...           delete an existing record in the DAF phase
//                                                                  table
//
//  object=phase  action=create                                     present a form that allows the user to specify
//                                                                  the characteristics of a new phase that is to be
//                                                                  added to the DAF phase table
//  object=phase  action=modify  ID=...                             present a form that allows the user to modify
//                                                                  the characteristics of an existing phase in the
//                                                                  DAF phase table
//
//  object=user  action=show                                        display all projects
//  object=user  action=show  ID=                                   display an individual project
// 
//  object=user  action=show    operation=create                    insert a new user in the DAF user table
//                              firstname=... lastname=....         with characteristics as specified by
//                              area=... email=...                  the POST'd variables
//                              password=... admin=...
//  object=user  action=show    operation=modify  ID=...            modify an existing user record in the 
//                              firstname=... lastname=....         DAF user table
//                              area=... email=...                  
//                              password=... admin=...
//  object=user  action=show    operation=delete  ID=...            delete an existing record in the DAF user
//                                                                  table
//
//  object=user  action=create                                      present a form that allows the user to specify
//                                                                  the characteristics of a new user that is to be
//                                                                  added to the DAF project table
//  object=user  action=modify  ID=...                              present a form that allows the user to modify
//                                                                  the characteristics of an existing user in the
//                                                                  DAF project table
//
//  object=actionoutcome  action=show                               display all actionoutcomes
//  object=actionoutcome  action=show  ID=                          display an individual actionoutcome
// 
//  object=actionoutcome  action=show    operation=create           insert a new actionoutcome in the DAF actionoutcome 
//                              name=... actiononpass=....          table with characteristics as specified by
//                              actiononfail... erroraction=...     the POST'd variables
//                              scenariotoberunonpass=...
//                              scenariotoberunonfail=...
//                              notificationonpass=...
//                              notificationonfail=...
//                              testerid=...
//                              maillistid=...
//  object=actionoutcome  action=show    operation=modify  ID=...   modify an existing actionoutcome record in the 
//                              name=... actiononpass=....          table with characteristics as specified by
//                              actiononfail... erroraction=...     the DAF outcomeaction table
//                              scenariotoberunonpass=...
//                              scenariotoberunonfail=...
//                              notificationonpass=...
//                              notificationonfail=...
//                              testerid=...
//                              maillistid=...
//  object=actionoutcome  action=show    operation=delete  ID=...   delete an existing record in the DAF actionoutcome
//                                                                  table
//
//  object=actionoutcome  action=create                             present a form that allows the user to specify
//                                                                  the characteristics of a new actionoutcome that is to be
//                                                                  added to the DAF actionoutcome table
//  object=actionoutcome  action=modify  ID=...                     present a form that allows the user to modify
//                                                                  the characteristics of an existing actionoutcome in the
//                                                                  DAF project table
//
//  object=maillist  action=show                                    display all maillist
//  object=maillist  action=show  ID=                               display an individual maillist
// 
//  object=maillist  action=show    operation=create                insert a new maillist in the DAF maillist table
//                              name=... description=....           with characteristics as specified by
//                                                                  the POST'd variables
//  object=maillist  action=show    operation=modify  ID=...        modify an existing maillist in the 
//                              name=... description=....           DAF maillist table
//  object=maillist  action=show    operation=delete  ID=...        delete an existing record in the DAF maillist
//                                                                  table
//
//  object=maillist  action=create                                  present a form that allows the user to specify
//                                                                  the characteristics of a new project that is to be
//                                                                  added to the DAF project table
//  object=maillist  action=modify  ID=...                          present a form that allows the user to modify
//                                                                  the characteristics of an existing project in the
//                                                                  DAF project table
//
//  object=maillistmember  action=show                              display all maillistmembers
//  object=maillistmember  action=show  ID=                         display an individual maillistmember
// 
//  object=maillistmember  action=show   operation=create           insert a new maillist in the DAF maillistmember table
//                              maillistID=... userid=....          with characteristics as specified by
//                                                                  the POST'd variables
//  object=maillistmember  action=show   operation=modify  ID=...   modify an existing maillistmember in the 
//                              maillistID=... userid=....          DAF maillistmember table
//  object=maillistmember  action=show    operation=delete  ID=...  delete an existing record in the DAF maillistmember
//                                                                  table
//
//  object=maillistmember  action=create                            present a form that allows the user to specify
//                                                                  the characteristics of a new maillistmember that is to be
//                                                                  added to the DAF maillistmember table
//  object=maillistmember  action=modify  ID=...                    present a form that allows the user to modify
//                                                                  the characteristics of an existing maillistmember in the
//                                                                  DAF maillistmember table
//


//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//
// Function:  determine_status_database_name
//
// Inputs:     $current_project    the name of the project whose scenario results we are looking at, eg "project1"
//             $phase              the phase of the project whose scenario results we are looking at, eg "phase23"
//
// Outputs:    none
//
// Returns:    the status database name,   eg  "project1_phase23";
//
// Description:
//
//      *** This routine must be kept in step with determine_status_database_name in afmysql.c ***
//
// The status (sceanrioresults, actionresults etc) for a given project / phase combination is kept in a separate 
// database whose name is made up from the names of the project and phase.  This routine creates the status database
// name from the $current_project and $phase supplied.
//
//
//----------------------------------------------------------------------------------------------------------------------

function determine_status_database_name($project, $phase) {

  return $project . "_" . $phase . "_results";
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:  determine_status_table_name
//
// Inputs:     $status_db         the name of the status database for the project/phase being considered
//             $tablename         the table name for the table of interest, eg "actionresults"
//
// Outputs:    none
//
// Returns:    the status table name,   eg  "project1_phase23.actionresults";
//
// Description:
//
//      *** This routine must be kept in step with determine_status_table_name in afmysql.c ***
//
// The status (sceanrioresults, actionresults etc) for a given project / phase combination is kept in a separate 
// database whose name is made up from the names of the project and phase.  This routine creates the qualified MYSQL
// name of the table of interest /
//
//----------------------------------------------------------------------------------------------------------------------

function determine_status_table_name($status_db, $tablename) {

  return $status_db . "." . $tablename;
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   project
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a project object.   The allowed actions are to create, modify or delete the 
// project record in the project table.
//
//----------------------------------------------------------------------------------------------------------------------

function project($gp, $current_project, $current_phase) {
  
  global $db_server;

  if ($gp['object'] !== "project") {
     add_error_message("DAF0001: project: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "project";
   }
   
   mysql_select_db('daf') or                                                   /* <<<<<<<<<<<<<<< hard coded */
      die("unable to select $db_database database: " . mysql_error());   

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.project", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAF0006: Cannot create project " . $gp['name'] . " because a project of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.project (Name, Description) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['description'] . "')";              
            do_mysql_query($query, $db_server, 1);
         
         }
      
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf.project", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAF0007: Cannot modify project name to " . $gp['name'] . " because a project of that name already exists");
         
         } else {

            $query = "UPDATE daf.project SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . "' " .
                     "WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1);
         }
         $gp['action'] = 'show';      
         $gp['ID'] = null;

      } else if ($gp['operation'] === "delete") {
               
         if (count_rows("daf.phase", "ProjectID = '" . $gp['ID'] . "'") > 0) {
          
            add_error_message("DAF0008: Cannot delete project " . $gp['name'] . " because it still contains one or more phases");
         
         } else {
         
            $query = "DELETE FROM daf.project where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';
         $gp['ID'] = null;
         
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_project();
    
   } else if ($gp['action'] === "modify") {

       modify_project($gp['ID']);
        
   } else if ($gp['action'] == 'selectcurrentprojectphase') {
   
       select_current_project_and_phase($current_project, $current_phase);
   
   } else if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_projects();
         
      } else {
 
         show_detailed_project($gp['ID']);
      }
   
   } else {
   
      report_bad_parms("project");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_project
//
// Inputs:     none   
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new project    The routine posts
//
// status.php?object=project&action=show&operation=create&name=...&description=...  
//
//----------------------------------------------------------------------------------------------------------------------

function create_project() {

   echo '<div dojoType="dijit.form.Form" id="create_project" jsId="create_project" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Project</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size = "' . DAF_PROJECT_LEN . '" maxlength="' . DAF_PROJECT_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PROJECT_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_PROJECT_LEN . '}" required="true" trim="true" promptMessage="Enter a Project Name" ' . 
        'invalidMessage="Invalid Project Name (must be 1 to ' . DAF_PROJECT_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size = "' . DAF_PROJECTDESCRIPTION_LEN . '" maxlength="' . DAF_PROJECTDESCRIPTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_PROJECTDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Project Description" ' . 
        'invalidMessage="Invalid Project Description (must be 1 to ' . DAF_PROJECTDESCRIPTION_LEN . ' characters)"/>';
   // echo   '<td><textarea id="fred1" name="description" dojoType="dijit.form.Textarea" style="width:100%;"></textarea>';
   echo "</td></tr>\n";
   echo "</table>\n"; 

   cellbutton(null, "project_create", "Create Project", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"project\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "project_cancel", "Cancel", "index.php?object=project&action=show", null);

   echo "</div>\n";
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_projects
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all projects..   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=project&action=create             to create a new project
// status.php?object=project&action=modify&ID=...      to modify an existing project
// status.php?object=project&action=delete&ID=...      to delete an existing project
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_projects() {

   $query = "SELECT ID, Name, Description FROM daf.project";   
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "Name", "Description");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Projects</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3  class=\"tab1\">";
   cellbutton(null, "project_create", "Create Project", "index.php?action=create&object=project");
   echo "</th>\n";
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th  class="tab1">' . $fieldnames[$i] . '</th>';
      $fieldindex[$fieldnames[$i]] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr class="tab1">';
       $ID = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']];
       echo '<td class="tab1">';
       cellbutton(null, "project_show_$j",   "show",   "index.php?action=show&object=project&name=$name&ID=$ID");
       echo '</td>';
       echo '<td class="tab1">';
       cellbutton(null, "project_modify_$j", "modify", "index.php?action=modify&object=project&name=$name&ID=$ID");
       echo '</td>';
       echo '<td class="tab1">';
       // cellbutton(null, "project_delete_$j", "delete", "index.php?action=show&operation=delete&object=project&ID=$ID");
       button_with_confirm("project_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=project&ID=$ID", "index.php?action=show&object=project");
       echo '</td>';
       
       for ($i = 0; $i < $num_fields; $i++) {       
          echo '<td class="tab1">'  . $row[$i] . '</td>';
       }   
       echo '</tr>';
       echo "\n";

   }
      
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_project
//
// Inputs:     ID                the ID of the row in the DAF project table containing the project whose details
//              \\\\\\\\\\\\\\\\\\\s                 are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a project
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_project($ID) {

   $query = "SELECT ID, Name, Description FROM daf.project where project.ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "Name", "Description");

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Project</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   
   $row = mysql_fetch_row($result);

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($result, $i) . '</th>';
      echo '<td>'  . $row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
            
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_project
//
// Inputs:     none   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a project that is already in the DAF project
// table.   The routine posts
//
// status.php?object=project&action=show&operation=modify&name=...&description=...&ID=....
//
//----------------------------------------------------------------------------------------------------------------------

function modify_project($ID) {

   $query = "SELECT ID, Name, Description FROM daf.project where project.ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "Name", "Description");

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="modify_project" jsId="modify_project" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify a Project</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   
   $row = mysql_fetch_row($result);
 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size = "' . DAF_PROJECT_LEN . '" maxlength="' . DAF_PROJECT_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PROJECT_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_PROJECT_LEN . '}" required="true" trim="true" promptMessage="Enter a Project Name" ' . 
              'invalidMessage="Invalid Project Name (must be 1 to ' . DAF_PROJECT_LEN . ' alphanumeric characters)" /></td>';
      } else if ($fieldname == "Description") {
         echo '<th>Description</th>';
         echo '<td><input type="text" name="description" value="' . $row[$i] . '" size = "' . DAF_PROJECTDESCRIPTION_LEN . '" maxlength="' . DAF_PROJECTDESCRIPTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_PROJECTDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Project Description" ' . 
              'invalidMessage="Invalid Project Description (must be 1 to ' . DAF_PROJECTDESCRIPTION_LEN . ' characters)" /></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   cellbutton('div', "project_modify", "Modify Project", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"project\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\" value=\"$ID\"/>\n";
   echo "<input type=\"hidden\" name=\"originalname\" value=\"$originalname\"/>\n";
   cellbutton(null, "project_cancel", "Cancel", "index.php?object=project&action=show&ID=$ID", null);
  
   echo "</div>\n";
         
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   phase
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a phase object.   The allowed actions are to create, modify or delete the 
// phase record in the phase table.
//
//----------------------------------------------------------------------------------------------------------------------

function phase($gp) {
  
  global $db_server;

  if ($gp['object'] !== "phase") {
     add_error_message("DAF0001: phase: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "phase";
   }
   
   mysql_select_db('daf') or                                                   /* <<<<<<<<<<<<<<< hard coded */
      die("unable to select daf database: " . mysql_error());       /* <<<<<<<<<<<<<<< hard coded */

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
          $lookup = lookuptable("daf.project", "ID", "Name");
          $projectname = $lookup[$gp['projectID']]; 
          
          if (count_rows("daf.phase", "ProjectID = '" . $gp['projectID'] . "' AND Name = '" . $gp['name'] . "'") > 0) {
          
             add_error_message("DAFE0009: Cannot create phase " . $gp['name'] . " in project " . $projectname . " because a phase of that name already exists in this project");
             
          } else {
   
             $query = "INSERT INTO daf.phase (Name, Description, ProjectID, Logdirectory) " .
                      "VALUES('" . $gp['name'] . "','" . $gp['description'] . "','" . $gp['projectID'] . "','" . $gp['logdirectory'] . "')";              
             if (do_mysql_query($query, $db_server, 1)) {
                create_phase_database($projectname, $gp['name']);
             }
             
          }

          $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {      
            
          $okaytochange = 1;
          if ($gp['originalname'] != $gp['name']) {
              /* name is changing - make sure there is not already a phase with that name */
             if (count_rows("daf.phase", "ProjectID = '" . $gp['projectID'] . "' AND Name = '" . $gp['name'] . "'") > 0) {
         
                $lookup = lookuptable("daf.project", "ID", "Name");        
                add_error_message("DAFE0010: Cannot rename phase to " . $gp['name'] . "in project " . $lookup[$gp['projectID']] . " because a phase of that name already exists in this project");
                $okaytochange = 0;
             }   
             
          } 
          
          if ($okaytochange == 1) {

            $query = "UPDATE daf.phase SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . "', ProjectID='" . $gp['projectID'] .
                     "', Logdirectory='" . $gp['logdirectory'] . "' WHERE ID='" . $gp['ID'] . "'";
            do_mysql_query($query, $db_server, 1); 
            
         }

         $gp['action'] = 'show';
         $gp['ID'] = null;

      } else if ($gp['operation'] === "delete") {
      
         $lookup = lookuptable("daf.project", "ID", "Name");
         $projectname = $lookup[$gp['projectID']]; 

         $query = "DELETE FROM daf.phase where ID='" . $gp['ID'] . "'";    
         if (do_mysql_query($query, $db_server, 1)) {
            drop_phase_database($projectname, $gp['name']);
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
      
      mysql_select_db('daf') or                                                   /* <<<<<<<<<<<<<<< hard coded */
         die("unable to select daf database: " . mysql_error());       /* <<<<<<<<<<<<<<< hard coded */
   
   }

   if ($gp['action'] === "create") {
   
       create_phase();
    
   } else if ($gp['action'] === "modify") {

       modify_phase($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_phases();
           
      } else {
 
         show_detailed_phase($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("phase");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_phase
//
// Inputs:     none   
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new phase    The routine posts
//
// status.php?object=phase&action=show&operation=create&name=...&description=...&projectID=...  
//
//----------------------------------------------------------------------------------------------------------------------

function create_phase() {
   
   echo '<div dojoType="dijit.form.Form" id="create_phase" jsId="create_phase" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Phase</div></caption>\n";

   echo "<tr><th>Name</th>";
   echo '<td><input type="text" name="name" size="' . DAF_PHASE_LEN . '" maxlength="' . DAF_PHASE_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PHASE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_PHASE_LEN . '}" required="true" trim="true" promptMessage="Enter a Phase Description" ' . 
        'invalidMessage="Invalid Phase Description (must be 1 to ' . DAF_PHASE_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size="' . DAF_PHASEDESCRIPTION_LEN . '" maxlength="' . DAF_PHASEDESCRIPTION_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_PHASEDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Phase Description" ' . 
        'invalidMessage="Invalid Phase Description (must be 1 to ' . DAF_PHASEDESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Project</th><td><select name=\"projectID\"/>\n";
   selectorID("project", "name");
   echo "</select></td></tr>\n";
   echo '<tr><th>Log Location Directory</th>';
   echo '<td><input type="text" name="logdirectory" size="' . DAF_LOGDIRECTORY_LEN . '" maxlength="' . DAF_LOGDIRECTORY_LEN . '" ' .  /* <<<< incomplete validation ??? */
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LOGDIRECTORY_LEN . 'em;" regExp=".{1,' . DAF_LOGDIRECTORY_LEN . '}" required="true" trim="true" promptMessage="Enter a pathname to a Log Directory" ' . 
        'invalidMessage="Invalid Log Directory (must be 1 to ' . DAF_LOGDIRECTORY_LEN . ' characters)"/>';
   echo "</td></tr>\n"; 
   echo "</table>\n";
   
   cellbutton(null, "phase_create", "Create Phase", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"phase\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "phase_cancel", "Cancel", "index.php?object=phase&action=show", null);
   
   echo "</div>";
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_phases
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all phases.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=phase?action=create             to create a new phase
// status.php?object=phase?action=modify&ID=...      to modify an existing phase
// status.php?object=phase?action=delete&ID=...      to delete an existing phase
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_phases() {

   $query = "SELECT ID, Name, Description, ProjectID, Logdirectory FROM daf.phase";   
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup     = lookuptable("daf.project", "ID", "Name");
   $fieldnames = array("ID", "Name", "Description", "Project", "Log Location Directory");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Phases</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "phase_create", "Create Phase", "index.php?action=create&object=phase");
   echo "</th>\n";
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $projectIDindex = $fieldindex['ProjectID'];
   $nameindex      = $fieldindex['Name'];
   $IDindex        = $fieldindex['ID'];
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$IDindex];
       $name = $row[$nameindex];
       $projectID = $row[$projectIDindex];
       echo "<td class=td_smd>";
       cellbutton(null, "phase_show_$j",   "show",   "index.php?action=show&object=phase&name=$name&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "phase_modify_$j", "modify", "index.php?action=modify&object=phase&name=$name&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       button_with_confirm("phase_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=phase&name=$name&projectID=$projectID&ID=$ID", 
                           "index.php?action=show&object=phase");
       echo "</td>";

       for ($i = 0; $i < $num_fields; $i++) { 
          if ($i == $projectIDindex) {
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

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_phase
//
// Inputs:     ID                the ID of the row in the DAF project table containing the phase whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a phase
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_phase($ID) {

   $query = "SELECT ID, Name, Description, ProjectID, Logdirectory FROM daf.phase where phase.ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup     = lookuptable("daf.project", "ID", "Name");
   $fieldnames = array("ID", "Name", "Description", "Project", "Log Location Directory");

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Phase</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $row = mysql_fetch_row($result);
   
   $projectIDindex = $fieldindex['ProjectID'];

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . $fieldnames[$i] . '</th>';
      if ($i == $projectIDindex) {
         echo '<td>'  . $lookup[$row[$i]] . '</td>';
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
// Function:   modify_phase
//
// Inputs:     $ID           the ID of the record in the phase table for the phase that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a phase that is already in the DAF project
// table.   The routine posts
//
// status.php?object=phase&action=show&operation=modify&name=...&description=...&projectID=...&ID=....
//
//----------------------------------------------------------------------------------------------------------------------

function modify_phase($ID) {

   $query = "SELECT ID, Name, Description, ProjectID, Logdirectory FROM daf.phase where phase.ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "Name", "Description", "Project", "Log Location Directory");

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="modify_phase" jsId="modify_phase" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Phase</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $nameindex      = $fieldindex['Name'];
   $projectIDindex = $fieldindex['ProjectID'];
  
   $row = mysql_fetch_row($result);
   $originalname = $row[$fieldindex['Name']];

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = $fieldnames[$i];
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($i == $projectIDindex) {
         echo '<th>Project</th><td><select name="projectID" />' . "\n";
         selectorID("project", "name", $row[$i]);
         echo '</select></td>' . "\n";
      } else if ($fieldname == "Name") {
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value = "' . $row[$i] . '" size="' . DAF_PHASE_LEN . '" maxlength="' . DAF_PHASE_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PHASE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_PHASE_LEN . '}" required="true" trim="true" promptMessage="Enter a Phase Description" ' . 
              'invalidMessage="Invalid Phase Description (must be 1 to ' . DAF_PHASE_LEN . ' alphanumeric characters)"/></td>';
      } else if ($fieldname == "Description") {
         echo '<th>Description</th>';
         echo '<td><input type="text" name="description" value = "' . $row[$i] . '" size="' . DAF_PHASEDESCRIPTION_LEN . '" maxlength="' . DAF_PHASEDESCRIPTION_LEN . '" ' . 
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_PHASEDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Phase Description" ' . 
              'invalidMessage="Invalid Phase Description (must be 1 to ' . DAF_PHASEDESCRIPTION_LEN . ' characters)"/></td>';
      } else if ($fieldname == "Logdirectory") {
         echo '<th>Log directory</th>';
         echo '<td><input type="text" name="logdirectory value = "' . $row[$i] . '" size="' . DAF_LOGDIRECTORY_LEN . '" maxlength="' . DAF_LOGDIRECTORY_LEN . '" ' .  /* <<<< incomplete validation ??? */
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LOGDIRECTORY_LEN . 'em;" regExp=".{1,' . DAF_LOGDIRECTORY_LEN . '}" required="true" trim="true" promptMessage="Enter a pathname to a Log Directory" ' . 
              'invalidMessage="Invalid Log Directory (must be 1 to ' . DAF_LOGDIRECTORY_LEN . ' characters)"/></td>';
      }
      echo "</tr>\n";
   }
   echo '</table>';
  
   cellbutton('div', "phase_modify", "Modify Phase", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"phase\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\" value=\"$ID\"/>\n";
   echo "<input type=\"hidden\" name=\"originalname\" value=\"$originalname\"/>\n";
   cellbutton(null, "phase_cancel", "Cancel", "index.php?object=phase&action=show&ID=$ID", null);

   echo "</div>\n";    
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_phase_database
//
// Inputs:     ID 
//
// Outputs:    none
//
// Returns:    the name of the new phase results database, eg bigstorage_evt_results
//
// Description:   When a new test phase is created, we must create a set of tables to hold the results of any scenarios 
//                that are run in this test phase.  Each phase therefore has a results database that holds these tables.
//                Since test phases in different projects may have the same name, the name of the results database for a
//                particular phase contains both the project and phase name.  Eg if the project is 'bigstorage' and the 
//                phase name is 'evt', then the results database will be 'bigstorage_evt_results'.  The name of the 
//                tables within the database, eg actionresult, archive_workqueue etc, do not have names that are 
//                specific to the phase that they belong to.
//
//                This routine also grants ALL priviledges to the SQL user 'daf_admin' on the local host for this new 
//                database so that DAF can admin this table
//
//----------------------------------------------------------------------------------------------------------------------

function create_phase_database($project, $phase) {

   $status_db = determine_status_database_name($project, $phase);
   $query = "CREATE DATABASE `$status_db`";
   if (! do_mysql_query($query, null, 1)) {
      add_error_message("DAFE0011: SQL failed for $query when creating new database $status_db: " . mysql_error());
      return;
   }

   if (! mysql_select_db($status_db)) {
     add_error_message("DAFE0012: Unable to select $status_db database: " . mysql_error());
     return;
   }

   $query = "SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"";
   if (! do_mysql_query($query, null, 1)) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   //$query = "GRANT ALL PRIVILEGES ON `$status_db`.* TO 'daf_admin'@'localhost'";
   //if (! do_mysql_query($query, null, 1)) {    
   //   return;
   //}

   $query = "CREATE TABLE $status_db.`actionresult` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `ScenarioresultID` int(11) NOT NULL,
     `Actiontype` varchar(32) NOT NULL,
     `Stepnumber` int(11) NOT NULL,
     `Status` enum('None','Canceled_due_to_error_in_step','Completed','No_available_host','No_remote_host_connect','Clnt_call_failed','Prepare_cmd_failed','Execute_cmd_failed') NOT NULL,
     `Hostname` varchar(32) NOT NULL,
     `Testcase` varchar(32) NOT NULL,
     `Invocation` varchar(1024) NOT NULL,
     `Pass` int(11) NOT NULL,
     `Start` datetime NOT NULL,
     `End` datetime NOT NULL,
     `Comments` varchar(128) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ; ";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }
    
   $query = "CREATE TABLE $status_db.`archive_workqueue` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `Project` varchar(32) NOT NULL,
     `Phase` varchar(32) NOT NULL,
     `ScenarioID` int(11) NOT NULL,
     `TestlevelID` int(11) NOT NULL,
     `TeststandID` int(11) NOT NULL,
     `ScenarioresultID` int(11) NOT NULL,
     `ActionresultID` int(11) NOT NULL,
     `EnvironmentID` int(11) NOT NULL,
     `OutcomeactionID` int(11) NOT NULL,
     `TesterID` int(11) NOT NULL,
     `MaillistID` int(11) NOT NULL,
     `Actiontype` varchar(32) NOT NULL,
     `Stepnumber` int(11) NOT NULL,
     `Hostname` varchar(64) NOT NULL,
     `State` enum('Notstarted','Tobecanceled','Preparing','Running','Pendingcompletion','Completed') NOT NULL,
     `Statemodifier` enum('None','Canceled_due_to_error_in_step','Completed','No_available_host','No_remote_host_connect','Clnt_call_failed','Prepare_cmd_failed', 'Execute_cmd_failed') NOT NULL,
     `Testcase` varchar(32) NOT NULL,
     `Invocation` varchar(1024) NOT NULL,
     `Duration` enum('Rununtilcomplete','RunforNminutes','TimeoutafterNminutes') NOT NULL,
     `Maxduration` int(11) NOT NULL,
     `Pass` int(11) NOT NULL,
     `Start` datetime NOT NULL,
     `End` datetime NOT NULL,
     `Tag` int(11) NOT NULL,
     `Loglocation` varchar(128) NOT NULL,
     `Scenariologfilename` varchar(64) NOT NULL,
  PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }

   $query = "CREATE TABLE $status_db.`archive_workrequest` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `Project` varchar(32) NOT NULL,
     `Phase` varchar(32) NOT NULL,
     `RequestType` enum('Scenario') NOT NULL,
     `TesterID` int(11) NOT NULL,
     `State` enum('Idle','Failedtestlevelvalidation','Running','Canceled','Completed') NOT NULL,
     `Tablename1` varchar(32) NOT NULL,
     `ObjectID1` int(11) NOT NULL,
     `Tablename2` varchar(32) NOT NULL,
     `ObjectID2` int(11) NOT NULL,
     `Tablename3` varchar(32) NOT NULL,
     `ObjectID3` int(11) NOT NULL,
     `Tablename4` varchar(32) NOT NULL,
     `ObjectID4` int(11) NOT NULL,
     `Tablename5` varchar(32) NOT NULL,
     `ObjectID5` int(11) NOT NULL,
     `Start` datetime NOT NULL,
     `End` datetime NOT NULL,
     `Logdirectory` varchar(128) NOT NULL,
     `Scenariologfilename` varchar(64) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }

   $query = "CREATE TABLE $status_db.`clusterrecord` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `ScenarioresultID` int(11) NOT NULL,
     `Name` int(11) NOT NULL,
     `Teststandname` varchar(32) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }

   $query = "CREATE TABLE $status_db.`noderecord` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `ScenarioresultID` int(11) NOT NULL,
     `Clustername` varchar(64) NOT NULL,
     `Name` varchar(32) NOT NULL,
     `Type` varchar(32) NOT NULL,
     `Model` varchar(16) NOT NULL,
     `Serial` varchar(16) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }

   $query = "CREATE TABLE $status_db.`scenarioresult` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `Project` varchar(32) NOT NULL,
     `Phase` varchar(32) NOT NULL,
     `Scenarioname` varchar(32) NOT NULL,
     `Jobname` varchar(48) NOT NULL,
     `State` enum('Notstarted','Failedtestlevelvalidation','Running','Canceled','Completed') NOT NULL DEFAULT 'Notstarted',
     `Actionsinscenario` int(11) NOT NULL,
     `Actionsattempted` int(11) NOT NULL,
     `Actionscompleted` int(11) NOT NULL,
     `Actionspassed` int(11) NOT NULL,
     `Actionsfailed` int(11) NOT NULL,
     `Pass` int(11) NOT NULL,
     `Start` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
     `End` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
     `Teststand` varchar(32) NOT NULL,
     `TeststandrecordID` int(11) NOT NULL,
     `Testlevel` varchar(32) NOT NULL,
     `TestlevelrecordID` int(11) NOT NULL,
     `Tester` varchar(32) NOT NULL,
     `Loglocation` varchar(128) NOT NULL,
     `Scenariologfilename` varchar(64) NOT NULL,
     `Comments` varchar(128) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }


   $query = "CREATE TABLE $status_db.`testlevelrecord` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `ScenarioresultID` int(11) NOT NULL,
     `Name` varchar(32) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }

   $query = "CREATE TABLE $status_db.`teststandrecord` (
     `ID` int(11) NOT NULL AUTO_INCREMENT,
     `ScenarioresultID` int(11) NOT NULL,
     `Name` varchar(32) NOT NULL,
     PRIMARY KEY (`ID`)
   ) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";

   if (! do_mysql_query($query, null, 1)) {
      return;
   }
   
   return $status_db;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   drop_phase_database
//
// Inputs:     ID 
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:   When a test phase is deleted, we delete all the result tables in that phase.  The results database for 
//                a particular phase contains both the project and phase name.  Eg if the project is 'bigstorage' and the 
//                phase name is 'evt', then the results database will be 'bigstorage_evt_results'.  The name of the 
//                tables within the database, eg actionresult, archive_workqueue etc, do not have names that are 
//                specific to the phase that they belong to.
//
//----------------------------------------------------------------------------------------------------------------------

function drop_phase_database($project, $phase) {

   $status_db = determine_status_database_name($project, $phase);
   $query = "DROP DATABASE `$status_db`";
   if (! do_mysql_query($query, null, 1)) 
      die ("SQL failed for $query when deleting status database $status_db: " . mysql_error());

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   user
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a user object.   The allowed actions are to create, modify or delete the 
// user record in the user table.
//
//----------------------------------------------------------------------------------------------------------------------

function user($gp) {
  
  global $db_server;

  if ($gp['object'] !== "user") {
     add_error_message("DAFE0001: user: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "user";
   }
   
   mysql_select_db('daf') or                                                   /* <<<<<<<<<<<<<<< hard coded */
      die("DAFE0012: unable to select daf database: " . mysql_error());         /* <<<<<<<<<<<<<<< hard coded */

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
          if (count_rows("daf.user", "email = '" . $gp['email'] . "'") > 0) {
          
             add_error_message("DAFE0013: Cannot create user " . $gp['firstname'] . " " . $gp['lastname'] . " with email " . $gp['email'] . 
                               " because a user with that email already exists");
             
          } else {
          
             if (($gp['admin'] == 0) || am_I_admin()) {
             
                if ($gp['password'] == $gp['confirm_password']) {
          
                   $token = encrypt_password($gp['password']);
   
                   $query = "INSERT INTO daf.user (Firstname, Lastname, Area, Email, Password, Admin, Masteruser) " .
                            "VALUES('" . $gp['firstname'] . "','" . $gp['lastname'] . "','" . $gp['area'] . "','" . 
                            $gp['email'] . "','" . encrypt_password($gp['password']) . "','" . $gp['admin'] . "','0')";              
                   do_mysql_query($query, $db_server, 1);
                } else {
                   add_error_message("DAFE0014: Password and Confirm Password fields did not match - please try again");
                   create_user($gp['firstname'], $gp['lastname'], $gp['area'], $gp['email']);
                   return;
                }
             } else {
                add_error_message("DAFE0015: Only an admin user can create another user with admin priviledges");
             }
                 
             $gp['action'] = 'show';
             
          }

      } else if ($gp['operation'] === "modify") {      
            
          if ((($gp['admin'] == 1) || (lookupfield("daf.user", "Admin", "ID", $gp['ID']) == 1)) && (! am_I_admin())) {
          
             add_error_message("DAFE0016: Cannot modify user as current user does not have Administrator priviledges");
             
          } else if (($gp['originalemail'] != $gp['email']) && count_rows("daf.user", "email = '" . $gp['email'] . "'") > 0) {  
         
             add_error_message("DAFE0017: Cannot rename user " . $gp['firstname'] . " " . $gp['lastname'] . " with email " . $gp['email'] . 
                               " because a user with that email already exists");
             modify_user($gp['ID']);
             return;
             
          } else if ((lookupfield("daf.user", "Masteruser", "ID", $gp['ID']) == 1) && ($gp['admin'] == 0)) {
 
              add_error_message("DAFE0018: Cannot set Admin = 0 on user " . $gp['firstname'] . " " . $gp['lastname'] . " with email " . $gp['email'] . 
                               " because this user is the Master Test User");
              modify_user($gp['ID']);
              return;
          
          } else { 
          
            /* there is an issue here, as if the email of the user is modified, then the SESSION values for email are now out of date <<<<<<<< */
          
            if (($gp['originalemail'] == $_SESSION['user_email']) || am_I_admin()) {
               $query = "UPDATE daf.user SET Firstname='" . $gp['firstname'] . "', Lastname='" . $gp['lastname'] . 
                        "', Area='" . $gp['area'] . "', Email='" . $gp['email'] . "', Admin='" . $gp['admin'] . "'";
               if ($gp['password'] != "") {
                  if ($gp['password'] == $gp['confirm_password']) {
                     $query .= ", Password='" . encrypt_password($gp['password']) . "'";
                  } else {
                     add_error_message("DAFE0014: Password and Confirm Password fields did not match - please try again");
                     modify_user($gp['ID']);
                     return;
                  }
               }
               $query .= " WHERE ID='" . $gp['ID'] . "'";
               if (do_mysql_query($query, $db_server, 1)) {
                  /* we have successfully modified the user - but if this user is the user that owns the current session */
                  /* then the session information is out of date and we need to update the session information */
                  if ($gp['originalemail'] == $_SESSION['user_email']) {
                      $_SESSION['user_name']  = $gp['firstname'] . " " . $gp['lastname'];
                      $_SESSION['user_ID']    = $gp['ID']; 
                      $_SESSION['user_email'] = $gp['email'];
                      $_SESSION['user_password'] = encrypt_password($gp['password']);
                      $_SESSION['user_admin'] = $gp['admin'];
                  }
               }
            } else {
                add_error_message("DAFE0019: Only an admin user can create modify the properties of another user");
            }    
            $gp['action'] = 'show';
            $gp['ID'] = null;
            
         }    

      } else if ($gp['operation'] === "delete") {
      
          if (! am_I_admin()) {
          
             add_error_message("DAFE0020: Cannot delete user as current user does not have Administrator priviledges");
             
         } else if (lookupfield("daf.user", "Masteruser", "ID", $gp['ID']) == 1) {
 
              add_error_message("DAFE0021: Cannot delete user because this user is the Master Test User");
              show_detailed_user($gp['ID']);
              return;
          
         } else {

            $query = "DELETE FROM daf.user where ID='" . $gp['ID'] . "'";    
            do_mysql_query($query, $db_server, 1);
         
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_user();
    
   } else if ($gp['action'] === "modify") {

       modify_user($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_users();
           
      } else {
 
         show_detailed_user($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("user");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_user
//
// Inputs:     none   
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new user    The routine posts
//
// status.php?object=user&action=show&operation=create&firstname=...&lastname=&area=...&emailD=...&password=....&admin=...  
//
//----------------------------------------------------------------------------------------------------------------------

function create_user($firstname = null, $lastname = null, $area = null, $email = null) {

   echo '<div dojoType="dijit.form.Form" id="create_user" jsId="create_user" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new User</div></caption>\n";

   echo '<tr><th>First Name</th>';
   echo '<td><input type="text" name="firstname"' . (isset($firstname) ? " value=\"$firstname\" " : "") . ' size="' . DAF_USER_FIRSTNAME_LEN . '" maxlength="' . DAF_USER_FIRSTNAME_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_FIRSTNAME_LEN . 'em;" style="width: ' . DAF_USER_FIRSTNAME_LEN . 'em;" regExp="[A-Za-z]{1,' . DAF_USER_FIRSTNAME_LEN . '}" required="true" trim="true" promptMessage="Enter a First Name" ' . 
        'invalidMessage="Invalid First Name (must be 1 to ' . DAF_USER_FIRSTNAME_LEN . ' alphabetic characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Last Name</th>';
   echo '<td><input type="text" name="lastname"' . (isset($lastname) ? " value=\"$lastname\" " : "") . ' size="' . DAF_USER_LASTNAME_LEN . '" maxlength="' . DAF_USER_LASTNAME_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_LASTNAME_LEN . 'em;" style="width: ' . DAF_USER_LASTNAME_LEN . 'em;" regExp="[A-Za-z\']{1,' . DAF_USER_LASTNAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Last Name" ' . 
        'invalidMessage="Invalid Last Name (must be 1 to ' . DAF_USER_LASTNAME_LEN . ' alphabetic or \' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Area</th>';
   echo '<td><input type="text" name="area"' . (isset($area) ? " value=\"$area\" " : "") . ' size="' . DAF_USER_AREA_LEN . '" maxlength="' . DAF_USER_AREA_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_AREA_LEN . 'em;" style="width: ' . DAF_USER_AREA_LEN . 'em;" regExp=".{1,' . DAF_USER_AREA_LEN . '}" promptMessage="Enter an optional Area (eg Department)" ' . 
        'invalidMessage="Invalid Area (must be 1 to ' . DAF_USER_AREA_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Email</th>';
   echo '<td><input type="text" name="email"' . (isset($email) ? " value=\"$email\" " : "") . ' size="' . DAF_USER_EMAIL_LEN . '" maxlength="' . DAF_USER_EMAIL_LEN . '" ' .   /* is this validation to flexible *?  */
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_EMAIL_LEN . 'em;" style="width: ' . DAF_USER_EMAIL_LEN . 'em;" regExp="[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}" required="true" trim="true" promptMessage="Enter a Email address (eg fred@acme.com)" ' . 
        'invalidMessage="Invalid Email address (must be 1 to ' . DAF_USER_EMAIL_LEN . ' characters in the fred@acme.com format)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Password</th>;';
   echo '<td><input type="password" name="password" size="' . DAF_USER_PASSWORD_LEN . '" maxlength="' . DAF_USER_PASSWORD_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" regExp=".{1,' . DAF_USER_PASSWORD_LEN . '}" required="true" trim="true" promptMessage="Enter a Password" ' . 
        'invalidMessage="Invalid Password (must be 1 to ' . DAF_USER_PASSWORD_LEN . ' characters)"/></td>';
        /* <<<<< the password above is POST'd in the clear  - can we encrypt it ?? */
   echo '<tr><th>Confirm Password</th>;';
   echo '<td><input type="password" name="confirm_password" size="' . DAF_USER_PASSWORD_LEN . '" maxlength="' . DAF_USER_PASSWORD_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" regExp=".{1,' . DAF_USER_PASSWORD_LEN . '}" required="true" trim="true" promptMessage="Enter a Password" ' . 
        'invalidMessage="Invalid Password (must be 1 to ' . DAF_USER_PASSWORD_LEN . ' characters)"/></td>';
        /* <<<<< the password above is POST'd in the clear  - can we encrypt it ?? */
   echo "</tr>\n";
   echo "<tr><th>Administrator</th><td><select name=\"admin\" />\n";
   echo "<option selected value=\"0\" >no</option>\n";
   echo "<option          value=\"1\">yes</option>\n";
   echo "</select></td></tr>\n";
   echo "</table>\n";
   
   cellbutton('div', "user_create", "Create User", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"user\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "user_cancel", "Cancel", "index.php?object=user&action=show", null);

   echo '</div>';   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_users
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all users.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=user&action=create            to create a new user
// status.php?object=user&action=modify&ID=...     to modify an existing user
// status.php?object=user&action=delete&ID=...     to delete an existing user
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_users() {

   $query = "SELECT ID, Firstname, Lastname, Area, Email, Admin FROM daf.user";   
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "First Name", "Last Name", "Area", "Email", "Admin");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Users</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "user_show_create", "Create User", "index.php?action=create&object=user");
   echo "</th>\n";
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $adminindex = $fieldindex['Admin'];
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       echo "<td class=td_smd>";
       cellbutton(null, "user_show_$j",   "show",   "index.php?action=show&object=user&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "user_modify_$j", "modify", "index.php?action=modify&object=user&ID=$ID");
       echo "</td>\n";
       //cellbutton('td', "user_delete_$j", "delete", "index.php?action=show&operation=delete&object=user&ID=$ID");
       echo "<td class=td_smd>";
       button_with_confirm("user_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=user&ID=$ID", 
                           "index.php?action=show&object=user");
       echo "</td>";
      
       for ($i = 0; $i < $num_fields; $i++) { 
          if ($i == $adminindex) {
             if ($row[$i] == 1) {
                echo '<td>yes</td>';
             } else {
                echo '<td>no</td>';
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
// Function:   show_detailed_user
//
// Inputs:     ID                the ID of the row in the DAF project table containing the user whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a user
//
//---------------------------------------------------------------------------------------------------------------------

function show_detailed_user($ID) {

   $query = "SELECT ID, Firstname, Lastname, Area, Email, Admin FROM daf.user where user.ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "First Name", "Last Name", "Area", "Email", "Admin");

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>User</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $row = mysql_fetch_row($result);
   
   $adminindex = $fieldindex['Admin'];
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . $fieldnames[$i] . '</th>';
      if ($i == $adminindex) {
         if ($row[$i] == 1) {
            echo '<td>yes</td>';
         } else {
            echo '<td>no</td>';
        }
      } else {
         echo '<td>' . $row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
            
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_user
//
// Inputs:     $ID           the ID of the record in the user table for the user that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a user that is already in the DAF project
// table.   The routine posts
//
// status.php?object=user?action=show&operation=modify&firstname=...&lastname=...&area=...&password=...&admin=...
//
//----------------------------------------------------------------------------------------------------------------------

function modify_user($ID) {

   $query = "SELECT ID, Firstname, Lastname, Area, Email, Admin FROM daf.user where user.ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) 
     die ("Database access failed for query=$query: " . mysql_error());
   
   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="modify_user" jsId="modify_user" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Modify User</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $adminindex = $fieldindex['Admin'];
   $emailindex = $fieldindex['Email'];
 
   $row = mysql_fetch_row($result);
  
   $originalemail = $row[$emailindex];
  
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>' . "\n";
      } else if ($fieldname == "Admin") {
         echo '<th>Administrator</th><td><select name="admin" />' . "\n";
         if ($row[$i] == 0) {
            echo '<option selected value="0">no</option>' . "\n";
         } else {
            echo '<option value="0">no</option>' . "\n";
         }
         if ($row[$i] == 1) {
            echo '<option selected value="1">yes</option>' . "\n";
         } else {
            echo '<option value="1">yes</option>' . "\n";
         }
         echo '</select></td>' . "\n";
      } else if ($fieldname == "Firstname") {
         echo '<th>First name</th>';
         echo '<td><input type="text" name="firstname" value="' . $row[$i] . '" size="' . DAF_USER_FIRSTNAME_LEN . '" maxlength="' . DAF_USER_FIRSTNAME_LEN . '" ' . 
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_FIRSTNAME_LEN . 'em;" regExp="[A-Za-z]{1,' . DAF_USER_FIRSTNAME_LEN . '}" required="true" trim="true" promptMessage="Enter a First Name" ' . 
              'invalidMessage="Invalid First Name (must be 1 to ' . DAF_USER_FIRSTNAME_LEN . ' alphabetic characters)"/></td>';
      } else if ($fieldname == "Lastname") {
        echo '<th>Last name</th>';
        echo '<td><input type="text" name="lastname" value="' . $row[$i] . '" size="' . DAF_USER_LASTNAME_LEN . '" maxlength="' . DAF_USER_LASTNAME_LEN . '" ' .
             'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_LASTNAME_LEN . 'em;" regExp="[A-Za-z\']{1,' . DAF_USER_LASTNAME_LEN . '}" required="true" trim="true" promptMessage="Enter a Last Name" ' . 
             'invalidMessage="Invalid Last Name (must be 1 to ' . DAF_USER_LASTNAME_LEN . ' alphabetic or \' characters)"/></td>';
      } else if ($fieldname == "Area") {
        echo '<th>Area</th>';
        echo '<td><input type="text" name="area" value="' . $row[$i] . '" size="' . DAF_USER_AREA_LEN . '" maxlength="' . DAF_USER_AREA_LEN . '" ' .
             'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_AREA_LEN . 'em;" regExp=".{1,' . DAF_USER_AREA_LEN . '}" promptMessage="Enter an optional Area (eg Department)" ' . 
             'invalidMessage="Invalid Area (must be 1 to ' . DAF_USER_AREA_LEN . ' characters)"/></td>';
      } else if ($fieldname == "Email") {
        echo '<th>Email</th>';
        echo '<td><input type="text" name="email" value="' . $row[$i] . '" size="' . DAF_USER_EMAIL_LEN . '" maxlength="' . DAF_USER_EMAIL_LEN . '" ' .   /* is this validation to flexible *?  */
             'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_EMAIL_LEN . 'em;" regExp="[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}" required="true" trim="true" promptMessage="Enter a Email address (eg fred@acme.com)" ' . 
             'invalidMessage="Invalid Email address (must be 1 to ' . DAF_USER_EMAIL_LEN . ' characters in the fred@acme.com format)"/></td>';
      }
      echo "</tr>\n";
   }
   echo '<tr><th>Password</th>';
   echo '<td><input type="password" name="password" /></td></tr>';
   echo '<th>Confirm Password</th>';
   echo '<td><input type="password" name="confirm_password" /></td>';

   echo '</table>';
 
   cellbutton(null, "user_modify", "Modify User", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"user\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\" name=\"originalemail\" value=\"$originalemail\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\" value=\"$ID\"/>\n";
   cellbutton(null, "user_cancel", "Cancel", "index.php?object=user&action=show&ID=$ID", null);
 
   echo "</div>\n";
  
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   outcomeaction
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a outcomeaction object.   The allowed actions are to create, modify or delete the 
// outcomeaction record in the outcomeaction table.
//
//----------------------------------------------------------------------------------------------------------------------

function outcomeaction($gp) {

  global $db_server;

  if ($gp['object'] !== "outcomeaction") {
     add_error_message("DAFE001: outcomeaction: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "outcomeaction";
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
        if ((count_rows("daf.outcomeaction", "Name = '" . $gp['name'] . "'") > 0)) {
          
            add_error_message("DAFE0022: Cannot create project " . $gp['name'] . " because a project of that name already exists");
       
        } else {
        
            $query = "INSERT INTO daf.outcomeaction (Name, Actiononpass, Actiononfail, Scenariotoberunonpass, Scenariotoberunonfail, Erroraction, Notificationonpass, Notificationonfail, MaillistID) " .
                     "VALUES('" . $gp['name'] . "','" . $gp['actiononpass'] . "','" .
                     $gp['actiononfail'] . "','" . $gp['scenariotoberunonpass'] . "','" . $gp['scenariotoberunonfail'] . "','" . $gp['erroraction'] . "','" . 
                     $gp['notificationonpass'] . "','" . $gp['notificationonfail'] . "','" . $gp['maillistID']  . "')";              
            do_mysql_query($query, $db_server, 1);
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {      
      
         if ((count_rows("daf.outcomeaction", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0023: Cannot modify outcomeaction name to " . $gp['name'] . " because a outcomeaction of that name already exists");
         
         } else {

            $query = "UPDATE daf.outcomeaction SET Name='" . $gp['name'] . "', Actiononpass='" . $gp['actiononpass'] . "', actiononfail='" . $gp['actiononfail'] . 
                     "', Scenariotoberunonpass='" . $gp['scenariotoberunonpass'] . "', Scenariotoberunonfail='" . $gp['scenariotoberunonfail'] . 
                     "', Erroraction='" . $gp['erroraction'] . "', Notificationonpass='" . $gp['notificationonpass']  . "', Notificationonfail='" . $gp['notificationonfail']  . 
                     "', MaillistID='" . $gp['maillistID'] . "' WHERE ID='" . $gp['ID'] . "'";              
            do_mysql_query($query, $db_server, 1);
         }
         $gp['action'] = 'show';      
         $gp['ID'] = null;

      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.outcomeaction where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_outcomeaction();
    
   } else if ($gp['action'] === "modify") {

       modify_outcomeaction($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_outcomeactions();
 
      } else {
 
         show_detailed_outcomeaction($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("outcomeaction");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_outcomeaction
//
// Inputs:     none   
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new outcomeaction.    The routine posts
//
// status.php?object=outcomeaction&action=show&operation=create&name=...&actiononpass=...&scenariotoberunonpass=...
//                                &actiononfail=...&scenariotoberunonfail=...&erroraction=...&notificationpass=...&notificationonfail=...&maillist=...  
//
//----------------------------------------------------------------------------------------------------------------------

function create_outcomeaction() {

   echo '<div dojoType="dijit.form.Form" id="create_outcomeaction" jsId="create_outcomeaction" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create an Outcome Action</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_OUTCOMEACTION_LEN . '" maxlength="' . DAF_OUTCOMEACTION_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OUTCOMEACTION_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OUTCOMEACTION_LEN . '}" required="true" trim="true" promptMessage="Enter an Outcome Action" ' . 
        'invalidMessage="Invalid Project Description (must be 1 to ' . DAF_OUTCOMEACTION_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo "<tr><th>Action on Pass</th><td><select name=\"actiononpass\" />\n";
   echo "<option value=\"none\">none</option>\n";
   echo "<option value=\"runscenario\">run scenario</option>\n";
   echo "</select></td></tr>\n";
   echo "<tr><th>Scenario to be run on Pass</th><td><select name=\"scenariotoberunonpass\" />\n";
   selectorID("scenario", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Action on Fail</th><td><select name=\"actiononfail\" />\n";
   echo "<option value=\"none\">none</option>\n";
   echo "<option value=\"runscenario\">run scenario</option>\n";
   echo "</select></td></tr>\n";
   echo "<tr><th>Scenario to be run on Fail</th><td><select name=\"scenariotoberunonfail\" />\n";
   selectorID("scenario", "Name");
   echo "</select></td></tr>\n";
   echo "<tr><th>Error Action</th><td><select name=\"erroraction\" />\n";
   echo "<option value=\"Terminatescenario\">terminate scenario</option>\n";
   echo "<option value=\"Continuescenario\">continue scenario</option>\n";
   echo "</select></td></tr>\n";
   echo "<tr><th>Notification on Pass</th><td><select name=\"notificationonpass\" />\n";
   echo "<option value=\"None\">none</option>\n";
   echo "<option value=\"Tester\">tester</option>\n";
   echo "<option value=\"Maillist\">mail list</option>\n";
   echo "</select></td></tr>\n";
   echo "<tr><th>Notification on Fail</th><td><select name=\"notificationonfail\" />\n";
   echo "<option value=\"None\">none</option>\n";
   echo "<option value=\"Tester\">tester</option>\n";
   echo "<option value=\"Maillist\">mail list</option>\n";
   echo "</select></td></tr>\n";
   echo "<tr><th>Mail list</th><td><select name=\"maillistID\" />\n";
   selectorID("maillist", "Name");
   echo "</select></td></tr>\n";
   echo "</table>\n";
   
   cellbutton(null, "actionoutcomes_create", "Create Outcome Action", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"create\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"outcomeaction\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "actionoutcomes_cancel", "Cancel", "index.php?object=outcomeaction&action=show", null);
   
   echo "</form>\n";
   echo "</div>";
      
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_outcomeactions
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all outcomeactions.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=outcomeaction?action=create&object=phase             to create a new outcomeaction
// status.php?object=outcomeaction?action=modify&object=phase&ID=...      to modify an existing outcomeaction
// status.php?object=outcomeaction?action=delete&object=phase&ID=...      to delete an existing outcomeaction
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_outcomeactions() {
   
   $query = "SELECT ID, Name, Actiononpass, Actiononfail, Scenariotoberunonpass, Scenariotoberunonfail, Erroraction, Notificationonpass, Notificationonfail, MaillistID FROM daf.outcomeaction";   
   $result = mysql_query($query);
    
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.scenario", "ID", "Name");
   $fieldnames = array("ID", "Name", "Action on Pass", "Action on Fail", "Scenario to be run on pass", "Scenario to be run on fail", "Error action", 
                       "Notification on pass", "Notification on fail", "Mail List");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenario Outcome Actions</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "actionoutcomes_create", "Create Outcome Action", "index.php?action=create&object=outcomeaction");
   echo "</th>\n";
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $IDindex                 = $fieldindex['ID'];
   $nameindex               = $fieldindex['Name'];
   $scenariotoberunonpassID = $fieldindex['Scenariotoberunonpass'];
   $scenariotoberunonfailID = $fieldindex['Scenariotoberunonfail'];
   $maillistIDindex         = $fieldindex['MaillistID'];

   $lookupmaillist = lookuptable("daf.maillist", "ID", "Name");
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       $ID = $row[$IDindex];
       $name = $row[$nameindex];
       echo '<tr>';
       echo "<td class=td_smd>";
       cellbutton(null, "actionoutcomes_show_$j",   "show",   "index.php?action=show&object=outcomeaction&name=" . urlencode($name)   . "&ID=$ID");   /* <<<< need to add urlencode elsewhere */
       echo "</td>";
       echo "<td class=td_smd>";
       cellbutton(null, "actionoutcomes_modify_$j", "modify", "index.php?action=modify&object=outcomeaction&name=" . urlencode($name) . "&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       // cellbutton('td', "actionoutcomes_delete_$j", "delete", "index.php?action=show&operation=delete&object=outcomeaction&ID=$ID");
       button_with_confirm("actionoutcomes_delete_$j", "delete", "delete", "index.php?action=show&operation=delete&object=outcomeaction&ID=$ID", 
                           "index.php?action=show&object=actionoutcomes");
       echo "</td>";
      
       for ($i = 0; $i < $num_fields; $i++) {       
          if (($i == $scenariotoberunonpassID) || ($i == $scenariotoberunonfailID)) {
             if (isset($lookup[$row[$i]])) {
                echo '<td>' . $lookup[$row[$i]] . '</td>';
             } else {
                echo '<td> &nbsp; </td>';
             }
          } else if ($i == $maillistIDindex) {
             if (isset($lookupmaillist[$row[$i]])) {                 
                echo '<td>' . $lookupmaillist[$row[$i]] . '</td>';
              } else {
                echo '<td> &nbsp; </td>';
             }            
          } else {         
            echo '<td>' . $row[$i] . '</td>';
          }
       }   
       echo '</tr>';
       echo "\n";

   }
  
   echo '</table>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_outcomeaction
//
// Inputs:     ID                the ID of the row in the DAF project table containing the outcomeaction whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of an outcomeaction
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_outcomeaction($ID) {

   $query = "SELECT ID, Name, Actiononpass, Actiononfail, Scenariotoberunonpass, Scenariotoberunonfail, Erroraction, Notificationonpass, Notificationonfail, " . 
             "MaillistID FROM daf.outcomeaction where ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.scenario", "ID", "Name");
   $fieldnames = array("ID", "Name", "Action on Pass", "Action on Fail", "Scenario to be run on pass", "Scenario to be run on fail", "Error Action", 
                       "Notification on pass", "Notification on fail", "Mail List");

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenario Outcome Action</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $scenariotoberunonpassID = $fieldindex['Scenariotoberunonpass'];
   $scenariotoberunonfailID = $fieldindex['Scenariotoberunonfail'];
   $maillistIDindex         = $fieldindex['MaillistID'];

   $lookupmaillist = lookuptable("daf.maillist", "ID", "Name");
   
   $row = mysql_fetch_row($result);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $scenariotoberunonpassID) {
         echo '<th>Scenario to be run on Pass</th>';
         if (isset($lookup[$row[$i]])) {
            echo '<td>' . $lookup[$row[$i]] . '</td>';
         } else {
            echo '<td> &nbsp; </td>';
         }
      } else if ($i == $scenariotoberunonfailID) {
         echo '<th>Scenario to be run on Fail</th>';
         if (isset($lookup[$row[$i]])) {
            echo '<td>' . $lookup[$row[$i]] . '</td>';
         } else {
            echo '<td> &nbsp; </td>';
         }
      } else if ($i == $maillistIDindex) {
         echo '<th>Mail List</th>';
         echo '<td>' . $lookupmaillist[$row[$i]] . '</td>';
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
// Function:   modify_outcomeaction
//
// Inputs:     $ID           the ID of the record in the outcomeaction table for the outcomeaction that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of an outcomeaction that is already in the DAF project
// table.   The routine posts
//
// status.php?object=outcomeaction&action=show&operation=create&name=...&actiononpass=...&scenariotoberunonpass=...
//                                 &actiononfail=...&scenariotoberunonfail=...&erroraction=...&notificationpass=...&notificationonfail=...&maillist=...
//                                   
//----------------------------------------------------------------------------------------------------------------------

function modify_outcomeaction($ID) {

   $query = "SELECT ID, Name, Actiononpass, Actiononfail, Scenariotoberunonpass, Scenariotoberunonfail, Erroraction, Notificationonpass, Notificationonfail, MaillistID FROM daf.outcomeaction where ID = '" . $ID . "'"; 
   $result = mysql_query($query);
   
   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.outcomeaction", "ID", "Name");
   $fieldnames = array("ID", "Name", "Action on Pass", "Action on Fail", "Scenario to be run on pass", "Scenario to be run on fail", "Error action", 
                        "Notification on pass", "Notification on fail", "Mail List");

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="modify_outcomeaction" jsId="modify_outcomeaction" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Scenario Outcome Action</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $actiononpassindex          = $fieldindex['Actiononpass'];
   $actiononfailindex          = $fieldindex['Actiononfail'];
   $scenariotoberunonpassindex = $fieldindex['Scenariotoberunonpass'];
   $scenariotoberunonfailindex = $fieldindex['Scenariotoberunonfail'];  
   $erroractionindex         = $fieldindex['Erroraction'];
   $notificationonpassindex = $fieldindex['Notificationonpass'];
   $notificationonfailindex = $fieldindex['Notificationonfail'];
   $maillistIDindex         = $fieldindex['MaillistID'];
   
   $row = mysql_fetch_row($result);
   $originalname            = $row[$fieldindex['Name']];
 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = $fieldnames[$i];
      if ($i == $IDindex) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if  ($i == $actiononpassindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"actiononpass\" />\n";  
         echo "<option value=\"none\" "        . ($row[$i] == "None"        ? "selected" : "") . ">none</option>\n";
         echo "<option value=\"runscenario\" " . ($row[$i] == "Runscenario" ? "selected" : "") . ">run scenario</option>\n";
         echo "</select><td>\n";
      } else if  ($i == $actiononfailindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"actiononfail\" />\n";  
         echo "<option value=\"none\" "        . ($row[$i] == "None"        ? "selected" : "") . ">none</option>\n";
         echo "<option value=\"runscenario\" " . ($row[$i] == "Runscenario" ? "selected" : "") . ">run scenario</option>\n";
         echo "</td></select></td>\n";
      } else if  ($i == $scenariotoberunonpassindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"scenariotoberunonpass\" />\n"; 
         selectorID("scenario", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if  ($i == $scenariotoberunonfailindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"scenariotoberunonfail\" />\n"; 
         selectorID("scenario", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if  ($i == $erroractionindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"erroraction\" />\n";  
         echo "<option value=\"terminatescenario\" "  . ($row[$i] == "Terminatescenario"  ? "selected" : "") . ">terminate scenario</option>\n";
         echo "<option value=\"continuescenario\" "   . ($row[$i] == "Continuescenario"   ? "selected" : "") . ">continue scenario</option>\n";
         echo "</select></td>\n";
      } else if  ($i == $notificationonpassindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"notificationonpass\" />\n";  
         echo "<option value=\"none\" "     . ($row[$i] == "None"      ? "selected" : "") . ">none</option>\n";
         echo "<option value=\"tester\" "   . ($row[$i] == "Tester"    ? "selected" : "") . ">tester</option>\n";
         echo "<option value=\"maillist\" " . ($row[$i] == "Maillist"  ? "selected" : "") . ">maillist</option>\n";
         echo "</select></td>\n";
      } else if  ($i == $notificationonfailindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"notificationonfail\" />\n";  
         echo "<option value=\"none\" "     . ($row[$i] == "None"      ? "selected" : "") . ">none</option>\n";
         echo "<option value=\"tester\" "   . ($row[$i] == "Tester"    ? "selected" : "") . ">tester</option>\n";
         echo "<option value=\"maillist\" " . ($row[$i] == "Maillist"  ? "selected" : "") . ">maillist</option>\n";
         echo "</select></td>\n"; 
      } else if ($i == $maillistIDindex) {
         echo '<th>' . $fieldname . '</th>';
         echo "<td><select name=\"maillistID\" />\n"; 
         selectorID("maillist", "Name", $row[$i]);
         echo "</select></td>\n";
      } else if ($fieldname == "Name") {
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size="' . DAF_OUTCOMEACTION_LEN . '" maxlength="' . DAF_OUTCOMEACTION_LEN . '" ' .
              'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_OUTCOMEACTION_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_OUTCOMEACTION_LEN . '}" required="true" trim="true" promptMessage="Enter an Outcome Action" ' . 
              'invalidMessage="Invalid Project Description (must be 1 to ' . DAF_OUTCOMEACTION_LEN . ' alphanumeric characters)"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
 
   cellbutton('div', "outcomeaction_modify", "Modify Scenario Outcome Action", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"outcomeaction\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\" value=\"$ID\"/>\n";
   echo "<input type=\"hidden\" name=\"originalname\" value=\"$originalname\"/>\n";
   cellbutton(null, "outcomeaction_cancel", "Cancel", "index.php?object=outcomeaction&action=show&ID=$ID", null);
  
   echo "</div>\n";
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   maillist
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a maillist object.   The allowed actions are to create, modify or delete the 
// maillist record in the maillist table, as well as add and delete maillist members
//
//--------------------------------------------------------------------------------------------------------------------

function maillist($gp) {

   global $db_server;

   if ($gp['object'] !== "maillist") {
      add_error_message("DAFE0001: maillist: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "maillist";
   }
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.maillist", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0024: Cannot create maillist " . $gp['name'] . " because a maillist of that name already exists");
             
         } else {
      
            $query = "INSERT INTO daf.maillist (Name, Description) " .
                     "VALUES('" . $gp['name'] . "', '" . $gp['description'] . "')"; 
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         if ((count_rows("daf.outmaillist", "Name = '" . $gp['name'] . "'") > 0) && ($gp['name'] != $gp['originalname'])) {
          
            add_error_message("DAFE0025: Cannot modify Mail List name to " . $gp['name'] . " because a Mail List of that name already exists");
         
         } else {
         
            $query = "UPDATE daf.maillist SET Name='" . $gp['name'] . "', Description='" . $gp['description'] . 
                     "' WHERE ID='" . $gp['ID'] . "'"; 
            do_mysql_query($query, $db_server, 1);
            
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;      

      } else if ($gp['operation'] === "createmaillistmember") {
      
         // check to see if there is already a maillistmember in this maillist for the requested userid - if there is then 
            // reject the attempt to create this maillistmember
            $exists = get_fields_in_objects("maillistmember", "ID", "UserID = '" . $gp['userID'] . "' AND MaillistID = '" . $gp['maillistID']  . "'");
            if (count($exists) > 0) {
               $lookupmaillist = lookuptable("daf.maillist", "ID", "Name");
               $lookupusername = lookuptableforusernames("daf.user", "ID");
               echo "There is already a mail list member for user " .  $lookupusername[$gp['userID']]  . " in maillist "  .  
                    $lookupmaillist[$gp['maillistID']] . " - cannot create a second one\n";
            }

         $query = "INSERT INTO daf.maillistmember (MaillistID, UserID) VALUES('" . $gp['maillistID'] . "', '" . $gp['userID'] . "')"; 
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = null;
  
      } else if ($gp['operation'] === "modifymaillistmember") {
      
         // check to see if there is already a maillistmember in this maillist for the requested userid - if there is then 
         // reject the attempt to modify this maillistmember
         $exists = get_fields_in_objects("maillistmember", "ID", "UserID = '" . $gp['userID'] . "' AND MaillistID = '" . $gp['maillistID']  . "'");
         if (count($exists) > 0) {
            $lookupmaillist = lookuptable("daf.maillist", "ID", "Name");
            $lookupusername = lookuptableforusernames("daf.user", "ID");
            echo "There is already a mail list member for user " .  $lookupusername[$gp['userID']]  . " in maillist "  .  
               $lookupmaillist[$gp['maillistID']] . " - cannot create a second one\n";
            modify_maillistmember($gp['ID']);   
            return;
         }

         $query = "UPDATE daf.maillistmember SET MaillistID='" . $gp['maillistID'] . "', UserID='" . $gp['userID'] .
                  "' WHERE ID='" . $gp['ID'] . "'"; 
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show'; 
         $gp['ID'] = $gp['maillistID'];     
  
      } else if ($gp['operation'] === "deletemaillistmember") {

         $query = "DELETE FROM daf.maillistmember where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;


      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.maillist where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      } 
   
   }
   
   if ($gp['action'] === "createmaillistmember") {
   
      create_maillistmember($gp['maillistID']);
   
   } else if ($gp['action'] === "modifymaillistmember") {
   
      modify_maillistmember($gp['ID']);   
   
   } else if ($gp['action'] === "create") {
   
       create_maillist();
    
   } else if ($gp['action'] === "modify") {

       modify_maillist($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_maillists("maillist", "Mail Lists");
           
      } else {
 
         show_detailed_maillist($gp['ID']);
      }
   
   } else {
    
      report_bad_parms("maillist");
   
   }
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   maillistmember
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a maillistmember object.   The allowed actions are to create or delete the 
// maillistmember record in the maillistmmber table.
//
//----------------------------------------------------------------------------------------------------------------------


function maillistmember($gp) {

   if ($gp['object'] !== "maillistmember") {
      add_error_message("DAFE0001: maillistmember: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "maillistmember";
   }
   
   if ($gp['action'] === "show") {

      show_table("maillistmember", "Mail List Members");         /* does this need a specialist display routine ? <<<< */
    
   } else {
    
      report_bad_parms("maillistmember");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_maillist
//
// Inputs:     none   
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new maillist    The routine posts
//
// status.php?object=maillist?action=show&operation=create&name=...&description=...  
//
//---------------------------------------------------------------------------------------------------------------------

function create_maillist() {

   echo '<div dojoType="dijit.form.Form" id="create_maillist" jsId="create_maillist" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Mail List</div></caption>\n";

   echo '<tr><th>Name</th>';
   echo '<td><input type="text" name="name" size="' . DAF_MAILLIST_LEN . '" maxlength="' . DAF_MAILLIST_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_MAILLIST_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_MAILLIST_LEN . '}" required="true" trim="true" promptMessage="Enter a Mail List name" ' . 
        'invalidMessage="Invalid Mail List name (must be 1 to ' . DAF_MAILLIST_LEN . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo '<tr><th>Description</th>';
   echo '<td><input type="text" name="description" size="' . DAF_MAILLIST_DESCRIPTION_LEN . '" maxlength="' . DAF_MAILLIST_DESCRIPTION_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_MAILLIST_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Mail List description" ' . 
        'invalidMessage="Invalid Mail List description (must be 1 to ' . DAF_MAILLIST_DESCRIPTION_LEN . ' characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";

   cellbutton(null, "maillist_create", "Create", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"create\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"maillist\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\"/>\n";
   cellbutton(null, "maillist_cancel", "Cancel", "index.php?object=maillist&action=show", null);
   
   echo "</div>\n";
         
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_maillists
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all phases.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=maillist&action=create             to create a new maillist
// status.php?object=maillist&action=modify&ID=...      to modify an existing maillist
// status.php?object=maillist&action=delete&ID=...      to delete an existing maillist
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_maillists() {

   $query  = "SELECT * FROM daf.maillist";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Mail Lists</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   echo "<th colspan=3>";
   cellbutton(null, "maillist_create", "Create Mail List", "index.php?action=create&object=maillist");
   echo "</th>\n";
   
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      echo '<th>' . $fieldname . '</th>';
   }
  
   echo "</tr>\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       echo "<td class=td_smd>";
       cellbutton(null, "maillist_show_$j", "show", "index.php?action=show&object=maillist&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       cellbutton(null, "maillist_modify_$j", "modify", "index.php?action=modify&object=maillist&ID=$ID");
       echo "</td>\n";
       echo "<td class=td_smd>";
       // cellbutton(null, "maillist_delete_$j", "delete", "index.php?action=show&operation=delete&object=maillist&ID=$ID");
       button_with_confirm("project_delete_$j", "delete", "delete", "delete", "index.php?action=show&operation=delete&object=maillist&ID=$ID", "index.php?action=show&object=maillistID");
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
// Function:   show_detailed_maillist
//
// Inputs:     $maillistID       the ID of the row in the DAF maillist table containing the maillist whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a maillist//
//
//--------------------------------------------------------------------------------------------------------------------

function show_detailed_maillist($maillistID) {

   $query = "SELECT * FROM daf.maillist where ID = '" . $maillistID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Mail List</div></caption>";
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
      echo '<th>' . mysql_field_name($result, $i) . '</th>';
      echo '<td>'  . $row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   $query = "SELECT maillistmember.ID, user.Firstname, user.Lastname, user.Email, maillistmember.userID FROM daf.maillistmember INNER JOIN daf.user " . 
             "ON maillistmember.userID=user.ID where maillistID = '" . $maillistID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "\n";
   
   $fieldindex = array();
   echo '<tr>';
   
   echo "<th colspan=2>";
   cellbutton(null, "maillist_add", "Add Mail List Member", "index.php?action=createmaillistmember&object=maillist&maillistID=$maillistID");
   echo "</th>\n";
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      echo "<th>$fieldname</th>\n";
   }
   echo "</tr>\n";   
   
   for ($i=0; $i<$num_rows; $i++) {
   
      $row = mysql_fetch_row($result);
      $maillistmemberID = $row[$fieldindex['ID']]; 

      echo '<tr>';
      echo "<td class=td_smd>";
      cellbutton(null, "maillist_add_$i", "modify", "index.php?action=modifymaillistmember&object=maillist&ID=$maillistmemberID&maillistID=$maillistID");
      echo "</td>";
      echo "<td class=td_smd>";
      // cellbutton(null, "maillist_delete_$i", "delete", "index.php?action=show&operation=deletemaillistmember&object=maillist&ID=$maillistmemberID&maillistID=$maillistID");
      button_with_confirm("maillist_delete_$j", "delete", "delete", "index.php?action=show&operation=deletemaillistmember&object=maillist&ID=$maillistmemberID&maillistID=$maillistID", 
                          "index.php?action=show&object=maillist");
      echo "</td>";
      for ($j = 0; $j < $num_fields; $j++) {
         echo '<td>'  . $row[$j] . '</td>';
         echo "\n";
      }
      echo '</tr>';
      
   }
   echo '</table>';
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_maillist
//
// Inputs:     $ID           the ID of the record in the maillist table that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a maillist that is already in the DAF project
// table.   The routine posts
//
// status.php?object=maillist&action=show&operation=modify& ....... ????? 
//                                   
//----------------------------------------------------------------------------------------------------------------------

function modify_maillist($ID) {

   $query = "SELECT ID, Name, Description FROM daf.maillist where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="modify_maillist" jsId="modify_maillist" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Mail List</div></caption>\n";
   
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
      if ($fieldname == "ID") {
         echo '<th>ID</th>';
         echo '<td>' . $row[$i] . '</td>';
      } else if ($fieldname == "Name") {
         $originalname = $row[$i];
         echo '<th>Name</th>';
         echo '<td><input type="text" name="name" value="' . $row[$i] . '" size="' . DAF_MAILLIST_LEN . '" maxlength="' . DAF_MAILLIST_LEN . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_MAILLIST_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_MAILLIST_LEN . '}" required="true" trim="true" promptMessage="Enter a Mail List name" ' . 
        'invalidMessage="Invalid Mail List name (must be 1 to ' . DAF_MAILLIST_LEN . ' alphanumeric characters)"/></td>';
      } else if ($fieldname == "Description") {
         echo '<th>Description</th>';
         echo '<td><input type="text" name="description" value="' . $row[$i] . '" size="' . DAF_MAILLIST_DESCRIPTION_LEN . '" maxlength="' . DAF_MAILLIST_DESCRIPTION_LEN . '" ' . 
              'dojoType="dijit.form.ValidationTextBox" style="width:100%" regExp=".{1,' . DAF_MAILLIST_DESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Mail List description" ' . 
              'invalidMessage="Invalid Mail List description (must be 1 to ' . DAF_MAILLIST_DESCRIPTION_LEN . ' characters)"/></td>';
      }      
      echo "</tr>\n";
   }

   echo '</table>';
   
   cellbutton('div', "maillist_modify", "Modify Mail List", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"maillist\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\" name=\"ID\" value=\"$ID\"/>\n";
   echo "<input type=\"hidden\" name=\"originalname\" value=\"$originalname\"/>\n";
   cellbutton(null, "maillist_cancel", "Cancel", "index.php?object=maillist&action=show&ID=$ID", null);
   
   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_maillistmember
//
// Inputs:     $maillistID           the ID of the record in the maillist table that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to create a new maillistmember that in the DAF project
// table.   The routine posts
//
// status.php?object=maillistmember&action=show&operation=create&maillistID=...&userID=...&maillistmemberID=...
//                                   
//----------------------------------------------------------------------------------------------------------------------

function create_maillistmember($maillistID) {

   echo '<div dojoType="dijit.form.Form" id="create_maillistmember" jsId="create_maillistmember" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Create a new Mail List Member</div></caption>\n";

   echo "<tr><th>User Name</th><td><select name=\"userID\" />\n";
   selectorIDforUserName($ID = null); 
   echo "</select></td></tr>\n"; 
   echo "</table>\n";
   
   cellbutton(null, "maillistmember_create", "Create Mail List Member", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"maillist\"/>\n";
   echo "<input type=\"hidden\" name=\"maillistID\" value=\"$maillistID\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"createmaillistmember\"/>\n";
   cellbutton(null, "maillistmember_cancel", "Cancel", "index.php?object=maillist&action=show", null);

   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_maillistmember
//
// Inputs:     $maillistmemberID           the ID of the record in the maillist table that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a maillistmember that is already in the DAF project
// table.   The routine posts
//
// status.php?object=maillistmember&action=show&operation=modify&maillistID=...&userID=...&maillistmemberID=...
//                                   
//----------------------------------------------------------------------------------------------------------------------

function modify_maillistmember($maillistmemberID) {

   $query = "SELECT ID, MaillistID, UserID FROM maillistmember where ID = '" . $maillistmemberID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<div dojoType="dijit.form.Form" id="modify_maillistmember" jsId="modify_maillistmember" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Mail List</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $userIDindex = $fieldindex['UserID'];
   $maillistIDindex = $fieldindex['MaillistID'];
   
   $lookupmaillist = lookuptable("maillist", "ID", "Name");
  
   $row = mysql_fetch_row($result);
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if ($i == $IDindex) {
         echo '<th>ID</th>';
         echo '<td> ' . $row[$i] . '</td>';
      } else if ($i == $maillistIDindex) {  
         echo '<th>Mail List</th>';       
         echo '<td><select name="maillistID"/>' . "\n";
         selectorID("maillist", "Name", $row[$i]);
         echo '</select></td>' . "\n";
      } else if ($i == $userIDindex) {
         echo '<th>User Name</th>' . "\n";
         echo '<td><select name="userID"/>' . "\n";
         selectorIDforUserName($ID = NULL); 
         echo '</select></td>' . "\n";
      }   
      echo "</tr>\n";
   }

   echo '</table>';
   
   cellbutton('div', "maillistmember_modify", "Modify Mail List Member", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"maillistmember\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"modify\"/>\n";
   echo "<input type=\"hidden\" name=\"maillistmemberID\" value=\"$ID\"/>\n";
   cellbutton(null, "maillistmember_cancel", "Cancel", "index.php?object=maillistmember&action=show&ID=$ID", null);
   
   echo "</div>\n";

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   scenarioset
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a scenarioset object.   The allowed actions are to create, modify or delete the 
// scenarioset record in the scenarioset table and to add and delete members to the scenarioset
//
//--------------------------------------------------------------------------------------------------------------------

function scenarioset($gp) {

   global $db_server;

   if ($gp['object'] !== "scenarioset") {
      add_error_message("DAFE0001: scenarioset: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "scenarioset";
   }  
   
   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
      
         if (count_rows("daf.scenarioset", "Name = '" . $gp['name'] . "'") > 0) {
          
            add_error_message("DAFE0026: Cannot create scenarioset " . $gp['name'] . " because a scenarioset of that name already exists");
             
         } else {
   
            $query = "INSERT INTO daf.scenarioset (Name, Description, ProjectID, PhaseID) " .
                     "VALUES('" . $gp['name'] . "', '" . $gp['description'] . "', '" . $gp['projectid'] . "', '" . $gp['phaseid'] . "')"; 
            do_mysql_query($query, $db_server, 1);
            
         }
         
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {
      
         $query = "UPDATE daf.scenarioset SET Name='" . $gp['name'] . "', Description='" . $gp['description']  .
                  "', ProjectID='" . $gp['projectid']  . "', PhaseID='" . $gp['phaseid'] . 
                  "' WHERE ID='" . $gp['ID'] . "'"; 
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;      

      } else if ($gp['operation'] === "createscenariosetmember") {
      
         // check to see if there is a scenariosetmember in this scenarioset for the requested scenariotype - if there is then 
            // reject the attempt to create this scenariosetmember
            $exists = get_fields_in_objects("scenariosetmember", "ID", "scenarioID = '" . $gp['scenarioID'] . "' AND scenariosetID = '" . $gp['scenariosetID']  . "'");
            if (count($exists) > 0) {
               $lookupscenarioset = lookuptable("daf.scenarioset", "ID", "Name");
               $lookupscenario = lookuptable("daf.scenario", "ID", "Name");
               add_error_message("DAFE0027: There is already a scenario set member for scenario " .  $lookupscenario[$gp['scenarioID']]  . " in scenarioset "  . 
                    $lookupscenarioset[$gp['scenariosetID']] . " - cannot create a second one");
               return;
            }


         $query = "INSERT INTO daf.scenariosetmember (scenarioID, scenariosetID) " .
                  "VALUES('" . $gp['scenarioID'] . "', '" . $gp['scenariosetID'] . "')"; 
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = $gp['scenariosetID'];
  
      } else if ($gp['operation'] === "modifyscenariosetmember") {

         $query = "UPDATE daf.scenariosetmember SET scenarioID='" . $gp['scenarioID'] . "', ScenariosetID='" . $gp['scenariosetID'] . 
                  "' WHERE ID='" . $gp['ID'] . "'"; 
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show'; 
         $gp['ID'] = $gp['scenariosetID'];     
  
      } else if ($gp['operation'] === "deletescenariosetmember") {

         $query = "DELETE FROM daf.scenariosetmember where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
         $gp['ID'] = $gp['scenariosetID'];


      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.scenarioset where ID='" . $gp['ID'] . "'";    
         do_mysql_query($query, $db_server, 1);
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      } 
   
   }
   
   if ($gp['action'] === "createscenariosetmember") {
   
      create_scenariosetmember($gp['scenariosetID']);
   
   } else if ($gp['action'] === "modifyscenariosetmember") {
   
      modify_scenariosetmember($gp['ID']);   
   
   } else if ($gp['action'] === "create") {
   
       create_scenarioset();
    
   } else if ($gp['action'] === "modify") {

       modify_scenarioset($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_scenariosets("scenarioset", "scenario Sets");
           
      } else {
 
         show_detailed_scenarioset($gp['ID']);
      }
   
   } else {
    
      echo "scenarioset: did not understand params";
      echo "<pre>";
      print_r($gp);
      echo "</pre>\n";
   
   }
    
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_scenariosetmember
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

function scenariosetmember($gp) {

  if ($gp['object'] !== "scenariosetmember") {
     add_error_message("DAFE0001: scenariosetmember: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "scenariosetmember";
   }
   
  if ($gp['action'] === "show") {

    show_table("scenariosetmember", "scenario Set Members");
    
     echo '<div style="float:left;" display="inline">';
     scenariosets_button();
     echo '</div>'; 
 
     echo '<div style="float:left;" display="inline">';
     scenarios_button();
     echo '</div>'; 
   
     echo '<div style="float:left;" display="inline">';
     home_button();
     echo '</div>';     
    
   } else {
    
      echo "scenariosetmember: did not understand params";
      echo "<pre>";
      print_r($gp);
      echo "</pre>\n";
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_scenarioset
//
// Inputs:     none   
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new scenarioset    The routine posts
//
// status.php?object=scenarioset&action=show&operation=create&name=...&description=...&projectID=...&phaseID=...
//
//---------------------------------------------------------------------------------------------------------------------

function create_scenarioset() {

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Create a new Scenario Set</div></caption>\n";

   echo "<tr><th>Name</th><td><input type=\"text\" name=\"name\" /></td></tr>\n";
   echo "<tr><th>Description</th><td><input type=\"text\" name=\"description\" /></td></tr>\n";
   echo "<tr><th>Project</th><td><select name=\"projectid\" />\n";
   selectorID("project", "name");
   echo "</select></td></tr>\n";         
   echo "<tr><th>Phase</th><td><select name=\"phaseid\" />\n";
   selectorID("phase", "name");
   echo "</select></td></tr>\n";
   echo "</table>\n";
            
   echo "<input type=\"hidden\" name=\"action\" value=\"show\" />\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"scenarioset\" />\n";
   echo "<input type=\"submit\" name=\"button\" value=\"Create scenario Set\" />\n";
   echo "</form>\n";

   echo '<p>  <div class="floatleft">';
   scenariosets_button();
   echo "</div>\n"; 

   echo '<div class="floatleft">';
   scenarios_button();
   echo "</div>\n";         
         
   echo '<div class="floatleft">';
   home_button();
   echo "</div></p>\n";
         
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_scenariosets
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all scenariosets.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=scenarioset&action=create             to create a new scenarioset
// status.php?object=scenarioset&action=modify&ID=...      to modify an existing scenarioset
// status.php?object=scenariosets&action=delete&ID=...     to delete an existing scenarioset
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_scenariosets() {

   $query  = "SELECT scenarioset.ID, scenarioset.Name, scenarioset.Description, project.Name as 'Project Name', phase.Name as 'Phase Name' FROM daf.scenarioset " . 
             "INNER JOIN daf.project ON scenarioset.projectID = project.ID " .
             "INNER JOIN daf.phase ON scenarioset.phaseID = phase.ID";
             
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());
   
   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>scenario Sets</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr><th>show</th><th>modify</th><th>delete</th>';
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
      echo '<th>' . $fieldname . '</th>';
   }
  
   echo '<tr><th>scenario sets</th>';
   echo '</tr>';
   echo "\n";
   
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($result);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       /* $name = $row[$fieldindex['Name']]; */
       echo "<td class=td_smd><a href=index.php?action=show&object=scenarioset&ID=$ID>show</a></td>";
       echo "<td class=td_smd><a href=index.php?action=modify&object=scenarioset&ID=$ID>modify</a></td>";
       echo "<td class=td_smd><a href=index.php?action=show&operation=delete&object=scenarioset&ID=$ID>delete</a></td>";
       
       for ($i = 0; $i < $num_fields; $i++) {
          echo '<td>'  . $row[$i] . '</td>';
       }   
       echo '</tr>';
       echo "\n";

   }
   echo '</table>';
   
   echo '<div style="float:left;" display="inline">';
   echo '<form action="index.php" method="post">' . "\n";
   echo '<input type="submit" name=button value="Create scenario Set" />' . "\n";
   echo '<input type="hidden" name="action" value="create" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioset" />' . "\n";
   echo '</form>' . "\n"; 
   echo '</div>'; 
         
   echo '<div style="float:left;" display="inline">';
   scenariosets_button();
   echo '</div>'; 
         
   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>';          

   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_detailed_scenarioset
//
// Inputs:     $sceanriosetID    the ID of the row in the DAF scenarioset table containing the scenarioset whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a scenariosetID
//
//--------------------------------------------------------------------------------------------------------------------

function show_detailed_scenarioset($scenariosetID) {

   $query = "SELECT * FROM daf.scenarioset where ID = '" . $scenariosetID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>scenario Set</div></caption>";
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
      echo '<th>' . mysql_field_name($result, $i) . '</th>';
      echo '<td>'  . $row[$i] . '</td>';
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   $query = "SELECT scenariosetmember.ID, scenario.Name FROM daf.scenariosetmember INNER JOIN daf.scenario " . 
             "ON daf.scenariosetmember.scenarioID=scenario.ID where scenariosetID = '" . $scenariosetID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenario Set members</div></caption>";
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
      $scenariosetmemberID = $row[$fieldindex['ID']]; 

      echo '<tr>';
      echo "<td class=td_smd><a href=index.php?action=modifyscenariosetmember&object=scenarioset&ID=$scenariosetmemberID&scenariosetID=$scenariosetID>modify</a></td>";
      echo "<td class=td_smd><a href=index.php?action=show&operation=deletescenariosetmember&object=scenarioset&ID=$scenariosetmemberID&scenariosetID=$scenariosetID>delete</a></td>";
      for ($j = 0; $j < $num_fields; $j++) {
         echo '<td>'  . $row[$j] . '</td>';
         echo "\n";
      }
      echo '</tr>';
      
   }
   echo '</table>';
   
   echo '<div>';
   echo '<form action="index.php" method="post">' . "\n";
   echo '<input type="submit" name=button value="Add scenario Set Member" />' . "\n";
   echo '<input type="hidden" name="action" value="createscenariosetmember" />' . "\n";
   echo '<input type="hidden" name="scenariosetID" value="' . $scenariosetID . '" />' . "\n";
   echo '<input type="hidden" name="object" value="scenarioset" />' . "\n";
   echo '</div>'; 
   echo '</form>' . "\n"; 

   echo '<div style="float:left;" display="inline">';
   scenariosets_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>'; 
         
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';    
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_scenarioset
//
// Inputs:     $ID           the ID of the record in the scenarioset table that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a scenarioset that is already in the DAF project
// table.   The routine posts
//
// status.php?object=scenarioset&action=show&operation=modify&projectid=...&phaseid=...&name=.....
//
//---------------------------------------------------------------------------------------------------------------------

function modify_scenarioset($ID) {

   $query = "SELECT * FROM daf.scenarioset where ID = '" . $ID . "'";
   $result = mysql_query($query);

   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Scenario Sets</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex        = $fieldindex['ID'];
   $projectIDindex = $fieldindex['ProjectID'];
   $phaseIDindex   = $fieldindex['PhaseID'];
 
   $row = mysql_fetch_row($result);
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if ($i == $IDindex) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>' . $row[$i]   . '</td>';       
      } else if ($i == $projectIDindex) {
         echo "<th>Project</th><td><select name=\"projectid\" />\n";
         selectorID("project", "name");
         echo "</select></td>\n";         
      } else if ($i == $phaseIDindex) {
         echo "<th>Phase</th><td><select name=\"phaseid\" />\n";
         selectorID("phase", "name");
         echo "</select></td>\n";         
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $row[$i] . '"/></td>'; 
      }     
      echo "</tr>\n";
   }

   echo '</table>';
   
   echo '<div>';
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="scenarioset" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="submit" name=button value="Modify scenario Set" />' . "\n";
   echo '</div>'; 
   echo '</form>' . "\n";
   
   echo '<div style="float:left;" display="inline">';
   scenariosets_button();
   echo '</div>';      
         
   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>'; 
   
}

function create_scenariosetmember($scenariosetID) {

   echo "<form action=\"index.php\" method=\"post\">\n";
   
   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Create a Scenario Result</div></caption>\n";

   echo "<tr><th>scenario</th><td><select name=\"scenarioID\"/>\n";
   selectorID("scenario", "Name");
   echo "</select></td></tr>\n";
   echo "</table>\n";
   
   echo "<input type=\"hidden\" name=\"scenariosetID\" value=\"$scenariosetID\"/>\n";
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"createscenariosetmember\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"scenarioset\"/>\n";
   echo "<input type=\"submit\" name=\"button\" value=\"Create scenario Set Member\"/>\n";
   echo '</form>';

   echo '<p><div style="float:left;" display="inline">';
   scenariosets_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>'; 
         
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div></p>';

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_scenariosetmember
//
// Inputs:     $scenariosetmemberID           the ID of the record in the scenarioset table that is to be modified   
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a scenariosetmember that is already in the DAF project
// table.   The routine posts
//
// status.php?object=scenariosetmember&action=show&operation=modify&scenarioID=...&scenariosetID=...&scenariosetmemberID=...
//
//---------------------------------------------------------------------------------------------------------------------

function modify_scenariosetmember($scenariosetmemberID) {

   $query = "SELECT * FROM scenariosetmember where ID = '" . $scenariosetmemberID . "'";
   $result = mysql_query($query);

   if (! $result) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($result);   // should only be one row
   $num_fields = mysql_num_fields($result);

   echo '<form action="index.php" method="post">' . "\n";

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>scenario Sets</div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldname = mysql_field_name($result, $i);
      $fieldindex[$fieldname] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $scenarioIDindex = $fieldindex['ScenarioID'];
   $scenariosetIDindex = $fieldindex['ScenariosetID'];
   
  /* $lookupscenarioset = lookuptable("scenarioset", "ID", "Name"); */
  
   $row = mysql_fetch_row($result);
   
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($result, $i);
      if ($i == $IDindex) {
         echo '<th>ID</th>';
         echo '<td> ' . $row[$i] . '</td>';
         $scenariosetID = $row[$i];
      } else if ($i == $scenarioIDindex) {  
         echo '<th>Scenario</th>';       
         echo '<td><select name="scenarioID"/>' . "\n";
         selectorID("scenario", "Name", $row[$i]);
         echo '</select></td>' . "\n";
      } else if ($i == $scenariosetIDindex) {  
         echo '<th>Scenario Set</th>';       
         echo '<td><select name="scenariosetID"/>' . "\n";
         selectorID("scenarioset", "Name", $row[$i]);
         echo '</select></td>' . "\n";
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $row[$i] . '"/></td>';
      }      
      echo "</tr>\n";
   }

   echo '</table>';
   
   echo "<input type=\"hidden\" name=\"ID\"             value=\"$scenariosetmemberID\"/>\n";
   echo "<input type=\"hidden\" name=\"scenariosetID\"  value=\"$scenariosetID\"/>\n";         /* <<<<<< already appears above ??? */
   echo "<input type=\"hidden\" name=\"action\"         value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\"      value=\"modifyscenariosetmember\"/>\n";
   echo "<input type=\"hidden\" name=\"object\"         value=\"scenarioset\"/>\n";
   echo "<input type=\"submit\" name=\"button\"         value=\"Modify scenario Set Member\"/>\n";
   
   echo "</form>\n";

   echo '<div style="float:left;" display="inline">';
   scenariosets_button();
   echo '</div>'; 

   echo '<div style="float:left;" display="inline">';
   scenarios_button();
   echo '</div>'; 
 
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';   

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   backup
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do backup and restores of project and daf database data.                                        
//
//----------------------------------------------------------------------------------------------------------------------

function backup($gp) {
  
  if ($gp['object'] !== "backup") {
     add_error_message("DAFE0001: backup: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "backup";
   }
   
   if (! am_I_admin()) {
      add_error_message("DAFE0028: backup: cannot perform operation as current user does not have Administrator priviledges");
      show_backup($gp, NULL);
      return;
   }  
   
   mysql_select_db('daf') or                                                 /* <<<<<<<<<<<<<<< hard coded */
      die("unable to select $db_database database: " . mysql_error());   

   if ($gp['action'] === "backupdaf") {
   
       backupdaf($gp);
    
   } else if ($gp['action'] === "backupproject") {

       backupproject($gp);

   } else if ($gp['action'] === "uploadfile") {
   
       if (isset($_FILES['file']['name'])) {

          upload_filename($_FILES['file']['name']); 
       
       } else {
       
          add_error_message("DAFE0029: Could not perform file upload - filename was missing");
          
       }
       
       $gp['action'] = "show";
       show_backup($gp, NULL);
        
   } else if ($gp['action'] === "restore") {

       restore_backup($gp);

   } else if ($gp['action'] === "delete") {

       delete_backup($gp);

   } else if ($gp['action'] === "show") {

       show_backup($gp, NULL);
        
   } else {
    
      report_bad_parms("backup");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   backup_database              
//
// Inputs:     databasename                   the name of the database that is to be backed up                       
//             db_user                        the database user used to perform the backup
//             db_password                    the password for the database user
//             pathname                       the pathname that will contain the backup
//             completion_message             A displayable message indicating success or failure
//
// Outputs:    none  
//
// Returns:    FALSE for failure, TRUE for success
//
// Description:
//
// Backups a database                                                                                                                     
//
//
//---------------------------------------------------------------------------------------------------------------------

function backup_database($database_name, $pathname, &$completion_message) {

   global $db_username, $db_password;
   $rc = 0;

   $backupcmd  = "/opt/lampp/bin/mysqldump -u $db_username -p$db_password $database_name >$pathname 2>&1";
   exec($backupcmd, $output, $rc);
   # sanitise password
   $backupcmd  = "/opt/lampp/bin/mysqldump -u $db_username -p$XXXX $database_name >$pathname 2>&1";
   add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $backupcmd1), $rc) ;

   if ($rc != 0) {
     $completion_message = "Error code $rc from mysqldump: $backupcmd\n" . implode("\n", $output) . "\n";
   } else {
     $completion_message = "Backup of $database_name in $pathname completed successfully\n";
     $rc = 1;
   }

   return $rc;
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_backup                  
//
// Inputs:     gp                                                                                                    
//             completion_message
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a menu allowing the user to choose to perform backups or restores                                                             
//
//---------------------------------------------------------------------------------------------------------------------

function show_backup($gp, $completion_message = NULL) {

   if (isset($completion_message)) {
      add_info_message($completion_message);
   }

   $completion_message1 = NULL;
   get_backup_filenames(NULL, $completion_message1, $existing_backups);
   if (isset($completion_message1)) {
      add_info_message($completion_message1);
   }

echo <<<_END
   <table >
   <caption class="cap1"><div>Database Backup and Restore</div></caption>

   <form action="index.php" method="post">
      <tr>
      <td><input type="submit" name=button value="Backup DAF database" /></td>
      <td></td>
      </tr>
      <input type="hidden" name=action value="backupdaf" />
      <input type="hidden" name=object value="backup" />
   </form>

   <form action="index.php" method="post">
      <tr>
      <td><input type="submit" name=button value="Backup Results for a Project" /></td>
      <td>Project: <select name="projectID" onchange="selectPhases(this, 'backupPhaseId')"/>
_END;
      selectorID("project", "Name");      
echo <<<_END
      </select>
      Phase: <select id="backupPhaseId" name="phase"/>
      <--- onchange on the project select will fill this in --->
      </select>
      </td>
      </tr>
      <input type="hidden" name=action value="backupproject" />
      <input type="hidden" name=object value="backup" />
   </form>

   <form action="index.php" method="post">
      <tr>
      <td><input type="submit" name=button value="Restore a Backup" /></td>
      <td><select name="filename"/>
_END;
      selector_on_array($existing_backups);      
echo <<<_END
      </select></td>
      </tr>
      <input type="hidden" name=action value="restore" />
      <input type="hidden" name=object value="backup" />
   </form>

   <form action="download_backup.php" method="post">
      <tr>
      <td><input type="submit" name=button value="Download a Backup" /></td>
      <td><select name="filename"/>
_END;
      selector_on_array($existing_backups);      
echo <<<_END
      </select></td>
      </tr>
   </form>
   
   <form action="index.php" method="post" enctype="multipart/form-data">
      <tr>
      <td><input type="submit" name=button value="Upload a Backup" /></td>
      <td><input type="file" name="file" id="file" /></td>
      <input type="hidden" name=action value="uploadfile" />
      <input type="hidden" name=object value="backup" />
      </tr>
   </form>

   <form action="index.php" method="post">
      <tr>
      <td><input type="submit" name=button value="Delete a Backup" /></td>
      <td><select name="filename"/>
_END;
      selector_on_array($existing_backups);      
echo <<<_END
      </select></td>
      </tr>
      <input type="hidden" name=action value="delete" />
      <input type="hidden" name=object value="backup" />
   </form>

   </table>
_END;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   backupdaf
//
// Inputs:     gp                                                                                                    
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Backs up the main DAF database                                                                                                         
//
//   format of the backupname is  /opt/lampp/htdocs/daf/backup/daf.YYYYMMDD.HHMMSS.sql
//                                /opt/lampp/htdocs/daf/backup/project.YYYYMMDD.HHMMSS.sql
//
//---------------------------------------------------------------------------------------------------------------------

function backupdaf($gp) {

   $tablename = "daf";
   $backup_dir = "/opt/lampp/htdocs/daf/backup";
   exec("mkdir -p $backup_dir", $output, $rc);
   $pathname =  "$backup_dir/" . $tablename . "." . date("Ymd.His", time()) . ".sql";
   backup_database($tablename, $pathname, $completion_message);

   show_backup($gp, $completion_message);

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   backupproject
//
// Inputs:     gp        
//                      uses  $gp['ProjectID']                                                                                            
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Backs up the main DAF database                                                                                                         
//
//   format of the backupname is  /opt/lampp/htdocs/daf/backup/daf.YYMMDD.HHMMSS.sql
//                                /opt/lampp/htdocs/daf/backup/project_phase_results.YYMMDD.HHMMSS.sql
//
//---------------------------------------------------------------------------------------------------------------------

function backupproject($gp) {

   if (! isset($gp['projectID'])) {
   
      $completion_message = "Cannot backup project as project name was not defined";
   
   } else if (! isset($gp['phase'])) {
 
     $completion_message = "Cannot backup project as project phase was not defined";   

   } else {
   
      $projectname = get_field_from_row("project", "Name", "ID=" . $gp['projectID']);
      $status_database_name = determine_status_database_name($projectname, $gp['phase']);

      $backup_dir = "/opt/lampp/htdocs/daf/backup";
      exec("mkdir -p $backup_dir", $output, $rc);
      $pathname =  "$backup_dir/" . $status_database_name . "." . date("Ymd.His", time()) . ".sql";
      backup_database($status_database_name, $pathname, $completion_message);
      
   }

   show_backup($gp, $completion_message);

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   restore_backup
//
// Inputs:     $gp[$filename]     eg, project1_phase1_results.20111204.191407.sql                                                                                            
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Restores a database                                                                                                         
//
//   format of the backupname is  /opt/lampp/htdocs/daf/backup/daf.YYMMDD.HHMMSS.sql
//                                /opt/lampp/htdocs/daf/backup/project_phase_results.YYMMDD.HHMMSS.sql
//
//---------------------------------------------------------------------------------------------------------------------

function restore_backup($gp) {

   global $db_username, $db_password;

   $filename = $gp['filename'];
   $backup_dir = "/opt/lampp/htdocs/daf/backup";
   $backup_pathname = $backup_dir . "/" . $filename;
   $num_matches = preg_match("/(.*)\.\d+\.\d+\.sql/", $filename, $matches);
   
   if ($num_matches == 1) {
   
      $destination_results_database = $matches[1];
   
      $restorecmd = "/opt/lampp/bin/mysql -u $db_username -p$db_password $destination_results_database <$backup_pathname 2>&1";
      exec($restorecmd, $output, $rc);
      # sanitise password
      $restorecmd = "/opt/lampp/bin/mysql -u $db_username -pXXXX $destination_results_database <$backup_pathname 2>&1";
      add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $restorecmd), $rc) ;
   
      if ($rc != 0) {
        $completion_message = "Error code $rc from: $restorecmd\n" . implode("\n", $output) . "\n";
      } else {
        $completion_message = "Restore of $backup_pathname to database $destination_results_database completed successfully\n";
        $rc = 1;
      }
   } else {
      $completion_message = "Restore of $backup_pathname failed - could not identify destination database from filename $filename\n";
   }

   show_backup($gp, $completion_message);

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   delete_backup
//
// Inputs:     gp                                                                                                    
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Restores a database                                                                                                         
//
//   format of the backupname is  /opt/lampp/htdocs/daf/backup/daf.YYMMDD.HHMMSS.sql
//                                /opt/lampp/htdocs/daf/backup/project.YYMMDD.HHMMSS.sql
//
//---------------------------------------------------------------------------------------------------------------------

function delete_backup($gp) {

   $filename = $gp['filename'];
   $backup_dir = "/opt/lampp/htdocs/daf/backup";
   $backup_pathname = $backup_dir . "/" . $filename;
   
   $deletecmd = "rm -f $backup_pathname 2>&1";
   exec($deletecmd, $output, $rc);
   add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $deletecmd), $rc) ;

   if ($rc != 0) {
     $completion_message = "Error code $rc when deleting backup: $deletecmd\n" . implode("\n", $output) . "\n";
   } else {
     $completion_message = "Delete of $backup_pathname completed successfully\n";
     $rc = 1;
   }

   show_backup($gp, $completion_message);

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   upload_filename
//
// Inputs:     $filename                                                                                                    
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// 
//---------------------------------------------------------------------------------------------------------------------

function upload_filename($filename) {

   $backup_dir = "/opt/lampp/htdocs/daf/backup";
   
   if (! isset($filename)) {
   
      add_error_message("DAFE0030: Error: Missing filename when performing upload");
   
   } else if ($filename == '') {
   
      add_error_message("DAFE0031: Error: Empty filename when performing upload");
   
   } else {

      $target_path = $backup_dir . "/" . $filename; 
      echo "<br />\n";  
      if ($_FILES["file"]["error"] > 0)      {
         add_error_message("DAFE0032: Error: Return Code: " . $_FILES["file"]["error"]);
      } else {
         add_info_message("Upload: " . $_FILES["file"]["name"]);
         // echo "Type: " .   $_FILES["file"]["type"] . "<br />";
         add_info_message("Size: " .  ($_FILES["file"]["size"] / 1024) . " Kb");
         // echo "Temp file: " . $_FILES["file"]["tmp_name"] . "<br />";
 
         if (file_exists("upload/" . $_FILES["file"]["name"])) {
            add_info_message($_FILES["file"]["name"] . " already exists");
         } else {
            if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_path)) {
               add_info_message("Stored in: " . "$backup_dir" . "/" . $_FILES["file"]["name"]);
            } else {
               add_error_message("DAFE0033: There was an error uploading the file to $target_path");
            }
         }
      }
   }

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   dafserver
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do DAF server admin.                                        
//
//----------------------------------------------------------------------------------------------------------------------

function dafserver($gp) {
  
   $completion_message = "";
 
   if ($gp['object'] !== "dafserver") {
      add_error_message("DAFE0001: dafserver: internal error - bad object - " . $gp['object']);
      $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "dafserver";
   }
   
   if ($gp['action'] === "checkdafserver") {
   
      checkdafserver($completion_message);
      $gp['action'] = "show";
      show_dafserver($gp, $completion_message);
   
   } else if ($gp['action'] === "restartdafserver") {
   
      if (am_I_admin()) {
         restartdafserver($completion_message);
      } else {
         add_error_message("DAFE0034: Only an admin user can restart the DAF server");
      }
      
      $gp['action'] = "show";
      show_dafserver($gp, $completion_message);
    
   //} else if ($gp['action'] === "restartdafwebserver") {
   //
   //  restartdafwebserver($completion_message);
   //  $gp['action'] = "show";
   //  show_dafserver($gp, $completion_message);
    
   //} else if ($gp['action'] === "restartdafsqlserver") {
   
   //  restartdafsqlserver($completion_message);
   //  $gp['action'] = "show";
   //  show_dafserver($gp, $completion_message);

   } else if ($gp['action'] === "viewdafserverlog") {
   
      show_path("/opt/daf/logs/" . gethostname() . "_console.log");                  /* <<<<<<<<<< what should this really be?  */
    
   } else if ($gp['action'] === "viewdafwebserverlog") {
   
      show_path("/opt/lampp/logs/error_log");                  /* <<<<<<<<<< what should this really be?  */
      
   } else if ($gp['action'] === "viewdafsqlserverlog") {
   
      show_path("/opt/lampp/var/mysql/" . gethostname() . ".err");                  /* <<<<<<<<<< what should this really be?  */
    
   } else if ($gp['action'] === "show") {

       show_dafserver($gp, NULL);
        
   } else {
    
      report_bad_parms("dafserver");

   }
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_dafserver                  
//
// Inputs:     gp                                                                                                    
//             completion_message
//
// Outputs:    none  
//
// Returns:    nothing
//
// Description:
//
// Presents a menu allowing the user to choose to do various DAF server admin tasks                                                             
//
//---------------------------------------------------------------------------------------------------------------------

function show_dafserver($gp, $completion_message = NULL) {

   if (isset($completion_message)) {
      add_info_message($completion_message);
   }

   if (checkdafserver($completion_message) == 0) {
      $dafserverstatus = "ONLINE";
   } else {
      $dafserverstatus = "OFFLINE";
   }

echo <<<_END
   <table >
   <caption class="cap1"><div>DAF Server Administration Tasks</div></caption>

   <tr>
      <td colspan=3>DAF server status: 
_END;
echo "$dafserverstatus";
echo <<<_END
      </td>
   </tr>
   
   <tr>
      <form action="index.php" method="post">
      <td><input type="submit" name=button value="Check DAF Server" /></td>
      <input type="hidden" name=action value="checkdafserver" />
      <input type="hidden" name=object value="dafserver" />
      </form>

      <form action="index.php" method="post">
      <td colspan=2><input type="submit" name=button value="Restart DAF Server" /></td>
      <input type="hidden" name=action value="restartdafserver" />
      <input type="hidden" name=object value="dafserver" />
      </form>
   </tr>
   
   <tr>
      <form action="index.php" method="post">
      <td><input type="submit" name=button value="View DAF Server logfile" /></td>
      <input type="hidden" name=action value="viewdafserverlog" />
      <input type="hidden" name=object value="dafserver" />
      </form>

      <form action="index.php" method="post">
      <td><input type="submit" name=button value="View DAF Web server logfile" /></td>
      <input type="hidden" name=action value="viewdafwebserverlog" />
      <input type="hidden" name=object value="dafserver" />
      </form>

      <form action="index.php" method="post">
      <td><input type="submit" name=button value="View DAF SQL server logfile" /></td>
      <input type="hidden" name=action value="viewdafsqlserverlog" />
      <input type="hidden" name=object value="dafserver" />
      </form>
   </tr>

   </table>
_END;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   am_i_admin                  
//
// Inputs:     none                                                                                                  
//
// Outputs:    none  
//
// Returns:    true if the current user has admin priviledges, false if not
//
// Description:
//
// Determines if the current user has admin priviledges                                                             
//
//---------------------------------------------------------------------------------------------------------------------

function am_i_admin() {

   if (isset($_SESSION['user_admin'])) {
      if ($_SESSION['user_admin'] == 1) {
         return true;
      }
   }
   return false;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   auditlog
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to view the auditlog.                                        
//
//----------------------------------------------------------------------------------------------------------------------

function auditlog($gp, $auditlog_startviewdate, $auditlog_endviewdate,
                  $auditlog_startviewtime, $auditlog_endviewtime, $auditlog_Ndays) {
  
   if ($gp['object'] !== "auditlog") {
      add_error_message("DAFE0001: auditlog: internal error - bad object - " . $gp['object']);
      $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "auditlog";
   }
   
   if ($gp['action'] === "show") {

       show_all_auditlogs1($gp, $auditlog_startviewdate, $auditlog_endviewdate,
                           $auditlog_startviewtime, $auditlog_endviewtime, $auditlog_Ndays); 
        
   } else if ($gp['action'] === "getstore") {
            
      query_auditlog_store();
      
   } else {
    
      report_bad_parms("auditlog");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_auditlog
//
// Inputs:     
//
// Outputs:    
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of audit log entries
//
//----------------------------------------------------------------------------------------------------------------------

function show_auditlog($gp, $auditlog_startviewdate, $auditlog_endviewdate,
                       $auditlog_startviewtime, $auditlog_endviewtime, $auditlog_Ndays) {

   echo '<div style="float:left">';
   echo 'Start view date: <input dojoType="dijit.form.DateTextBox" name=startviewdate value="' . $auditlog_startviewdate . '" />' . "\n";
   echo '</div>';
   
   echo '<div style="float:left">';
   echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=startviewtime value="' . $auditlog_startviewtime . '" />' . "\n";
   echo '</div>';
         
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp; End view date: <input dojoType="dijit.form.DateTextBox" name=endviewdate value="' . $auditlog_endviewdate . '" />' . "\n";
   echo '</div>'; 
   
   echo '<div style="float:left">';
   echo '&nbsp; time: <input dojoType="dijit.form.TimeTextBox" name=endviewtime value="' . $auditlog_endviewtime . '" />' . "\n";
   echo '</div>'; 
         
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "auditlog_refreshview", "Refresh View", null, "submit");
   echo '<input type="hidden" name="auditlogviewrequest" value="auditlogviewrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="auditlog" />' . "\n";
   echo '</div>';
   echo '</form>' . "\n";

   echo '<form style="float:left" action="index.php" method="post">' . "\n";
   echo '<div style="float:left">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "auditlog_viewndays", "View N Days", null, "submit");
   echo '&nbsp;&nbsp;&nbsp; N days: <input type="text" name="auditlog_Ndays" size="5" value="' . $auditlog_Ndays . '" />' . "\n";
   echo '<input type="hidden" name="auditlogviewNdaysrequest" value="auditlogviewNdaysrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="auditlog" />' . "\n";
   echo '</div>';       
   echo '</form>' . "\n";       

   echo '<form action="index.php" method="post">' . "\n";
   echo '<div">';
   echo '&nbsp;&nbsp;&nbsp;';
   cellbutton(null, "auditlog_viewall", "View All", null, "submit");
   echo '<input type="hidden" name="auditlogviewallrequest" value="auditlogviewallrequest" />' . "\n";
   echo '<input type="hidden" name="action" value="show" />' . "\n";
   echo '<input type="hidden" name="object" value="auditlog" />' . "\n";
   echo '</div>';       
   echo '</form>' . "\n";
         
   echo '<div style="clear:both" >';    
         
   $query = "SELECT ID, Date, User, Originip, Errno, Content FROM daf.auditlog " . 
            "WHERE Date >= '" . $auditlog_startviewdate . " " .  $auditlog_startviewtime . "' AND " . 
            "Date <= '" . $auditlog_endviewdate . " " .  $auditlog_endviewtime . "' ORDER BY ID desc";
   $result = mysql_query($query);
   
   if (! $result) 
      die ("Database access failed for query=$query: " . mysql_error());
   
   $fieldnames = array("ID", "Date", "User", "Originating IP", "Sql Error No", "Content");

   $num_rows   = mysql_num_rows($result);
   $num_fields = mysql_num_fields($result);

   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>Audit Log</div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr>';
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldindex[$fieldnames[$i]] = $i;
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
// Function:   show_all_auditlogs1
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all the entries nodes in the DAF auditlog table.   
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_auditlogs1($status_db) {

echo <<<_END

   <link rel="stylesheet" href="./dojo/dojox/grid/resources/Grid.css" media="screen">
   <link rel="stylesheet" href="./dojo/dojox/grid/resources/claroGrid.css" media="screen">

   <div style="width:100%; height:600px;">   
   <div id="AuditlogStatus" ></div>
   </div>
   
   <script type="text/javascript">
      dojo.require("dojo.parser");
      dojo.require("dojox.grid.DataGrid");
      dojo.require("dojo.data.ObjectStore");

      dojo.ready(function() {

         dojo.require("dojo.store.JsonRest");
_END;

         echo "         myStore = new dojo.store.JsonRest({target:\"rest/daf/auditlog/\"});";
  
echo <<<_END
         
         var grid = new dojox.grid.DataGrid({    // use the adapter to give the grid a Dojo Data compliant interface to the store    
                            store: dataStore = dojo.data.ObjectStore({objectStore: myStore}),
		                    escapeHTMLInData: false,          /* <<<<<<<< is this safe (cross site attacks) */
		                    columnReordering: true,
		                    defaultcell: {width:"8em", styles:"	border-style: solid;border-width: 1px; border-color:red; background-color: #B7DBFF;"},
                            structure: [ {name:"ID", field:"ID", width:"3em", styles:"text-align:center;border-width: 1px; border-color:red;"},        
		                                 {name:"Date", field:"Date", width:"10em", styles:"text-align:center;"},        
		                                 {name:"User", field:"User", width:"12em", styles:"text-align:center;"},        
		                                 {name:"Origin IP", field:"Originip", width:"8em"},        
		                                 {name:"Error Number", field:"Errno", width:"5em", styles:"text-align:center;"},        
		                                 {name:"Content", field:"Content", width:"24em"}
		                               ]}, 
		                    "AuditlogStatus");

		 grid.startup();
         
      });
      
   </script>
_END;

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   add_auditlog_entry
//
// Inputs:     $email
//             $content
//             $ip
//
// Outputs:    none
//
// Returns:    nothing 
//
// Description:
//
// Adds an entry to the audit log 
//
//----------------------------------------------------------------------------------------------------------------------

function add_auditlog_entry($email, $ip="", $content, $sql_errno) {

   $errmsg = "";
   $query = "INSERT INTO daf.auditlog (Date, User, Originip, Errno, Content) VALUES(NOW(), '" . $email . "', '" . $ip . "', '" . $sql_errno . "', '" . $content . "')"; 
   if (! mysql_query($query)) {
      add_error_message("DAFE0035: auditlog: insert $query failed - " . mysql_error());
   } 

}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   query_auditlog_store
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a JSON store array object for the auditlog records.
//
//---------------------------------------------------------------------------------------------------------------------

function query_auditlog_store() {
   
   $where = '';
   
   $query = "SELECT ID, Date, User, Originip, Content, Errno FROM daf.auditlog";
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
     
             echo '"' . mysql_field_name($result, $i) . '": ';
             echo '"' . str_replace('"', '\\"', $row[$i]) . '"';   /* <<<<<< is this escaping of "  correct ??  need to do same elsewhere ??? */
             
          }   
          echo '}';
      }

   }
   
   echo '] }';

   exit;  /* ???<<<<<<<< should we use exit here */
   
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   select_current_project_and_phase                  
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
// Presents a menu allowing the user to choose the current project and phase                                                             
//
//---------------------------------------------------------------------------------------------------------------------

function select_current_project_and_phase($current_project, $current_phase) {

   $current_projectID = lookupID("daf.project", $current_project);
   $current_phaseID = lookupID("daf.phase", $current_phase);

echo <<<_END

   <form action="index.php" method="post">

   <table >
   <caption class="cap1"><div>Select Current Project and Phase</div></caption>

      <tr><th>Project</th>
      <td><select name="projectID" onchange="selectPhases1(this, 'phaseselectortdID')"/>
_END;
      selectorID("project", "Name", $current_projectID);      
echo <<<_END
      </select></td></tr>
      <tr><th>Phase</th>
      <td id="phaseselectortdID"><select id="backupPhaseId" name="phase"/>
      <--- onchange on the project select will fill this in --->
_END;
      selectorID("phase", "Name", $current_phaseID);      
echo <<<_END
      </select></td></tr>

   </table>
_END;
   cellbutton(null, "select_current_project_phase", "Select Current Project and Phase", null, "submit");
echo <<<_END
   <input type="hidden" name=setcurrentprojectphase value="setcurrentprojectphase" />
   <input type="hidden" name=action value="show" />
   <input type="hidden" name=object value="project" />

   </form>

_END;
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   licence
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do with the licence key.                                        
//
//----------------------------------------------------------------------------------------------------------------------

function licence($gp) {
  
   if ($gp['object'] !== "licence") {
      add_error_message("DAFE0001: licence: internal error - bad object - " . $gp['object']);
      $gp['action'] = 'show'; $gp['ID'] = NULL; $gp['object'] = "licence";
   }

   if (isset($gp['operation'])) {  
   
      if ($gp['operation'] === "update") {
      
         $licence = $gp['licence1'] . " " . $gp['licence2'] . " " . $gp['licence3'] . " " . $gp['licence4'] . " " . $gp['licence5'] . " " . 
                    $gp['licence6'] . " " . $gp['licence7'] . " " . $gp['licence8'] . " " . $gp['licence9'] . " " . $gp['licence10'];
         
         $cmd = "/opt/daf/daf_licence -write_licence " . DAF_LICENCE_PATHNAME . " -licence \"" . $licence . "\" 2>&1";
         exec($cmd, $output, $rc);
         add_auditlog_entry($_SESSION['user_email'], (isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : ""), str_replace("'", "\\'", $cmd), $rc) ;
         
         if ($rc != 0) {
         
            add_error_message("DAFE0036: Could not write licence file " . DAF_LICENCE_PATHNAME);
            $err = implode("<br>", $output); 
            add_error_message($err);    /* need to remove or escape ' stuff like this  ???  */
            $gp['action'] = 'show';
            
         }
         
      }
   
   }
   
   if ($gp['action'] === "update") {
   
       update_licence_info();
   
   } else if ($gp['action'] === "show") {

       show_licence_info(); 
        
   } else {
    
      report_bad_parms("licence");
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   islicence_valid_and_current
//
// Inputs:     
//
// Outputs:    none
//
// Returns:    1 if licence is valid, 0 if not
//
// Description:
//
//--------------------------------------------------------------------------------------------------------------------

function is_licence_valid_and_current() {

   $cmd = "/opt/daf/daf_licence -validate_licence " . DAF_LICENCE_PATHNAME . " 2>&1";
   exec($cmd, $output, $rc);

   $licence_serial = "unknown";
   $licence_duration = "0 days";
   $licence_origin = "unknown";
   $licence_type = "unknown";
   $licence_max_steps = "0";
   
   if ($rc == 0) {
     /* Licence:VALID:00001001:60:1324042820:0:100 */
     foreach ($output as $line) {
        $num_matches = preg_match('/Licence:(\S+):(\S+):(\S+):(\S+):(\S+):(\S+)/', $line, $matches);
        if ($num_matches > 0) {
           $valid = $matches[1];
           $licence_serial = $matches[2];
           $licence_duration = $matches[3];
           $licence_origin = $matches[4];
           $licence_type = $matches[5];
           $licence_max_steps = $matches[6];
        } 
     }
   } else {
     add_error_message("DAFE0037: Could not read licence file " . DAF_LICENCE_PATHNAME);
     $err = implode("<br>", $output); 
     add_error_message($err);    /* need to remove or escape ' stuff like this  ???  */
     // return 0;
     return 1;
   } 
   
   if ($valid != 'VALID') {
      return E_NOTOK;
      if ($licence_duration != 0) {
         $cutoff = $licence_origin + licence_duration * 24 * 60 * 60;
         if ($cutoff > time()) {
            return 0;          /* licence was of limited duration and has expired */
         }
      }
   }
   
   return 1;   
     
}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_licence_info
//
// Inputs:     
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the current licence 
//
//--------------------------------------------------------------------------------------------------------------------

function show_licence_info() {

   $cmd = "/opt/daf/daf_licence -validate_licence " . DAF_LICENCE_PATHNAME . " 2>&1";
   exec($cmd, $output, $rc);

   $licence_serial = "unknown";
   $licence_duration = "0 days";
   $licence_origin = "unknown";
   $licence_type = "unknown";
   $licence_max_actions = "0";
   
   if ($rc == 0) {
     /* Licence:VALID:00001001:60:1324042820:0:100 */
     foreach ($output as $line) {
        $valid_file = 0;
        $num_matches = preg_match('/Licence:(\S+):(\S+):(\S+):(\S+):(\S+):(\S+)/', $line, $matches);
        if ($num_matches > 0) {
           $valid = $matches[1];
           $licence_serial = $matches[2];
           $licence_duration = $matches[3];
           if ($licence_duration == 0) {
              $licence_duration = "unlimited duration";
           } else {
              $licence_duration .= " days";
           }
           $licence_origin = $matches[4];
           $licence_type = $matches[5];
           $licence_max_actions = $matches[6];
           echo '<table class="tab1">';
           echo "<caption class=\"cap1\"><div>Licence Information</div></caption>\n";
           echo "<tr><th>Licence Type</th><td>$licence_type</td></tr>\n";
           echo "<tr><th>Licence Serial</th><td>$licence_serial</td></tr>\n";
           echo "<tr><th>Licence Origin</th><td>$licence_origin (" . strftime("%c", $licence_origin) . ")</td></tr>\n";
           echo "<tr><th>Licence Duration</th><td>$licence_duration</td></tr>\n";
           echo "<tr><th>Licence Max Concurrent Actions</th><td>$licence_max_actions</td></tr>\n";
           echo '</table>';
           $valid_file = 1;
        } 
     }
     if (! $valid_file) {
        add_error_message("DAFE0038: Bad licence file " . DAF_LICENCE_PATHNAME);
        $err = implode("<br>", $output); 
        add_error_message($err);    /* need to remove or escape ' stuff like this  ???  */
     }
   } else {
     add_error_message("DAFE0037: Could not read licence file " . DAF_LICENCE_PATHNAME);
     $err = implode("<br>", $output); 
     add_error_message($err);    /* need to remove or escape ' stuff like this  ???  */
   }   

     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   update_licence_info
//
// Inputs:     $gp 
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the current licence 
//
//--------------------------------------------------------------------------------------------------------------------

function update_licence_info() {

   echo '<div dojoType="dijit.form.Form" id="update_licence" jsId="update_licence" encType="multipart/form-data" action="index.php" method="post">';
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
   echo "<caption class=\"cap1\"><div>Enter a New Licence Key</div></caption>\n";
   
   // There are DAF_LICENCE_LEN chars in the Licence Key
   // There are 10 entry fields
   $LFWIDTH = DAF_LICENCE_LEN / 10;

   echo '<tr><th>Licence Key</th><td>';
   echo '<input type="text" name="licence1" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence2" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence3" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence4" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence5" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence6" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence7" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence8" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence9" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo '<input type="text" name="licence10" size="' . $LFWIDTH . '" maxlength="' . $LFWIDTH . '" ' .
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . $LFWIDTH . 'em;" regExp="[A-Za-z0-9]{' . $LFWIDTH . ',' . $LFWIDTH . '}" required="true" trim="true" promptMessage="Enter a DAF Licence Field" ' . 
        'invalidMessage="Invalid DAF Licence Field (must be ' . $LFWIDTH . ' alphanumeric characters)"/>';
   echo "</td></tr>\n";
   echo "</table>\n";

   cellbutton(null, "licence_upddate", "Update Licence Key", null, "submit");
   echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
   echo "<input type=\"hidden\" name=\"object\" value=\"licence\"/>\n";
   echo "<input type=\"hidden\" name=\"operation\" value=\"update\"/>\n";
   cellbutton(null, "licence_cancel", "Cancel", "index.php?object=licence&action=show", null);
   
   echo "</div>\n";
         
}



?>
