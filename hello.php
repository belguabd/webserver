<?php
// Get the query string from the URL
$name = isset($_GET['name']) ? $_GET['name'] : 'World';
$age = isset($_GET['age']) ? $_GET['age'] : 'unknown';

// Print the content type (this is required for CGI)
header('Content-Type: text/html');

// Print a simple HTML response
echo "<html><body>";
echo "<h1>Hello, $name!</h1>";
ech o "<p>You are $age years old.</p>";

// Infinite loop
// while (true) {
//     echo "<p>This is an infinite loop!</p>";
//     // Add a sleep to prevent excessive CPU usage
//     sleep(1); // Sleep for 1 second
// }

echo "</body></html>";
echo "Done\n";
?>