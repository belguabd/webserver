<?php
// Simple hardcoded credentials
$valid_users = [
    "admin" => "admin",
    "ali" => "ali",
    "jojo" => "jojo"
];

// Check if user is already logged in
if (isset($_COOKIE['user'])) {
    // Start output only after all header operations
    echo "<!DOCTYPE html>
    <html lang='en'>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Welcome Back</title>
        <script src='https://cdn.tailwindcss.com'></script>
        <link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;500;600;700&display=swap' rel='stylesheet'>
        <style>
            body { font-family: 'Poppins', sans-serif; background: url('https://source.unsplash.com/random/1920x1080/?nature') no-repeat center center fixed; background-size: cover; }
            .glass-card { background: rgba(255, 255, 255, 0.85); backdrop-filter: blur(10px); border-radius: 20px; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1); }
            .btn-gradient { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; }
        </style>
    </head>
    <body class='flex items-center justify-center min-h-screen p-4'>
        <div class='glass-card p-10 max-w-md w-full text-center'>
            <img src='https://cdn-icons-png.flaticon.com/512/190/190411.png' alt='Welcome' class='w-24 h-24 mx-auto mb-6'>
            <h2 class='text-3xl font-bold mb-4 text-gray-800'>Welcome back, " . htmlspecialchars($_COOKIE['user']) . "!</h2>
            <p class='text-gray-600 mb-8'>You're already logged in to your account</p>
            <div class='space-y-4'>
                <a href='logout.php' class='btn-gradient px-6 py-3 rounded-lg font-medium inline-block w-full hover:shadow-lg transition-all'>Logout</a>
                <a href='login.php' class='text-indigo-600 hover:text-indigo-800 font-medium'>Return to home</a>
            </div>
        </div>
    </body>
    </html>";
    exit;
}

// Check form submission
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';

    if (isset($valid_users[$username]) && $valid_users[$username] === $password) {
        // Set cookie for 1 hour - must be before any output
        setcookie("user", $username, time() + 3600, "/");
        
        // Now send output
        echo "<!DOCTYPE html>
        <html lang='en'>
        <head>
            <meta charset='UTF-8'>
            <meta name='viewport' content='width=device-width, initial-scale=1.0'>
            <title>Login Successful</title>
            <script src='https://cdn.tailwindcss.com'></script>
            <link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;500;600;700&display=swap' rel='stylesheet'>
            <style>
                body { font-family: 'Poppins', sans-serif; background: url('https://source.unsplash.com/random/1920x1080/?success') no-repeat center center fixed; background-size: cover; }
                .glass-card { background: rgba(255, 255, 255, 0.85); backdrop-filter: blur(10px); border-radius: 20px; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1); }
                .btn-gradient { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; }
            </style>
        </head>
        <body class='flex items-center justify-center min-h-screen p-4'>
            <div class='glass-card p-10 max-w-md w-full text-center'>
                <img src='https://cdn-icons-png.flaticon.com/512/190/190411.png' alt='Success' class='w-24 h-24 mx-auto mb-6'>
                <h2 class='text-3xl font-bold mb-4 text-gray-800'>Login Successful!</h2>
                <p class='text-gray-600 mb-8'>Welcome back, " . htmlspecialchars($username) . "</p>
                <a href='login.php' class='btn-gradient px-6 py-3 rounded-lg font-medium inline-block w-full hover:shadow-lg transition-all'>Continue to Dashboard</a>
            </div>
        </body>
        </html>";
        exit;
    } else {
        $error = "Invalid credentials! Please try again.";
    }
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login | Your Brand</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <style>
        body { 
            font-family: 'Poppins', sans-serif; 
            background: url('https://source.unsplash.com/random/1920x1080/?login') no-repeat center center fixed; 
            background-size: cover; 
        }
        .glass-card { 
            background: rgba(255, 255, 255, 0.85); 
            backdrop-filter: blur(10px); 
            border-radius: 20px; 
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1); 
        }
        .btn-gradient { 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
            color: white; 
        }
        .input-field {
            transition: all 0.3s ease;
            background: rgba(255, 255, 255, 0.7);
        }
        .input-field:focus {
            background: rgba(255, 255, 255, 0.95);
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.2);
        }
    </style>
</head>
<body class="flex items-center justify-center min-h-screen p-4">
    <div class="glass-card p-10 max-w-md w-full">
        <div class="text-center mb-8">
            <img src="https://cdn-icons-png.flaticon.com/512/295/295128.png" alt="Login" class="w-20 h-20 mx-auto">
            <h1 class="text-3xl font-bold text-gray-800 mt-4">Welcome Back</h1>
            <p class="text-gray-600 mt-2">Sign in to access your account</p>
        </div>
        
        <?php if (!empty($error)): ?>
            <div class="mb-6 p-4 bg-red-50 text-red-700 rounded-lg flex items-center">
                <svg class="w-5 h-5 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"></path>
                </svg>
                <?php echo $error; ?>
            </div>
        <?php endif; ?>
        
        <form method="post" action="" class="space-y-6">
            <div>
                <label for="username" class="block text-sm font-medium text-gray-700 mb-2">Username</label>
                <div class="relative">
                    <div class="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none">
                        <svg class="h-5 w-5 text-gray-400" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z"></path>
                        </svg>
                    </div>
                    <input type="text" name="username" id="username" class="input-field pl-10 w-full px-4 py-3 rounded-lg border border-gray-200 focus:outline-none" placeholder="Enter your username" required>
                </div>
            </div>
            <div>
                <label for="password" class="block text-sm font-medium text-gray-700 mb-2">Password</label>
                <div class="relative">
                    <div class="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none">
                        <svg class="h-5 w-5 text-gray-400" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z"></path>
                        </svg>
                    </div>
                    <input type="password" name="password" id="password" class="input-field pl-10 w-full px-4 py-3 rounded-lg border border-gray-200 focus:outline-none" placeholder="Enter your password" required>
                </div>
            </div>
            <div class="flex items-center justify-between">
                <div class="flex items-center">
                    <input id="remember-me" name="remember-me" type="checkbox" class="h-4 w-4 text-indigo-600 focus:ring-indigo-500 border-gray-300 rounded">
                    <label for="remember-me" class="ml-2 block text-sm text-gray-700">Remember me</label>
                </div>
                <div class="text-sm">
                    <a href="#" class="font-medium text-indigo-600 hover:text-indigo-500">Forgot password?</a>
                </div>
            </div>
            <div>
                <button type="submit" class="btn-gradient w-full px-6 py-3 rounded-lg font-medium hover:shadow-lg transition-all">
                    Sign In
                </button>
            </div>
        </form>
        
        <div class="mt-6 text-center">
            <p class="text-gray-600">Don't have an account? <a href="#" class="text-indigo-600 hover:text-indigo-800 font-medium">Contact admin</a></p>
        </div>
    </div>
</body>
</html>