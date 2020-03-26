<?php
  include 'dbase.php';
  date_default_timezone_set('Asia/Jakarta');
  $dates = date("d/m/Y");
  $times = date("H:i:s");
  $sensor = @$_GET['sensor'];
  if (isset($sensor)) {
    $sql = mysqli_query($sqli, "UPDATE tb_database SET sensor='$sensor', times='$times', dates='$dates' WHERE id=1");
    if (!$sql) {
      //echo "ERROR: GET";
    }
  }
  $switch = @$_GET['switch'];
  if (isset($switch)) {
    $sql = mysqli_query($sqli, "UPDATE tb_database SET switch='$switch' WHERE id=1");
    if (!$sql) {
      // echo "ERROR: GET";
    }
  }
  $load = mysqli_query($sqli, "SELECT * FROM tb_database WHERE id=1");
  $result = mysqli_fetch_assoc($load);
  $switch = $result['switch'];
  echo $switch;
?>
