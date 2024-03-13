// Fill out your copyright notice in the Description page of Project Settings.


#include "Axololt.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
//#include "Blueprint/WidgetLayoutLibrary.h"


// Sets default values
AAxololt::AAxololt()
{
	RotatorProjectile = FRotator(0, 0, 0);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, -30.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // We want to pull camera in when it collides with level

	// Create a camera...
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

    CharacterMovementComponent = GetCharacterMovement();

	MaxHealth = 100;

}

// Called when the game starts or when spawned
void AAxololt::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContent, 0);
		}
	}

}

// Called every frame
void AAxololt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RangedMouse();

    /*if (CharacterMovementComponent->IsFalling())
    {
        CharacterMovementComponent->DisableMovement();
    }
    else
    {
        CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
    }*/

}

// Called to bind functionality to input
void AAxololt::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		//EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AAxololt::Dashing);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAxololt::Move);
		
		EnhancedInputComponent->BindAction(RangedAction, ETriggerEvent::Triggered, this, &AAxololt::RangedAttack);

		//EnhancedInputComponent->BindAction(LAttackAction, ETriggerEvent::Triggered, this, &AAxololt::LightAttack);
	}

}

void AAxololt::Move(const FInputActionValue& _value) {
	FVector2D MovementVector = _value.Get<FVector2D>();

	if (Controller != nullptr) {
		const FRotator CameraRotation = CameraBoom->GetComponentRotation();
		//const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, CameraRotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		//Calcula la direccion del movimiento
		FVector MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;

		if (!MovementDirection.IsNearlyZero()) {
			
			if (Health > 0)
			{
				//Normaliza la direccion del movimiento
				MovementDirection.Normalize();

				//Calcula la rotacion hacia la que el personaje deberia mirara
				FRotator NewLookAt = MovementDirection.Rotation();

				//Obtiene la rotacion actual del personaje 
				FRotator CurrentRotation = GetActorRotation();

				//Interpola suavemente hacia la nueva rotacion
				FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, NewLookAt, GetWorld()->GetDeltaSeconds(), 10.0f);

				//Actualiza la rotacion del perosnaje para que mire en la direccion del movimiento
				SetActorRotation(SmoothRotation);
				//UE_LOG(LogTemp, Display, TEXT("SmoothRotation: %s"), *SmoothRotation.ToString());
			}

		}
	}
}

void AAxololt::Dashing() 
{	
	if (!DashDisable || DashCounterAux > 0)
	{
        float EdgeThreshold = 120.0f;
        FHitResult Hit;

		const FRotator CameraRotation = CameraBoom->GetComponentRotation();
		const FRotator YawRotation(0, CameraRotation.Yaw - 15, 0);
        
		FRotator PlayerRotation = GetActorRotation() + YawRotation;
		FVector RotatedLocationEdge = PlayerRotation.RotateVector(LocationEdge);
		UE_LOG(LogTemp, Display, TEXT("RotatedLocationEdge %s"), *RotatedLocationEdge.ToString());

		FVector EndPosition = GetActorLocation() - RotatedLocationEdge - FVector(0, 0, EdgeThreshold);
		//UE_LOG(LogTemp, Display, TEXT("StartPosition %s"), *StartPosition.ToString());

		GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), EndPosition, ECC_Visibility, GetIgnoreCharacterParams());
		DrawDebugLine(GetWorld(), GetActorLocation(), EndPosition, FColor::Red, true, 1.0f);

        bool IsNearEdge = !Hit.IsValidBlockingHit();

        const FVector ForwardDir = this->GetActorForwardVector();

        if (IsNearEdge)
        {
			UE_LOG(LogTemp, Display, TEXT("NearEdge"));
            LaunchCharacter(ForwardDir * AirborneDashDistance, true, true);
        }
        else
        {
			UE_LOG(LogTemp, Display, TEXT("Not Edge"));
		    LaunchCharacter(ForwardDir * DashDistance, true, true);
        }

		DashCounterAux--;
	}
}

void AAxololt::LightAttack()
{
	//UE_LOG(LogTemp, Display, TEXT("Se hace"));

	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * AttackRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	CollisionParams.AddIgnoredActors(CharacterChildren);
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	//UE_LOG(LogTemp, Display, TEXT("Se entra? %b"), bHit);


	if (bHit) {
		AActor* HitActor = HitResult.GetActor();
		if (HitActor != nullptr) {

			FDamageEvent DamageEvent(PlayerDamageType);


			AController* InstitigatedBy = GetWorld()->GetFirstPlayerController();

			AActor* DamageCauser = this;

			HitActor->TakeDamage(WeponDamage, DamageEvent, InstitigatedBy, DamageCauser);
			//UE_LOG(LogTemp, Display, TEXT("Pegado"));
		}
	}
}

bool AAxololt::IsAttacking()
{
	return false;
}

void AAxololt::AddHealth(float _healthAmount) {
	if (Health + _healthAmount > MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += _healthAmount;
	}
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

void AAxololt::RangedAttack(const FInputActionValue& _value)
{
	FVector2D MovementVector = _value.Get<FVector2D>();

	if (_value.IsNonZero())
	{
		ControllerConnected = true;

		FVector Vector(MovementVector.X, MovementVector.Y, 0);
		RotatorProjectile = Vector.GetSafeNormal().Rotation();
		RotatorProjectile.Yaw += 65;
		//UE_LOG(LogTemp, Display, TEXT("Valor de RotatorProjectile: %s"), *RotatorProjectile.ToString());
		//UE_LOG(LogTemp, Display, TEXT("Input detected"));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Valor de _value: %s"), *_value.ToString());
}

void AAxololt::RangedMouse()
{
	if (!ControllerConnected)
	{
		//UE_LOG(LogTemp, Display, TEXT("Input not detected"));

		FHitResult Hit;
		bool bHitSuccessful = false;

		FVector Start = GetActorLocation();

		bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

		if (bHitSuccessful) {
			ProjectileDirection = Hit.Location - Start;
		}

		float ArrowSize = 10.f;
		float LifeTime = 0.1f;
		uint8 DepthPriority = 0;
		float Thickness = 2.0f;
		FColor Color = FColor::Red;

		ProjectileDirection.Z = Start.Z;

		//DrawDebugDirectionalArrow(GetWorld(), Start, ProjectileDirection + Start, ArrowSize, Color, false, LifeTime, DepthPriority, Thickness);

		FVector NormalizedStart = Start.GetSafeNormal();
		FVector NormalizedDirection = ProjectileDirection.GetSafeNormal();

		FRotator RotationProjectile = NormalizedDirection.Rotation();
		FRotator RotationStart = NormalizedStart.Rotation();

		FRotator NewLookAt = RotationProjectile;

		//float DotProduct = FVector::DotProduct(NormalizedStart, NormalizedDirection);
		//float AngleRad = FMath::Acos(DotProduct);

		//AngleProjectile = FMath::RadiansToDegrees(AngleRad);

		RotatorProjectile = NewLookAt;
		RotatorProjectile.Pitch = 0.f;
		//UE_LOG(LogTemp, Display, TEXT("Valor de RotatorProjectile Bueno: %s"), *RotatorProjectile.ToString());

	}
}

ENUM_UPGRADES AAxololt::ChooseUpgrade()
{

	return (ENUM_UPGRADES) FMath::RandRange((int) ENUM_UPGRADES::UPGRADE_MORE_LIFE, (int) ENUM_UPGRADES::UPGRADE_COOLDOWN_SPECIAL);
}

void AAxololt::ApplyUpgrade(ENUM_UPGRADES _upgrade)
{
	switch (_upgrade)
	{
		case ENUM_UPGRADES::UPGRADE_MORE_LIFE:
		{
			MaxHealth += 20;
			Health = MaxHealth;
			GEngine->AddOnScreenDebugMessage(1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_More_life"));
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_More_life %d"), MaxHealth);
			break;
		}

		case ENUM_UPGRADES:: UPGRADE_LIFE_PER_ROOM:
		{
			upgrade_room_life = true;
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_room_life true"));
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_room_life true"));
			break;
		}

		case ENUM_UPGRADES:: UPGRADE_LIFE_ADD:
		{
			float addHealth = 10;
			AddHealth(addHealth);
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_Life_Add"));
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_Life_Add %i"), addHealth);
			break;
		}

		case ENUM_UPGRADES::UPGRADE_DOUBLE_DASH:
		{
			DashCounter = 2;
			DashCounterAux = 2;
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_Double_dash"));
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_Double_dash %i"), DashCounter);
			break;
		}
		
		case ENUM_UPGRADES::UPGRADE_BASIC_ATTACK:
		{
			float addAttack = 5;
			BasicAttackDamage += addAttack;
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_Basic_Attack"));
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_Basic_Attack %d"), addAttack);
			break;
		}

		case ENUM_UPGRADES::UPGRADE_SPECIAL_ATTACK:
		{
			float addAttack = 5;
			SpecialAttackDamage += addAttack;
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_Special_Attack"));
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_Special_Attack %d"), addAttack);
			break;
		}

		case ENUM_UPGRADES::UPGRADE_COOLDOWN_SPECIAL:
		{
			CoolDownSpecialAttack = 1;
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp]	Upgrade_Cooldown_Special 1"));
			UE_LOG(LogTemp, Display, TEXT("[Axololt.cpp]	Upgrade_Cooldown_Special 1"));
			break;
		}

		default:
		{
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("[Axololt.cpp] DEFAULT"));
			break;
		}
	}
}

FCollisionQueryParams AAxololt::GetIgnoreCharacterParams() const
{
    FCollisionQueryParams result;

    TArray<AActor*> CharacterChildren;
    GetAllChildActors(CharacterChildren);
    result.AddIgnoredActors(CharacterChildren);
    result.AddIgnoredActor(this);
    
    return result;
}