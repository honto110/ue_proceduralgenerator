// Copyright Epic Games, Inc. All Rights Reserved.

#include "StarsGameModeBase.h"
#include "GameFramework/Actor.h"
#include "WorldGenerationLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PaperTileMapComponent.h"
#include "Chunk.h"
#include "TestPawn.h"

AStarsGameModeBase::AStarsGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UClass> CharacterClassFinder(TEXT("Blueprint'/Game/BP_StarCharacter.BP_StarCharacter_C'"));
	if (CharacterClassFinder.Object)
	{
		StarCharacterClass = CharacterClassFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialClassFinder(TEXT("Material'/Game/StarMaterials/TileMapMaterial.TileMapMaterial'"));
	if (MaterialClassFinder.Object)
	{
		TileMapMaterial = MaterialClassFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPaperTileSet> GameTileSetClassFinder(TEXT("PaperTileSet'/Game/StarsSets/StarsSet.StarsSet'"));
	if (GameTileSetClassFinder.Object)
	{
		GameTileSet = GameTileSetClassFinder.Object;
	}
}

void AStarsGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (World)
	{
		int SpawnWeight = WORLD_SIZE / 2;
		int SpawnHeight = GetHeight(SpawnWeight) + 45;
	    FVector SpawnLocation(SpawnWeight * TILE_SIZE, 0.0f, SpawnHeight * TILE_SIZE);
		
		FActorSpawnParameters SpawnParameters;
		StarCharacter = World->SpawnActor<APawn>(ATestPawn::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
		UGameplayStatics::GetPlayerController(World, 0)->Possess(StarCharacter);
		UGameplayStatics::GetPlayerController(World, 0)->bShowMouseCursor = 1;

		int I, J;
		I = (int)(SpawnLocation.X / (CHUNK_SIZE * TILE_SIZE));
		J = (int)(SpawnLocation.Z / (CHUNK_SIZE * TILE_SIZE));

		int SIZE = 3;
		TArray<FIntPoint> ChunkSpawnPointArray;
		int A = I - SIZE;
		int B = J - SIZE;
		for (int i = 0; i <= 2 * SIZE; i++)
		{
			for (int j = 0; j <= 2 * SIZE; j++)
			{
				ChunkSpawnPointArray.Add(FIntPoint(A + i, B + j));
			}
		}

		for (auto& ChunkSpawnPoint : ChunkSpawnPointArray)
		{
			ChunkSpawn(ChunkSpawnPoint.X, ChunkSpawnPoint.Y);
		}
	}
}

void AStarsGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	 
	if (UWorld* World = GetWorld())
	{
		if (StarCharacter)
		{
			int I, J;
			FVector CharacterLocation = StarCharacter->GetActorLocation();
			I = (int)(CharacterLocation.X / (CHUNK_SIZE * TILE_SIZE));
			J = (int)(CharacterLocation.Z / (CHUNK_SIZE * TILE_SIZE));

			int SIZE = 3;
			TArray<FIntPoint> ChunkSpawnPointArray;
			int A = I - SIZE;
			int B = J - SIZE;
			for (int i = 0; i <= 2 * SIZE; i++)
			{
				for (int j = 0; j <= 2 * SIZE; j++)
				{
					ChunkSpawnPointArray.Add(FIntPoint(A + i, B + j));
				}
			}

			TArray<FIntPoint> DeletedChunks;
			for (auto& Chunk : Chunks)
			{
				if (DeletedChunks.Num() >= 1) break;
				if (!ChunkSpawnPointArray.Contains(Chunk.Key)) DeletedChunks.Add(Chunk.Key);
			}

			for (auto& DeletedChunk : DeletedChunks)
			{
				AChunk* ChunkPtr = *Chunks.Find(DeletedChunk);
				if (ChunkPtr->Destroy()) Chunks.Remove(DeletedChunk);
			}

			int SpawnCount = 0;
			for (auto& ChunkSpawnPoint : ChunkSpawnPointArray)
			{
				if (SpawnCount >= 1) break;
				if (!Chunks.Contains(ChunkSpawnPoint))
				{
					ChunkSpawn(ChunkSpawnPoint.X, ChunkSpawnPoint.Y);
					SpawnCount++;
				}
			}
		}
	}
}

void AStarsGameModeBase::SetBlockInChunk(AChunk* ChunkPtr, int LayerIndex, int a, int b, int GameTileIndex)
{
	ChunkPtr->GetRenderComponent()->TileMap->TileLayers[LayerIndex]->SetCell(a, FMath::Abs(b - CHUNK_SIZE + 1), GetGameTileInfo(GameTileIndex));
}

void AStarsGameModeBase::ChunkSpawn(int I, int J)
{
	if (I < 0 || J < 0 || I >= (WORLD_SIZE / CHUNK_SIZE) || J >= (WORLD_SIZE / CHUNK_SIZE))
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		FVector ChunkSpawnLocation(I * CHUNK_SIZE * TILE_SIZE + TILE_SIZE / 2.0f, 0.0f, J * CHUNK_SIZE * TILE_SIZE + CHUNK_SIZE * TILE_SIZE - TILE_SIZE / 2.0f);
		AChunk* Chunk = World->SpawnActor<AChunk>(AChunk::StaticClass(), ChunkSpawnLocation, FRotator::ZeroRotator, SpawnParams);
		Chunk->GetRenderComponent()->TileMap->ResizeMap(CHUNK_SIZE, CHUNK_SIZE);
		Chunk->GetRenderComponent()->TileMap->Material = TileMapMaterial;
		if (Chunk)
		{
			UPaperTileLayer* CollisionLayer = NewObject<UPaperTileLayer>(UPaperTileLayer::StaticClass());
			if (CollisionLayer)
			{
				CollisionLayer->ResizeMap(CHUNK_SIZE, CHUNK_SIZE);

				int HeightMap[64];

				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					HeightMap[i] = GetHeight(I * CHUNK_SIZE + i);
				}

				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					for (int j = 0; j < CHUNK_SIZE; j++)
					{
						int RealHeight = J * CHUNK_SIZE + j;
						if (RealHeight <= HeightMap[i])
						{
							CollisionLayer->SetCell(i, FMath::Abs(j - CHUNK_SIZE + 1), GetGameTileInfo(0));
						}

					}
				}

				Chunk->GetRenderComponent()->TileMap->AddExistingLayer(CollisionLayer, 0);
			}

			UPaperTileLayer* TreeLayer = NewObject<UPaperTileLayer>(UPaperTileLayer::StaticClass());
			if (TreeLayer)
			{
				TreeLayer->ResizeMap(CHUNK_SIZE, CHUNK_SIZE);
				if (UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/StarsTextures/tree2"))))
				{
					for (int i = -Texture->GetSizeX(); i < CHUNK_SIZE + Texture->GetSizeX(); i++)
					{
						int RealWeight = I * CHUNK_SIZE + i;
						if (IsX_InWorld(RealWeight) && TreeDistribution(RealWeight))
						{
							InsertTextureInLayer(Texture, TreeLayer, FIntPoint(I, J), FIntPoint(RealWeight, GetHeight(RealWeight)), FIntPoint(64, 0));
						}
					}
				}
				Chunk->GetRenderComponent()->TileMap->AddExistingLayer(TreeLayer, 1);
			}

			Chunk->GetRenderComponent()->TileMap->RebuildCollision();
			Chunk->GetRenderComponent()->ReregisterComponent();
			Chunks.Add(FIntPoint(I, J), Chunk);
		}
	}
}

int AStarsGameModeBase::GetHeight(int x)
{
	int Height = SEA_HEIGHT;

	float temp = UWorldGenerationLibrary::PerlinNoise((float)(x) / ((float)(WORLD_SIZE - 1) / 32.0f), SEED);
	if (temp >= 0)
	{
		Height += (int)(temp * MOUNTAIN_HEIGHT * 1.414f);
	}
	else
	{
		Height += (int)(temp * OCEAN_DEPTH * 1.414f);
	}

	temp = UWorldGenerationLibrary::PerlinNoise((float)(x) / ((float)(WORLD_SIZE - 1) / 128.0f), SEED);
	if (temp >= 0)
	{
		Height += (int)(temp * MOUNTAIN_HEIGHT * 1.414f / 4.0f);
	}
	else
	{
		Height += (int)(temp * OCEAN_DEPTH * 1.414f / 4.0f);
	}
	
	return Height;
}

void AStarsGameModeBase::DeleteGroupCollisionBlocks(TSet<FIntPoint> Blocks)
{
	TSet<AChunk*> ChangedChunks;

	for (auto& Block : Blocks)
	{
		if (Block.X < 0 || Block.Y < 0 || Block.X >= WORLD_SIZE || Block.Y >= WORLD_SIZE)
		{
			continue;
		}

		int I, J;
		I = Block.X / CHUNK_SIZE;
		J = Block.Y / CHUNK_SIZE;
		FIntPoint key(I, J);

		if (Chunks.Contains(key))
		{
			int a = Block.X - I * CHUNK_SIZE;
			int b = Block.Y - J * CHUNK_SIZE;
			AChunk* Chunk = *Chunks.Find(key);
			Chunk->GetRenderComponent()->TileMap->TileLayers[0]->SetCell(a, FMath::Abs(b - CHUNK_SIZE + 1), GetGameTileInfo(-1));
			ChangedChunks.Add(Chunk);
		}
	}

	for (auto& ChangedChunk : ChangedChunks)
	{
		ChangedChunk->GetRenderComponent()->TileMap->RebuildCollision();
		ChangedChunk->GetRenderComponent()->ReregisterComponent();
	}
}

FPaperTileInfo AStarsGameModeBase::GetGameTileInfo(int index)
{
	if (index == -1)
	{
		FPaperTileInfo EmptyTileInfo;
		return EmptyTileInfo;
	} 
	else if(0 <= index && index < GameTileSet->GetTileCount())
	{
		FPaperTileInfo GameTileInfo;
		GameTileInfo.TileSet = GameTileSet;
		GameTileInfo.PackedTileIndex = index;
		
		return GameTileInfo;
	} 
	else
	{
		FPaperTileInfo EmptyTileInfo;
		return EmptyTileInfo;
	}
}

void AStarsGameModeBase::InsertTextureInLayer(UTexture2D* Texture, UPaperTileLayer* Layer, FIntPoint LayerLocation, FIntPoint WorldLocation, FIntPoint TextureOffset)
{
	FTexture2DMipMap* MipMap = &Texture->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MipMap->BulkData;
	int TextureWidth = MipMap->SizeX;
	int TextureHeight = MipMap->SizeY;
	FColor* FormatedImageData = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

	TArray<FIntPoint> Array1;
	TArray<FIntPoint> Array2;
	for (int i = 0; i < TextureWidth; i++)
	{
		for (int j = 0; j < TextureHeight; j++)
		{
			if (FormatedImageData[FMath::Abs(j - TextureHeight + 1) * TextureWidth + i] == FColor::Black)
			{
				Array1.Add(FIntPoint(WorldLocation.X - TextureOffset.X + i, WorldLocation.Y - TextureOffset.Y + j));
			}
			if (FormatedImageData[FMath::Abs(j - TextureHeight + 1) * TextureWidth + i] == FColor::Green)
			{
				Array2.Add(FIntPoint(WorldLocation.X - TextureOffset.X + i, WorldLocation.Y - TextureOffset.Y + j));
			}
		}
	}



	RawImageData->Unlock();

	for (auto& Elem : Array1)
	{
		if (Elem.X < 0 || Elem.Y < 0 || Elem.X >= WORLD_SIZE || Elem.Y >= WORLD_SIZE)
		{
			continue;
		}

		int I = Elem.X / CHUNK_SIZE;
		int J = Elem.Y / CHUNK_SIZE;
		if (I == LayerLocation.X && J == LayerLocation.Y)
		{
			int a = Elem.X - I * CHUNK_SIZE;
			int b = Elem.Y - J * CHUNK_SIZE;
			Layer->SetCell(a, FMath::Abs(b - CHUNK_SIZE + 1), GetGameTileInfo(3));
		}
	}

	for (auto& Elem : Array2)
	{
		if (Elem.X < 0 || Elem.Y < 0 || Elem.X >= WORLD_SIZE || Elem.Y >= WORLD_SIZE)
		{
			continue;
		}

		int I = Elem.X / CHUNK_SIZE;
		int J = Elem.Y / CHUNK_SIZE;
		if (I == LayerLocation.X && J == LayerLocation.Y)
		{
			int a = Elem.X - I * CHUNK_SIZE;
			int b = Elem.Y - J * CHUNK_SIZE;
			Layer->SetCell(a, FMath::Abs(b - CHUNK_SIZE + 1), GetGameTileInfo(2));
		}
	}
}

void AStarsGameModeBase::InsertTextureInChunk(UTexture2D* Texture, FIntPoint WorldLocation, FIntPoint TextureOffset, FIntPoint Chunk, int LayerIndex)
{
	if (Chunks.Contains(Chunk))
	{
		AChunk* ChunkPtr = *Chunks.Find(Chunk);
		FTexture2DMipMap* MipMap = &Texture->PlatformData->Mips[0];
		FByteBulkData* RawImageData = &MipMap->BulkData;
		int TextureWidth = MipMap->SizeX;
		int TextureHeight = MipMap->SizeY;
		FColor* FormatedImageData = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

		TArray<FIntPoint> Array;
		for (int i = 0; i < TextureWidth; i++)
		{
			for (int j = 0; j < TextureHeight; j++)
			{
				if (FormatedImageData[FMath::Abs(j - TextureHeight + 1) * TextureWidth + i] != FColor::White)
				{
					Array.Add(FIntPoint(WorldLocation.X - TextureOffset.X + i, WorldLocation.Y - TextureOffset.Y + j));
				}
			}
		}
		RawImageData->Unlock();

		if (Array.Num())
		{
			for (auto& Elem : Array)
			{
				if (Elem.X < 0 || Elem.Y < 0 || Elem.X >= WORLD_SIZE || Elem.Y >= WORLD_SIZE)
				{
					continue;
				}

				int I = Elem.X / CHUNK_SIZE;
				int J = Elem.Y / CHUNK_SIZE;
				if (I == Chunk.X && J == Chunk.Y)
				{
					int a = Elem.X - I * CHUNK_SIZE;
					int b = Elem.Y - J * CHUNK_SIZE;
					SetBlockInChunk(ChunkPtr, a, b, 1, LayerIndex);
				}
			}

			ChunkPtr->GetRenderComponent()->TileMap->RebuildCollision();
			ChunkPtr->GetRenderComponent()->ReregisterComponent();
		}
	}
}

bool AStarsGameModeBase::IsXZ_InWorld(int X, int Z)
{
	if (X < 0 || Z < 0 || X >= WORLD_SIZE || Z >= WORLD_SIZE)
	{
		return false;
	} 
	else
	{
		return true;
	}
}

bool AStarsGameModeBase::IsX_InWorld(int X)
{
	if (X < 0 || X >= WORLD_SIZE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool AStarsGameModeBase::IsZ_InWorld(int Z)
{
	if (Z < 0 || Z >= WORLD_SIZE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool AStarsGameModeBase::TreeDistribution(int x)
{
	float temp = UWorldGenerationLibrary::PerlinNoise(SEED, (float)(x) / ((float)(WORLD_SIZE - 1) / 32.0f));
	if (x % 128 == 64 && temp >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
