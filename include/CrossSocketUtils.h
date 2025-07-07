#ifndef __CROSS_SOCKET_UTILS_H
#define __CROSS_SOCKET_UTILS_H

#include <stdexcept>
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using socket_t = SOCKET;

#undef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#undef EALREADY
#define EALREADY WSAEALREADY
#undef ECONNRESET
#define ECONNRESET WSAECONNRESET
#undef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED

#undef errno
#define errno WSAGetLastError()
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using socket_t = int;

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
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
