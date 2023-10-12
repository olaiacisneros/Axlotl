// Fill out your copyright notice in the Description page of Project Settings.


#include "Axololt.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

		EnhancedInputComponent->BindAction(LAttackAction, ETriggerEvent::Triggered, this, &AAxololt::LightAttack);
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
