<?php
  $hostname = 'localhost';
  $username = 'localhost';
  $password = 'localhost';
  $database = 'localhost';
  $sqli = mysqli_connect($hostname, $username, $password, $database);
  if (mysqli_connect_errno()) {
    // echo 'ERROR: '.mysqli_connect_errno();
  }
?>
