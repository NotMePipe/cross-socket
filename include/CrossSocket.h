#ifndef __CROSS_SOCKET_H
#define __CROSS_SOCKET_H

#include <iostream>
#include <string>

#include "CrossSocketUtils.h"

namespace CrossSocket
{
	class Socket
	{
	private:
		socket_t mSocket;

		void Error(std::string message) const;

	public:
		Socket();
		explicit Socket(socket_t existingSocket);
		~Socket();

		void CloseSocket();

		void SetNonBlockingMode(bool enable);

		void ConnectTo(short family, const char* address, u_short port);
		void BindTo(u_short port);
		void Listen(int backlog = 5);
		Socket AcceptConnection();

		bool IsReadyToRead(int timeoutMillis = 0) const;
		bool IsReadyToWrite(int timeoutMillis = 0) const;

		void Send(const char* buf, int len, int flags) const;
		void Send(const char* buf, int len, int flags, const sockaddr* to, int tolen) const;

		int Receive(char* buf, int len, int flags) const;
		int Receive(char* buf, int len, int flags, sockaddr* from, int* fromlen) const;

		socket_t GetRawSocket() const;
	};
}

#endif
