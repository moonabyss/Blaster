// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterProjectileMoveComponent.h"

UProjectileMovementComponent::EHandleBlockingHitResult UBlasterProjectileMoveComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
    Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
    return UProjectileMovementComponent::EHandleBlockingHitResult::AdvanceNextSubstep;
}

void UBlasterProjectileMoveComponent::HandleImpact(const FHitResult& Hit, float TimeSlice /*= 0.f*/, const FVector& MoveDelta /*= FVector::ZeroVector*/)
{
    // Projectiles should not stop; only explode when their CollisionBox detects a hit.
}
