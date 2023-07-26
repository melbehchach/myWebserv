<?php
session_start();

if(isset($_SESSION['visited'])){
  echo "Session is already set";
}else{
  $_SESSION['visited'] = true;
  echo "Session is not set";
}
?>
