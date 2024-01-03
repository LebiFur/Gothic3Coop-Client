#include "mp_player.h"
#include "mp_session.h"
#include "mp_output.h"

MPPlayer::MPPlayer(uint16_t id, char *name) : id(id), name(name)
{
    const bCVector &pos = MPSession::GetInstance()->session->GetHero()->GetWorldPosition();

    entity = MPSession::GetInstance()->session->SpawnEntity(bCString("PC_Hero"), pos, false);
    entity->SetName(bCString(name));
    entity->RemovePropertySetAt(entity->GetPropertySetIndex(entity->GetPropertySet(eEPropertySetType_CharacterControl)));
    entity->RemovePropertySetAt(entity->GetPropertySetIndex(entity->GetPropertySet(eEPropertySetType_CharacterMovement)));
    //reinterpret_cast<gCCharacterMovement_PS*>(entity->GetPropertySet(eEPropertySetType_CharacterMovement))->SetApplyGravity(false);
    //reinterpret_cast<gCNPC_PS*>(entity->GetPropertySet(eEPropertySetType_CharacterMovement))->SetCurrentTargetEntity(eCEntityProxy(entity));
}

void MPPlayer::SetMatrix(bCMatrix *matrix)
{
    //entity->SetToWorldMatrix(*matrix);
    currentPosition = entity->GetWorldPosition();
    currentRotation = entity->GetWorldRotation();
    targetPosition = matrix->GetTranslation();
    targetRotation = bCQuaternion(matrix->GetRotation());
    lerpTimer = 0;
}

void MPPlayer::Update(float const &deltaTime)
{
    float value = lerpTimer / PLAYER_UPDATE_FREQUENCY;
    entity->SetToWorldMatrix(bCMatrix(currentRotation.GetSlerp(targetRotation, value), currentPosition.GetLerp(targetPosition, value)));
    lerpTimer += deltaTime;
    if (lerpTimer > PLAYER_UPDATE_FREQUENCY) lerpTimer = PLAYER_UPDATE_FREQUENCY;
}

void MPPlayer::Delete()
{
    entity->Kill();
    //entity->RemoveFromParent();
}