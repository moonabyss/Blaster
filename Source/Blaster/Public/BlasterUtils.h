// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#define BLASTER_LOG_ERROR(CategoryName, Format, ...)                      \
    {                                                                     \
        UE_INTERNAL_LOG_IMPL(CategoryName, Error, Format, ##__VA_ARGS__); \
    }

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
