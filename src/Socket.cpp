#include "Socket.h"

namespace CrossSocket
{
	/**
	 * @brief Create a new Socket object
	 */
	Socket::Socket()
	{
		if (CS_Utils::Initialize()) // If CrossSocket has not been initialized, attempt to initialize it
		{
			mSocket = socket(AF_INET, SOCK_STREAM, 0); // Create an IPv4 TCP socket
			if (mSocket == INVALID_SOCKET)			   // If there is an socket creation error, shut down CrossSocket and error
			{
				CS_Utils::Cleanup();
				throw std::runtime_error("Socket creation failed");
			}
		}
		else // If there was an initialization failure, error
		{
			std::cerr << "Winsock not initialized" << std::endl; // Specifying Winsock because the CrossSocket does not require initialization on Unix machines
			mSocket = 0;
			throw std::runtime_error("Winsock not initialized");
		}
	}

	/**
	 * @brief Wrap an existing socket into a new Socket object
	 *
	 * @param existingSocket Socket to wrap
	 */
	Socket::Socket(socket_t existingSocket)
	{
		if (CS_Utils::Initialize()) // If CrossSocket has not been initialized, attempt to initialize it
		{
			mSocket = existingSocket;
		}
		else // If there was an initialization failure, error
		{
			std::cerr << "Winsock not initialized" << std::endl; // Specifying Winsock because the CrossSocket does not require initialization on Unix machines
			mSocket = 0;
			throw std::runtime_error("Winsock not initialized");
		}
	}

	/**
	 * @brief Socket destructor
	 */
	Socket::~Socket()
	{
		Close(); // Close the socket if it hasn't already been closed (it is bad practice to rely on this)
	}

	/**
	 * @brief Send an error message, close the socket, shut down CrossSocket, and throw and exception
	 *
	 * @param message Error message
	 */
	void Socket::Error(std::string message)
	{
		std::cerr << message << std::endl;
		Close();
		CS_Utils::Cleanup();
		throw std::runtime_error(message);
	}

	/**
	 * @brief Close the Socket
	 */
	void Socket::Close()
	{
		if (mSocket != INVALID_SOCKET)
		{
			Shutdown();
			CS_Utils::close_socket(mSocket);
			mSocket = INVALID_SOCKET;
		}
	}

	/**
	 * @brief Disable sends and/or receives on the socket
	 *
	 * @param how Flag that determines what operation to disable. 0-RECEIVE, 1-SEND, 2-BOTH (if no value passed, 2)
	 */
	void Socket::Shutdown(int how) const
	{
		if (mSocket != INVALID_SOCKET)
		{
			shutdown(mSocket, how);
		}
	}

	/**
	 * @brief Allow the Socket to block the program while connecting, sending, and receiving or to skip code if no data is present in any of those cases (default mode enables code blocking)
	 *
	 * @param enable True to enable nonblocking mode. False to enable code blocking
	 */
	void Socket::SetNonblockingMode(bool enable)
	{
#ifdef _WIN32 // Windows functionality
		u_long mode = enable ? 1 : 0;
		if (ioctlsocket(mSocket, FIONBIO, &mode) != 0)
		{
			Error("Failed to set non-blocking mode");
		}
#else // Unix functionality
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

	/**
	 * @brief Connect a CLIENT Socket to a SERVER Socket
	 *
	 * @param family Address family, usually AF_INET (IPv4)
	 * @param address IP Address of the server
	 * @param port Port the Server Socket is on
	 */
	void Socket::ConnectTo(short family, const char *address, u_short port)
	{
		sockaddr_in server{};
		server.sin_family = family;
		server.sin_port = htons(port);

		if (inet_pton(family, address, &server.sin_addr) <= 0) // If the parameters do not match up to a valid address
		{
			throw std::runtime_error("Invalid address");
		}

		if (connect(mSocket, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) // If there is an error connecting to the server
		{
			int error = errno;
			// If the error code matches any of these three errors, we can ignore the error and continue the program as normal
			if (error != EWOULDBLOCK && error != EINPROGRESS && error != EALREADY)
			{
				// If the connection refuses, the server is likely offline. Rather than erroring, just continue the program as normal
				// NOTE: In this applicaiton, continuing as normal means trying to connect again
				// This is technically a risky/bad practice, so it has its own if block for easy removability
				if (error == ECONNREFUSED)
				{
					std::cout << "Connection refused. Retrying..." << std::endl;
				}
				else // If the error is not ignored, error
				{
					Error("Connection failed with error " + std::to_string(error));
				}
			}
		}
	}

	/**
	 * @brief Bind a SERVER Socket to a port
	 *
	 * @param port Port to bind to
	 */
	void Socket::BindTo(u_short port)
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);

		if (bind(mSocket, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) // If there was a failure binding, error
		{
			Error("Bind failed");
		}
	}

	/**
	 * @brief Tell the SERVER Socket to listen for connections
	 *
	 * @param backlog Maximum length of the queue of pending connections (if no value passed, 5)
	 */
	void Socket::Listen(int backlog)
	{
		if (listen(mSocket, backlog) == SOCKET_ERROR) // If there was a failure listening, error
		{
			Error("Listen failed");
		}
	}

	/**
	 * @brief Attempt to accept connection to a SERVER Socket
	 *
	 * @return Connected CLIENT Socket
	 */
	Socket Socket::AcceptConnection()
	{
		socket_t client = accept(mSocket, nullptr, nullptr); // Attempt to accept a socket from any address
		if (client == INVALID_SOCKET)						 // If there is an error while accepting...
		{
			int error = errno;
			if (error != EWOULDBLOCK) // ...and the error code is not ignorable, error
			{
				Error("Accept failed");
			}
		}
		return Socket(client);
	}

	/**
	 * @brief Check if the Socket is ready to read data
	 *
	 * @param timeoutMillis Timeout for check in milliseconds
	 * @return Socket read readiness
	 */
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

	/**
	 * @brief Check if the Socket is ready to send data
	 *
	 * @param timeoutMillis Timeout for check in milliseconds
	 * @return Socket write readiness
	 */
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

	/**
	 * @brief Send data through a TCP connection
	 *
	 * @param buf Data to send
	 * @param len Size (in bytes) of the data to send
	 * @param flags Sending flags
	 */
	void Socket::Send(const char *buf, int len, int flags)
	{
		int total_sent = 0;
		while (total_sent < len) // TCP doesn't always send all the data at once, so continue trying to send data until it is all sent
		{
			int sent = send(mSocket, buf + total_sent, len - total_sent, flags);
			if (sent == SOCKET_ERROR)
			{
				Error("Send failed with error " + std::to_string(errno));
			}
			total_sent += sent;
		}
	}

	/**
	 * @brief Send data through a UDP connection
	 *
	 * @param buf Data to send
	 * @param len Size (in bytes) of the data to send
	 * @param flags Sending flags
	 * @param to Destination address
	 * @param tolen Size (in bytes) of destination address
	 */
	void Socket::Send(const char *buf, int len, int flags, const sockaddr *to, int tolen)
	{
		if (sendto(mSocket, buf, len, flags, to, tolen) == SOCKET_ERROR)
		{
			Error("SendTo failed with error " + std::to_string(errno));
		}
	}

	/**
	 * @brief Receive data through a TCP connection
	 *
	 * @param buf Destination to store data
	 * @param len Size (in bytes) of the data received
	 * @param flags Receiving flags
	 * @return Data size in bytes
	 */
	int Socket::Receive(char *buf, int len, int flags)
	{
		int bytesReceived = 0;
		while (bytesReceived < len) // TCP doesn't always receive all the data at once, so continue trying to receive data until it all arrives
		{
			int received = recv(mSocket, buf + bytesReceived, len - bytesReceived, flags);
			if (received == 0) // If nothing is received, but there is no error
			{
				// Connection closed
				return bytesReceived;
			}
			else if (received == SOCKET_ERROR) // If there is an error while receiving...
			{
				int error = errno;
				if (error != EWOULDBLOCK && error != EINPROGRESS && error != EALREADY) // ...and the error cannot be ignored
				{
					if (error == ECONNRESET) // This error usually doesn't result in a problem, but there will not be any more data, so stop receiving
					{
						std::cerr << "Connection reset" << std::endl;
					}
					else // Otherwise, error
					{
						Error("Recv failed with error " + std::to_string(error));
					}
				}
				return bytesReceived; // Stop receiving data regardless of the error
			}
			bytesReceived += received;
		}
		return bytesReceived;
	}

	/**
	 * @brief Receive data through a UDP connection
	 *
	 * @param buf Destination to store data
	 * @param len Size (in bytes) of the data received
	 * @param flags Receiving flags
	 * @param from Source address
	 * @param fromlen Size (in bytes) of the source address
	 * @return Data size in bytes
	 */
	int Socket::Receive(char *buf, int len, int flags, sockaddr *from, int *fromlen)
	{
		int bytesReceived = recvfrom(mSocket, buf, len, flags, from, fromlen);
		if (bytesReceived == SOCKET_ERROR)
		{
			Error("RecvFrom failed with error " + std::to_string(errno));
		}
		return bytesReceived;
	}

	/**
	 * @brief Return the unwrapped socket
	 *
	 * @return Socket in its lowest-level form
	 */
	socket_t Socket::GetRawSocket() const
	{
		return mSocket;
	}
}
