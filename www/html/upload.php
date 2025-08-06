<?php
// CSS for styling the messages
echo "<style>
.message-container {
    max-width: 600px;
    margin: 0 auto;
    padding: 20px;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    text-align: center;
}

.message {
    display: flex;
    align-items: center;
    justify-content: center; /* Center the content */
    padding: 20px;
    margin: 15px 0;
    border-radius: 12px;
    font-size: 16px;
    box-shadow: 0 6px 15px rgba(0, 0, 0, 0.1);
    font-weight: 500;
}

.message.success {
    background-color: #4CAF50;  /* Fresh green */
    color: white;
    border: 1px solid #388E3C;
}

.message.error {
    background-color: #FF5733;  /* Warm red */
    color: white;
    border: 1px solid #C1351D;
}

.message.warning {
    background-color: #FFC107;  /* Bright yellow */
    color: black;
    border: 1px solid #FF8F00;
}

.message p {
    margin: 0;
    font-size: 1.1em;
    line-height: 1.4;
}

.message.success .icon {
    color: #388E3C;  /* Dark green */
}

.message.error .icon {
    color: #C1351D;  /* Dark red */
}

.message.warning .icon {
    color: #FF8F00;  /* Dark yellow */
}
</style>";

// Check if a file has been uploaded
if (isset($_FILES['file'])) {
    $file = $_FILES['file'];
    $targetDir = "upload/";
    
    $targetFile = $targetDir . basename($file["name"]);
    
    // Try to move the uploaded file
    if (move_uploaded_file($file["tmp_name"], $targetFile)) {
        // Success message without the icon
        $message = "<div class='message-container'>
                      <div class='message success'>
                        <p><strong>" . htmlspecialchars(basename($file["name"])) . "</strong> has been uploaded successfully!</p>
                      </div>
                    </div>";
    } else {
        // Error message without the icon
        $message = "<div class='message-container'>
                      <div class='message error'>
                        <p>Sorry, there was an error uploading your file. Please try again later.</p>
                      </div>
                    </div>";
    }
} else {
    // Warning message if no file was uploaded
    $message = "<div class='message-container'>
                  <div class='message warning'>
                    <p>No file was uploaded. Please select a file and try again.</p>
                  </div>
                </div>";
}

// Display the message
echo $message;
?>
