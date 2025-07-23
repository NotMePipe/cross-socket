// NOTE: WHILE THERE ARE FUNCTIONS FOR SENDING AND RECEIVING OVER UDP CONNECTIONS, THIS LIBRARY CURRENTLY DOES NOT SUPPORT CONNECTING TO SOCKETS VIA UDP
#ifndef __SOCKET_H
#define __SOCKET_H

#include <iostream>
#include <string>

#include "CrossSocketUtils.h"

namespace CrossSocket
{
	class Socket
	{
	private:
		socket_t mSocket;

		/**
		 * @brief Send an error message, close the socket, shut down CrossSocket, and throw and exception
		 *
		 * @param message Error message
		 */
		void Error(std::string message);

	public:
		/**
		 * @brief Create a new Socket object
		 */
		Socket();
		/**
		 * @brief Wrap an existing socket into a new Socket object
		 *
		 * @param existingSocket Socket to wrap
		 */
		explicit Socket(socket_t existingSocket);
		/**
		 * @brief Socket destructor
		 */
		~Socket();

		/**
		 * @brief Close the Socket
		 */
		void Close();

		/**
		 * @brief Disable sends and/or receives on the socket
		 *
		 * @param how Flag that determines what operation to disable. 0-RECEIVE, 1-SEND, 2-BOTH (if no value passed, 2)
		 */
		void Shutdown(int how = 2) const;

		/**
		 * @brief Allow the Socket to block the program while connecting, sending, and receiving or to skip code if no data is present in any of those cases (default mode enables code blocking)
		 *
		 * @param enable True to enable nonblocking mode. False to enable code blocking
		 */
		void SetNonblockingMode(bool enable);

		/**
		 * @brief Connect a CLIENT Socket to a SERVER Socket
		 *
		 * @param family Address family, usually AF_INET (IPv4)
		 * @param address IP Address of the server
		 * @param port Port the Server Socket is on
		 */
		void ConnectTo(short family, const char *address, u_short port);
		/**
		 * @brief Bind a SERVER Socket to a port
		 *
		 * @param port Port to bind to
		 */
		void BindTo(u_short port);
		/**
		 * @brief Tell the SERVER Socket to listen for connections
		 *
		 * @param backlog Maximum length of the queue of pending connections (if no value passed, 5)
		 */
		void Listen(int backlog = 5);
		/**
		 * @brief Attempt to accept connection to a SERVER Socket
		 *
		 * @return Connected CLIENT Socket
		 */
		Socket AcceptConnection();

		/**
		 * @brief Check if the Socket is ready to read data
		 *
		 * @param timeoutMillis Timeout for check in milliseconds
		 * @return Socket read readiness
		 */
		bool IsReadyToRead(int timeoutMillis = 0) const;
		/**
		 * @brief Check if the Socket is ready to send data
		 *
		 * @param timeoutMillis Timeout for check in milliseconds
		 * @return Socket write readiness
		 */
		bool IsReadyToWrite(int timeoutMillis = 0) const;

		/**
		 * @brief Send data through a TCP connection
		 *
		 * @param buf Data to send
		 * @param len Size (in bytes) of the data to send
		 * @param flags Sending flags
		 */
		void Send(const char *buf, int len, int flags);
		/**
		 * @brief Send data through a UDP connection
		 *
		 * @param buf Data to send
		 * @param len Size (in bytes) of the data to send
		 * @param flags Sending flags
		 * @param to Destination address
		 * @param tolen Size (in bytes) of destination address
		 */
		void Send(const char *buf, int len, int flags, const sockaddr *to, int tolen);

		/**
		 * @brief Receive data through a TCP connection
		 *
		 * @param buf Destination to store data
		 * @param len Size (in bytes) of the data received
		 * @param flags Receiving flags
		 * @return Data size in bytes
		 */
		int Receive(char *buf, int len, int flags);
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
		int Receive(char *buf, int len, int flags, sockaddr *from, int *fromlen);

		/**
		 * @brief Return the unwrapped socket
		 *
		 * @return Socket in its lowest-level form
		 */
		socket_t GetRawSocket() const;
	};
}

#endif // __SOCKET_H
