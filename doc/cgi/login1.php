<?php
session_start();

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $username = $_POST["username"] ?? "";
    $password = $_POST["password"] ?? "";

    if ($username === "admin" && $password === "password") {
        setcookie("session_id", session_id(), time() + 3600, "/");
        echo "<h1>Login successful!</h1>";
    } else {
        echo "<h1>Login failed.</h1>";
    }
} else {
    echo '
    <form method="POST" action="/cgi/login1.php">
        <label>Username: <input name="username" /></label><br/>
        <label>Password: <input type="password" name="password" /></label><br/>
        <input type="submit" value="Login" />
    </form>
    ';
}
?>
