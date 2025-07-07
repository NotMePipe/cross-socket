#ifndef __SOCKET_MANAGER_H
#define __SOCKET_MANAGER_H

#include "CrossSocket.h"
#include "CrossSocketUtils.h"

#include <vector>

namespace CrossSocket
{
    class SocketManager
    {
    public:
        static SocketManager* Instance();

        int AddSocket(Socket& socket, bool monitorRead, bool monitorWrite, void (*onRead)(Socket&) = nullptr, void (*onWrite)(Socket&) = nullptr);

        void RunOnce(int timeoutMillis = 1000);

        void RunLoop(bool* condition = nullptr);

        void CloseSocket(int id);

        void Release();

    private:
        static SocketManager* sInstance;

        SocketManager();
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
