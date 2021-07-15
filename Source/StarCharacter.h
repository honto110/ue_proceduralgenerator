#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StarCharacter.generated.h"

UCLASS()
class STARS_API AStarCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* Sprite;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UToolComponent* Tool;

	UPROPERTY()
	TMap<int, int> Inventory;

protected:
	virtual void BeginPlay() override;
	virtual void MoveRight(float Value);
	virtual void MoveUp(float Value);

public:	
	AStarCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	virtual void PostInitializeComponents() override;
	FORCEINLINE class UPaperFlipbookComponent* GetSprite() const { return Sprite; }
	void OnClick();
};
