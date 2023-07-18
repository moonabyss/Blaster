// Blaster Multiplayer Game. All rights reserved.

#include "AnimNotify/BlasterAnimNotify_PlaySound.h"
#include "Kismet/GameplayStatics.h"

void UBlasterAnimNotify_PlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    // Don't call super to avoid call back in to blueprints
    if (Sound && MeshComp)
    {
        if (!Sound->IsOneShot())
        {
            UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to play a sound asset which is not a one-shot: '%s'. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(Sound));
            return;
        }

#if WITH_EDITORONLY_DATA
        UWorld* World = MeshComp->GetWorld();
        if (bPreviewIgnoreAttenuation && World && World->WorldType == EWorldType::EditorPreview)
        {
            UGameplayStatics::PlaySound2D(World, Sound, VolumeMultiplier, PitchMultiplier);
        }
        else
#endif
        {
            if (bPlayOnlyLocally)
            {
                if (APawn* Pawn = Cast<APawn>(MeshComp->GetOwner()))
                {
                    if (Pawn->IsLocallyControlled())
                    {
                        UGameplayStatics::PlaySound2D(MeshComp->GetWorld(), Sound, VolumeMultiplier, PitchMultiplier);
                    }
                }
            }
            else
            {
                if (bFollow)
                {
                    UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
                }
                else
                {
                    UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), Sound, MeshComp->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
                }
            }
        }
    }
}
