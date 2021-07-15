// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Chunk.h"
#include "Math/IntPoint.h"
#include "StarCharacter.h"
#include "PaperTileSet.h"
#include "PaperTileLayer.h"
#include "PaperTileMap.h"
#include "StarsGameModeBase.generated.h"

UCLASS()
class STARS_API AStarsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	float SEED = 564547.0f;
	int WORLD_SIZE = 131072;
	int CHUNK_SIZE = 64;
	int SEA_HEIGHT = 32767;
	int MOUNTAIN_HEIGHT = 2600;
	int OCEAN_DEPTH = 260;
	float TILE_SIZE = 32.0f;
	int COLLISION_LAYER_INDEX = 1;
	
	TMap<FIntPoint, AChunk*> Chunks;
	TSubclassOf<class AStarCharacter> StarCharacterClass;
	APawn* StarCharacter;
	UMaterial* TileMapMaterial;
	class UPaperTileSet* GameTileSet;

	void SetBlockInChunk(AChunk* ChunkPtr, int LayerIndex, int a, int b, int PackedTileIndex);
	void ChunkSpawn(int I, int J);
	int GetHeight(int x);
	void InsertTextureInLayer(UTexture2D* Texture, UPaperTileLayer* Layer, FIntPoint LayerLocation, FIntPoint WorldLocation, FIntPoint TextureOffset);
	void InsertTextureInChunk(UTexture2D* Texture, FIntPoint WorldLocation, FIntPoint TextureOffset, FIntPoint Chunk, int LayerIndex);
	bool IsXZ_InWorld(int X, int Z);
	bool IsX_InWorld(int X);
	bool IsZ_InWorld(int Z);
	bool TreeDistribution(int x);

protected:
	virtual void BeginPlay() override;

public:
	AStarsGameModeBase();
	virtual void Tick(float DeltaTime) override;
	void DeleteGroupCollisionBlocks(TSet<FIntPoint> Blocks);
	FPaperTileInfo GetGameTileInfo(int index);

};
