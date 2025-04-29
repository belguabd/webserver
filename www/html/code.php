<?php
// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the 'name' parameter from POST data
    $name = isset($_POST['name']) ? $_POST['name'] : '';

    // Output the value
    echo "Received name: " . htmlspecialchars($name);
} else {
    echo "Invalid request method.";
}
?>
