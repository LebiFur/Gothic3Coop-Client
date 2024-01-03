#ifndef MP_DISPATCHER_H_INCLUDED
#define MP_DISPATCHER_H_INCLUDED

#include <map>

class MPClient;

typedef char* (*callback)(MPClient *client, char *id, char *data, size_t dataLength);

struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
        return std::strcmp(a, b) < 0;
   }
};

class MPDispatcher
{
    public:
        MPDispatcher() {};
        ~MPDispatcher() {};

        void Register(char *call, callback callback);
        void Dispatch(MPClient *client, char *call, char *id, char *data, size_t dataLength);
    
    private:
        std::map<char*, callback, cmp_str> callbacks;
};

#endif