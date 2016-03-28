<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr">

	<head>
	    <style type="text/css">
            body, html { font-family:helvetica,arial,sans-serif; font-size:90%; }
        </style>

		<title>DAF Test Automation</title>
	    <link rel="stylesheet" type="text/css" href="../dojo/dijit/themes/claro/claro.css" />

		<!-- load dojo and provide config via data attribute -->
		<script src="../dojo/dojo/dojo.js" type="text/javascript" djConfig="parseOnLoad: true"></script>
		<script>
			 dojo.require("dijit.MenuBar");
             dojo.require("dijit.PopupMenuBarItem");
             dojo.require("dijit.Menu");
             dojo.require("dijit.MenuItem");
             dojo.require("dijit.PopupMenuItem");
		</script>
	</head>
    <body class="claro">
        <div dojoType="dijit.MenuBar" id="navMenu">
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Projects
                </span>
                <div dojoType="dijit.Menu" id="projectMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=project&action=show'">
                        Projects
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=phase&action=show'">
                        Phases
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Scenarios
                </span>
                <div dojoType="dijit.Menu" id="scenarioMenu">
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=scenario&action=show'">
                        Scenarios
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=testcase&action=show'">
                        Testcases
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=level&action=show'">
                        Testlevels
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=hostselectortype&action=query'">
                        Host Selector Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=outcomeaction&action=query'">
                        Outcome Actions
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Teststands
                </span>
                <div dojoType="dijit.Menu" id="teststandMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=testcase&action=show'">
                        Teststands
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=host&action=show'">
                        Hosts
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=objecttype&action=show'">
                        Object Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=objectattributetype&action=show'">
                        Object Attribute Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=object&action=query'">
                        Query Objects
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Collectors
                </span>
                <div dojoType="dijit.Menu" id="collectorMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=collectortype&action=show'">
                        Collector Types
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=collectorvalue&action=show'">
                        Collector Values
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=collectorset&action=show'">
                        Collector Sets
                    </div>
                    <div dojoType="dijit.MenuItem" onClick="window.location = 'status.php?object=collectorsetmember&action=show'">
                        Collector Set Members
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Environments
                </span>
                <div dojoType="dijit.Menu" id="environmentMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=environment&action=show'">
                        Environments
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Levels
                </span>
                <div dojoType="dijit.Menu" id="levelMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=level&action=show'">
                        Levels
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=levelinstance&action=show'">
                        Level Instances
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Users
                </span>
                <div dojoType="dijit.Menu" id="userMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=user&action=show'">
                        Users
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=maillist&action=show'">
                        Mail Lists
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    What is running?
                </span>
                <div dojoType="dijit.Menu" id="runningMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=active_workrequest&action=show'">
                        Active Scenarios
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=active_workqueue&action=query'">
                        Workqueue items
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Results
                </span>
                <div dojoType="dijit.Menu" id="resultMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=scenarioresult&action=show'">
                        Scenario Results
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=scenarioresult&action=query'">
                        Query Scenario Results
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=actionresult&action=show'">
                        Testcase Action Results
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=archive_workrequest&action=show'">
                        Archived Work Requests
                    </div>
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=archive_workqueue&action=show'">
                        Archived Work Queue
                    </div>
                </div>
            </div>
            <div dojoType="dijit.PopupMenuBarItem">
                <span>
                    Admin
                </span>
                <div dojoType="dijit.Menu" id="adminMenu">
                    <div dojoType="dijit.MenuItem" onclick="window.location = 'status.php?object=backup&action=show'">
                        Backup and Restore
                    </div>
                </div>
            </div>
        </div>
    </body>
</html>















