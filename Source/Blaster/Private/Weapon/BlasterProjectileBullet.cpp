// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileBullet.h"
#include "Kismet/GameplayStatics.h"

#include "Interfaces/Hitable.h"

void ABlasterProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (IsValid(OtherActor) && OtherActor->Implements<UHitable>())
    {
        auto Victim = Cast<IHitable>(OtherActor);
        Victim->HitByProjectile();
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
    }

    Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
