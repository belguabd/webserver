<?php
$email = isset($_POST["email"]) ? htmlspecialchars($_POST["email"]) : '';
$message = isset($_POST["message"]) ? htmlspecialchars($_POST["message"]) : '';
if ($email && $message) {
    echo "Email: $email <br>";
    echo "Message: $message";
}else
{
    echo "Waiting for POST submission...";
}
?>
