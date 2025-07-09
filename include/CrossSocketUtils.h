#ifndef __CROSS_SOCKET_UTILS_H
#define __CROSS_SOCKET_UTILS_H

#include <stdexcept>
#ifdef _WIN32 // If using Windows, include Winsock headers
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link Winsock to the compiler

using socket_t = SOCKET;

// Define platform-neutral error codes
#define CSEWOULDBLOCK WSAEWOULDBLOCK   // Fires when nonblocking mode is on and an operation cannot be completed immediately
#define CSEINPROGRESS WSAEINPROGRESS   // Fires when a blocking operation is in progress because only one blocking operation can run per task/thread
#define CSEALREADY WSAEALREADY		   // Fires when a nonblocking socket calls an operation while running another operation
#define CSECONNRESET WSAECONNRESET	   // Fires when the Server Socket forcibly closes the connection
#define CSECONNREFUSED WSAECONNREFUSED // Fires when the target computer actively refuses it. This usually happens when attempting to connect to a target with no Server Socket

#define CSERROR WSAGetLastError()
#else // If using a Unix system, include <sys/socket.h> and similar headers (because this is the C standard, Winsock macros have been renamed to match the defaults here)
// Some of these includes may be somewhat redundant and/or unused
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using socket_t = int; // socket_t type definition required for platform-neutrality

// Define INVALID_SOCKET and SOCKET_ERROR to match the Winsock definitions
// Unix returns -1 in the same cases where Winsock returns these macros (assigned to ~0 and -1 respectively), so defining these macros to equal -1 is the cleaner and easier route
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

// Define platform-neutral error codes
#define CSEWOULDBLOCK EWOULDBLOCK	// Fires when nonblocking mode is on and an operation cannot be completed immediately
#define CSEINPROGRESS EINPROGRESS	// Fires when a blocking operation is in progress because only one blocking operation can run per task/thread
#define CSEALREADY EALREADY			// Fires when a nonblocking socket calls an operation while running another operation
#define CSECONNRESET ECONNRESET		// Fires when the Server Socket forcibly closes the connection
#define CSECONNREFUSED ECONNREFUSED // Fires when the target computer actively refuses it. This usually happens when attempting to connect to a target with no Server Socket

#define CSERROR errno
#endif

namespace CrossSocket
{
	class CS_Utils
	{
	private:
		static bool initialized;

		/**
		 * @brief Initialize CrossSocket
		 *
		 * @return True if CrossSocket is initialized. False if CrossSocket is not initialized
		 */
		static bool Initialize();

		/**
		 * @brief Shut down CrossSocket
		 */
		static void Cleanup();

		/**
		 * @brief Platform-neutral function to close a socket
		 *
		 * @param socket Socket to close
		 */
		static inline void close_socket(socket_t socket)
		{
#ifdef _WIN32
			closesocket(socket);
#else
			close(socket);
#endif
		}

		friend class Socket;
		friend class SocketManager;
	};
}

#endif
