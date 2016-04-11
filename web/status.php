<?php

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
// This file is part of the DAF user interface PHP code.  
//
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
require_once 'what_is_running.php';

$debugflag = 1; 	             /* set this variable to 1 to turn on debugging messages */
$ignore_passwords = 0;           /* set this variable to 1 if any password is allowed for any userF */
$userdetailsok = 0;
$current_user_name = null;
$current_user_ID = null;
$current_user_email = null;

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


// check if scenarioresult view has been submitted, if it has there will be cookies to set
if (isset($_POST['scenarioresultviewrequest'])){
  $scenarioresult_startviewdate = $_POST['startviewdate'];
  $scenarioresult_endviewdate   = $_POST['endviewdate'];
  $scenarioresult_startviewtime = $_POST['startviewtime'];
  $scenarioresult_endviewtime   = $_POST['endviewtime'];
  //set cookies
  setcookie("scenarioresult_startviewdate", $scenarioresult_startviewdate, time()+3600*24*30);
  setcookie("scenarioresult_endviewdate",   $scenarioresult_endviewdate,   time()+3600*24*30);
  setcookie("scenarioresult_startviewtime", $scenarioresult_startviewtime, time()+3600*24*30);
  setcookie("scenarioresult_endviewtime",   $scenarioresult_endviewtime,   time()+3600*24*30);
} else if (isset($_POST['scenarioresultviewallrequest'])){
  $scenarioresult_startviewdate = '2010-01-01';
  $scenarioresult_endviewdate   = '2020-12-31';
  $scenarioresult_startviewtime = '00:00';
  $scenarioresult_endviewtime   = '23:59';
  //set cookies
  setcookie("scenarioresult_startviewdate", $scenarioresult_startviewdate, time()+3600*24*30);
  setcookie("scenarioresult_endviewdate",   $scenarioresult_endviewdate,   time()+3600*24*30);
  setcookie("scenarioresult_startviewtime", $scenarioresult_startviewtime, time()+3600*24*30);
  setcookie("scenarioresult_endviewtime",   $scenarioresult_endviewtime,   time()+3600*24*30);
} else if  (isset($_POST['scenarioresultviewNdaysrequest'])){

  $scenarioresult_Ndays         = $_POST['scenarioresult_Ndays'];
  $scenarioresult_startviewdate = date("Y-m-d", time() - ($scenarioresult_Ndays * 24 *3600));
  $scenarioresult_endviewdate   = date("Y-m-d", time()+3600);
  $scenarioresult_startviewdate = date("H:i:s", time() - ($scenarioresult_Ndays * 24 *3600));
  $scenarioresult_endviewdate   = date("H:i:s", time()+3600);
  //set cookies
  setcookie("actionresult_startviewdate", $scenarioresult_startviewdate, time()+3600*24*30);
  setcookie("actionresult_endviewdate",   $scenarioresult_endviewdate,   time()+3600*24*30);
  setcookie("actionresult_startviewtime", $scenarioresult_startviewtime, time()+3600*24*30);
  setcookie("actionresult_endviewtime",   $scenarioresult_endviewtime,   time()+3600*24*30);
  setcookie("scenarioresult_Ndays",       $scenarioresult_Ndays,         time()+3600*24*30);
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
      $scenarioresult_startviewtime = '00:00';
   }
}

if (!isset($scenarioresult_endviewtime)) {
   if (isset($_COOKIE['scenarioresult_endviewtime'])) {
      $scenarioresult_endviewtime = $_COOKIE['scenarioresult_endviewtime'];
   } else {
      $scenarioresult_endviewtime = '23:59';
   }
}

// check if actionresult view has been submitted, if it has there will be cookies to set
if (isset($_POST['actionresultviewrequest'])){
  $actionresult_startviewdate = $_POST['startviewdate'];
  $actionresult_endviewdate   = $_POST['endviewdate'];
  //set cookies
  setcookie("actionresult_startviewdate", $actionresult_startviewdate, time()+3600*24*30);
  setcookie("actionresult_endviewdate",   $actionresult_endviewdate,   time()+3600*24*30);
} else if  (isset($_POST['actionresultviewallrequest'])){
  $actionresult_startviewdate = '2010-01-01';
  $actionresult_endviewdate   = '2020-12-31';
  //set cookies
  setcookie("actionresult_startviewdate", $actionresult_startviewdate, time()+3600*24*30);
  setcookie("actionresult_endviewdate",   $actionresult_endviewdate,   time()+3600*24*30);
} else if  (isset($_POST['actionresultviewNdaysrequest'])){

  $actionresult_Ndays         = $_POST['actionresult_Ndays'];
  $actionresult_startviewdate = date("Y-m-d H:i:s", time() - ($actionresult_Ndays * 24 *3600));
  $actionresult_endviewdate   = date("Y-m-d H:i:s", time()+3600);

  //set cookies
  setcookie("actionresult_startviewdate", $actionresult_startviewdate, time()+3600*24*30);
  setcookie("actionresult_endviewdate",   $actionresult_endviewdate,   time()+3600*24*30);
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

if (!isset($actionresult_Ndays)) {
   if (isset($_COOKIE['$actionresult_Ndays'])) {
      $actionresult_Ndays = $_COOKIE['$actionresult_Ndays'];
   } else {
      $actionresult_Ndays = 1;
   }
}

set_field_widths();

$db_server = mysql_connect($db_hostname, $db_username, $db_password);
if (!$db_server) die ("Unable to connect to MySQL: " . mysql_error());
mysql_select_db('daf') or die ("Unable to select database: " . mysql_error());  /* <<<<< hardcoded */


if (isset($_COOKIE['projectID'])) {
   $current_projectname = $_COOKIE['projectname'];
} else {
   $project_names = get_names_of_objects("project", NULL);
   if (count($project_names) == 0) {
      $current_project = "none";
   } else {
      $current_project = $project_names[0];
   }      
}

if ($current_project == "none") {
  $current_phase = "none";
} else {
   if (isset($_COOKIE['phaseID'])) {
      $current_phase = $_COOKIE['phase'];
   } else {
      if ($current_project == "none") {
         $current_phase = "none";
      } else {
         $phase_names = get_names_of_phases_in_project($current_project);
         if (count($phase_names) == 0) {
            $current_phase = "none";
         } else {
            $current_phase = $phase_names[0];
         }  
      }
   }
}

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

   $msg1 = "";

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
            $_SESSION['authenticated'] = 1;
            $_SESSION['user_name']  = $current_user_name;
            $_SESSION['user_ID']    = $current_user_ID; 
            $_SESSION['user_email'] = $current_user_email;
            $_SESSION['user_password'] = $pw_temp;
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
            $msg1 = " bad password - Invalid username/password combination";
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
         $msg1 = "Invalid username/password combination";
      }

   } else if (isset($_SESSION['authenticated'])) {
      if (strlen($_SESSION['authenticated']) > 0) {
         $current_user_name = $_SESSION['user_name'];
         $current_user_ID = $_SESSION['user_ID']; 
         $current_user_email = $_SESSION['user_email'];
         $userdetailsok = 1;
      }
   }


      
      if ($debugflag) {
         if ($gp['action'] != 'getjson') {                            /* need a better way of handling debug flag <<<< */
            echo "<pre>userdetailsok = $userdetailsok $msg1</pre>\n";
         }
       }

   if (! $userdetailsok) {
   
      document();
      head();
      end_head();
      body();

      // echo "<form action=\"status.php\" method=\"post\">\n";
   echo '<div dojoType="dijit.form.Form" id="userlogin" jsId="userlogin" encType="multipart/form-data" action="" method="">';
   echo <<<_END
      <script type="dojo/method" event="onSubmit">
        if (! this.validate()) {
            alert('Form cannot be submitted as one or more fields is invalid');
            return false;
        }
        return true;
    </script>
_END;

      echo "<pre>$msg1</pre>\n";
   
      echo '<table class="auto-style1">';
      echo "<caption class=\"cap1\"><div>User Login</div></caption>\n";

      echo '<tr><th>User Name</th><td><select name="user_email" />' . "\n";
      selectorIDforUserEmail($current_email);
      echo "</select></td></tr>\n"; 
      echo "<tr><th>Password</th>";
      echo '<td><input type="password" name="user_password" size = "' . DAF_USER_PASSWORD_LEN . '" maxlength="' . DAF_USER_PASSWORD_LEN . '" ' . 
           'dojoType="dijit.form.ValidationTextBox" regExp="\w{6,16}" required="true" trim="true"promptMessage="Enter a password" ' . 
           'invalidMessage="Invalid Password - must be 6 to 16 non space characters)"/>';
      echo "</td></tr>\n";
      echo '</table>';
      cellbutton("div", "user_login", "Login", null, "submit");
      echo "<input type=\"hidden\" name=\"action\"  value=\"login\" />\n";
      // echo "</form>\n";
      echo "</div>\n";
   
      end_body();
      end_document();
        
   } else {    

      if ($debugflag) {
         if ($gp['action'] != 'getjson') {
         echo '<pre>';                           
         print_r($gp);
         echo '</pre>';
      }     
   }

   $status_db = determine_status_database_name($current_project, $current_phase);
   /* mysql_select_db($db_database) or die("unable to select $db_database database: " . mysql_error()); */
  
   if ($gp['action'] != 'getjson') {
      document(); 
      head();
      end_head();
      body($current_project, $current_phase, $current_user_name);
   }
   
   if ($gp['action'] === "none") {
   
      show_main_menu($current_project, $current_phase);

   } else {

      if ($gp['object'] === 'scenario') {     
         scenario($gp, $current_project, $current_phase, $current_user_ID);      
      } else if ($gp['object'] === 'testcase') {       
         testcase($gp);   
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
      } else if ($gp['object'] === 'objectattributetype') {       
         objectattributetype($gp);
      } else if ($gp['object'] === 'workrequest') {       
         workrequest($gp, $current_project, $current_phase);   
      } else if ($gp['object'] === 'workqueue') {       
         workqueue($gp, $current_project, $current_phase);    
      } else if ($gp['object'] === 'teststand') {       
         teststand($gp); 
      } else if ($gp['object'] === 'host') {       
         host($gp);
      } else if ($gp['object'] === 'scenarioresult') {
         scenarioresult($gp, $current_project, $current_phase);
      } else if ($gp['object'] === 'actionresult') { 
         actionresult($gp, $current_project, $current_phase);
      } else if ($gp['object'] === 'active_workrequest') {       
         show_active_workrequest($current_project, $current_phase);
      } else if ($gp['object'] === 'active_workqueue') {       
         show_active_workqueue($current_project, $current_phase);
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
         project($gp);
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
         echo "<script src=\"js/project.js\"></script>\n";            /* */

      } else {   
         show_table($gp['object'], $gp['object']);     /*  <<<<<< some of these are in daf and some in status */
      }
   }

   end_body();
   end_document();
   
}   

function show_main_menu($current_project, $current_phase) {

echo "<p><p><div>Welcome to the Distributed Automation Framework</div>";

/* 
echo <<<_END
Project: $current_project     Phase: $current_phase
<br><a href="status.php?action=show&object=project">Projects</a>
<br><a href="status.php?action=show&object=phase">Phases</a>
<br><a href="status.php?action=show&object=user">Users</a>
<br><a href="status.php?action=show&object=scenarioset">Scenario Sets</a>
<br><a href="status.php?action=show&object=scenariosetmembers">Scenario Set Members</a>

<br><a href="status.php?action=show&object=scenario">Scenarios</a>
<br><a href="status.php?action=show&object=step">Steps</a>
<br><a href="status.php?action=show&object=testcase">Testcases</a>
<br><a href="status.php?action=show&object=testlevel">Testlevels</a>
<br><a href="status.php?action=show&object=level">Levels</a>
<br><a href="status.php?action=show&object=levelinstance">Level Instances</a>
<br><a href="status.php?action=show&object=teststand">Teststands</a>
<br><a href="status.php?action=show&object=host">Hosts</a>
<br><a href="status.php?action=show&object=objecttype">Object Types</a>
<br>
_END;
   
$objecttypes = lookuptable("daf.objecttype", "ID", "Name");
foreach ($objecttypes as $objecttype) {
   echo "&nbsp;&nbsp;<a href=\"status.php?action=show&object=object&objecttype=" . $objecttype . "\">$objecttype</a>";
}

echo <<<_END
<br><a href="status.php?action=query&object=object">Query Objects</a>
<br><a href="status.php?action=show&object=objectattributetype">Object Attribute Types</a>

<br><a href="status.php?action=show&object=collectortype">Collector Types</a>
<br><a href="status.php?action=show&object=collectorvalue">Collector Values</a>
<br><a href="status.php?action=show&object=collectorset">Collector Sets</a>
<br><a href="status.php?action=show&object=collectorsetmember">Collector Set Members</a>
<br><a href="status.php?action=show&object=environment">Environments</a>
<br><a href="status.php?action=show&object=hostselectortype">Host Selector Types</a>
<br><a href="status.php?action=show&object=outcomeaction">Outcome actions</a>
<br><a href="status.php?action=show&object=maillist">Mail Lists</a>
<br><a href="status.php?action=show&object=backup">Backup and Restore</a>
<br>
<br><a href="status.php?action=show&object=scenarioresult">Scenario Results</a>
<br><a href="status.php?action=query&object=scenarioresult">Query Scenario Results</a>

<br><a href="status.php?action=show&object=actionresult">Testcase/Action Results</a>
<br><a href="status.php?action=show&object=active_workrequest">What scenarios are running?</a>
<br><a href="status.php?action=show&object=active_workqueue">What workqueue items are running?</a>
<p>
<br><a href="status.php?action=show&object=workrequest">WorkRequests</a>
<br><a href="status.php?action=show&object=workqueue">WorkQueue</a>
<p>
<br><a href="status.php?action=show&object=archive_workrequest">Archived WorkRequest</a>
<br><a href="status.php?action=show&object=archive_workqueue">Archived WorkQueue</a>


<p>

_END;

*/

}

?>