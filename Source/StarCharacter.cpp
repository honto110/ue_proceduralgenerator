#include "StarCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "ToolComponent.h"
#include "Hands.h"

AStarCharacter::AStarCharacter()
{
	Sprite = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComponent"));
	Sprite->AlwaysLoadOnClient = true;
	Sprite->AlwaysLoadOnServer = true;
	Sprite->bOwnerNoSee = false;
	Sprite->bAffectDynamicIndirectLighting = true;
	Sprite->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Sprite->SetupAttachment(GetCapsuleComponent());
	static FName CollisionProfileName(TEXT("CharacterMesh"));
	Sprite->SetCollisionProfileName(CollisionProfileName);
	Sprite->SetGenerateOverlapEvents(false);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2048.0f * 2.5;
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
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->AirControl = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	Tool = CreateDefaultSubobject<UHands>(TEXT("Hands"));
	Tool->SetupAttachment(Sprite);
}

void AStarCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AStarCharacter::MoveRight(float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AStarCharacter::MoveUp(float Value)
{
	AddMovementInput(FVector(0.0f, 0.0f, 1.0f), Value);
}

void AStarCharacter::OnClick()
{

	if (Tool)
	{
		Tool->OnClick();
	}
}

void AStarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &AStarCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AStarCharacter::MoveUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("OnClick", IE_Pressed, this, &AStarCharacter::OnClick);
}

void AStarCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (Sprite)
		{
			if (Sprite->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
			{
				Sprite->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
			}
		}
	}
}

