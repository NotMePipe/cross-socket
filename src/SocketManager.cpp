#include "SocketManager.h"

namespace CrossSocket
{
    SocketManager* SocketManager::sInstance = NULL;

    SocketManager* SocketManager::Instance()
    {
        if (sInstance == NULL)
        {
            sInstance = new SocketManager();
        }
        return sInstance;
    }

    SocketManager::SocketManager()
    {
        CS_Utils::Initialize();
    }

    void SocketManager::Release()
    {
        CS_Utils::Cleanup();
        delete sInstance;
        sInstance = NULL;
    }

    int SocketManager::AddSocket(Socket& socket, bool monitorRead, bool monitorWrite, void (*onRead)(Socket&), void (*onWrite)(Socket&))
    {
        sockets.push_back(WatchedSocket{ &socket, (int)sockets.size(), monitorRead, monitorWrite, onRead, onWrite });
        return sockets.size() - 1;
    }

    void SocketManager::RunOnce(int timeoutMillis)
    {
        fd_set readSet, writeSet;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        socket_t maxFd = 0;

        for (WatchedSocket& ws : sockets)
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

        timeval timeout;
        timeout.tv_sec = timeoutMillis / 1000;
        timeout.tv_usec = (timeoutMillis % 1000) * 1000;

        int result = select(static_cast<int>(maxFd + 1), &readSet, &writeSet, nullptr, &timeout);
        if (result < 0)
        {
            throw std::runtime_error("select() failed in event loop");
        }

        for (WatchedSocket& ws : sockets)
        {
            socket_t s = ws.socket->GetRawSocket();
            if (ws.monitorRead && FD_ISSET(s, &readSet) && ws.onRead)
            {
                ws.onRead(*ws.socket);
            }
            if (ws.monitorWrite && FD_ISSET(s, &writeSet) && ws.onWrite)
            {
                ws.onWrite(*ws.socket);
            }
        }
    }

    void SocketManager::RunLoop(bool* condition)
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

    void SocketManager::CloseSocket(int id)
    {
        sockets[id].socket->CloseSocket();
        for (int i = id; i < sockets.size(); i++)
        {
            sockets[i].id--;
        }
        sockets.erase(sockets.begin() + id);
    }
}
