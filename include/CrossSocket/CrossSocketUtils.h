#ifndef __CROSS_SOCKET_UTILS_H
#define __CROSS_SOCKET_UTILS_H

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>

using socket_t = SOCKET;

#define CSEWOULDBLOCK WSAEWOULDBLOCK   // Fires when nonblocking mode is on and an operation cannot be completed immediately
#define CSEINPROGRESS WSAEINPROGRESS   // Fires when a blocking operation is in progress because only one blocking operation can run per task/thread
#define CSEALREADY WSAEALREADY		   // Fires when a nonblocking socket calls an operation while running another operation
#define CSECONNRESET WSAECONNRESET	   // Fires when the Server Socket forcibly closes the connection
#define CSECONNREFUSED WSAECONNREFUSED // Fires when the target computer actively refuses it. This usually happens when attempting to connect to a target with no Server Socket

#define CSERROR WSAGetLastError()
#else
// Some of these includes may be somewhat redundant and/or unused
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using socket_t = int;

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#define CSEWOULDBLOCK EWOULDBLOCK	// Fires when nonblocking mode is on and an operation cannot be completed immediately
#define CSEINPROGRESS EINPROGRESS	// Fires when a blocking operation is in progress because only one blocking operation can run per task/thread
#define CSEALREADY EALREADY			// Fires when a nonblocking socket calls an operation while running another operation
#define CSECONNRESET ECONNRESET		// Fires when the Server Socket forcibly closes the connection
#define CSECONNREFUSED ECONNREFUSED // Fires when the target computer actively refuses it. This usually happens when attempting to connect to a target with no Server Socket

#define CSERROR errno
#endif // _WIN32

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

#endif // __CROSS_SOCKET_UTILS_H
