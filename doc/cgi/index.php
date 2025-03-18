<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Gradient File Upload with POST</title>
    
    <!-- Font Awesome Icons (CDN) -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.2/css/all.min.css">
    
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Arial', sans-serif;
        }

        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background: linear-gradient(135deg, #2193b0, #6dd5ed);
            padding: 20px;
        }

        .upload-box {
            background: white;
            padding: 30px;
            border-radius: 12px;
            text-align: center;
            width: 360px;
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.2);
            animation: fadeIn 0.5s ease-in-out;
        }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        h2 {
            margin-bottom: 15px;
            color: #444;
        }

        .drop-area {
            border: 2px dashed #aaa;
            padding: 40px;
            cursor: pointer;
            background: #f9f9f9;
            transition: 0.3s;
            border-radius: 10px;
            position: relative;
            margin-bottom: 15px;
        }

        .drop-area:hover {
            background: #eef7fa;
        }

        .drop-area i {
            font-size: 40px;
            color: #2193b0;
            margin-bottom: 10px;
        }

        .drop-area p {
            font-size: 16px;
            color: #333;
            font-weight: bold;
        }

        .drop-area.active {
            background: #e0f7fa;
            border-color: #00bcd4;
            box-shadow: 0 0 15px rgba(0, 188, 212, 0.5);
        }

        input[type="file"] {
            display: none;
        }

        .upload-btn {
            padding: 12px 22px;
            background: #2193b0;
            color: white;
            border: none;
            cursor: pointer;
            border-radius: 6px;
            font-size: 16px;
            transition: 0.3s;
            width: 100%;
            font-weight: bold;
            margin-top: 10px;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 8px;
        }

        .upload-btn:hover {
            background: #1a7a90;
            transform: scale(1.05);
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
        }

        #file-name {
            font-size: 14px;
            color: #555;
            margin-top: 12px;
            display: none;
        }

        .progress-bar {
            width: 100%;
            height: 8px;
            background: #ddd;
            margin-top: 15px;
            border-radius: 5px;
            overflow: hidden;
            display: none;
        }

        .progress {
            width: 0;
            height: 100%;
            background: #2193b0;
            transition: width 0.5s ease-in-out;
        }
    </style>
</head>
<body>

    <form action="upload.php" method="POST" enctype="multipart/form-data">
        <div class="upload-box">
            <h2><i class="fas fa-cloud-upload-alt"></i> Upload Your File</h2>
            <div class="drop-area" id="drop-area">
                <i class="fas fa-file-upload"></i>
                <p>Drag & Drop or <label for="file" style="color: #2193b0; cursor: pointer;">Browse</label></p>
                <input type="file" id="file" name="file" required>
            </div>
            <button class="upload-btn" type="submit" id="uploadBtn">
                <i class="fas fa-cloud-arrow-up"></i> Upload
            </button>
            <p id="file-name"></p>
            <div class="progress-bar">
                <div class="progress"></div>
            </div>
        </div>
    </form>




    <script>
        const dropArea = document.getElementById("drop-area");
        const fileInput = document.getElementById("file");
        const fileNameDisplay = document.getElementById("file-name");

        dropArea.addEventListener("dragover", (e) => {
            e.preventDefault();
            dropArea.classList.add("active");
        });

        dropArea.addEventListener("dragleave", () => {
            dropArea.classList.remove("active");
        });

        dropArea.addEventListener("drop", (e) => {
            e.preventDefault();
            dropArea.classList.remove("active");
            handleFile(e.dataTransfer.files[0]);
        });

        fileInput.addEventListener("change", () => {
            handleFile(fileInput.files[0]);
        });

        function handleFile(file) {
            if (file) {
                fileNameDisplay.textContent = `📂 ${file.name}`;
                fileNameDisplay.style.display = "block";
                fileNameDisplay.style.color = "#333";
            }
        }

        dropArea.addEventListener("click", () => fileInput.click());
    </script>
</body>
</html>