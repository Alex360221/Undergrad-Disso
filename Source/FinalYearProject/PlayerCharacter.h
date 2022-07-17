// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class FINALYEARPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		UStaticMeshComponent* MeshComp;
	class AMapController* mapRef;

	UFUNCTION(BlueprintCallable)
		class AMapController* GetMapController() { return mapRef; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float vaule);
	void MoveRight(float vaule);
	void TurnAtRate(float vaule);
	void LookUpRate(float vaule);
	void UpPressed() { up = true; }
	void UpReleased() { up = false; }
	void DownPressed() { down = true; }
	void DownReleased() { down = false; }
	void Up();
	void Down();
	bool up;
	bool down;
	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
