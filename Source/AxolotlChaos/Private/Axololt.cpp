// Fill out your copyright notice in the Description page of Project Settings.


#include "Axololt.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"


// Sets default values
AAxololt::AAxololt()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAxololt::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContent, 0);
		}
	}
	
}

// Called every frame
void AAxololt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAxololt::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AAxololt::Dashing);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAxololt::Move);

		//EnhancedInputComponent->BindAction(LAttackAction, ETriggerEvent::Triggered, this, &AAxololt::LightAttack);
	}

}

void AAxololt::Move(const FInputActionValue& _value) {
	FVector2D MovementVector = _value.Get<FVector2D>();

	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAxololt::Dashing() {
	const FVector ForwardDir = this->GetActorForwardVector();
	LaunchCharacter(ForwardDir * DashDistance, true, false);
}

void AAxololt::LightAttack()
{
	UE_LOG(LogTemp, Display, TEXT("Se hace"));

	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * AttackRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	CollisionParams.AddIgnoredActors(CharacterChildren);
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	UE_LOG(LogTemp, Display, TEXT("Se entra? %b"), bHit);


	if (bHit) {
		AActor* HitActor = HitResult.GetActor();
		if (HitActor != nullptr) {
			
			FDamageEvent DamageEvent(PlayerDamageType);


			AController* InstitigatedBy = GetWorld()->GetFirstPlayerController();

			AActor* DamageCauser = this;

			HitActor->TakeDamage(WeponDamage, DamageEvent, InstitigatedBy, DamageCauser);
			UE_LOG(LogTemp, Display, TEXT("Pegado"));
		}
	}
}

bool AAxololt::IsAttacking()
{
	return false;
}

void AAxololt::AddHealth(float _healthAmount) {
	Health += _healthAmount;
}

float AAxololt::InternalTakePointDamage(float Damage, FPointDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;
	return Health;
}

float AAxololt::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = DamageAmount;

	UDamageType const* const DamageTypeCDO = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		// point damage event, pass off to helper function
		FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
		ActualDamage = InternalTakePointDamage(ActualDamage, *PointDamageEvent, EventInstigator, DamageCauser);

		// K2 notification for this actor
		if (ActualDamage != 0.f)
		{
			ReceivePointDamage(ActualDamage, DamageTypeCDO, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.ImpactNormal, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, EventInstigator, DamageCauser, PointDamageEvent->HitInfo);
			OnTakePointDamage.Broadcast(this, ActualDamage, EventInstigator, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, DamageTypeCDO, DamageCauser);

			// Notify the component
			UPrimitiveComponent* const PrimComp = PointDamageEvent->HitInfo.Component.Get();
			if (PrimComp)
			{
				PrimComp->ReceiveComponentDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			}
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		// radial damage event, pass off to helper function
		FRadialDamageEvent* const RadialDamageEvent = (FRadialDamageEvent*)&DamageEvent;
		ActualDamage = InternalTakeRadialDamage(ActualDamage, *RadialDamageEvent, EventInstigator, DamageCauser);

		// K2 notification for this actor
		if (ActualDamage != 0.f)
		{
			FHitResult const& Hit = (RadialDamageEvent->ComponentHits.Num() > 0) ? RadialDamageEvent->ComponentHits[0] : FHitResult();
			ReceiveRadialDamage(ActualDamage, DamageTypeCDO, RadialDamageEvent->Origin, Hit, EventInstigator, DamageCauser);
			OnTakeRadialDamage.Broadcast(this, ActualDamage, DamageTypeCDO, RadialDamageEvent->Origin, Hit, EventInstigator, DamageCauser);

			// add any desired physics impulses to our components
			for (int HitIdx = 0; HitIdx < RadialDamageEvent->ComponentHits.Num(); ++HitIdx)
			{
				FHitResult const& CompHit = RadialDamageEvent->ComponentHits[HitIdx];
				UPrimitiveComponent* const PrimComp = CompHit.Component.Get();
				if (PrimComp && PrimComp->GetOwner() == this)
				{
					PrimComp->ReceiveComponentDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
				}
			}
		}
	}

	// generic damage notifications sent for any damage
	// note we will broadcast these for negative damage as well
	if (ActualDamage != 0.f)
	{
		ReceiveAnyDamage(ActualDamage, DamageTypeCDO, EventInstigator, DamageCauser);
		OnTakeAnyDamage.Broadcast(this, ActualDamage, DamageTypeCDO, EventInstigator, DamageCauser);
		if (EventInstigator != nullptr)
		{
			EventInstigator->InstigatedAnyDamage(ActualDamage, DamageTypeCDO, this, DamageCauser);
		}
	}

	return ActualDamage;
}

void AAxololt::RangedAttack() {
	
	/*FVector Start = GetActorLocation();

	FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

	FVector WorldLocation;
	FVector WorldDirection;
	
	FVector End;

	if (GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection)) {

		WorldDirection.Z = 0;
		WorldDirection = WorldDirection.GetSafeNormal();
		End = Start + WorldDirection * AttackRange;
		UE_LOG(LogTemp, Display, TEXT("WorldLocation: %s"), *WorldLocation.ToString());
	}


	FVector LookingAt = FVector(MousePosition.X, MousePosition.Y, Start.Z);

	/*float mouseX;
	float mouseY;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetMousePosition(mouseX, mouseY);

	FVector LookingAt = FVector(mouseX, mouseY, Start.Z);


	FColor Color = FColor::Red;

	float ArrowSize = 10.f;
	float LifeTime = 0.1f;
	uint8 DepthPriority = 0;
	float Thickness = 2.0f;

	DrawDebugDirectionalArrow(GetWorld(), Start, End, ArrowSize, Color, true, LifeTime, DepthPriority, Thickness);*/

	UE_LOG(LogTemp, Display, TEXT("Draw Arrow"));

}
