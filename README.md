# Simple HTTP Server in C

A minimal HTTP server built from scratch using POSIX sockets. The purpose of this project is to understand how HTTP communication works at a low level using TCP sockets.

## Overview

The server performs the following steps:

1. Create a TCP socket.
2. Bind the socket to a local IP address and port.
3. Listen for incoming connections.
4. Accept a client connection.
5. Read the HTTP request.
6. Open `index.html`.
7. Send an HTTP response containing the file contents.
8. Close the client connection.

Flow:

```text
Browser
    |
    v
accept()
    |
    v
read()
    |
    v
open index.html
    |
    v
write()
    |
    v
Browser renders page
```

---

## Networking Concepts

### Socket

A socket is an endpoint used for communication.

```c
socket(AF_INET, SOCK_STREAM, 0);
```

- `AF_INET` → IPv4
- `SOCK_STREAM` → TCP
- Returns a file descriptor representing the socket

---

### IP Address and Port

A server listens on an IP address and port.

Examples:

```text
127.0.0.1:9000
192.168.1.100:9000
0.0.0.0:9000
```

`INADDR_ANY` means:

```text
Bind to all available network interfaces.
```

---

### sockaddr_in

Used to represent an IPv4 address.

```c
struct sockaddr_in {
    sa_family_t    sin_family;
    in_port_t      sin_port;
    struct in_addr sin_addr;
};
```

Example:

```c
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(9000);
```

---

### Network Byte Order

Network protocols use big-endian byte ordering.

Ports must be converted before being stored:

```c
htons(port);
```

Meaning:

```text
Host TO Network Short
```

Related functions:

```c
htons()
htonl()
ntohs()
ntohl()
```

---

## Important System Calls

### socket()

Creates a socket.

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

Returns a socket file descriptor.

---

### bind()

Associates a socket with an IP address and port.

```c
bind(
    sockfd,
    (struct sockaddr *)&serv_addr,
    sizeof(serv_addr)
);
```

Conceptually:

```text
socket
   |
   v
0.0.0.0:9000
```

---

### listen()

Marks the socket as a listening socket.

```c
listen(sockfd, 5);
```

The second argument is the connection backlog.

---

### accept()

Waits for a client connection.

```c
int clientfd = accept(sockfd, NULL, NULL);
```

Returns a new socket dedicated to the connected client.

```text
sockfd   -> listening socket
clientfd -> connected client
```

---

### read()

Reads bytes from the client.

```c
int n = read(
    clientfd,
    buffer,
    sizeof(buffer) - 1
);
```

Returns the number of bytes read.

---

### write()

Sends bytes to the client.

```c
write(clientfd, response, length);
```

Used to send HTTP headers and page contents.

---

### close()

Closes a file descriptor.

```c
close(clientfd);
```

---

## Reading HTTP Requests

Example request received from a browser:

```http
GET / HTTP/1.1
Host: localhost:9000
User-Agent: Mozilla/5.0
Accept: text/html
```

The request arrives as plain text.

After reading:

```c
buffer[n] = '\0';
```

The buffer becomes a valid C string that can be printed or parsed.

---

## File Handling

Open a file:

```c
FILE *fp = fopen("index.html", "r");
```

Read contents:

```c
size_t bytes =
    fread(
        html,
        1,
        sizeof(html) - 1,
        fp
    );
```

Add a null terminator:

```c
html[bytes] = '\0';
```

Close the file:

```c
fclose(fp);
```

---

## HTTP Response Structure

Browsers expect a properly formatted HTTP response:

```http
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 123

<html>
...
</html>
```

The empty line separates:

```text
Headers
Body
```

Without this blank line, the browser cannot correctly identify where the headers end and the page content begins.

---

## snprintf()

Used to safely construct response headers.

```c
snprintf(
    header,
    sizeof(header),
    "Content-Length: %zu\r\n",
    bytes
);
```

Unlike `sprintf()`, it prevents writing past the end of the buffer.

---

## Current Limitations

The server currently:

- Handles one client at a time.
- Serves only one HTML file.
- Does not parse routes.
- Does not support POST requests.
- Does not support concurrency.
- Performs minimal HTTP parsing.

---

## Possible Improvements

- Parse HTTP methods and paths.
- Add routing (`/`, `/about`, `/users`).
- Serve static files automatically.
- Return JSON responses.
- Handle POST requests.
- Add logging.
- Support multiple simultaneous clients.
- Build a lightweight backend framework on top of the server.

---

## Key Takeaway

At its core, web communication is simply:

```text
TCP Connection
    ↓
HTTP Request (text)
    ↓
HTTP Response (headers + body)
```

Frameworks such as Express, Flask, FastAPI, Django, Spring, and others ultimately build abstractions on top of these same underlying concepts.
