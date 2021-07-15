#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "ToolComponent.generated.h"

UCLASS()
class STARS_API UToolComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()

public:
	virtual void OnClick();
};
