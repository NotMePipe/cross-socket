#ifndef __SOCKET_MANAGER_H
#define __SOCKET_MANAGER_H

#include "Socket.h"
#include "CrossSocketUtils.h"

#include <vector>

namespace CrossSocket
{
    class SocketManager
    {
    public:
        /**
        * @brief Get SocketManager Singleton object
        *
        * @return SocketManager Singleton
        */
        static SocketManager* Instance();
        /**
        * @brief Free memory used by the Singleton
        */
        void Release();

        /**
        * @brief Add a Socket to the SocketManager event loop
        *
        * @param socket Socket to add
        * @param monitorRead Boolean to enable listening for data receiving
        * @param monitorWrite Boolean to enable listening for data sending
        * @param onRead Function pointer to callback upon data receiving (must take Socket& as only parameter)
        * @param onWrite Function pointer to callback upon data sending (must take Socket& as only parameter)
        * @return Socket ID in vector
        */
        int AddSocket(Socket& socket, bool monitorRead, bool monitorWrite, void (*onRead)(Socket&) = nullptr, void (*onWrite)(Socket&) = nullptr);
        
        /**
        * @brief Check all watched Sockets for updates
        *
        * @param timeoutMillis Timeout for check in milliseconds
        */
        void RunOnce(int timeoutMillis = 1000);

        /**
        * @brief Continuously check all watched Sockets for updates
        *
        * @param condition Reference to a boolean value which controls the event loop. When the value is false, the loop will stop. If no pointer is passed, the loop with continue infintely
        */
        void RunLoop(bool* condition = nullptr);

        /**
        * @brief Remove a socket from the event loop
        *
        * @param id Socket ID to remove
        */
        void CloseSocket(int id);

        /**
        * @brief Close all sockets in event loop
        */
        void CloseSockets();

    private:
        static SocketManager* sInstance;

        /**
        * @brief SocketManager initialization. Private in order to ensure Singleton
        */
        SocketManager();
        /**
        * @brief Destructor
        */
        ~SocketManager() {};

        struct WatchedSocket
        {
            Socket* socket;
            int id;
            bool monitorRead;
            bool monitorWrite;
            void (*onRead)(Socket&);
            void (*onWrite)(Socket&);
        };

        std::vector<WatchedSocket> sockets;
    };
}

#endif
