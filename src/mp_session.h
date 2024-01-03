#ifndef MP_SESSION_H_INCLUDED
#define MP_SESSION_H_INCLUDED

#include <unordered_map>

#include <Game.h>
#include <Engine.h>
#include <Script.h>

#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_J 0x4A
#define VK_K 0x4B

constexpr size_t DELTA_TIME_SMOOTHING = 60;
constexpr float PLAYER_UPDATE_FREQUENCY = 1.0F / 10.0F;

class MPClient;
class MPPlayer;

class MPSession
{
    public:
        MPSession(MPSession &other) = delete;
        void operator=(const MPSession&) = delete;

        gCSession *session;
        eCWnd *wnd;
        MPClient *client;

        uint16_t id;
        std::unordered_map<uint16_t, MPPlayer*> players;

        bCUnicodeString output[10];
        bool drawOutput = true;

        static MPSession* GetInstance();
        void Update();

    private:
        MPSession();
        ~MPSession() {};

        static MPSession* instance;

        bCString username = "Ubolyr";
        bCString address = "127.0.0.1";

        float pingTimer = 0.0F;
        float updateTimer = 0.0F;

        uint64_t lastTimestamp = 0;
        uint64_t frame = 0;
        float deltaTime = 0.0F;
        float deltaTimes[DELTA_TIME_SMOOTHING];
        float smoothDeltaTime = 0.0F;

        const char keys[6] { VK_F1, VK_F, VK_G, VK_H, VK_J, VK_K };
        std::unordered_map<char, bool> keyLastState;
        std::unordered_map<char, bool> keyState;
};

#endif