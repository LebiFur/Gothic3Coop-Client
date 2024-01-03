#include <thread>

#include "mp_client.h"
#include "mp_session.h"
#include "mp_dispatcher.h"
#include "mp_output.h"

MPClient::MPClient(MPDispatcher *dispatcher) : dispatcher(dispatcher), connected(false)
{
    session = MPSession::GetInstance();
}

void MPClient::Listen()
{
    if (!connected) return;

    while (true)
    {
        char buffer[512];
        int length = recv(connectSocket, buffer, 512, 0);
        if (length <= 0) break;

        char call[20];
        char id[20];
        char dataBuffer[512];
        char *data = 0;
        char *current = call;
        size_t j = 0;

        for (size_t i = 0; i < length; i++)
        {
            if (buffer[i] == '-' && current != dataBuffer)
            {
                if (current == call)
                {
                    current[i] = 0;
                    current = id;
                }
                else if (current == id)
                {
                    current[i - j] = 0;
                    current = dataBuffer;
                }

                j = i + 1;
                continue;
            }

            current[i - j] = buffer[i];
        }

        size_t dataLength = length - j;
        if (dataLength > 0)
        {
            data = new char[dataLength];
            memcpy(data, dataBuffer, dataLength);
        }

        if (strcmp(call, "resping") != 0) session->output[4] = Format("Last received: %s-%s | %dB\n", call, id, dataLength);

        dispatcher->Dispatch(this, call, id, data, dataLength);
    }

    if (connected) Disconnect();
}

bool MPClient::Connect(char* address, uint16_t port)
{
    if (connected) return false;

    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        Print(Format("Error at WSAStartup with error: %d\n", iResult));
        return false;
    }
    
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        Print(Format("Socket failed with error: %u\n", WSAGetLastError()));
        WSACleanup();
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_port = htons(port);

    iResult = connect(connectSocket, (SOCKADDR*)&addr, sizeof(addr));
    if (iResult == SOCKET_ERROR)
    {
        Print(Format("Connect failed with error: %u\n", WSAGetLastError()));
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }

    std::thread(&MPClient::Listen, this).detach();

    connected = true;

    Print(Format("Connected to %s:%d\n", address, port));

    return true;
}

void MPClient::Send(char *call, char *id, char *data, size_t dataLength)
{
    if (!connected) return;

    char *first = Format("%s-%s-", call, id);
    size_t firstLength = strlen(first);

    size_t messageLength = firstLength + dataLength;
    char *message = new char[messageLength];

    memcpy(message, first, firstLength);
    if (dataLength > 0 || data == 0) memcpy(message + firstLength, data, dataLength);

    send(connectSocket, message, messageLength, 0);

    if (strcmp(call, "reqping") != 0) session->output[3] = Format("Last sent: %.*s | %dB\n", firstLength - 1, first, dataLength);
}

/*
void MPClient::Request(char *call, char *id, char *message, callback callback)
{
    if (!connected) return;

    char *msg = Format("%s-%s", call, id);
    requests[msg] = callback;
    if (message != 0) send(connectSocket, Format("req%s-%s", msg, message), strlen(msg) + strlen(message) + 4, 0);
    else send(connectSocket, Format("req%s", msg), strlen(msg) + 3, 0);
    Print(Format("Sent request: %s\n", msg));
}
*/

void MPClient::Disconnect()
{
    if (!connected) return;
    
    closesocket(connectSocket);
    WSACleanup();

    connected = false;

    Print("Disconnected\n");
}