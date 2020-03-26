<?php
  include 'dbase.php';
  $load = mysqli_query($sqli, "SELECT * FROM tb_database WHERE id=1");
  $result = mysqli_fetch_assoc($load);
  $sensor = json_encode($result);
  echo $sensor;
?>
