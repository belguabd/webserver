<?php
$name = isset($_GET["name"]) ? htmlspecialchars($_GET["name"]) : '';
echo "Hello $name!";
?>