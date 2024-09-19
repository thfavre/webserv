# webserv
*C++98 HTTP/1.1 web server from scratch.*

## Overview
**Webserv** is an HTTP server written in C++98 that complies with the HTTP/1.1 protocol. This project is an in-depth exploration of how URLs begin with HTTP and a step towards understanding the inner workings of web servers. The server is fully functional with a web browser, and follows the constraints of C++98, offering non-blocking operations and supporting essential HTTP methods such as GET, POST, and DELETE.

## Features
- HTTP/1.1 compliant server.
- Handles multiple simultaneous connections using poll() (or equivalent).
- Supports multiple HTTP methods: GET, POST, and DELETE.
- Non-blocking operations using file descriptors (FD).
- Customizable via configuration file.
- Error handling with default error pages.
- File uploads and static file serving.
  
## Requirements
- C++98 standard.

## Compile
To compile the project, simply run the following command:
``` bash
make
```
This will generate the executable `webserv`.

## Usage
Run the server with a configuration file:

```bash
./webserv [configuration file]
```

## Configuration File
The server uses a configuration file to define its behavior, such as ports, routes, and error handling.

## HTTP Methods Supported
- GET: Retrieve static files or resources.
- POST: Submit data to the server.
- DELETE: Remove specified resources on the server.
