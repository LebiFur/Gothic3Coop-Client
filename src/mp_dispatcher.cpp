#include "mp_dispatcher.h"

void MPDispatcher::Register(char *call, callback callback)
{
    callbacks[call] = callback;
}

void MPDispatcher::Dispatch(MPClient *client, char *call, char *id, char *data, size_t dataLength)
{
    if (!callbacks.count(call))
    {
        printf("Dispatch call %s not found\n", call);
        return;
    }
    char *message = callbacks[call](client, id, data, dataLength);
}