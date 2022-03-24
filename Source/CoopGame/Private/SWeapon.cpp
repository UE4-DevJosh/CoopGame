// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/Actor.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "CoopGame.h"
#include "SCharacter.h"
#include <Components/SceneComponent.h>
#include "Camera/CameraComponent.h"
#include <TimerManager.h>


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	RateOfFire = 600;

}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;

}

void ASWeapon::Fire()
{

	ASCharacter* MyOwner = Cast<ASCharacter>(GetOwner());
	APlayerController* OwnerController = Cast<APlayerController>(MyOwner->GetController());

	if (MyOwner && OwnerController) {

		int32 ViewportSizeX;
		int32 ViewportSizeY;

		OwnerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

		FVector WorldLocation;
		FVector WorldDirection;

		OwnerController->DeprojectScreenPositionToWorld(ViewportSizeX / 2, ViewportSizeY / 2, WorldLocation, WorldDirection);

		FVector ShotDirection = MyOwner->GetCameraComponent()->GetComponentRotation().Vector();

		FVector StartTrace;
		FQuat WeaponRotation;

		this->MeshComp->GetSocketWorldLocationAndRotation(MuzzleSocketName, StartTrace, WeaponRotation);

		FVector ForwardVector = MyOwner->GetCameraComponent()->GetForwardVector();
		FVector TraceEnd = ((WorldDirection * 10000.f) + WorldLocation);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, TraceEnd, COLLISION_WEAPON, QueryParams)) {

			//Blocking hit! Process Damage

			AActor* HitActor = Hit.GetActor();
			
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE) {

				ActualDamage *= 4.0f;

			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			if (SelectedEffect) {

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			}

			TracerEndPoint = Hit.ImpactPoint;

		}

		if (DebugWeaponDrawing > 0) {

			DrawDebugLine(GetWorld(), StartTrace, TraceEnd, FColor::Black, false, 1.0f, 0, 1.0f);

		}

		PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;

	}

}

void ASWeapon::StartFire()
{

	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}


	if (MuzzleEffect) {

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		
		if (TracerComp) {

			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);

		}

	}

	APawn* MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner && FireCamSake) {
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());

		if (PC) {

			PC->ClientPlayCameraShake(FireCamSake);

		}
	}
}