#include "TestPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ATestPawn::ATestPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2048.0f * 2.5 * 2;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f * 5;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->SetUsingAbsoluteRotation(true);

	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
}

void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &ATestPawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ATestPawn::MoveUp);
	PlayerInputComponent->BindAction("ZoomPlus", IE_Pressed, this, &ATestPawn::ZoomPlus);
	PlayerInputComponent->BindAction("ZoomMinus", IE_Pressed, this, &ATestPawn::ZoomMinus);
}

void ATestPawn::MoveUp(float Value)
{
	AddMovementInput(FVector(0.0f, 0.0f, 1.0f), Value);
}

void ATestPawn::MoveRight(float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ATestPawn::ZoomPlus()
{
	SideViewCameraComponent->OrthoWidth = SideViewCameraComponent->OrthoWidth / 2.0f;
}

void ATestPawn::ZoomMinus()
{
	SideViewCameraComponent->OrthoWidth = SideViewCameraComponent->OrthoWidth * 2.0f;
}