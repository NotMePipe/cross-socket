#ifndef __CROSS_SOCKET_UTILS_H
#define __CROSS_SOCKET_UTILS_H

#include <stdexcept>
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>

using socket_t = SOCKET;

#undef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#undef EALREADY
#define EALREADY WSAEALREADY
#undef ECONNRESET
#define ECONNRESET WSAECONNRESET

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
	inline void close_socket(socket_t socket)
	{
#ifdef _WIN32
		closesocket(socket);
#else
		close(socket);
#endif
	}

	class CS_Utils
	{
	private:
		static bool initialized;

	public:
		static bool Initialize();

		static void Cleanup();
	};
}

#endif
