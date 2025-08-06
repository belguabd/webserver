# Webserv - HTTP Server Implementation in C++98

> *"This is when you finally understand why URLs start with HTTP"*

## 📋 Project Overview

**Webserv** is a custom HTTP server implementation written in C++98. This project provides a deep understanding of the HTTP protocol by building a fully functional web server from scratch that can handle real browser requests.

### Key Features
- ✅ HTTP/1.0 compliant server
- ✅ Non-blocking I/O operations
- ✅ Multi-port listening capability
- ✅ Static website serving
- ✅ File upload functionality
- ✅ CGI execution support
- ✅ Configurable via configuration files
- ✅ Compatible with standard web browsers

## 🚀 Getting Started

### Prerequisites
- C++ compiler supporting C++98 standard
- Make utility
- Unix-based operating system (Linux/macOS)

### Installation

1. **Clone the repository**
   ```bash
   git clone [repository-url]
   cd webserv
   ```

2. **Compile the project**
   ```bash
   make
   ```

3. **Run the server**
   ```bash
   ./webserv [configuration_file]
   ```

   If no configuration file is provided, the server will use the default configuration.

## ⚙️ Configuration

The server uses a configuration file similar to NGINX's server block syntax. Here's what you can configure:

### Basic Server Settings
```nginx
server {
    listen 8080;                    # Port to listen on
    server_name example.com;        # Server name (optional)
    client_max_body_size 1M;        # Maximum request body size
    
    # Error pages
    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;
    
    # Document root
    root /var/www/html;
    index index.html index.php;
}
```

### Location Blocks
```nginx
location / {
    allowed_methods GET POST DELETE;
    autoindex on;                   # Enable directory listing
}

location /upload {
    allowed_methods POST;
    upload_enable on;
    upload_store /tmp/uploads;
}

location /api {
    return 301 /api/v1;            # HTTP redirection
}

location ~ \.php$ {
    fastcgi_pass 127.0.0.1:9000;  # CGI configuration
    root /var/www/html;
}
```

### Multiple Server Blocks
```nginx
# Website 1
server {
    listen 8080;
    server_name site1.com;
    root /var/www/site1;
}

# Website 2  
server {
    listen 8081;
    server_name site2.com;
    root /var/www/site2;
}
```

## 🔧 Technical Implementation

### Architecture
- **Non-blocking I/O**: Uses `poll()`, `select()`, `kqueue()`, or `epoll()` for efficient connection handling
- **Single-threaded**: All operations handled in one thread using event-driven programming
- **Modular design**: Separate modules for parsing, routing, and response generation

### HTTP Methods Supported
- `GET` - Retrieve resources
- `POST` - Submit data/upload files
- `DELETE` - Remove resources

### CGI Support
The server supports Common Gateway Interface (CGI) for dynamic content:
- PHP scripts (`.php`)
- Python scripts (`.py`)
- Any executable with proper shebang

#### CGI Environment Variables
- `REQUEST_METHOD`
- `QUERY_STRING`
- `CONTENT_LENGTH`
- `CONTENT_TYPE`
- `PATH_INFO`
- `SCRIPT_NAME`
- And more...

## 🗂️ Project Structure

```
webserv/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── HttpRequest.cpp
│   ├── HttpResponse.cpp
│   ├── ConfigParser.cpp
│   └── CgiHandler.cpp
├── include/
│   ├── Server.hpp
│   ├── HttpRequest.hpp
│   ├── HttpResponse.hpp
│   └── ConfigParser.hpp
├── config/
│   ├── default.conf
│   └── example.conf
├── www/
│   ├── index.html
│   ├── upload/
│   └── cgi-bin/
└── README.md
```

## 🧪 Testing

### Manual Testing
```bash
# Basic GET request
curl http://localhost:8080/

# POST request with data
curl -X POST -d "name=test" http://localhost:8080/form

# File upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# Using telnet
telnet localhost 8080
GET / HTTP/1.1
Host: localhost

```

### Browser Testing
1. Start the server: `./webserv config/default.conf`
2. Open your browser and navigate to `http://localhost:8080`
3. Test various features:
   - Static file serving
   - Directory listing
   - File uploads
   - Error pages
   - CGI scripts

### Stress Testing
The server should handle multiple concurrent connections without blocking or crashing:
```bash
# Apache Bench example
ab -n 1000 -c 10 http://localhost:8080/

# Custom stress test
python stress_test.py
```

## ⚠️ Important Notes

### Compliance Requirements
- **No blocking operations**: All I/O must go through polling mechanisms
- **No external libraries**: Only standard C++98 and allowed system calls
- **Memory management**: No memory leaks or crashes under any circumstances
- **Error handling**: Proper HTTP status codes and error pages

### Allowed System Calls
```c
// Network operations
socket, accept, listen, send, recv, bind, connect
setsockopt, getsockname, getaddrinfo, freeaddrinfo

// I/O multiplexing  
select, poll, epoll, kqueue, kevent

// Process management (CGI only)
execve, fork, pipe, waitpid, kill

// File operations
open, close, read, write, access, stat
opendir, readdir, closedir, chdir

// Utilities
dup, dup2, fcntl, signal, strerror, errno
htons, htonl, ntohs, ntohl
```

### macOS Specific
On macOS, you may use `fcntl()` with these flags only:
- `F_SETFL`
- `O_NONBLOCK` 
- `FD_CLOEXEC`

## 🐛 Common Issues & Solutions

### Server Won't Start
- Check if port is already in use: `lsof -i :8080`
- Verify configuration file syntax
- Ensure proper permissions for document root

### CGI Scripts Not Working
- Check file permissions (executable)
- Verify shebang line (`#!/usr/bin/php`)
- Ensure CGI binary is in PATH
- Check error logs

### File Upload Issues
- Verify upload directory exists and is writable
- Check `client_max_body_size` setting
- Ensure proper form encoding (`multipart/form-data`)

## 📚 Learning Resources

- [RFC 2616 - HTTP/1.1](https://tools.ietf.org/html/rfc2616)
- [RFC 1945 - HTTP/1.0](https://tools.ietf.org/html/rfc1945)
- [CGI Specification](https://tools.ietf.org/html/rfc3875)
- [NGINX Configuration Guide](https://nginx.org/en/docs/beginners_guide.html)

## 🏆 Bonus Features

If the mandatory part is completed perfectly, you can implement:
- **Cookie and session management**
- **Multiple CGI type support** (PHP, Python, Perl, etc.)
- **Virtual host support**
- **HTTPS/SSL support**
- **Request/response logging**

## 📝 Development Guidelines

### Code Standards
- Follow C++98 standard strictly
- Use RAII for resource management
- Implement proper exception handling
- Write clean, readable code with comments
- Use appropriate C++ features over C equivalents

### Testing Strategy
1. Unit test individual components
2. Integration testing with real browsers
3. Stress testing for performance
4. Edge case testing (malformed requests, large files, etc.)
5. CGI functionality testing

### Debugging Tips
- Use tools like `valgrind` for memory leak detection
- Monitor with `strace`/`ltrace` for system call debugging
- Test with `curl`, `telnet`, and multiple browsers
- Compare behavior with NGINX when uncertain

---

## 🤝 Contributing

1. Ensure code follows C++98 standards
2. Test thoroughly before submitting
3. Document any new features or configurations
4. Follow the existing code style
