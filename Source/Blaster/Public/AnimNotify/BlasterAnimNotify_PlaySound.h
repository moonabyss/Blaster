// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "CoreMinimal.h"

#include "BlasterAnimNotify_PlaySound.generated.h"

UCLASS(meta = (DisplayName = "Blaster Play Sound"))
class BLASTER_API UBlasterAnimNotify_PlaySound : public UAnimNotify_PlaySound
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    // If this sound should play only on Locally Controlled player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
    uint32 bPlayOnlyLocally : 1;
};
