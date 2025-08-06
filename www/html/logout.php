<?php
// Clear the cookie
setcookie("user", "", time() - 3600, "/");
// header("Location: login.php");
echo "<script type='text/javascript'>
    window.location.href = 'login.php';
</script>";
// sleep(4);
exit;
?>