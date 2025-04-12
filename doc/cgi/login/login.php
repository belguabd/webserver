<?php
// Simple hardcoded credentials
// $valid_username = "admin";
// $valid_password = "admin";
$valid_users = [
    "admin" => "admin",
    "ali" => "ali",
    "jojo"   => "jojo"
];

// Check if user is already logged in
if (isset($_COOKIE['user'])) {
    echo "Welcome back, " . htmlspecialchars($_COOKIE['user']) . "!<br>";
    echo '<a href="logout.php">Logout</a>';
    exit;
}

// Check form submission
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';

    // if ($username === $valid_username && $password === $valid_password) {
    if (isset($valid_users[$username]) && $valid_users[$username] === $password) {
        // Set cookie for 1 hour
        setcookie("user", $username, time() + 3600, "/"); // Must be before any output
        echo "Login successful!<br>";
        echo '<a href="login.php">Go to home</a>';
        exit;
    } else {
        $error = "Invalid credentials!";
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
</head>
<body>
    <h2>Login</h2>
    <?php if (!empty($error)) echo "<p style='color:red;'>$error</p>"; ?>
    <form method="post" action="">
        Username: <input type="text" name="username"><br><br>
        Password: <input type="password" name="password"><br><br>
        <input type="submit" value="Login">
    </form>
</body>
</html>
