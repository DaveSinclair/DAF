<?php 

//---------------------------------------------------------------------------------------------------------------------
//
//  DAF (Distributed Automation Framework)
//
//  cluster.php
//
//  This file is part of the DAF user interface PHP code.  This file deals with actions relating to cluster objects.
//
//
//  The following POST'd parameters are handled
//                                
//  Post'd variables                                                     Action to be carried out
//
//  object=cluster  action=show                                          display all clusters
//  object=cluster  action=show  ID=                                     display an individual cluster
// 
//  object=cluster  action=show    operation=create                      insert a new cluster in the DAF cluster table
//                                 name=... comments=....                with characteristics as specified by
//                                 teststandID=.... type=....            the POST'd variables
//  object=cluster  action=show    operation=modify  ID=...              modify an existing cluster record in the 
//                                 name=... comments=....                DAF cluster table
//                                 teststandID=.... type=....
//  object=cluster  action=show    operation=delete  ID=...              delete an existing record in the DAF cluster
//                                                                       table
//
//  object=cluster  action=create                                        present a form that allows the user to specify
//                                                                       the characteristics of a new cluster that is to be
//                                                                       added to the DAF cluster table
//  object=cluster  action=modify  ID=...                                present a form that allows the user to modify
//                                                                       the characteristics of an existing cluster in the
//                                                                       DAF cluster table
//
//---------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------------------
//
// Function:   cluster
//
// Inputs:     $gp                  array containing posted parameters (eg $gp['object'])
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Handles requests to do something with a cluster object.   The allowed actions are to create, modify or delete the 
// cluster record in the cluster table.
//
//----------------------------------------------------------------------------------------------------------------------

function cluster($gp) {

  global $db_server;

  if ($gp['object'] !== "cluster") {
     echo "cluster: internalerror - bad object - " . $gp['object'];
     return;  
   }

   if (isset($gp['operation'])) {
   
      if ($gp['operation'] === "create") {
   
         $query = "INSERT INTO daf.cluster (Name, TeststandID, Comments) " .
                  "VALUES('" . $gp['name'] . "','" . $gp['teststandID'] . "','" . $gp['comments'] . "')";              
         // var_dump($query);           
         if (! mysql_query($query, $db_server)) {
            echo "<br />insert $query failed - " . mysql_error() . "<br />";
            return;
         }    
         $gp['action'] = 'show';

      } else if ($gp['operation'] === "modify") {

         $query = "UPDATE daf.cluster SET Name='" . $gp['name'] . "', TeststandID='" . $gp['teststandID'] . 
                  "', Comments='" . $gp['comments'] . "' WHERE ID='" . $gp['ID'] . "'";              
         var_dump($query);           
         if (! mysql_query($query, $db_server)) {
            echo "<br />insert $query failed - " . mysql_error() . "<br />";
            return;
         }    
         $gp['action'] = 'show';      

      } else if ($gp['operation'] === "delete") {
      
         if (count_rows("daf.nodes", "ClusterID = '" . $gp['ID'] . "'") > 0) {
         
            echo "<pre>\nCannot delete that cluster because it is still contains one or more nodes\n</pre>\n";
           
         } else {
         
            $query = "DELETE FROM daf.cluster where ID='" . $gp['ID'] . "'";    
            // var_dump($query);           
            if (! mysql_query($query, $db_server)) {
               echo "<br />delete $query failed - " . mysql_error() . "<br />";
               return;
            } 
            
         }
         $gp['action'] = 'show';
         $gp['ID'] = NULL;
  
      }
   
   }

   if ($gp['action'] === "create") {
   
       create_cluster();
    
   } else if ($gp['action'] === "modify") {

       modify_cluster($gp['ID']);
        
   } else  if ($gp['action'] === "show") {

      if (! isset($gp['ID'])) {
      
         show_all_clusters();
           
         echo '<div style="float:left;" display="inline">';
         echo '<form action="status.php" method="post"><pre>' . "\n";
         echo '<input type="submit" name=button value="Create Cluster" />' . "\n";
         echo '<input type="hidden" name="action" value="create" />' . "\n";
         echo '<input type="hidden" name="object" value="cluster" />' . "\n";
         echo '</pre></form>' . "\n"; 
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
 
      } else {
 
         show_detailed_cluster($gp['ID']);
      }
   
   } else {
    
      echo "did not understand params";
      var_dump($gp);
   
   }
   
}

//---------------------------------------------------------------------------------------------------------------------
//
// Function:   create_cluster
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
// status.php?object=cluster?action=show&object=cluster&operation=create&name=...&comments=&clusterID=....&type=....  
//
//----------------------------------------------------------------------------------------------------------------------

function create_cluster() {

echo "<form action=\"status.php\" method=\"post\">\n";
echo "<pre>\n";
echo "       Name <input type=\"text\" name=\"name\" size = \"" . DAF_CLUSTER_LEN . "\" maxlength=\"" . DAF_CLUSTER_LEN . "\/>\n";
echo "   Comments <input type=\"text\" name=\"comments\" size = \"" . DAF_COMMENTS_LEN . "\" maxlength=\"" . DAF_COMMENTS_LEN . "\/>\n";
echo "            <input type=\"hidden\" name=\"action\" value=\"show\" />\n";
echo "            <input type=\"hidden\" name=\"operation\" value=\"create\" />\n";
echo "            <input type=\"hidden\" name=\"object\" value=\"cluster\" />\n";
echo "            <input type=\"submit\" name=\"button\" value=\"Create cluster\" />\n";
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
// Function:   show_all_clusters
//
// Inputs:     none
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays a tabular list of all the cluster in the DAF node table.   The user may select a link in this table that
// causes the following possible URLs to be posted:
//
// status.php?object=node?action=create&object=cluster             to create a new cluster
// status.php?object=node?action=modify&object=cluster&ID=...      to modify an existing cluster
// status.php?object=node?action=delete&object=cluster&ID=...      to delete an existing cluster
//
//
//----------------------------------------------------------------------------------------------------------------------

function show_all_clusters() {

   // note, it possible for a cluster not to be in a test stand so we can't just use the following
   // $query = "SELECT cluster.ID, cluster.Name, teststand.Name, cluster.Comments FROM cluster, teststand where " . 
   //          "cluster.teststandID = teststand.ID";
   
   $query = "SELECT ID, Name, TeststandID, Comments FROM daf.cluster";   
   $cluster = mysql_query($query);
    
   if (! $cluster) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.teststand", "ID", "Name");
   $fieldnames = array("ID", "Name", "Teststand", "Comments");

   $num_rows   = mysql_num_rows($cluster);
   $num_fields = mysql_num_fields($cluster);

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table>';
   echo "<caption><div><div>Clusters</div></div></caption>";
   echo "\n";

   $fieldindex = array();
   echo '<tr><th>show</th><th>modify</th><th>delete</th>';
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<th>' . $fieldnames[$i] . '</th>';
      $fieldindex[$fieldnames[$i]] = $i;
   }
   echo '</tr>';
   echo "\n";
   
   $nameindex = $fieldindex['Teststand'];
   for ($j = 0; $j < $num_rows; $j++) {
       $row = mysql_fetch_row($cluster);
       echo '<tr>';
       $ID = $row[$fieldindex['ID']];
       $name = $row[$fieldindex['Name']];
       echo "<td><a href=status.php?action=show&object=cluster&name=$name&ID=$ID>show</a></td>";
       echo "<td><a href=status.php?action=modify&object=cluster&name=$name&ID=$ID>modify</a></td>";
       echo "<td><a href=status.php?action=show&operation=delete&object=cluster&ID=$ID>delete</a></td>";
       
       for ($i = 0; $i < $num_fields; $i++) {       
          if ($i == $nameindex) {
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
// Function:   show_detailed_cluster
//
// Inputs:     ID                the ID of the row in the DAF cluster table containing the cluster whose details
//                               are to be displayed
//
// Outputs:    none
//
// Returns:    nothing
//
// Description:
//
// Displays the detailed characteristics of a cluster
//
//----------------------------------------------------------------------------------------------------------------------

function show_detailed_cluster($ID) {

   $query = "SELECT ID, Name, teststandID, Comments FROM daf.cluster where cluster.ID = '" . $ID . "'"; 
   $cluster = mysql_query($query);
   
   if (! $cluster) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.teststand", "ID", "Name");
   $fieldnames = array("ID", "Name", "Teststand", "Comments");

   $num_rows   = mysql_num_rows($cluster);   // should only be one row
   $num_fields = mysql_num_fields($cluster);

   echo '<form action="status.php" method="post">' . "\n";

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table class="auto-style1">';
   echo "<caption><div><div>Clusters</div></div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   $nameindex = $fieldindex['Teststand'];
   
   $cluster_row = mysql_fetch_row($cluster);
   /* $ScenarioID = $scenario_row[$fieldindex['ID']]; */

   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      echo '<td>' . $fieldnames[$i] . '</td>';
      if ($i == $nameindex) {
         if (isset($lookup[$cluster_row[$i]])) {
            echo '<td>'  . $lookup[$cluster_row[$i]] . '</td>';
         } else {
            echo '<td> not allocated </td>';
         }
      } else {        
         echo '<td>'  . $cluster_row[$i] . '</td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   $query = "SELECT ID, Name, Type, Model, Serial FROM daf.node WHERE ClusterID = '" . $ID . "'";
   $testcase = mysql_query($query);

   if (! $testcase) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($testcase);   // should only be one row
   $num_fields = mysql_num_fields($testcase);

   echo '<form action="status.php" method="post">' . "\n";

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table class="auto-style1">';
   echo "<caption><div><div>Nodes</div></div></caption>";
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
            echo "<td><a href=status.php?action=show&object=node&ID=$testcase_row[$idindex]>$testcase_row[$j]</a></td>";  
         } else {       
            echo '<td>'  . $testcase_row[$j] . '</td>';
         }
      } 
      echo "</tr>\n";
      
   }
   
   echo "</tr>\n";
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
// Function:   modify_cluster
//
// Inputs:     none   
//
// Outputs:    Post's a list of variables that describe the characteristics of the cluster that is to be modified 
//
// Returns:    nothing
//
// Description:
//
// Presents a form that allows the user to specify the modified characteristics of a cluster that is already in the DAF node
// table.   The routine posts
//
// status.php?object=cluster?action=show&object=cluster&operation=modify&name=...&comments=&teststandID=....&type=....
//
//----------------------------------------------------------------------------------------------------------------------

function modify_cluster($ID) {

   $query = "SELECT ID, Name, teststandID, Comments FROM daf.cluster where cluster.ID = '" . $ID . "'"; 
   $cluster = mysql_query($query);
   
   if (! $cluster) die ("Database access failed for query=$query: " . mysql_error());
   
   $lookup = lookuptable("daf.teststand", "ID", "Name");
   $fieldnames = array("ID", "Name", "Teststand", "Comments");

   if (! $cluster) die ("Database access failed for query=$query: " . mysql_error());

   $num_rows   = mysql_num_rows($cluster);   // should only be one row
   $num_fields = mysql_num_fields($cluster);

   echo '<form action="status.php" method="post">' . "\n";

   // echo '<table class="auto-style1" style="width: 100%">';
   echo '<table class="auto-style1">';
   echo "<caption><div><div>Clusters</div></div></caption>";
   echo "\n";
   
   $fieldindex = array();
   for ($i = 0; $i < $num_fields; $i++) {
      $fieldindex[$fieldnames[$i]] = $i;
   }
   $IDindex   = $fieldindex['ID'];
   $nameindex = $fieldindex['Teststand'];
   
   $cluster_row = mysql_fetch_row($cluster);
 
   for ($i = 0; $i < $num_fields; $i++) {
      echo '<tr>';
      if ($i == $IDindex) {
         echo '<th>' . $fieldnames[$i] . '</th>';
         echo '<td>' . $cluster_row[$i] . '</td>';
      } else if ($i == $nameindex) {
         echo '<th>' . $fieldnames[$i] . '</th>';
         echo '<td><select name="teststandID" />' . "\n";
         selectorID("teststand", "Name", $cluster_row[$i]);
         echo '</select></td>' . "\n";         
      } else {
         echo '<th>' . $fieldnames[$i] . '</th>';
         echo '<td><input type="text" name="' . strtolower($fieldnames[$i]) . '" value="' . $cluster_row[$i] . '"/></td>';
      }
      echo '</tr>';
      echo "\n";
   }
   echo '</table>';
   
   echo '<div style="float:left;" display="inline">';
   echo '<input type="hidden" name="action"    value="show" />' . "\n";
   echo '<input type="hidden" name="operation" value="modify" />' . "\n";
   echo '<input type="hidden" name="object"    value="cluster" />' . "\n";
   echo '<input type="hidden" name="ID"        value="' . $ID .'" />' . "\n";
   echo '<input type="submit" name=button value="Modify Cluster" />' . "\n";
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