#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

UCLASS()
class STARS_API AChunk : public AActor
{
	GENERATED_BODY()

	class UPaperTileMapComponent* RenderComponent;

public:	
	AChunk();
	FORCEINLINE class UPaperTileMapComponent* GetRenderComponent() const { return RenderComponent; }
};
