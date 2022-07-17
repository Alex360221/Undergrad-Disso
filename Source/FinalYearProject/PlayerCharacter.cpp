// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.h"
#include "MapController.h"
// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(20.f, 40.0f);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->bUsePawnControlRotation = true;
	CameraComp->SetRelativeLocation(FVector(0, 0, 15));
	up = false;
	down = false;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	mapRef = GetWorld()->SpawnActor<AMapController>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (mapRef != nullptr)
	{
		mapRef->playerRef = this;
	}


	//enable flying
	GetCharacterMovement()->bCheatFlying = true;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	
}

void APlayerCharacter::MoveForward(float vaule)
{
	if ((Controller) && (vaule != 0.0f))
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), vaule);
	}
}

void APlayerCharacter::MoveRight(float vaule)
{
	if ((Controller) && (vaule != 0.0f))
	{
		AddMovementInput(GetActorRightVector(), vaule);
	}
}

void APlayerCharacter::TurnAtRate(float vaule)
{
	APlayerCharacter::AddControllerYawInput(vaule);
}

void APlayerCharacter::LookUpRate(float vaule)
{
	APlayerCharacter::AddControllerPitchInput(vaule);
}

void APlayerCharacter::Up()
{
	SetActorLocation(GetActorLocation() + FVector(0, 0, 5));
	
}

void APlayerCharacter::Down()
{
	SetActorLocation(GetActorLocation() - FVector(0, 0, 5));
}


// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (up) { Up(); }
	if (down) { Down(); }
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);//uses the inbuilt jump function
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &APlayerCharacter::UpPressed);
	PlayerInputComponent->BindAction("Up", IE_Released, this, &APlayerCharacter::UpReleased);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &APlayerCharacter::DownPressed);
	PlayerInputComponent->BindAction("Down", IE_Released, this, &APlayerCharacter::DownReleased);

	InputComponent->BindAxis("MoveX", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveY", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpRate);
}

