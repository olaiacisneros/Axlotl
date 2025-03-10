// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"
#include "Axololt.generated.h"

UENUM(BlueprintType)
enum class ENUM_UPGRADES : uint8
{
	UPGRADE_MORE_LIFE					UMETA(DisplayName = "UPGRADE_MORE_LIFE"),
	UPGRADE_LIFE_PER_ROOM				UMETA(DisplayName = "UPGRADE_LIFE_PER_ROOM"),
	UPGRADE_LIFE_ADD					UMETA(DisplayName = "UPGRADE_LIFE_ADD"),
	UPGRADE_DOUBLE_DASH					UMETA(DisplayName = "UPGRADE_DOUBLE_DASH"),
	UPGRADE_BASIC_ATTACK				UMETA(DisplayName = "UPGRADE_BASIC_ATTACK"),
	UPGRADE_SPECIAL_ATTACK				UMETA(DisplayName = "UPGRADE_SPECIAL_ATTACK"),
	UPGRADE_COOLDOWN_SPECIAL			UMETA(DisplayName = "UPGRADE_COOLDOWN_SPECIAL"),
	UPGRADE_ALL							UMETA(DisplayName = "UPGRADE_ALL"),
};

UCLASS()
class TAILLESS_API AAxololt : public ACharacter
{
	GENERATED_BODY()

private:
	//Spring Arm Component para que la c�mara persiga detras al player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true")) class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true")) class UCameraComponent* FollowCamera;

public:
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float DashDistance = 2000;
    UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float AirborneDashDistance = 100;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float WeponDamage = 5;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float AttackRange = 1000;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) TSubclassOf<UDamageType> PlayerDamageType;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float MaxHealth = 100;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float Health = 100;
	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) FVector ProjectileDirection;
	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) float AngleProjectile = 0;
	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) FRotator RotatorProjectile;

	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) float BasicAttackDamage = 10;
	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) float BasicAttackComboDamage = 15;
	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) float SpecialAttackDamage = 20;
	UPROPERTY(EditAnyWhere, Category = "Attacks", BlueprintReadWrite) float CoolDownSpecialAttack = 1.5f;


	UPROPERTY(EditAnyWhere, Category = "Dash", BlueprintReadWrite) bool DashDisable = false;
    UPROPERTY(EditAnyWhere, Category = "Dash", BlueprintReadWrite) FVector LocationEdge;
	UPROPERTY(EditAnyWhere, Category = "Dash", BlueprintReadWrite) int DashCounter = 1;
	UPROPERTY(EditAnyWhere, Category = "Dash", BlueprintReadWrite) int DashCounterAux = 1;

	UPROPERTY(EditAnyWhere, Category = "Upgrades", BlueprintReadWrite) bool upgrade_room_life = false;

	UPROPERTY(EditAnywhere, Category = "Upgrades", BlueprintReadWrite) ENUM_UPGRADES Upgrade_1;
	UPROPERTY(EditAnywhere, Category = "Upgrades", BlueprintReadWrite) ENUM_UPGRADES Upgrade_2;

private:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	//Llamado para adelante/atras inputs
	void Move(const FInputActionValue& _value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Dashing();

	UFUNCTION(BlueprintCallable, Category = "Attacks")
	void LightAttack();

	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool IsAttacking();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	void RangedAttack(const FInputActionValue& _value);

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	void RangedMouse();

	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	ENUM_UPGRADES ChooseUpgrade();

	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void ApplyUpgrade(ENUM_UPGRADES _upgrade);

protected:
	float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& RadialDamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

public:
	// Sets default values for this character's properties
	AAxololt();

	//APawn interface
protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite)
	float NormalSpeedRun;
	
	bool ControllerConnected = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContent;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* RangedAction;


	APlayerController* PlayerController = NULL;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement") UCharacterMovementComponent* CharacterMovementComponent;

public:
    FCollisionQueryParams GetIgnoreCharacterParams() const;
};
