#include "CrossSocket/SocketManager.h"

namespace CrossSocket
{
    SocketManager *SocketManager::sInstance = nullptr;

    /**
     * @brief Get SocketManager Singleton object
     *
     * @return SocketManager Singleton
     */
    SocketManager *SocketManager::Instance()
    {
        if (sInstance == nullptr)
        {
            sInstance = new SocketManager();
        }
        return sInstance;
    }

    /**
     * @brief Free memory used by the Singleton
     */
    void SocketManager::Release()
    {
        CS_Utils::Cleanup();
        delete sInstance;
        sInstance = nullptr;
    }

    /**
     * @brief SocketManager initialization. Private in order to ensure Singleton
     */
    SocketManager::SocketManager()
    {
        CS_Utils::Initialize();
    }

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
    int SocketManager::AddSocket(Socket &socket, bool monitorRead, bool monitorWrite, void (*onRead)(Socket &), void (*onWrite)(Socket &))
    {
        sockets.push_back(WatchedSocket{&socket, (int)sockets.size(), monitorRead, monitorWrite, onRead, onWrite});
        return static_cast<int>(sockets.size()) - 1;
    }

    /**
     * @brief Check all watched Sockets for updates
     *
     * @param timeoutMillis Timeout for check in milliseconds
     */
    void SocketManager::RunOnce(int timeoutMillis)
    {
        fd_set readSet{}, writeSet{};
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        socket_t maxFd = 0;

        for (WatchedSocket &ws : sockets)
        {
            socket_t s = ws.socket->GetRawSocket();
            if (ws.monitorRead)
            {
                FD_SET(s, &readSet);
            }
            if (ws.monitorWrite)
            {
                FD_SET(s, &writeSet);
            }
            if (s > maxFd)
            {
                maxFd = s;
            }
        }

        timeval timeout{};
        timeout.tv_sec = timeoutMillis / 1000;
        timeout.tv_usec = (timeoutMillis % 1000) * 1000;

        int result = select(static_cast<int>(maxFd + 1), &readSet, &writeSet, nullptr, &timeout);
        if (result == SOCKET_ERROR)
        {
            throw std::runtime_error("select() failed in event loop" + std::to_string(CSERROR));
        }

        for (WatchedSocket &ws : sockets) // Handle event callbacks
        {
            socket_t s = ws.socket->GetRawSocket();
            if (ws.monitorRead && FD_ISSET(s, &readSet) && ws.onRead)
            {
                ws.onRead(*ws.socket); // Run the onRead callback
            }
            if (ws.monitorWrite && FD_ISSET(s, &writeSet) && ws.onWrite)
            {
                ws.onWrite(*ws.socket);
            }
        }
    }

    /**
     * @brief Continuously check all watched Sockets for updates
     *
     * @param condition Reference to a boolean value which controls the event loop. When the value is false, the loop will stop. If no pointer is passed, the loop with continue infintely
     */
    void SocketManager::RunLoop(bool *condition)
    {
        if (condition == nullptr)
        {
            while (true)
            {
                RunOnce();
            }
        }
        else
        {
            while (*condition)
            {
                RunOnce();
            }
        }
    }

    /**
     * @brief Remove a socket from the event loop
     *
     * @param id Socket ID to remove
     */
    void SocketManager::CloseSocket(int id)
    {
        sockets[id].socket->Close();
        for (int i = id; i < sockets.size(); ++i)
        {
            --sockets[i].id;
        }
        sockets.erase(sockets.begin() + id);
    }

    /**
     * @brief Close all sockets in event loop
     */
    void SocketManager::CloseSockets()
    {
        for (WatchedSocket &ws : sockets)
        {
            ws.socket->Close();
        }
        sockets.clear();
        sockets.resize(0);
    }
}
