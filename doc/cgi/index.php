<?php
// CGI scripts MUST start with a Content-Type header
header("Content-Type: text/html");

// Additional headers can be sent before output
header("test: hello");
header("X-Custom-Header: MyValue");


// Blank line to separate headers from content (automatically handled by PHP)
// echo "<html><body><h1>Hello, World!</h1></body></html>";
// while (true) {
//     // Blank line to separate headers from content (automatically handled by PHP)
//     echo "<html><body><h1>Hello, World!</h1></body></html>";

//     // Optionally, to prevent too much load, you can add a small sleep interval
//     sleep(1); // Sleep for 1 second to avoid overloading the server
// }
?>
