#ifndef MP_PLAYER_H_INCLUDED
#define MP_PLAYER_H_INCLUDED

#include <Game.h>
#include <Engine.h>
#include <Script.h>

class MPPlayer
{
    public:
        MPPlayer(uint16_t id, char *name);
        ~MPPlayer() {};

        void SetMatrix(bCMatrix *matrix);
        void Delete();
        char* GetName() { return name; }
        void Update(float const &deltaTime);

    private:
        uint16_t id;
        char *name;
        eCEntity *entity;

        bCVector targetPosition, currentPosition;
        bCQuaternion targetRotation, currentRotation;
        float lerpTimer = 0;
};

#endif