
2.  Audit log - done but what about large audit logs - what about backing up the audit log ?
19. Doc updates (for licensing, admin password, audit log, recovery procedures, error messages)
24. htmlentities is needed to escape ' etc in outputs
24. should the parameters field be a multline textarea?
24. view 1 day origin does not work
24. Should we lock tables as per:
LOCK TABLES `NASbox` WRITE;
/*!40000 ALTER TABLE `NASbox` DISABLE KEYS */;
INSERT INTO `NASbox` VALUES (1,6,'QNAPTS410A','QNAP TS410 NAS File server');
/*!40000 ALTER TABLE `NASbox` ENABLE KEYS */;
UNLOCK TABLES;
25.  root@localhost has no password on mysql install
27. Push method to update the remote agents
30. What about a null software level?
37. How do we record the details of the teststand (etc ?)
38. What happens if a remote agent crashes and a testcase never completes?
40. Setting the current project - need to have the current proj/phase already selected in the drop downs...
41. projects1_phase1_results...... this name will be too long if both the project and phase name are maximum (32? 64?) length.....
42. need a title on the Show scenario results and show actionresults pages
43. can you skip the initial set up phase by selecting a menu from the menu pull down?
44.  need to check return codes in install phase:
46. are all the permissions, setuids set correctly ??
47. audit log needs scrolling system
48. date/selection on the scenarioresults/actionsresults and auditlog views




24. Add permissions table to docs:

  Object                   View   Create   Modify    Delete  Launch  Cancel
  
  Backup/Restore                  Admin    Admin     Admin
  User (Admin)                    Admin    Admin     Admin
  User (Non Admin)                any      any       Admin
  Project                         any      any       any
  Phase                           any      any       any
  Scenario                        any      any       any      any    any
  Testcase                        any      any       any
  Testlevel                       any      any       any
  Host Selector Type              any      any       any
  Host Selector Value             any      any       any
  Outcome Action                  any      any       any
  Teststand                       any      any       any
  Host                            any      any       any
  Object Type                     any      any       any
  Object Attribute Type           any      any       any
  Object                          any      any       any
  Environment                     any      any       any
  Level                           any      any       any
  Level Instance                  any      any       any
  Mail List                       any      any       any
  
  Active Scenarios          any
  Workqueue                 any
  Scenario Results          any            Admin     Admin
  Testcase Action Results   any            Admin     Admin
  Archived Scenario Results any            
  Archived Workqueue        any                     
  Auditlog                  any
  
                               Check Server  Restart Server  View DAF server logfile   View DAF web server logfile  View SQL server logfile
  DAF Server Administration    any           Admin           any                       any                          any



Done:

7.  Host selector drop down values are not right throughout - done ?
12. Can't select different projects/phases !
14. Create daf_admin user when daf is installed
16. add an entry to the audit log when DAF is upgraded or installed...
8.  when the number of enums in the objecttypeattribute is changd, we don't change it in the actual object tables  - done ?
15. Create user gave an error message and did not appear in the audit log
21. Currently it is possible to delete the master user.  Also make it impossible to set Admin=0 on Master Test user
22. Need to check that DAF has been uninstalled before installing a new installation
18. Setting the admin password
6.  Field width in the validate text boxes is not right
24. ProxyCollectorSetId in Levels view
4.  Are all the Name, Author, OutcomeactionID, Createdon, Lastchanged fields in Scenario implemented correctly?
24. need a check to see if a scenario of this name already exists - need this check for other object types too ! 
5.  If you copy a scenario and then copy if again, you get a name already exists error
24. Caption titles are too small
24. No title in this table:  http://suse11a/daf/index.php?action=modifyaction&object=scenario&ID=24&actionID=33
24. Width of fields in table above exceeds screen with
24. Parameters field should be optional in http://suse11a/daf/index.php?action=modifyaction&object=scenario&ID=24&actionID=33 
17. Licensing
24. still allowed to modify a project (and other objects) to make its name the same as an existing project 
23. Make port numbers user selectable on installation
10. is there is an issue here, as if the email of the user is modified, then the SESSION values for email are now out of date
24. Afterinstallation the daf_admin priviledges are wrong:
    msyql query: CREATE DATABASE `Project1_Phase1_results` failed - Access denied for user 'daf_admin'@'localhost' to database 'Project1_Phase1_results'
    SQL failed for CREATE DATABASE `Project1_Phase1_results` when creating new database Project1_Phase1_results:
1.  Use two password input fields - done, but if pw don't match, all fields get reset

9.  In the create an attribute type there needs to be a dynamic menu choice - done - but needs validation
38. Need a cancel button on things like 'Create Action' panel
34. When you do a 'Copy Action'  the list of step numbers should be restricted to those steps that actually exist.
25.  The daf_agent install process is broken.   If we compile for 64 bit then it does not run on 32 bit machines.  If we compile with -M32
for 32 bit, then the SQL libraries don't link.
26.  Also need to set setuid bit on the install images 
28.  Who should be allowed to modify action results and scenario results ?  remove or limit the (modify) button on the actionresult and scenarioresult views?
29  The actionresult and scenarioresult modify panels need better validation and input control
24. initial non dojo menu is seen - partial fix ??
31. Should you be able to delete an actionresult record in a sceanrioresult ? if you do won't the pass/fail arithmetic be wrong?
13. Handling All display of scenarioresults when there is a large number of results
41. Need an 'Are you sure' on delete items
33. In the Create a New Action menu, the fields need to be presented dynamically Host Selector Value etc
39. We don't seem to remember the new password after a logout/login
DAFE0036: Could not write licence file /opt/daf/daf_licence
sh: /opt/daf/daf: not found
DAFE0037: Could not read licence file /opt/daf/daf_licence
sh: /opt/daf/daf: not found
------- installing daf executable ----------
./daf: error while loading shared libraries: libmysqlclient.so.16: cannot open shared object file: No such file or directory
45. do we need a subroutine for shell_exec of daf commands so we can set the library path more tidyly ?



Enhancements

1.  Progammatic interface for submitting scenarios and getting status
2.  Assertion and test plan interface
3.  Graphing
4.  Scenariosets
3.  User roles - admin, stand manager, tester
20. Error log and error email notification
24. diagnostic package capture in DAF Server admin
24. diagnostic package to collect remote agent log files
24. After installation you cannot access XAMP from a remote host
    edit /opt/lampp/etc/extra# vi httpd-xampp.conf
36. In the modify step panel you cannot change the step number 
35. Should you be able to modify an action and change its step number




Bugs

If the log files are larege they take a long time (much > 2 secs) to copy across
 
suse11a:/tmp/log876/scenario_46_nfst_disk_pull # ls -ltr
total 15352
-rw-r--r-- 1 root root     1243 Oct 25 18:10 agent_log_toshiba_175.log
-rw-r--r-- 1 root root     8619 Oct 25 18:10 toshiba_175_nfst.stdout
-rw-r--r-- 1 root root        0 Oct 25 18:10 toshiba_175_nfst.stderr
-rw-r--r-- 1 root root    10648 Oct 25 18:11 console.out
-rw-r--r-- 1 root root        0 Oct 25 18:11 suse11a_174_nfst.stderr
-rw-r--r-- 1 root root        0 Oct 25 18:11 cgisock.11969
-rw-r--r-- 1 root root     1247 Oct 25 18:11 agent_log_suse11a_174.log
-rw-r--r-- 1 root root        0 Oct 25 18:11 cgisock.6411
-rw-r--r-- 1 root root     8409 Oct 25 18:11 suse11a_174_nfst.stdout
-rw-r--r-- 1 root root 15675392 Oct 25 18:13 error_log  <<<<<<<<<<<<<<<<<<<<<<<<<<



For ubuntu I had to

sudo apt-get install ia32-libs

sudo apt-get install build-essential

sudo apt-get install gdb

sudo apt-get install portmap

main: setuid returned -1 errno 1 (Operation not permitted)
      check that the SETUID bit is set on the daf_agent executable
sinclair@toshiba:/tmp/daf$ vi daf*inst*

chmod u+s /opt/daf/daf

:w!

"DAFE0001: actionresult: internal error - bad object - " . $gp['object']
"DAFE0002: Cannot set user email to " . $gp['email'] . " because a user with that email already exists"
"DAFE0003: $routinename: did not understand parameters"
"DAFE0004: mysql query: $query failed - " . mysql_error()
"DAFE0005: do_mysql query_id: $query failed - " . mysql_error()
"DAFE0006: Cannot create project " . $gp['name'] . " because a project of that name already exists"
"DAFE0007: Cannot modify project name to " . $gp['name'] . " because a project of that name already exists"
"DAFE0008: Cannot delete project " . $gp['name'] . " because it still contains one or more phases"
"DAFE0009: Cannot create phase " . $gp['name'] . " in project " . $projectname . " because a phase of that name already exists in this project")
"DAFE0010: Cannot rename phase to " . $gp['name'] . "in project " . $lookup[$gp['projectID']] . " because a phase of that name already exists in this project"
"DAFE0011: SQL failed for $query when creating new database $status_db: " . mysql_error()
"DAFE0012: Unable to select $status_db database: " . mysql_error()
"DAFE0012: unable to select daf database: " . mysql_error()
"DAFE0013: Cannot create user " . $gp['firstname'] . " " . $gp['lastname'] . " with email " . $gp['email'] . " because a user with that email already exists")
"DAFE0014: Password and Confirm Password fields did not match - please try again"
"DAFE0015: Only an admin user can create another user with admin priviledges"
"DAFE0016: Cannot modify user as current user does not have Administrator priviledges"
"DAFE0017: Cannot rename user " . $gp['firstname'] . " " . $gp['lastname'] . " with email " . $gp['email'] . " because a user with that email already exists")
"DAFE0018: Cannot set Admin = 0 on user " . $gp['firstname'] . " " . $gp['lastname'] . " with email " . $gp['email'] . " because this user is the Master Test User"
"DAFE0019: Only an admin user can create modify the properties of another user"
"DAFE0020: Cannot delete user as current user does not have Administrator priviledges"
"DAFE0021: Cannot delete user because this user is the Master Test User"
"DAFE0022: Cannot create project " . $gp['name'] . " because a project of that name already exists"
"DAFE0023: Cannot modify project name to " . $gp['name'] . " because a project of that name already exists"
"DAFE0024: Cannot create maillist " . $gp['name'] . " because a maillist of that name already exists"
"DAFE0025: Cannot modify Mail List name to " . $gp['name'] . " because a Mail List of that name already exists"
"DAFE0026: Cannot create scenarioset " . $gp['name'] . " because a scenarioset of that name already exists"
"DAFE0027: There is already a scenario set member for scenario " .  $lookupscenario[$gp['scenarioID']]  . " in scenarioset "  . $lookupscenarioset[$gp['scenariosetID']] . " - cannot create a second one");
"DAFE0028: backup: cannot perform operation as current user does not have Administrator priviledges"
"DAFE0029: Could not perform file upload - filename was missing"
"DAFE0030: Error: Missing filename when performing upload"
"DAFE0031: Error: Empty filename when performing upload"
"DAFE0032: Error: Return Code: " . $_FILES["file"]["error"]....
"DAFE0033: There was an error uploading the file to $target_path"
"DAFE0034: Only an admin user can restart the DAF server"
"DAFE0035: auditlog: insert $query failed - " . mysql_error()
"DAFE0036: Could not write licence file " . DAF_LICENCE_PATHNAME
"DAFE0037: Could not read licence file " . DAF_LICENCE_PATHNAME
"DAFE0038: Bad licence file " . DAF_LICENCE_PATHNAME
"DAFE0039: Cannot create scenario " . $gp['name'] . " because a scenario of that name already exists"
"DAFE0040: Cannot modify scenario " . $gp['name'] . " because a scenario of that name already exists"
"DAFE0041: insertstep specified without specifying where"
"DAFE0042: copystep specified without specifying where"
"DAFE0043: Cannot add action to step " . $gp['stepnumber'] . " since that step number does not exist in this scenario"
"DAFE0044: Cannot create scenario " . $new_scenario_name . " because a scenario of that name already exists"
"DAFE0045: Cannot start a new scenario as there is no current licence for this DAF server"
"DAFE0046: There are offline hosts (" . implode(",", $offline_hosts) . ") in test stand " . $gp['teststand'] . " - cannot run scenario"
"DAFE0047: Cannot create environment " . $gp['name'] . " because an environment of that name already exists"
"DAFE0048: Cannot modify environment " . $gp['name'] . " because an environment of that name already exists"
"DAFE0049: Cannot delete that environment because it is still used in one or more actions in scenarios"
"DAFE0050: Cannot delete that environment because it still has one or more environment values set"
"DAFE0051: Cannot create environment value " . $gp['name'] . " because an create value of that name already exists"
"DAFE0053: Cannot modify environment value " . $gp['name'] . " because an environment value of that name already exists"
"DAFE0054: Cannot delete that Host Selector Value because it is still used in one or more host definitions"
"DAFE0055: Cannot show results for project $current_project phase $current_phase"
"DAFE0056: Cannot delete that because it still contains one or more action result records"
"DAFE0057: There is no testlevel for this Scenario result"
"DAFE0058: There is no teststand for this Scenario result"
"DAFE0059: Could not fetch data from $path at offset $offset for length $maxlen"
"DAFE0060: File $path does not exist"
"DAFE0061: Cannot create testcase " . $gp['name'] . " because a testcase of that name already exists"
"DAFE0062: Cannot modify testcase name to " . $gp['name'] . " because a testcase of that name already exists"
"DAFE0063: Cannot delete that because it is still used in one or more scenarios"
"DAFE0064: Cannot create testlevel " . $gp['name'] . " because a testlevel of that name already exists"
"DAFE0065: Cannot modify testcase name to " . $gp['name'] . " because a testcase of that name already exists"
"DAFE0066: Cannot create that Test Level Member because an identicial member already exists in the Test Level"
"DAFE0067: Cannot delete that Testlevel because it still contains one or more Testlevelmembers"
"DAFE0068: Cannot create level " . $gp['name'] . " because a level of that name already exists"
"DAFE0069: Cannot modify Level name to " . $gp['name'] . " because a Level of that name already exists"
"DAFE0070: Cannot delete that Level because it still contains one or more Level Instances"
"DAFE0071: Cannot create levelinstance " . $gp['name'] . " because a levelinstance of that name already exists"
"DAFE0072: Cannot delete that Level because it still contains one or more Levelinstances"
"DAFE0073: Cannot create object type " . $gp['name'] . " because an object type of that name already exists"
"DAFE0074: Cannot modify Object Type to " . $gp['name'] . " because an Object Type with that name already exists"
"DAFE0075: Cannot delete that object type because one or more objects of that type still exist"
"DAFE0076: Cannot create object " . $gp['name'] . " of type " . $gp['objecttype'] . " because an object of that name and type already exists"
"DAFE0077: Cannot modify Object name to " . $gp['name'] . " because an Object with that name already exists"
"DAFE0078: Cannot create object attribute type " . $gp['name'] . " because an object attribute type of that name already exists"
"DAFE0079: unknown field type = " . $gp['type']
"DAFE0080: Cannot modify Object name to " . $gp['name'] . " because an Object with that name already exists"
"DAFE0081: Cannot create teststand " . $gp['name'] . " because a teststand of that name already exists"
"DAFE0082: Cannot modify Test Stand name to " . $gp['name'] . " because an Test Stand with that name already exists"
"DAFE0083: Cannot delete that teststand because it is still contains one or more hosts"
"DAFE0084: Cannot create host " . $gp['name'] . " because a host of that name already exists"
"DAFE0085: There is already a primary host (" . implode(",", $primary) . ") in test stand "  . $lookup[$gp['teststandid']] . " - cannot create a second one"
"DAFE0086: Cannot modify Host name to " . $gp['name'] . " because a Host with that name already exists"
"DAFE0087: There is already a primary host (" . implode(",", $primary) . ") in test stand "  . $lookup[$gp['teststandid']] . " - cannot create a second one"
"DAFE0088: Cannot create collector type " . $gp['name'] . " because a collector type of that name already exists"
"DAFE0089: Cannot modify Collector Type name to " . $gp['name'] . " because a Collector Type with that name already exists"
"DAFE0090: Cannot create collector set " . $gp['name'] . " because a collector set of that name already exists"
"DAFE0091: Cannot modify Collector Set name to " . $gp['name'] . " because a Collector Set with that name already exists"
"DAFE0092: There is already a collector set member for collector type " .  $lookupcollectortype[$gp['collectorsetID']]  . " in collectorset "  . $lookupcollectorset[$gp['collectorsetID']] . " - cannot create a second one"
"DAFE0093: movestep specified without specifying where"
"DAFE0094: Only an admin user can modify a Scenario Result"
"DAFE0095: Only an admin user can delete a Scenario Result"
"DAFE0096: Only an admin user can modify an Action Result"
"DAFE0097: Only an admin user can delete an Action Result"
"DAFE0098: Invalid username/password combination"









































