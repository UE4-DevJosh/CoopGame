// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeProjectile.h"
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <TimerManager.h>
#include <Kismet/GameplayStatics.h>
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoopGame.h"
#include <Object.h>
#include <GameFramework/DamageType.h>

// Sets default values
ASGrenadeProjectile::ASGrenadeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileComponent->InitialSpeed = 2000.f;
	ProjectileComponent->MaxSpeed = 2000.f;
	ProjectileComponent->bShouldBounce = true;

	SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComponent"));

	SphereComponent->SetSimulatePhysics(true);

	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	RootComponent = SphereComponent;
}

// Called when the game starts or when spawned
void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ExplodeTimerHandle, this, &ASGrenadeProjectile::Explode, 1.0f, false);


}

void ASGrenadeProjectile::Explode()
{
	OnExplode();
	
	if (ImpactEffect) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation());
	
	}
	
	TArray<AActor*> IgnoredActors;

	UGameplayStatics::ApplyRadialDamage(GetWorld(), 
		100.0f,
		GetActorLocation(), 
		DamageRadius, 
		UDamageType::StaticClass(), 
		IgnoredActors, 
		this, 
		GetInstigator()->GetController(), 
		true, 
		COLLISION_WEAPON);

	Destroy();

}

// Called every frame
void ASGrenadeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGrenadeProjectile::EndPlay(const EEndPlayReason::Type EEndPlayReason)
{

	Super::EndPlay(EEndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ExplodeTimerHandle);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

}

