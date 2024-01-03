#ifndef MP_CLIENT_H_INCLUDED
#define MP_CLIENT_H_INCLUDED

#include <Winsock2.h>

class MPDispatcher;
class MPSession;

class MPClient
{
    public:
        MPClient(class MPDispatcher *dispatcher);
        ~MPClient() {};

        bool Connect(char *address, uint16_t port);
        bool GetConnected() { return connected; }
        void Send(char *call, char *id, char *data, size_t dataLength);
        //void Request(char *call, char *id, char *message, callback callback);
        void Disconnect();

    private:
        SOCKET connectSocket;
        bool connected;
        MPDispatcher *dispatcher;
        MPSession *session;

        void Listen();
};

#endif