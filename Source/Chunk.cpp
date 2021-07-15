#include "Chunk.h"
#include "PaperTileSet.h"
#include "PaperTileLayer.h"
#include "PaperTileMap.h"
#include "PaperTileMapComponent.h"

AChunk::AChunk()
{
	RenderComponent = CreateDefaultSubobject<UPaperTileMapComponent>(TEXT("RenderComponent"));
	UPaperTileMap* Map = CreateDefaultSubobject<UPaperTileMap>(TEXT("WorldTileMap"));
	
	RenderComponent->TileMap = Map;
	RootComponent = RenderComponent;
}

