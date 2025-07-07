#include "CrossSocket.h"

namespace CrossSocket
{
	Socket::Socket()
	{
		if (CS_Utils::Initialize())
		{
			mSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (mSocket == INVALID_SOCKET)
			{
				CS_Utils::Cleanup();
				throw std::runtime_error("Socket creation failed");
			}
		}
		else
		{
			std::cerr << "Winsock2 not initialized" << std::endl;
			mSocket = NULL;
			throw std::runtime_error("Winsock2 not initialized");
		}
	}

	Socket::Socket(socket_t existingSocket)
	{
		if (CS_Utils::Initialize())
		{
			mSocket = existingSocket;
		}
		else
		{
			std::cerr << "Winsock2 not initialized" << std::endl;
			mSocket = NULL;
			throw std::runtime_error("Winsock2 not initialized");
		}
	}

	Socket::~Socket()
	{
		close_socket(mSocket);
	}

	void Socket::Error(std::string message) const
	{
		std::cerr << message << std::endl;
		close_socket(mSocket);
		CS_Utils::Cleanup();
		throw std::runtime_error(message);
	}

	void Socket::CloseSocket()
	{
		if (mSocket != INVALID_SOCKET)
		{
			close_socket(mSocket);
			mSocket = INVALID_SOCKET;
		}
	}

	void Socket::SetNonBlockingMode(bool enable)
	{
#ifdef _WIN32
		u_long mode = enable ? 1 : 0;
		if (ioctlsocket(mSocket, FIONBIO, &mode) != 0)
		{
			Error("Failed to set non-blocking mode");
		}
#else
		int flags = fcntl(mSocket, F_GETFL, 0);
		if (flags == -1)
		{
			Error("fcntl(F_GETFL) failed");
		}

		if (enable)
		{
			flags |= O_NONBLOCK;
		}
		else
		{
			flags &= ~O_NONBLOCK;
		}

		if (fcntl(mSocket, F_SETFL, flags) == -1)
		{
			Error("fcntl(F_SETFL) failed");
		}
#endif
	}

	void Socket::ConnectTo(short family, const char* address, u_short port)
	{
		sockaddr_in server{};
		server.sin_family = family;
		server.sin_port = htons(port);

		if (inet_pton(family, address, &server.sin_addr) <= 0)
		{
			throw std::runtime_error("Invalid address");
		}

		if (connect(mSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			int error = errno;
			if (error != EWOULDBLOCK && error != EINPROGRESS && error != EALREADY)
			{
				Error("Connection failed with error " + std::to_string(error));
			}
		}
	}

	void Socket::BindTo(u_short port)
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);

		if (bind(mSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			Error("Bind failed");
		}
	}

	void Socket::Listen(int backlog)
	{
		if (listen(mSocket, backlog) == SOCKET_ERROR)
		{
			Error("Listen failed");
		}
	}

	Socket Socket::AcceptConnection()
	{
		socket_t client = accept(mSocket, nullptr, nullptr);
		if (client == INVALID_SOCKET)
		{
			int error = errno;
			if (error != EWOULDBLOCK)
			{
				Error("Accept failed");
			}
		}
		return Socket(client);
	}

	bool Socket::IsReadyToRead(int timeoutMillis) const
	{
		fd_set readSet{};
		FD_ZERO(&readSet);
		FD_SET(mSocket, &readSet);

		timeval timeout{};
		timeout.tv_sec = timeoutMillis / 1000;
		timeout.tv_usec = (timeoutMillis % 1000) * 1000;

		int result = select(static_cast<int>(mSocket + 1), &readSet, nullptr, nullptr, &timeout);
		if (result < 0)
		{
			throw std::runtime_error("select() failed on read check");
		}
		return FD_ISSET(mSocket, &readSet);
	}

	bool Socket::IsReadyToWrite(int timeoutMillis) const
	{
		fd_set writeSet{};
		FD_ZERO(&writeSet);
		FD_SET(mSocket, &writeSet);

		timeval timeout{};
		timeout.tv_sec = timeoutMillis / 1000;
		timeout.tv_usec = (timeoutMillis % 1000) * 1000;

		int result = select(static_cast<int>(mSocket + 1), nullptr, &writeSet, nullptr, &timeout);
		if (result < 0)
		{
			throw std::runtime_error("select() failed on write check");
		}
		return FD_ISSET(mSocket, &writeSet);
	}

	void Socket::Send(const char* buf, int len, int flags) const
	{
		int total_sent = 0;
		while (total_sent < len)
		{
			int sent = send(mSocket, buf + total_sent, len - total_sent, flags);
			if (sent == SOCKET_ERROR)
			{
				Error("Send failed with error " + std::to_string(errno));
			}
			total_sent += sent;
		}
	}

	void Socket::Send(const char* buf, int len, int flags, const sockaddr* to, int tolen) const
	{
		if (sendto(mSocket, buf, len, flags, to, tolen) == SOCKET_ERROR)
		{
			Error("SendTo failed with error " + std::to_string(errno));
		}
	}

	int Socket::Receive(char* buf, int len, int flags) const
	{
		int bytesReceived = 0;
		while (bytesReceived < len)
		{
			int received = recv(mSocket, buf + bytesReceived, len - bytesReceived, flags);
			if (received == 0)
			{
				// Connection closed
				return bytesReceived;
			}
			else if (received == SOCKET_ERROR)
			{
				int error = errno;
				if (error != EWOULDBLOCK && error != EINPROGRESS && error != EALREADY)
				{
					if (error == ECONNRESET)
					{
						std::cerr << "Connection reset" << std::endl;
					}
					else
					{
						Error("Recv failed with error " + std::to_string(error));
					}
				}
				return bytesReceived;
			}
			bytesReceived += received;
		}
		return bytesReceived;
	}

	int Socket::Receive(char* buf, int len, int flags, sockaddr* from, int* fromlen) const
	{
		int bytesReceived = recvfrom(mSocket, buf, len, flags, from, fromlen);
		if (bytesReceived == SOCKET_ERROR)
		{
			Error("RecvFrom failed with error " + std::to_string(errno));
		}
		return bytesReceived;
	}

	socket_t Socket::GetRawSocket() const
	{
		return mSocket;
	}
}
