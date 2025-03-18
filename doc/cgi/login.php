<?php
// Check if the user is logged in via cookie
if (isset($_COOKIE['username'])) {
    echo "<h2>Welcome, you are login successfully !</h2>";
    echo "<a href='?logout=true'>Logout</a>";
    
    // Logout handler
    // if (isset($_GET['logout'])) {
    //     setcookie('username', '', time() - 3600, '/');
    //     header("Location: ".$_SERVER['PHP_SELF']);
    //     exit();
    // }
} else {
    echo "<h2>Login</h2>";
    echo "<form method='POST'  enctype='multipart/form-data'>
            <label>Username: <input type='text' name='username' required></label><br>
            <label>Password: <input type='password' name='password' required></label><br>
            <button type='submit'>Login</button>
          </form>";
    
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $username = $_POST['username'] ?? '';
        $password = $_POST['password'] ?? '';
        
        if ($username === 'admin' && $password === 'password123') {
            setcookie('username', $username, time() + 3600, '/');
            header("Location: ".$_SERVER['PHP_SELF']);
            exit();
        } else {
            echo "<p style='color:red;'>Invalid username or password</p>";
        }
    }
}
?>