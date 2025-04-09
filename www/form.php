<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Improved Form</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.2/css/all.min.css"/>
  <style>
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    }

    body {
      height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      background: linear-gradient(135deg, #667eea, #764ba2);
    }

    .content-wrapper {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 20px;
    }

    .form-container {
      background: #fff;
      padding: 40px 30px;
      border-radius: 15px;
      width: 380px;
      box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
      animation: slideIn 0.6s ease;
    }

    @keyframes slideIn {
      from { opacity: 0; transform: translateY(20px); }
      to { opacity: 1; transform: translateY(0); }
    }

    h2 {
      text-align: center;
      color: #333;
      margin-bottom: 25px;
    }

    .input-group {
      position: relative;
      margin-bottom: 25px;
    }

    .input-group input {
      width: 100%;
      padding: 14px 12px;
      border: 1px solid #ccc;
      border-radius: 8px;
      font-size: 16px;
      outline: none;
      transition: all 0.3s;
    }

    .input-group label {
      position: absolute;
      left: 12px;
      top: 14px;
      background: white;
      padding: 0 4px;
      color: #777;
      transition: 0.3s;
      pointer-events: none;
    }

    .input-group input:focus + label,
    .input-group input:not(:placeholder-shown) + label {
      top: -9px;
      left: 10px;
      font-size: 12px;
      color: #667eea;
    }

    .submit-btn {
      background: #667eea;
      color: #fff;
      border: none;
      padding: 14px;
      width: 100%;
      font-size: 16px;
      border-radius: 8px;
      cursor: pointer;
      transition: background 0.3s, transform 0.2s;
    }

    .submit-btn:hover {
      background: #5a67d8;
      transform: translateY(-2px);
    }

    .submit-btn i {
      margin-right: 8px;
    }

    .result-container {
      background: #fff;
      padding: 20px 25px;
      border-radius: 10px;
      width: 380px;
      box-shadow: 0 10px 25px rgba(0, 0, 0, 0.1);
      text-align: left;
      color: #333;
      animation: fadeIn 0.4s ease;
    }

    @keyframes fadeIn {
      from { opacity: 0; }
      to { opacity: 1; }
    }

    .result-container h3 {
      margin-bottom: 10px;
      color: #444;
    }

    .result-container p {
      margin-bottom: 6px;
      font-size: 15px;
    }
  </style>
</head>
<body>

  <div class="content-wrapper">
    <form class="form-container" id="userForm">
      <h2><i class="fas fa-user-edit"></i> User Info</h2>

      <div class="input-group">
        <input type="text" name="input1" id="input1" placeholder=" " required>
        <label for="input1">First Input</label>
      </div>

      <div class="input-group">
        <input type="text" name="input2" id="input2" placeholder=" " required>
        <label for="input2">Second Input</label>
      </div>

      <button type="submit" class="submit-btn">
        <i class="fas fa-paper-plane"></i> Submit
      </button>
    </form>

    <div class="result-container" id="resultBox" style="display: none;">
      <h3><i class="fas fa-check-circle"></i> Submitted Info</h3>
      <p id="output1"></p>
      <p id="output2"></p>
    </div>
  </div>

  <script>
    const form = document.getElementById('userForm');
    const resultBox = document.getElementById('resultBox');
    const output1 = document.getElementById('output1');
    const output2 = document.getElementById('output2');

    form.addEventListener('submit', function (e) {
      e.preventDefault();

      const input1 = document.getElementById('input1').value;
      const input2 = document.getElementById('input2').value;

      output1.textContent = `First Input: ${input1}`;
      output2.textContent = `Second Input: ${input2}`;
      resultBox.style.display = 'block';

      form.reset();
    });
  </script>

</body>
</html>
