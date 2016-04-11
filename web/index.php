<?php

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
// This file is part of the DAF user interface PHP code 
//
//
//---------------------------------------------------------------------------------------------------------------------

require_once 'login.php';
require_once 'myphp.php';
require_once 'scenario.php';
require_once 'testcase.php';
require_once 'testlevel.php';
require_once 'teststand.php';
require_once 'scenarioresult.php';
require_once 'actionresult.php';
require_once 'project.php';
require_once 'workqueue.php';
require_once 'parameter.php';

/* ---------------------------------------------------------------------------------------------------------------------------*/
/*  main                                                                                                                      */
/* ---------------------------------------------------------------------------------------------------------------------------*/

$debugflag = 0; 	             /* set this variable to 1 to turn on debugging messages */
$ignore_passwords = 0;           /* set this variable to 1 if any password is allowed for any userF */
$userdetailsok = 0;
$current_user_name = null;
$current_user_ID   = null;
$current_user_email = null;
$current_user_is_admin = null;
$current_project = "none";
$current_phase = "none";

$scenarioresult_startviewdate = null;
$scenarioresult_endviewdate   = null;
$scenarioresult_startviewtime = null;
$scenarioresult_endviewtime   = null;
$scenarioresult_Ndays         = null;

$auditlog_startviewdate = null;
$auditlog_endviewdate   = null;
$auditlog_startviewtime = null;
$auditlog_endviewtime   = null;
$auditlog_Ndays         = null;

$actionresult_startviewdate = null;
$actionresult_endviewdate   = null;
$actionresult_startviewtime = null;
$actionresult_endviewtime   = null;
$actionresult_Ndays         = null;

$gp = getparms();

session_start();          /* must precede all output of html or white space */         /* <<<<<<<<< how does the session data on the server eventually get cleared up ??? */

if ($gp['action'] === "logout") {

   // Unset all of the session variables.
   $_SESSION = array();

   // If it's desired to kill the session, also delete the session cookie.
   // Note: This will destroy the session, and not just the session data!
   if (ini_get("session.use_cookies")) {
     $params = session_get_cookie_params();
     setcookie(session_name(), '', time() - 42000,
         $params["path"], $params["domain"],
         $params["secure"], $params["httponly"]
     );
   }

   // Finally, destroy the session and start a new one
   session_destroy();
   session_start();                /* does this generate a new session id? <<<<<<<<<<< */
}

if ($gp['object'] == 'scenarioresult') {
   deal_with_scenarioresult_cookies($scenarioresult_startviewdate, $scenarioresult_endviewdate,
                 $scenarioresult_startviewtime, $scenarioresult_endviewtime, $scenarioresult_Ndays);
} else if ($gp['object'] == 'auditlog') {
   deal_with_auditlog_cookies($auditlog_startviewdate, $auditlog_endviewdate,
                 $auditlog_startviewtime, $auditlog_endviewtime, $auditlog_Ndays);
} else if ($gp['object'] == 'actionresult') {
   deal_with_actionresult_cookies($actionresult_startviewdate, $actionresult_endviewdate,
                 $actionresult_startviewtime, $actionresult_endviewtime, $actionresult_Ndays);
} 



$db_server = mysql_connect($db_hostname, $db_username, $db_password);
if (!$db_server) 
   die ("Unable to connect to MySQL ($db_hostname, $db_username, $db_password): " . mysql_error());
mysql_select_db('daf') 
   or die ("Unable to select database ($db_hostname, $db_username, $db_password): " . mysql_error());  /* <<<<< hardcoded */

deal_with_current_project_phase_cookies($current_project, $current_phase);

set_field_widths();

/* ---------------------------------------------------------------------------------------*/
/* find out if we are running in a session that has already been authenticated            */
/* if we are, then we can uses the session variables to set                               */
/* $current_user_name     to the name of the user eg Fred Blogs                           */
/* $current_user_ID       to the ID of the user  eg 23                                    */
/* $current_user_email    to the email of the user,  eg fred@gmail.com                    */
/* if the session has not been authenticated yet, then offer a menu to allow the user to  */
/* provided userid/password etc so that we can authenticate the user into this session    */
/*                                                                                        */
/* ---------------------------------------------------------------------------------------*/

if ($gp['action'] === "login") {

   $un_temp = mysql_entities_fix_string($gp['user_email']);
   $pw_temp = encrypt_password(mysql_entities_fix_string($gp['user_password']));
 
   $query = "SELECT ID, Firstname, Lastname, Area, Email, Admin, Password FROM daf.user WHERE email = '$un_temp'";
   $result = mysql_query($query);
   if (!$result) {
      echo "problem >>>>>>>>>>>>>>>>>\n";
      /* die("Database access failed: " . mysql_error()); */
      die("cannot access user table ");
   } else if (mysql_num_rows($result)) {
      
      $row = mysql_fetch_row($result);
      
      if (($pw_temp == $row[6]) || $ignore_passwords) {
         $current_user_name  = "$row[1] $row[2]";
         $current_user_ID    = "$row[0]";
         $current_user_email = $row[4];
         $current_user_is_admin = $row[5];
         $_SESSION['authenticated'] = 1;
         $_SESSION['user_name']  = $current_user_name;
         $_SESSION['user_ID']    = $current_user_ID; 
         $_SESSION['user_email'] = $current_user_email;
         $_SESSION['user_password'] = $pw_temp;
         $_SESSION['user_admin'] = $current_user_is_admin;
         $userdetailsok = 1;
      } else {
         if (isset($_SESSION['user_name'])) {
            unset($_SESSION['user_name']);
         }
         if (isset($_SESSION['user_password'])) {
            unset($_SESSION['user_password']);
         }
         if (isset($_SESSION['authenticated'])) { 
            unset($_SESSION['authenticated']);
         }
         $msg1 = "DAFE0098: Invalid username/password combination";
      }
      $gp['action'] = 'none';

   } else {
      if (isset($_SESSION['user_name'])) {
         unset($_SESSION['user_name']);
      }
      if (isset($_SESSION['user_password'])) {
         unset($_SESSION['user_password']);
      }
      if (isset($_SESSION['authenticated'])) {
        unset($_SESSION['authenticated']);
      }
      $msg1 = "DAFE0098: Invalid username/password combination";
   }

} else if (isset($_SESSION['authenticated'])) {
   if (strlen($_SESSION['authenticated']) > 0) {
      $current_user_name = $_SESSION['user_name'];
      $current_user_ID = $_SESSION['user_ID']; 
      $current_user_email = $_SESSION['user_email'];
      $userdetailsok = 1;
   }
}

if (! $userdetailsok) {
 
   document();
   head();
   end_head();
   body();

   echo '<div dojoType="dijit.form.Form" id="userlogin" jsId="userlogin" encType="multipart/form-data" action="index.php" method="post">';
   echo <<<_END
      <script type="dojo/method" event="onSubmit">
        if (! this.validate()) {
            alert('Form cannot be submitted as it contains one or more invalid fields');
            return false;
        }
        return true;
      </script>
_END;

   if (isset($msg1)) {
      add_error_message($msg1);
   }
   
   echo '<table class="tab1">';
   echo "<caption class=\"cap1\"><div>User Login</div></caption>\n";
   echo '<tr><th>User Name</th><td><select name="user_email" />' . "\n";
   selectorIDforUserEmail(null);
   echo "</select></td></tr>\n"; 
   echo "<tr><th>Password</th>";
   echo '<td><input type="password" name="user_password" size = "' . DAF_USER_PASSWORD_LEN . '" maxlength="' . DAF_USER_PASSWORD_LEN . '" ' . 
        'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" regExp="\w{6,16}" required="true" trim="true" promptMessage="Enter your password" ' . 
        'invalidMessage="Invalid Password - must be 6 to 16 non space characters)"/>';
   echo "</td></tr>\n";
   echo '</table>';
   cellbutton("div", "user_login", "Login", null, "submit");
   echo "<input type=\"hidden\" name=\"action\"  value=\"login\" />\n";
   echo "</div>\n";
   
   end_body();
   end_document();
        
} else {    

   if ($debugflag) {
      if (($gp['action'] != 'getjson') && ($gp['action'] != 'getstore'))  {
         echo '<pre>';                           
         print_r($gp);
         print_r($_SESSION);
         print_r($_SERVER);
         echo '</pre>';
      }     
   }

   $status_db = determine_status_database_name($current_project, $current_phase);
  
   if (($gp['action'] != 'getjson') && ($gp['action'] != 'getstore')) {
      document(); 
      head();
      end_head();
      body($current_project, $current_phase, $current_user_name);
   }
   
   if ($gp['action'] === "none") {
   
      // prompt the user to change the administrator password, if it has not been changed from its default
      if (lookupfield("daf.user", "Password", "Masteruser", 1) == encrypt_password("passw0rd")) {
         show_main_menu("modifydafadminemailpw");
      // prompt the user to create an initial project phase, if there are no projects yet
      } else if (count_rows("daf.project", null) == 0) {
         show_main_menu("createinitialprojectphase");
      // normal login, go to main menu
      } else {
         show_main_menu();
      }

   } else {

      if ($gp['object'] === 'scenario') {     
         scenario($gp, $current_project, $current_phase, $current_user_ID);      
         echo "<script src=\"js/scenario.js\"></script>\n";            /*  <<<<<<<< */
      } else if ($gp['object'] === 'testcase') {       
         testcase($gp); 
      } else if ($gp['object'] === 'parameter') {
         	parameter($gp);
      } else if ($gp['object'] === 'testlevel') {       
         testlevel($gp); 
      } else if ($gp['object'] === 'level') {       
         level($gp);   
      } else if ($gp['object'] === 'levelinstance') {       
         levelinstance($gp);   
      } else if ($gp['object'] === 'objecttype') {       
         objecttype($gp); 
      } else if ($gp['object'] === 'object') {       
         object($gp);
         echo "<script src=\"js/testlevel.js\"></script>\n";            /*  <<<<<<<< */
      } else if ($gp['object'] === 'objectattributetype') {       
         objectattributetype($gp);
         echo "<script src=\"js/testlevel.js\"></script>\n";            /*  <<<<<<<< */
      } else if ($gp['object'] === 'workrequest') {       
         workrequest($gp, $current_project, $current_phase);   
      } else if ($gp['object'] === 'workqueue') {       
         workqueue($gp, $current_project, $current_phase);    
      } else if ($gp['object'] === 'teststand') {       
         teststand($gp); 
      } else if ($gp['object'] === 'host') {       
         host($gp);
      } else if ($gp['object'] === 'scenarioresult') {
         scenarioresult($gp, $current_project, $current_phase, $scenarioresult_startviewdate, $scenarioresult_endviewdate,
                        $scenarioresult_startviewtime, $scenarioresult_endviewtime, $scenarioresult_Ndays);
      } else if ($gp['object'] === 'actionresult') { 
         actionresult($gp, $current_project, $current_phase);
      } else if ($gp['object'] === 'active_workrequest') {       
        show_active_workrequest1($current_project, $current_phase);
      } else if ($gp['object'] === 'active_workqueue') {       
        show_active_workqueue1($current_project, $current_phase);
      } else if ($gp['object'] === 'collectortype') {       
        collectortype($gp);
      } else if ($gp['object'] === 'collectorvalue') {       
        collectorvalue($gp);
      } else if ($gp['object'] === 'collectorset') {       
        collectorset($gp);
      } else if ($gp['object'] === 'collectorsetmember') {       
        collectorsetmember($gp);
      } else if ($gp['object'] === 'environment') {       
        environment($gp);
      } else if ($gp['object'] === 'environmentvalue') {       
        environmentvalue($gp);
      } else if ($gp['object'] === 'outcomeaction') {       
        outcomeaction($gp);
      } else if ($gp['object'] === 'maillist') {       
        maillist($gp);
      } else if ($gp['object'] === 'directory') {       
        directory($gp);
      } else if ($gp['object'] === 'path') {       
        path($gp);
      } else if ($gp['object'] === 'project') {       
        project($gp, $current_project, $current_phase);
        echo "<script src=\"js/project.js\"></script>\n";            /*  <<<<<<<< */
      } else if ($gp['object'] === 'phase') {       
        phase($gp);
      } else if ($gp['object'] === 'user') {       
        user($gp);
      } else if ($gp['object'] === 'scenarioset') {       
        scenarioset($gp);
      } else if ($gp['object'] === 'scenariosetmember') {       
        scenariosetmember($gp);
      } else if ($gp['object'] == 'teststandrecord') {
        $tablename = determine_status_table_name($status_db, "teststandrecord");
        show_table($tablename, "Test stand record for scenario result XXXX in project $current_project, phase $current_phase");   /* <<<<<< */
      } else if ($gp['object'] == 'testlevelrecord') {
        $tablename = determine_status_table_name($status_db, "testlevelrecord");
        show_table($tablename, "Test level record for scenario result XXXX in project $current_project, phase $current_phase");   /* <<<<<< */
      } else if ($gp['object'] == 'archive_workrequest') {
        $tablename = determine_status_table_name($status_db, "archive_workrequest");
        show_table($tablename, "Archive workrequest in project $current_project, phase $current_phase");
      } else if ($gp['object'] == 'archive_workqueue') {
        $tablename = determine_status_table_name($status_db, "archive_workqueue");
        show_table($tablename, "Archive workqueue in project $current_project, phase $current_phase");
      } else if ($gp['object'] === 'backup') {       
        backup($gp);
        echo "<script src=\"js/project.js\"></script>\n";            /*  <<<<<<<< */
      } else if ($gp['object'] === 'dafserver') {       
        dafserver($gp);
      } else if ($gp['object'] === 'hostselectortype') {       
        show_table('hostselectortype', 'Host Selector Types');     
      } else if ($gp['object'] === 'hostselectorvalue') {  
        hostselectorvalue($gp);    
      } else if ($gp['object'] === 'auditlog') {  
        auditlog($gp, $auditlog_startviewdate, $auditlog_endviewdate,
                 $auditlog_startviewtime, $auditlog_endviewtime, $auditlog_Ndays);
      } else if ($gp['object'] === 'initialsetup') {  
        initialsetup($gp);
      } else if ($gp['object'] === 'licence') {  
        licence($gp);
      } else {   
        show_table($gp['object'], $gp['object']);     /*  <<<<<< some of these are in daf and some in status */
      }
   }
   
   if (($gp['action'] != 'getjson') && ($gp['action'] != 'getstore')) {
      end_body();
      end_document();
   }
   
} 

/* ---------------------------------------------------------------------------------------------------------------------------*/
/*  end of main                                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------------------*/


function initialsetup($gp) {
  
  global $db_server;
  $nextstep = "none";

  if ($gp['object'] !== "initialsetup") {
     add_error_message("DAFE0001: initialsetup: internal error - bad object - " . $gp['object']);
     $gp['action'] = 'show'; $gp['ID'] = null; $gp['object'] = "initialsetup"; 
   }
   
   mysql_select_db('daf') or                                                   /* <<<<<<<<<<<<<<< hard coded */
      die("unable to select daf database: " . mysql_error());     /* <<<<<<<<<<<<<<< hard coded */

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "modifydafadminemailpw") {      
            
          if (($gp['email']) && count_rows("daf.user", "email = '" . $gp['email'] . "'") > 0) {  
         
             add_error_message("DAFE0002: Cannot set user email to " . $gp['email'] . " because a user with that email already exists");
             modify_user($gp['ID']);
             return;  /* <<<< return ??? */
             
          } else { 
          
            /* there is an issue here, as if the email of the user is modified, then the SESSION values for email are now out of date <<<<<<<< */
          
             $query = "UPDATE daf.user SET Email='" . $gp['email'] . "', Password='" . encrypt_password($gp['password']) . "'";
             $query .= " WHERE Firstname='daf' AND Lastname='administrator'";
             do_mysql_query($query, $db_server, 1);

             $gp['action'] = 'show';
             $gp['ID'] = null;
             $nextstep = 'createnewprojectphase';
            
         }    
         
      } else if ($gp['operation'] === "createinitialprojectphase") {
      
         $query = "INSERT INTO daf.project (Name, Description) " .
                  "VALUES('" . $gp['projectname'] . "','" . $gp['projectdescription'] . "')";              
         $projectID = do_mysql_query_id($query, $db_server, 1);
            
         $query = "INSERT INTO daf.phase (Name, Description, ProjectID, Logdirectory) " .
                  "VALUES('" . $gp['phasename'] . "','" . $gp['phasedescription'] . "','" . $projectID . "','" . $gp['logdirectory'] . "')";              
         if (do_mysql_query($query, $db_server, 1)) {
            create_phase_database($gp['projectname'], $gp['phasename']);
            $nextstep = 'enterlicence';
        }
      
      }  
      
   }
   
   show_main_menu($nextstep);
   
}
  
//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_main_menu
//
// Inputs:     $nextstep
//
// Outputs:    none
//
// Returns:    nothing 
//
// Description:
//
// Displays a simple welcome message - this is the first screen the user sees after logging in 
// this is the first time the user has logged on - they must set up various things:
// 1. change daf_admin password
// 2. Define a new project and a new phase
// 3. Create a test user
//----------------------------------------------------------------------------------------------------------------------

function show_main_menu($nextstep = "none") {

   if ($nextstep == 'modifydafadminemailpw') {
   
      $email = lookupfield("daf.user", "Email", "Masteruser", 1);
   
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
         echo "<caption class=\"cap1\"><div>Set Email and Password for DAF administrator master test user</div></caption>\n";

         echo '<tr><th>Email</th>';
         echo '<td><input type="text" name="email" value="' . $email . '" size="' . DAF_USER_EMAIL_LEN . '" maxlength="' . DAF_USER_EMAIL_LEN . '" ' .   /* is this validation to flexible *?  */
           'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_EMAIL_LEN . 'em;" regExp="[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}" required="true" trim="true" promptMessage="Enter a Email address (eg fred@acme.com)" ' . 
           'invalidMessage="Invalid Email address (must be 1 to ' . DAF_USER_EMAIL_LEN . ' characters in the fred@acme.com format)"/>';
         echo "</td></tr>\n";
         echo '<tr><th>Password</th>;';
         echo '<td><input type="password" name="password" size="' . DAF_USER_PASSWORD_LEN . '" maxlength="' . DAF_USER_PASSWORD_LEN . '" ' .
           'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" regExp=".{1,' . DAF_USER_PASSWORD_LEN . '}" required="true" trim="true" promptMessage="Enter a Password" ' . 
           'invalidMessage="Invalid Password (must be 1 to ' . DAF_USER_PASSWORD_LEN . ' characters)"/></td>';
           /* <<<<< the password above is POST'd in the clear  - can we encrypt it ?? */
         echo '<tr><th>Confirm Password</th>;';
         echo '<td><input type="password" name="confirm_password" size="' . DAF_USER_PASSWORD_LEN . '" maxlength="' . DAF_USER_PASSWORD_LEN . '" ' .
           'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_USER_PASSWORD_LEN . 'em;" regExp=".{1,' . DAF_USER_PASSWORD_LEN . '}" required="true" trim="true" promptMessage="Enter a Password" ' . 
           'invalidMessage="Invalid Password (must be 1 to ' . DAF_USER_PASSWORD_LEN . ' characters)"/></td>';
           /* <<<<< the password above is POST'd in the clear  - can we encrypt it ?? */
         echo "</tr>\n";
         echo "</table>\n";
   
         cellbutton('div', "user_create", "Modify DAF administrator Email and Password", null, "submit");
         echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
         echo "<input type=\"hidden\" name=\"object\" value=\"initialsetup\"/>\n";
         echo "<input type=\"hidden\" name=\"operation\" value=\"modifydafadminemailpw\"/>\n";
         cellbutton(null, "user_cancel", "Cancel", "index.php?object=user&action=show", null);

      echo '</div>';  
  
   } else if ($nextstep == 'createnewprojectphase') {
   
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
         echo "<caption class=\"cap1\"><div>Create an inital Project and Phase</div></caption>\n";

         echo '<tr><th>Project Name</th>';
         echo '<td><input type="text" name="projectname" size = "' . DAF_PROJECT_LEN . '" maxlength="' . DAF_PROJECT_LEN . '" ' .
           'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PROJECT_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_PROJECT_LEN . '}" required="true" trim="true" promptMessage="Enter a Project Name" ' . 
           'invalidMessage="Invalid Project Name (must be 1 to ' . DAF_PROJECT_LEN . ' alphanumeric characters)"/>';
         echo "</td></tr>\n";
         echo '<tr><th>Project Description</th>';
         echo '<td><input type="text" name="projectdescription" size = "' . DAF_PROJECTDESCRIPTION_LEN . '" maxlength="' . DAF_PROJECTDESCRIPTION_LEN . '" ' .
           'dojoType="dijit.form.ValidationTextBox" style="width:90%" regExp=".{1,' . DAF_PROJECTDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Project Description" ' . 
           'invalidMessage="Invalid Project Description (must be 1 to ' . DAF_PROJECTDESCRIPTION_LEN . ' characters)"/>';
         echo "</td></tr>\n";
         
         echo "<tr><th>Phase Name</th>";
         echo '<td><input type="text" name="phasename" size="' . DAF_PHASE_LEN . '" maxlength="' . DAF_PHASE_LEN . '" ' .
           'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_PHASE_LEN . 'em;" regExp="[A-Za-z0-9_]{1,' . DAF_PHASE_LEN . '}" required="true" trim="true" promptMessage="Enter a Phase Description" ' . 
           'invalidMessage="Invalid Phase Description (must be 1 to ' . DAF_PHASE_LEN . ' alphanumeric characters)"/>';
         echo "</td></tr>\n";
         echo '<tr><th>Phase Description</th>';
         echo '<td><input type="text" name="phasedescription" size="' . DAF_PHASEDESCRIPTION_LEN . '" maxlength="' . DAF_PHASEDESCRIPTION_LEN . '" ' . 
           'dojoType="dijit.form.ValidationTextBox" style="width:90%" regExp=".{1,' . DAF_PHASEDESCRIPTION_LEN . '}" required="true" trim="true" promptMessage="Enter a Phase Description" ' . 
           'invalidMessage="Invalid Phase Description (must be 1 to ' . DAF_PHASEDESCRIPTION_LEN . ' characters)"/>';
         echo "</td></tr>\n";
         echo '<tr><th>Log Location Directory</th>';
         echo '<td><input type="text" name="logdirectory" size="' . DAF_LOGDIRECTORY_LEN . '" maxlength="' . DAF_LOGDIRECTORY_LEN . '" ' .  /* <<<< incomplete validation ??? */
           'dojoType="dijit.form.ValidationTextBox" style="width: ' . DAF_LOGDIRECTORY_LEN . 'em;" regExp=".{1,' . DAF_LOGDIRECTORY_LEN . '}" required="true" trim="true" promptMessage="Enter a pathname to a Log Directory" ' . 
           'invalidMessage="Invalid Log Directory (must be 1 to ' . DAF_LOGDIRECTORY_LEN . ' characters)"/>';
         echo "</td></tr>\n"; 

         echo "</table>\n"; 

         cellbutton(null, "project_create", "Create Initial Project and Phase", null, "submit");
         echo "<input type=\"hidden\" name=\"action\" value=\"show\"/>\n";
         echo "<input type=\"hidden\" name=\"object\" value=\"initialsetup\"/>\n";
         echo "<input type=\"hidden\" name=\"operation\" value=\"createinitialprojectphase\"/>\n";

      echo "</div>\n";
   
   } else if ($nextstep == 'enterlicence') {
      
      update_licence_info();

   } else {
   
      echo "<p><p><div>Welcome to the Distributed Automation Framework</div>";
   
   }

}


//---------------------------------------------------------------------------------------------------------------------
//
// Function:   deal_with_scenarioresult_cookies
//
// Inputs:     $gp
//
// Outputs:    
//
// Returns:    nothing 
//
// Description:
//
// Looks at the current cookies 
//
//----------------------------------------------------------------------------------------------------------------------

function deal_with_scenarioresult_cookies(&$scenarioresult_startviewdate, &$scenarioresult_endviewdate,
                                          &$scenarioresult_startviewtime, &$scenarioresult_endviewtime, &$scenarioresult_Ndays) {

      // check if scenarioresult view has been submitted, if it has there will be cookies to set
      if (isset($_POST['scenarioresultviewrequest'])) {
              
        $scenarioresult_startviewdate = $_POST['startviewdate'];
        $scenarioresult_endviewdate   = $_POST['endviewdate'];
        $scenarioresult_startviewtime = $_POST['startviewtime'];
        $scenarioresult_endviewtime   = $_POST['endviewtime'];
        //set cookies
        setcookie("scenarioresult_startviewdate", $scenarioresult_startviewdate, time()+3600*24*30);
        setcookie("scenarioresult_endviewdate",   $scenarioresult_endviewdate,   time()+3600*24*30);
        setcookie("scenarioresult_startviewtime", $scenarioresult_startviewtime, time()+3600*24*30);
        setcookie("scenarioresult_endviewtime",   $scenarioresult_endviewtime,   time()+3600*24*30);
  
      } else if (isset($_POST['scenarioresultviewallrequest'])) {

        $scenarioresult_startviewdate = '2010-01-01';
        $scenarioresult_endviewdate   = '2020-12-31';
        $scenarioresult_startviewtime = 'T00:00:00';
        $scenarioresult_endviewtime   = 'T23:59:00';
        //set cookies
        setcookie("scenarioresult_startviewdate", $scenarioresult_startviewdate, time()+3600*24*30);
        setcookie("scenarioresult_endviewdate",   $scenarioresult_endviewdate,   time()+3600*24*30);
        setcookie("scenarioresult_startviewtime", $scenarioresult_startviewtime, time()+3600*24*30);
        setcookie("scenarioresult_endviewtime",   $scenarioresult_endviewtime,   time()+3600*24*30);
  
      } else if (isset($_POST['scenarioresultviewNdaysrequest'])) {

        $scenarioresult_Ndays         = $_POST['scenarioresult_Ndays'];
        $scenarioresult_startviewdate = date("Y-m-d", time() - ($scenarioresult_Ndays * 24 *3600));
        $scenarioresult_endviewdate   = date("Y-m-d", time()+3600);
        $scenarioresult_startviewtime = "T" . date("H:i:s", time() - ($scenarioresult_Ndays * 24 *3600));
        $scenarioresult_endviewtime   = "T" . date("H:i:s", time()+3600);
        //set cookies
        setcookie("scenarioresult_startviewdate", $scenarioresult_startviewdate, time()+3600*24*30);
        setcookie("scenarioresult_endviewdate",   $scenarioresult_endviewdate,   time()+3600*24*30);
        setcookie("scenarioresult_startviewtime", $scenarioresult_startviewtime, time()+3600*24*30);
        setcookie("scenarioresult_endviewtime",   $scenarioresult_endviewtime,   time()+3600*24*30);
        setcookie("scenarioresult_Ndays",         $scenarioresult_Ndays,         time()+3600*24*30);
    
      } 

      if (!isset($scenarioresult_startviewdate)) {
         if (isset($_COOKIE['scenarioresult_startviewdate'])) {
            $scenarioresult_startviewdate = $_COOKIE['scenarioresult_startviewdate'];
         } else {
            $scenarioresult_startviewdate = '2010-01-01';
         }
      }

      if (!isset($scenarioresult_endviewdate)) {
         if (isset($_COOKIE['scenarioresult_endviewdate'])) {
            $scenarioresult_endviewdate = $_COOKIE['scenarioresult_endviewdate'];
         } else {
            $scenarioresult_endviewdate = '2020-12-31';
         }
      }

      if (!isset($scenarioresult_startviewtime)) {
         if (isset($_COOKIE['scenarioresult_startviewtime'])) {
            $scenarioresult_startviewtime = $_COOKIE['scenarioresult_startviewtime'];
         } else {
            $scenarioresult_startviewtime = 'T00:00:00';
         }
      }

      if (!isset($scenarioresult_endviewtime)) {
         if (isset($_COOKIE['scenarioresult_endviewtime'])) {
            $scenarioresult_endviewtime = $_COOKIE['scenarioresult_endviewtime'];
         } else {
            $scenarioresult_endviewtime = 'T23:59:00';
         }
      }
}

function deal_with_auditlog_cookies(&$auditlog_startviewdate, &$auditlog_endviewdate,
                                    &$auditlog_startviewtime, &$auditlog_endviewtime, &$auditlog_Ndays) {
   
      // check if auditlog view has been submitted, if it has there will be cookies to set
      if (isset($_POST['auditlogviewrequest'])) {

        $auditlog_startviewdate = $_POST['startviewdate'];
        $auditlog_endviewdate   = $_POST['endviewdate'];
        $auditlog_startviewtime = $_POST['startviewtime'];
        $auditlog_endviewtime   = $_POST['endviewtime'];
        //set cookies
        setcookie("auditlog_startviewdate", $auditlog_startviewdate, time()+3600*24*30);
        setcookie("auditlog_endviewdate",   $auditlog_endviewdate,   time()+3600*24*30);
        setcookie("auditlog_startviewtime", $auditlog_startviewtime, time()+3600*24*30);
        setcookie("auditlog_endviewtime",   $auditlog_endviewtime,   time()+3600*24*30);
  
      } else if  (isset($_POST['auditlogviewallrequest'])) {

        $auditlog_startviewdate = '2010-01-01';
        $auditlog_endviewdate   = '2020-12-31';
        $auditlog_startviewtime = 'T00:00:00';
        $auditlog_endviewtime   = 'T23:59:00';
        //set cookies
        setcookie("auditlog_startviewdate", $auditlog_startviewdate, time()+3600*24*30);
        setcookie("auditlog_endviewdate",   $auditlog_endviewdate,   time()+3600*24*30);
        setcookie("auditlog_startviewtime", $auditlog_startviewtime, time()+3600*24*30);
        setcookie("auditlog_endviewtime",   $auditlog_endviewtime,   time()+3600*24*30);
  
      } else if  (isset($_POST['auditlogviewNdaysrequest'])) {

        $auditlog_Ndays         = $_POST['auditlog_Ndays'];
        $auditlog_startviewdate = date("Y-m-d", time() - ($auditlog_Ndays * 24 *3600));
        $auditlog_endviewdate   = date("Y-m-d", time()+3600);
        $auditlog_startviewtime = "T" . date("H:i:s", time() - ($auditlog_Ndays * 24 *3600));
        $auditlog_endviewtime   = "T" . date("H:i:s", time()+3600);

        //set cookies
        setcookie("auditlog_startviewdate", $auditlog_startviewdate, time()+3600*24*30);
        setcookie("auditlog_endviewdate",   $auditlog_endviewdate,   time()+3600*24*30);
        setcookie("auditlog_startviewtime", $auditlog_startviewtime, time()+3600*24*30);
        setcookie("auditlog_endviewtime",   $auditlog_endviewtime,   time()+3600*24*30);
        setcookie("auditlog_Ndays",         $auditlog_Ndays,         time()+3600*24*30);

      }

      if (!isset($auditlog_startviewdate)) {
         if (isset($_COOKIE['auditlog_startviewdate'])) {
            $auditlog_startviewdate = $_COOKIE['auditlog_startviewdate'];
         } else {
            $auditlog_startviewdate = '2010-01-01';
         }
      }

      if (!isset($auditlog_endviewdate)) {
         if (isset($_COOKIE['auditlog_endviewdate'])) {
            $auditlog_endviewdate = $_COOKIE['auditlog_endviewdate'];
         } else {
            $auditlog_endviewdate = '2010-01-01';
         }
      }

      if (!isset($scenarioresult_Ndays)) {
         if (isset($_COOKIE['$scenarioresult_Ndays'])) {
            $scenarioresult_Ndays = $_COOKIE['$scenarioresult_Ndays'];
         } else {
            $scenarioresult_Ndays = 1;
         }
      }

      if (!isset($auditlog_startviewtime)) {
         if (isset($_COOKIE['auditlog_startviewtime'])) {
            $auditlog_startviewtime = $_COOKIE['auditlog_startviewtime'];
         } else {
            $auditlog_startviewtime = 'T00:00:00';
         }
      }

      if (!isset($auditlog_endviewtime)) {
         if (isset($_COOKIE['auditlog_endviewtime'])) {
            $auditlog_endviewtime = $_COOKIE['auditlog_endviewtime'];
         } else {
            $auditlog_endviewtime = 'T23:59:00';
         }
      }

      if (!isset($auditlog_Ndays)) {
         if (isset($_COOKIE['$auditlog_Ndays'])) {
            $auditlog_Ndays = $_COOKIE['$auditlog_Ndays'];
         } else {
            $auditlog_Ndays = 1;
         }
      }
      
}

function deal_with_actionresult_cookies(&$actionresult_startviewdate, &$actionresult_endviewdate,
                                        &$actionresult_startviewtime, &$actionresult_endviewtime, &$actionresult_Ndays) {
   
         // check if actionresult view has been submitted, if it has there will be cookies to set
      if (isset($_POST['actionresultviewrequest'])) {

        $actionresult_startviewdate = $_POST['startviewdate'];
        $actionresult_endviewdate   = $_POST['endviewdate'];
        $actionresult_startviewtime = $_POST['startviewtime'];
        $actionresult_endviewtime   = $_POST['endviewtime'];
        //set cookies
        setcookie("actionresult_startviewdate", $actionresult_startviewdate, time()+3600*24*30);
        setcookie("actionresult_endviewdate",   $actionresult_endviewdate,   time()+3600*24*30);
        setcookie("actionresult_startviewtime", $actionresult_startviewtime, time()+3600*24*30);
        setcookie("actionresult_endviewtime",   $actionresult_endviewtime,   time()+3600*24*30);
  
      } else if  (isset($_POST['actionresultviewallrequest'])) {

        $actionresult_startviewdate = '2010-01-01';
        $actionresult_endviewdate   = '2020-12-31';
        $actionresult_startviewtime = 'T00:00:00';
        $actionresult_endviewtime   = 'T23:59:00';
        //set cookies
        setcookie("actionresult_startviewdate", $actionresult_startviewdate, time()+3600*24*30);
        setcookie("actionresult_endviewdate",   $actionresult_endviewdate,   time()+3600*24*30);
        setcookie("actionresult_startviewtime", $actionresult_startviewtime, time()+3600*24*30);
        setcookie("actionresult_endviewtime",   $actionresult_endviewtime,   time()+3600*24*30);
  
      } else if  (isset($_POST['actionresultviewNdaysrequest'])) {


        $actionresult_Ndays         = $_POST['actionresult_Ndays'];
        $actionresult_startviewdate = date("Y-m-d", time() - ($actionresult_Ndays * 24 *3600));
        $actionresult_endviewdate   = date("Y-m-d", time()+3600);
        $actionresult_startviewtime = "T" . date("H:i:s", time() - ($actionresult_Ndays * 24 *3600));
        $actionresult_endviewtime   = "T" . date("H:i:s", time()+3600);

        //set cookies
        setcookie("actionresult_startviewdate", $actionresult_startviewdate, time()+3600*24*30);
        setcookie("actionresult_endviewdate",   $actionresult_endviewdate,   time()+3600*24*30);
        setcookie("actionresult_startviewtime", $actionresult_startviewtime, time()+3600*24*30);
        setcookie("actionresult_endviewtime",   $actionresult_endviewtime,   time()+3600*24*30);
        setcookie("actionresult_Ndays",         $actionresult_Ndays,         time()+3600*24*30);

      }

      if (!isset($actionresult_startviewdate)) {
         if (isset($_COOKIE['actionresult_startviewdate'])) {
            $actionresult_startviewdate = $_COOKIE['actionresult_startviewdate'];
         } else {
            $actionresult_startviewdate = '2010-01-01';
         }
      }

      if (!isset($actionresult_endviewdate)) {
         if (isset($_COOKIE['actionresult_endviewdate'])) {
            $actionresult_endviewdate = $_COOKIE['actionresult_endviewdate'];
         } else {
            $actionresult_endviewdate = '2010-01-01';
         }
      }

      if (!isset($scenarioresult_Ndays)) {
         if (isset($_COOKIE['$scenarioresult_Ndays'])) {
            $scenarioresult_Ndays = $_COOKIE['$scenarioresult_Ndays'];
         } else {
            $scenarioresult_Ndays = 1;
         }
      }

      if (!isset($actionresult_startviewtime)) {
         if (isset($_COOKIE['actionresult_startviewtime'])) {
            $actionresult_startviewtime = $_COOKIE['actionresult_startviewtime'];
         } else {
            $actionresult_startviewtime = 'T00:00:00';
         }
      }

      if (!isset($actionresult_endviewtime)) {
         if (isset($_COOKIE['actionresult_endviewtime'])) {
            $actionresult_endviewtime = $_COOKIE['actionresult_endviewtime'];
         } else {
            $actionresult_endviewtime = 'T23:59:00';
         }
      }

      if (!isset($actionresult_Ndays)) {
         if (isset($_COOKIE['$actionresult_Ndays'])) {
            $actionresult_Ndays = $_COOKIE['$actionresult_Ndays'];
         } else {
            $actionresult_Ndays = 1;
         }
      }
}


function deal_with_current_project_phase_cookies(&$current_project, &$current_phase) {
   
   // check if setcurrentprojectphase view has been submitted, if it has there will be cookies to set
   if (isset($_POST['setcurrentprojectphase'])) {

      $current_project = lookupfield('project', 'Name', 'ID', $_POST['projectID']);
      $current_phase   = lookupfield('phase',   'Name', 'ID', $_POST['phase']);

      $project_names = get_names_of_objects("project", NULL);

      //set cookies
      setcookie("current_project", $current_project, time()+3600*24*30);
      setcookie("current_phase",   $current_phase,   time()+3600*24*30);
  
   } else {

      if (isset($_COOKIE['current_project'])) {
         $current_project = $_COOKIE['current_project'];
      } else {
         $project_names = get_names_of_objects("project", NULL);
         if (count($project_names) == 0) {
            $current_project = "none";
         } else {
            $current_project = $project_names[0];
            setcookie("current_project", $current_project, time()+3600*24*30);
         }      
      }
    
      if ($current_project == "none") {
        $current_phase = "none";
      } else {
         if (isset($_COOKIE['current_phase'])) {
            $current_phase = $_COOKIE['current_phase'];
         } else {
            if ($current_project == "none") {
               $current_phase = "none";
            } else {
               $phase_names = get_names_of_phases_in_project($current_project);
               if (count($phase_names) == 0) {
                  $current_phase = "none";
               } else {
                  $current_phase = $phase_names[0];
                  setcookie("current_phase",   $current_phase,   time()+3600*24*30);
               }  
            }
        }
     }
   }
}



?>
