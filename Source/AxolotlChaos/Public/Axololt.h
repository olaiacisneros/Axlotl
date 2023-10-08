// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Axololt.generated.h"

UCLASS()
class AXOLOTLCHAOS_API AAxololt : public ACharacter
{
	GENERATED_BODY()

private:
	//Spring Arm Component para que la cámara persiga detras al player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true")) class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true")) class UCameraComponent* FollowCamera;

public:
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float DashDistance = 2000;

private:
	//Llamado para adelante/atras inputs
	void Move(const FInputActionValue& _value);

	void Dashing();

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputMappingContext* DefaultMappingContent;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;
};
