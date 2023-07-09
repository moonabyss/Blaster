// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#define BLASTER_LOG_ERROR(CategoryName, Format, ...)                          \
    {                                                                         \
        if (!UE_BUILD_SHIPPING)                                               \
        {                                                                     \
            UE_INTERNAL_LOG_IMPL(CategoryName, Error, Format, ##__VA_ARGS__); \
        }                                                                     \
    }

namespace Blaster
{

}
