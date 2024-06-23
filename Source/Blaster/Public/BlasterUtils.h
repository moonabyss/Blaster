// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class BlasterUtils
{
public:
    template <typename T>
    static T* GetBlasterPlayerComponent(AActor* PlayerPawn)
    {
        if (!PlayerPawn) return nullptr;

        const auto Component = PlayerPawn->GetComponentByClass(T::StaticClass());
        return Cast<T>(Component);
    }
};
