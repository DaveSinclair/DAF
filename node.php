<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  node.php
//
//  This file is part of the DAF user interface PHP code.  This file deals with actions relating to node objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                  Action to be carried out
//
//  object=node  action=show                                          display all nodes
//  object=node  action=show  ID=                                     display an individual node
// 
//  object=node  action=show    operation=create                      insert a new node in the DAF node table
//                              name=... comments=....                with characteristics as specified by
//                              clusterID=.... type=....              the POST'd variables
//  object=node  action=show    operation=modify  ID=...              modify an existing node record in the 
//                              name=... comments=....                DAF node table
//                              clusterID=.... type=....
//  object=node  action=show    operation=delete  ID=...              delete an existing record in the DAF node
//                                                                    table
//
//  object=node  action=create                                        present a form that allows the user to specify
//                                                                    the characteristics of a new node that is to be
//                                                                    added to the DAF node table
//  object=node  action=modify  ID=...                                present a form that allows the user to modify
//                                                                    the characteristics of an existing node in the
//                                                                    DAF node table
//
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   node
//
// Inputs:     $gp            array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a node object.   The allowed actions are to create, modify or delete the 
// node record in the node table.
//
//----------------------------------------------------------------------------------------------------------------------

function  node($gp) {
  
  global $db_server;

  if ($gp['object'] !== "node") {
     echo "node: internalerror - bad object - " . $gp['object'];
     return;  
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         $query = "INSERT INTO daf.node (Name, ClusterID, Type, Model, Serial, Comments) " .
                  "VALUES('" . $gp['name'] . "','" . $gp['clusterID'] . "','" .
                  $gp['type'] . "','" . $gp['model'] . "','" . $gp['serial'] . "','" . $gp['comments'] . "')";              
         // var_dump($query);           
         if (! mysql_query($query, $db_server)) {
            echo "<br />insert $query failed - " . mysql_error() . "<br />";
            return;
         }    
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         $query = "UPDATE daf.node SET Name='" . $gp['name'] . "', ClusterID='" . $gp['clusterID'] . 
                  "', Type='" . $gp['type'] . "', Model='" . $gp['model'] . "', Serial='" . $gp['serial'] . 
                  "', Comments='" . $gp['comments'] . "' WHERE ID='" . $gp['ID'] . "'";              
         var_dump($query);           
         if (! mysql_query($query, $db_server)) {
            echo "<br />insert $query failed - " . mysql_error() . "<br />";
            return;
         }    
         $gp['action'] = 'show';      

      } else if ($gp['operation'] === "delete") {

         $query = "DELETE FROM daf.node where ID='" . $gp['ID'] . "'";    
         // var_dump($query);           
         if (! mysql_query($query, $db_server)) {
            echo "<br />delete $query failed - " . mysql_error() . "<br />";
            return;
         }  
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_node();
    
   } else if ($gp['action'] === "modify") {

       modify_node($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_nodes();
           
         echo '<div style="float:left;" display="inline">';
         echo '<form action="status.php" method="post"><pre>' . "\n";
         echo '<input type="submit" name=button value="Create Node" />' . "\n";
         echo '<input type="hidden" name="action" value="create" />' . "\n";
         echo '<input type="hidden" name="object" value="node" />' . "\n";
         echo '</pre></form>' . "\n"; 
         echo '</div>'; 
         
         echo '<div style="float:left;" display="inline">';
         clusters_button();
         echo '</div>'; 
   
         echo '<div style="float:left;" display="inline">';
         teststands_button();
         echo '</div>'; 
         
         echo '<div style="float:left;" display="inline">';
         home_button();
         echo '</div>';    
 
      } else {
 
         show_detailed_node($gp['ID']);
      }
   
   } else {
    
      echo "did not understand params";
      var_dump($gp);
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_node
//
// Inputs:     none   
//
// Outputs:    Post's a list of variables that describe the characteristics of the node that is to be created 
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the characteristics of a new node that is to be added to the DAF node
// table.   The routine posts
//
// status.php?object=node?action=show&operation=create&name=...&comments=&clusterID=....&type=....  
//
//----------------------------------------------------------------------------------------------------------------------

function create_node() {

echo "<form action=\"status.php\" method=\"post\">\n";
echo "<pre>\n";
echo "       Name <input type=\"text\" name=\"name\" size = \"" . DAF_NODE_NAME_LEN . "\" maxlength=\"" . DAF_NODE_NAME_LEN . "\/>\n";
echo "       Type <select name=\"type\" />\n";
echo "            <option value=\"SVC\">SVC</option>\n";
echo "           <option value=\"v7000\">v7000</option>\n";
echo "           <option value=\"IFS\">IFS</option>\n";
echo "            </select>\n";
echo "      Model <input type=\"text\" name=\"model\" size = \"" . DAF_NODE_MODEL_LEN . "\" maxlength=\"" . DAF_NODE_MODEL_LEN . "\/>\n";
echo "     Serial <input type=\"text\" name=\"serial\" size = \"" . DAF_NODE_SERIAL_LEN . "\" maxlength=\"" . DAF_NODE_SERIAL_LEN . "\/>\n";
echo "   Comments <input type=\"text\" name=\"comments\" size = \"" . DAF_NODE_COMMENTS_LEN . "\" maxlength=\"" . DAF_NODE_COMMENTS_LEN . "\/>\n";
echo '    Cluster <select name="clusterID" />' . "\n";
selectorID("cluster", "name");
echo '</select>' . "\n";         
echo "<input type=\"hidden\" name=\"action\"    value=\"show\" />\n";
echo "<input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
echo "<input type=\"hidden\" name=\"object\"    value=\"node\" />\n";
echo "<input type=\"submit\" name=\"button\"      value=\"Create Node\" />\n";
echo "</pre></form>\n";

   echo '<div style="float:left;" display="inline">';
   nodes_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   clusters_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   teststands_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>'; 
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   show_all_nodes
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all the nodes in the DAF node table.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=node?action=create&object=node             to create a new node
// status.php?object=node?action=modify&object=node&ID=...      to modify an existing node
// status.php?object=node?action=delete&object=node&ID=...      to delete an existing node
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_nodes() {

   // note, it possible for a node not to be in a test stand so we can't just use the following
   // $query = "SELECT node.ID, node.Name, teststand.Name, node.Comments FROM node, teststand where " . 
   //          "node.teststandID = teststand.ID";
   
   $query = "SELECT ID, Name, Type, Model, Serial, ClusterID, Comments FROM daf.node";   
   $node = mysql_query($query);
    
   if (! $node) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.cluster", "ID", "Name");
   $fieldnames = array("ID", "Name", "Type", "Model", "Serial", "Cluster", "Comments");

   $num_rows   = mysql_num_rows($node);
   $num_fields = mysql_num_fields($node);

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table>';
   echo "<caption><div><div>Nodes</div></div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr><th>show</th><th>modify</th><th>delete</th>';
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldindex[$fieldnames[$i]] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $clusterindex = $fieldindex['Cluster'];
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($node);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']];
       echo "<td><a href=status.php?action=show&object=node&name=$name&ID=$ID>show</a></td>";
       echo "<td><a href=status.php?action=modify&object=node&name=$name&ID=$ID>modify</a></td>";
       echo "<td><a href=status.php?action=show&operation=delete&object=node&ID=$ID>delete</a></td>";
       
       for ($i = 0; $i < $num_fields; $i++) {       
          if ($i == $clusterindex) {
            if (isset($lookup[$row[$i]])) {
               echo '<td>'  . $lookup[$row[$i]] . '</td>';
            } else {
               echo '<td> not allocated </td>';
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
// Function:   show_detailed_node
//
// Inputs:     ID                the ID of the row in the DAF node table containing the node whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a node
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_node($ID) {

   $query = "SELECT ID, Name, Type, Model, Serial, clusterID, Comments FROM daf.node where node.ID = '" . $ID . "'"; 
   $node = mysql_query($query);
   
   if (! $node) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.cluster", "ID", "Name");
   $fieldnames = array("ID", "Name", "Type", "Model", "Serial", "Cluster", "Comments");

   $num_rows   = mysql_num_rows($node);   // should only be one row
   $num_fields = mysql_num_fields($node);

   echo '<form action="status.php" method="post">' . "\n";

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table class="auto-style1">';
   echo "<caption><div><div>Nodes</div></div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   $clusternameindex = $fieldindex['Cluster'];
   
   $node_row = mysql_fetch_row($node);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<th>' . mysql_field_name($node, $i) . '</th>';
      if ($i == $clusternameindex) {
         if (isset($lookup[$node_row[$i]])) {
            echo '<td>'  . $lookup[$node_row[$i]] . '</td>';
         } else {
            echo '<td> not allocated </td>';
         }
      } else {        
         echo '<td>'  . $node_row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
            
   echo '<div style="float:left;" display="inline">';
   nodes_button();   
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   clusters_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   teststands_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';    
     
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   modify_node
//
// Inputs:     none   
//
// Outputs:    Post's a list of variables that describe the characteristics of the node that is to be modified 
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a node that is already in the DAF node
// table.   The routine posts
//
// status.php?object=node?action=show&operation=modify&name=...&comments=&clusterID=....&type=....  
//
//----------------------------------------------------------------------------------------------------------------------

function modify_node($ID) {

   $query = "SELECT ID, Name, Type, Model, Serial, clusterID, Comments FROM daf.node where node.ID = '" . $ID . "'"; 
   $node = mysql_query($query);
   
   if (! $node) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.cluster", "ID", "Name");
   $fieldnames = array("ID", "Name", "Type", "Model", "Serial", "Cluster", "Comments");

   if (! $node) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($node);   // should only be one row
   $num_fields = mysql_num_fields($node);

   echo '<form action="status.php" method="post">' . "\n";

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table class="auto-style1">';
   echo "<caption><div><div>Nodes</div></div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   
   $IDindex = $fieldindex['ID'];
   $clusternameindex = $fieldindex['Cluster'];
  
   $node_row = mysql_fetch_row($node);
 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      $fieldname = mysql_field_name($node, $i);
      if ($i == $IDindex) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td>' . $node_row[$i] . '</td>';
      } else if ($i == $clusternameindex) {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><select name="clusterID" />' . "\n";
         selectorID("cluster", "Name", $node_row[$i]);
         echo '</select></td>' . "\n";         
      } else {
         echo '<th>' . $fieldname . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldname) . '" value="' . $node_row[$i] . '"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   echo '<div style="float:left;" display="inline">';
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="node" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="submit" name=button value="Modify Node" />' . "\n";
   echo '</form>' . "\n";
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   nodes_button();   
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   clusters_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   teststands_button();
   echo '</div>'; 
   
   echo '<div style="float:left;" display="inline">';
   home_button();
   echo '</div>';  
   
}

?>