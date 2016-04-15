<?php

// Get the filename that was uploaded from the user, if one exists
if (isset($_FILES['file']['name'])) {
  $filename = $_FILES['file']['name'];
} else {
  $filename = '';
}

if ($filename == '') {
  # We don't have a filename yet, so generate
  # a file-picking menu page.
  menu();
} else {
  # We have a filename, so process the uploaded file.
  process_uploaded_file();
}  


function menu() {

   echo <<<_END
   <html>
      <body>
         <div>
         <form action="upload_backup.php" method="post" enctype="multipart/form-data">
            <label for="file">Filename:</label>
            <input type="file" name="file" id="file" /> 
            <br />
            <input type="submit" name="submit" value="Submit" />
         </form>
         </div>
         <div>
<form action="status.php" method="post">
<input type="submit" name=button value="Backups" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="backup" />
</form>
         </div>
         <div style="float:left;" display="inline">
<form action="status.php" method="post">
<input type="submit" name=button value="Home" />
</form>
         </div>;
      </body>
   </html>
_END;
   
}
 

function process_uploaded_file() {

   $backup_dir = "/opt/lampp/htdocs/status/backup";

   $target_path = $backup_dir . "/" . basename($_FILES['file']['name']); 
   
   echo "<html>\n";
   echo "<body>\n";
   echo "<div>\n";

   if ($_FILES["file"]["error"] > 0)      {
     echo "Error: Return Code: " . $_FILES["file"]["error"] . "<br />";
   } else {
     echo "Upload: " . $_FILES["file"]["name"] . "<br />";
     // echo "Type: " .   $_FILES["file"]["type"] . "<br />";
     echo "Size: " .  ($_FILES["file"]["size"] / 1024) . " Kb<br />";
     // echo "Temp file: " . $_FILES["file"]["tmp_name"] . "<br />";
 
    if (file_exists("upload/" . $_FILES["file"]["name"])) {
       echo $_FILES["file"]["name"] . " already exists. ";
    } else {
       if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_path)) {
          echo "Stored in: " . "$backup_dir" . "/" . $_FILES["file"]["name"];
       } else {
          echo "There was an error uploading the file to $target_path";
       }
     }
   }
   
   echo "</div>\n";
   
   echo '<div >';
   echo <<<_END
<form action="status.php" method="post">
<input type="submit" name=button value="Backups" />
<input type="hidden" name=action value="show" />
<input type="hidden" name=object value="backup" />
</form>
_END;
   echo "</div>\n";
   
   echo '<div style="float:left;" display="inline">';
echo <<<_END
<form action="status.php" method="post">
<input type="submit" name=button value="Home" />
</form>
_END;
   echo "</div>\n";
   
   echo "</body>\n";
   echo "</html>\n";

}

?>
