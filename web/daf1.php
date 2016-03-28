<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

	<head>
		<title>Demo: Layout with Dijit</title>
		<link rel="stylesheet" href="demo.css" media="screen"></link>
		<link rel="stylesheet" href="style.css" media="screen"></link>
		<link rel="stylesheet" href="claro.css" media="screen"></link>
		<!-- load dojo and provide config via data attribute -->
		<script src="../dojo/dojo/dojo.js" type="text/javascript" data-dojo-config="isDebug: true,parseOnLoad: true"></script>
		<script>
			dojo.require("dijit.layout.BorderContainer");
			dojo.require("dijit.layout.TabContainer");
			dojo.require("dijit.layout.ContentPane");
		</script>
	</head>
	<body class="claro">
		<div id="appLayout" class="demoLayout" data-dojo-type="dijit.layout.BorderContainer" data-dojo-props="design: 'headline'">
			<div class="centerPanel" data-dojo-type="dijit.layout.TabContainer" data-dojo-props="region: 'center', tabPosition: 'top'">
				<div data-dojo-type="dijit.layout.ContentPane" data-dojo-props="title: 'Group 1'">
					<h4>Group 1 Content</h4>
					<p>Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. 
					Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor 
					in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, 
					sunt in culpa qui officia deserunt mollit anim id est laborum.</p>
				</div>
				<div data-dojo-type="dijit.layout.ContentPane" data-dojo-props="title: 'Group Two'">
					<h4>Group 2 Content</h4>
				</div>
				<div data-dojo-type="dijit.layout.ContentPane" data-dojo-props="title: 'Long Tab Label for this One'">
					<h4>Group 3 Content</h4>
				</div>
			</div>
			<div class="edgePanel" data-dojo-type="dijit.layout.ContentPane" data-dojo-props="region: 'top'">Header content (top)</div>
			<div id="leftCol" class="edgePanel" data-dojo-type="dijit.layout.ContentPane" data-dojo-props="region: 'left', splitter: true">Sidebar content (left)</div>
		</div>
	</body>
</html>


