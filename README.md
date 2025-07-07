# CrossSocket
## Version 1.0
- Added `CrossSocket.h`, `CrossSocketUtils.h`, and `SocketManager.h`
  - All use the `CrossSocket` namespace
- Implemented support for TCP over IPv4
- Implement cross-platform compatibility
  - Redefined the macros `EWOULDBLOCK`, `EINPROGRESS`, `EALREADY`, `ECONNRESET` on Windows to match Unix errors
  - Redefined the `errno` macro on Windows to match Unix error fetching
  - Defined the `INVALID_SOCKET` and `SOCKET_ERROR` macros on Unix to match Windows
  - Abstracted `SOCKET` (Windows) and `int` (Unix) to one type `socket_t`
- Works for both Server and Client Sockets
- Calls to CrossSocket should be wrapped in `try-catch` statements. In the event of an error, CrossSocket functions throw `std::exception`
### CrossSocket.h
- Creates `Socket` objects
  - `Socket` acts as a `socket_t` wrapper for simpler platform-neutral use
- Used to handle Socket data sending and receiving
  - `Send()`
  - `Receive()`
- Sockets can enable nonblocking mode with `SetNonBlockingMode()`
### CrossSocketUtils.h
- Handles macros and functions for platform-neutrality
- Handles includes
- Handles initialization and cleanup of CrossSocket
- Everything in this class should only ever be used by CrossSocket
### SocketManager.h
- Added support for handling multiple Sockets at once
- Can only work with one type of Socket (Server/Client) at a time
- Added support for calling Socket commands:
  - Once
    - With a timeout
  - Infinitely
  - Until a condition is met
- Sockets must be closed one by one if they are handled by the SocketManager
  - It is recommended that they are also closed by the file which added them to the SocketManager
### *Warnings*
<a id="1.0W1"></a>
- CrossSocket has never been tested on platforms other than Windows. CrossSocket has been designed with those platforms in mind, but behavior is currently undefined (1.0W1)
<a id="1.0W2"></a>
- CrossSocket has only ever been compiled with MSVC. MSVC automatically adds many includes related to strings and C-strings, so using other compilers may result in errors related to not having those includes (1.0W2)
<a id="1.0W3"></a>
- CrossSocket allows you to call Server commands on a Client Socket and Client commands on a Server Socket. This **WILL NOT WORK** and will result in errors. Currently, the only way to prevent this from happening is simply not doing it (1.0W3)
<a id="1.0W4"></a>
- The macro undefining and redefining in `CrossSocketUtils.h` is functional an techincally proper in C++, but it follows some bad practices. If errors occur related to error-checking and handling, ensure that those macros are not causing problems. (1.0W4)
<a id="1.0W5"></a>
- While the `Send()` and `Receive()` methods are capable of functioning with UDP connections, CrossSocket currently does not have a way of creating a UDP socket. All sockets function using TCP streams, so attempting to communicate using UDP will result in undefined behavior (1.0W5)
<a id="1.0W6"></a>
- Nonblocking mode has very inconsistent behavior when used with the SocketManager. There is currently no explanation for why and no method to fix it yet (1.0W6)
<a id="1.0W7"></a>
- Because the `EWOULDBLOCK`, `EINPROGRESS`, `EALREADY`, `ECONNRESET`, and `errno` macros have all been redefined on Windows, any references to them in files including CrossSocket will likely function incorrectly (1.0W7)
<a id="1.0W8"></a>
- `SocketManager.cpp` occasionally outputs build warnings about local variables and data type conversion. Functionality isn't impacted by this (1.0W8)
