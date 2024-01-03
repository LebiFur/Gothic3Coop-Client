#include <chrono>

#include <util/Memory.h>
#include <util/Hook.h>

#include "mp_session.h"
#include "mp_client.h"
#include "mp_dispatcher.h"
#include "mp_player.h"
#include "mp_output.h"

MPSession* MPSession::instance = 0;

template<mCFunctionHook &Hook>
void GE_STDCALL UpdateHook()
{
    MPSession::GetInstance()->Update();
    Hook.GetOriginalFunction(UpdateHook<Hook>)();
}
mCFunctionHook Hook_Update;

char* Ping(MPClient *client, char *id, char *data, size_t dataLength)
{
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    MPSession::GetInstance()->output[2] = bCUnicodeString(Format("Latency: %dms", timestamp - *reinterpret_cast<uint64_t*>(data)));
    return 0;
}

char* Start(MPClient *client, char *id, char *data, size_t dataLength)
{
    uint16_t myId;

    memcpy(&myId, data, sizeof(uint16_t));
    size_t offset = sizeof(uint16_t);

    MPSession::GetInstance()->id = myId;
    Print(Format("My id: %d\n", myId));
    if (dataLength == sizeof(uint16_t)) Print("Currently connected players:\n");

    while (offset < dataLength)
    {
        uint16_t playerId;
        char nameLength = 0;
        char *name;

        memcpy(&playerId, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        memcpy(&nameLength, data + offset, sizeof(char));
        offset += sizeof(char);

        name = new char[nameLength];
        memcpy(name, data + offset, nameLength);
        offset += nameLength;

        Print(Format(" - %s | %d\n", name, playerId));

        MPPlayer *player = new MPPlayer(playerId, name);
        MPSession::GetInstance()->players[playerId] = player;
    }

    return 0;
}

char* Connect(MPClient *client, char *id, char *data, size_t dataLength)
{
    uint16_t playerId;
    char nameLength = 0;
    char *name;

    memcpy(&playerId, data, sizeof(uint16_t));
    memcpy(&nameLength, data + sizeof(uint16_t), sizeof(char));

    name = new char[nameLength];
    memcpy(name, data + sizeof(uint16_t) + sizeof(char), nameLength);

    Print(Format("%s | %d connected\n", name, playerId));

    MPPlayer *player = new MPPlayer(playerId, name);
    MPSession::GetInstance()->players[playerId] = player;

    return 0;
}

char* Disconnect(MPClient *client, char *id, char *data, size_t dataLength)
{
    uint16_t playerId = *(reinterpret_cast<uint16_t*>(data));

    Print(Format("%s | %d disconnected\n", MPSession::GetInstance()->players[playerId]->GetName(), playerId));

    MPSession::GetInstance()->players[playerId]->Delete();
    MPSession::GetInstance()->players.erase(playerId);

    return 0;
}

char* UpdatePlayer(MPClient *client, char *id, char *data, size_t dataLength)
{
    uint16_t playerId = *(reinterpret_cast<uint16_t*>(data));
    MPSession::GetInstance()->players[playerId]->SetMatrix(reinterpret_cast<bCMatrix*>(data + sizeof(uint16_t)));

    return 0;
}

MPSession::MPSession()
{
    instance = this;

    Print("Started\n");

    eCConfigFile config = eCConfigFile();
    if(config.ReadFile(bCString("online.ini")))
    {
        username = config.GetString(bCString("Online"), bCString("Username"), username);
        address = config.GetString(bCString("Online"), bCString("Address"), address);
    }

    Print(Format("Server address: %s\n", (char*)address.GetText()));
    Print(Format("Username: %s\n", (char*)username.GetText()));

    Hook_Update.Hook(RVA_Engine(0x15d90), UpdateHook<Hook_Update>, mCBaseHook::mEHookType_ThisCall);

    wnd = eCWnd::GetDesktopWindow();
    session = &gCSession::AccessSession();

    MPDispatcher *dispatcher = new MPDispatcher();
    dispatcher->Register("resping", Ping);
    dispatcher->Register("resstart", Start);
    dispatcher->Register("update", UpdatePlayer);
    dispatcher->Register("connect", Connect);
    dispatcher->Register("disconnect", Disconnect);

    client = new MPClient(dispatcher);

    output[0] = bCUnicodeString("Gothic 3 Online v0.0.1");
    output[9] = bCUnicodeString("Domo rabyca percyumo");
}

MPSession* MPSession::GetInstance()
{
    if (instance == 0) instance = new MPSession();
    return instance;
}

void MPSession::Update()
{
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    deltaTime = (float)(timestamp - lastTimestamp) / 1000.0F;
    deltaTimes[frame % DELTA_TIME_SMOOTHING] = deltaTime;
    float sum = 0;
    for (float &num : deltaTimes) sum += num;
    smoothDeltaTime = sum / (float)DELTA_TIME_SMOOTHING;
    lastTimestamp = timestamp;

    for (const char key : keys) keyState[key] = GetAsyncKeyState(key) != 0;

    for (const std::pair<char, bool> &pair : keyState)
    {
        if (pair.second != keyLastState[pair.first])
        {
            if (pair.second)
            {
                switch (pair.first)
                {
                    case VK_F1:
                        drawOutput = !drawOutput;
                        break;
                    case VK_F:
                        if (session->IsGameRunning())
                        {
                            if (!client->Connect((char*)address.GetText(), 21370)) break;
                            
                            client->Send("reqstart", "0", (char*)username.GetText(), username.GetLength() + 1);
                        }
                        else Print("You must be in game to connect\n");
                        break;
                    case VK_G:
                        client->Send("jajca", "0", 0, 0);
                        break;
                    case VK_H:
                        client->Disconnect();
                        break;
                }
            }
            keyLastState[pair.first] = pair.second;
        }
    }

    for (std::pair<uint16_t, MPPlayer*> player : players)
    {
        player.second->Update(smoothDeltaTime);
    }

    if (pingTimer < 1) pingTimer += smoothDeltaTime;
    else if (client->GetConnected())
    {
        client->Send("reqping", Format("%d", frame), reinterpret_cast<char*>(&timestamp), 8);
        pingTimer = 0;
    }

    if (updateTimer < PLAYER_UPDATE_FREQUENCY) updateTimer += smoothDeltaTime;
    else
    {
        Print(Format("%d | %s\n", id, client->GetConnected() ? "true" : "false"));

        if (client->GetConnected())
        {
            bCMatrix const &matrix = MPSession::GetInstance()->session->GetHero()->GetWorldMatrix();
            client->Send("update", "0", (char*)&matrix, sizeof(bCMatrix));
            updateTimer = 0;
        }
    }

    output[1] = bCUnicodeString(Format("FPS: %d", (size_t)(1.0F / smoothDeltaTime)));
    output[5] = bCUnicodeString(Format("Is in test mode: %s", session->IsInTestMode() ? "true" : "false"));

    if (drawOutput)
    {
        bCByteAlphaColor black = bCByteAlphaColor(0, 0, 0, 255);

        for (size_t i = 0; i < 10; i++)
        {
            GEI32 x = 4;
            GEI32 y = (i * 16) + 4;
            float value = 1.0F - ((float)i / 9.0F);

            wnd->Print(bCPoint(x, y), output[i], &bCByteAlphaColor(255, (GEU8)(Lerp(255.0F, 128.0F, value)), (GEU8)(Lerp(255.0F, 0.0F, value)), 255), (eEWinTextMode)1);

            wnd->Print(bCPoint(x, y + 2), output[i], &black, (eEWinTextMode)1);
            wnd->Print(bCPoint(x + 2, y), output[i], &black, (eEWinTextMode)1);
            wnd->Print(bCPoint(x, y - 2), output[i], &black, (eEWinTextMode)1);
            wnd->Print(bCPoint(x - 2, y), output[i], &black, (eEWinTextMode)1);

            wnd->Print(bCPoint(x + 1, y - 1), output[i], &black, (eEWinTextMode)1);
            wnd->Print(bCPoint(x + 1, y + 1), output[i], &black, (eEWinTextMode)1);
            wnd->Print(bCPoint(x - 1, y + 1), output[i], &black, (eEWinTextMode)1);
            wnd->Print(bCPoint(x - 1, y - 1), output[i], &black, (eEWinTextMode)1);
        }
    }

    frame++;
}