#pragma once

#include "CoreMinimal.h"
#include "ToolComponent.h"
#include "PaperTileLayer.h"
#include "PaperTileMap.h"
#include "Hands.generated.h"

DECLARE_DELEGATE_OneParam(DeleteGroupCollisionBlocksDelegate, TSet<FIntPoint>);

UCLASS()
class STARS_API UHands : public UToolComponent
{
	GENERATED_BODY()

	DeleteGroupCollisionBlocksDelegate DeleteGroupCollisionBlocks;
	UPaperTileMap* HitTileMap;

protected:
	virtual void BeginPlay() override;

public:
	UHands();
	virtual void OnClick();
};
