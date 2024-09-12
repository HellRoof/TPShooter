// Fill out your copyright notice in the Description page of Project Settings.


#include "FTaleProjectile.h"

#include "FTaleTestProject/Characters/FTaleTestProjectCharacter.h"
#include "FTaleTestProject/Components/FTaleAttributeComponent.h"

// Sets default values
AFTaleProjectile::AFTaleProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileSMComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

}

// Called when the game starts or when spawned
void AFTaleProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(ProjectileSMComponent))
	{
		ProjectileSMComponent->OnComponentBeginOverlap.AddDynamic(this, &AFTaleProjectile::OnComponentOverlap);
	}
}

void AFTaleProjectile::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFTaleTestProjectCharacter* OverlappedActor = Cast<AFTaleTestProjectCharacter>(OtherActor))
	{
		if (IsValid(OverlappedActor->AttributeComponent))
		{
			OverlappedActor->AttributeComponent->ApplyDamage(DamageToApply);

			Destroy();
		}
	}
}

