<?php

require_once('phprestsql.php');
require_once '../login.php';



$RestfulSQL = new RestfulSQL($db_hostname, $db_username, $db_password, $db_database);
$RestfulSQL->exec();


// echo '<pre>';
// var_dump($PHPRestSQL->output);


?>