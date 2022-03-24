// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "SGrenadeProjectile.h"
#include "GameFramework/Actor.h"
#include "SCharacter.h"

void ASProjectileWeapon::Fire()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner && ProjectileClass) {

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* Projectile = GetWorld()->SpawnActor<ASGrenadeProjectile>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

		ASCharacter* Owner = Cast<ASCharacter>(GetOwner());

		if (Owner) {
					   
			Projectile->SetInstigator(Owner);

		}

	
	}
}
