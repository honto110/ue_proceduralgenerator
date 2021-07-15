#include "Hands.h"
#include "Kismet/GameplayStatics.h"
#include "StarsGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UHands::UHands()
{
	static ConstructorHelpers::FObjectFinder<UPaperTileMap> TileMapClassFinder(TEXT("PaperTileMap'/Game/StarMaps/HandHitMap.HandHitMap'"));
	if (TileMapClassFinder.Object)
	{
		HitTileMap = TileMapClassFinder.Object;
	}
	else
	{
		HitTileMap = NULL;
	}
}

void UHands::BeginPlay()
{
	Super::BeginPlay();

	AStarsGameModeBase* GameMode = Cast<AStarsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	DeleteGroupCollisionBlocks.BindUObject(GameMode, &AStarsGameModeBase::DeleteGroupCollisionBlocks);
}

void UHands::OnClick()
{
	UE_LOG(LogTemp, Warning, TEXT("UHands::OnClick()"));
	if (AActor* Owner = GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnClick()::GetOwner()"));
		if (UWorld* World = Owner->GetWorld())
		{
			UE_LOG(LogTemp, Warning, TEXT("OnClick()::GetWorld()"));
			if (UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/StarsTextures/test.test"))))
			{
				UE_LOG(LogTemp, Warning, TEXT("OnClick()::StaticLoadObject()"));
				FTexture2DMipMap* MipMap = &Texture->PlatformData->Mips[0];
				FByteBulkData* RawImageData = &MipMap->BulkData;
				int TextureWidth = MipMap->SizeX;
				int TextureHeight = MipMap->SizeY;
				FColor* FormatedImageData = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

				APlayerController* PlayerController = World->GetFirstPlayerController();
				FVector MouseLocation, MouseRotation;
				PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseRotation);
				int X, Z;
				X = (int)MouseLocation.X / 32.0f;
				Z = (int)MouseLocation.Z / 32.0f;
				FVector2D NormalVector = UKismetMathLibrary::Normal2D(FVector2D(MouseLocation.X - Owner->GetActorLocation().X, MouseLocation.Z - Owner->GetActorLocation().Z));
				
				float angle;
				if (NormalVector.X >= 0)
				{
					angle = UKismetMathLibrary::Acos(NormalVector.Y);
				}
				else
				{
					angle = -UKismetMathLibrary::Acos(NormalVector.Y);
				}
				
				float Ox = 8.5f;
				float Oy = 8.5f;
				int OffsetX = 8;
				int OffsetZ = 8;

				TSet<FIntPoint> Set;

				for (int i = 0; i < TextureWidth; i++)
				{
					for (int j = 0; j < TextureHeight; j++)
					{

						float x = (float)i + 0.5;
						float y = (float)j + 0.5;

						float u = (x - Ox) * UKismetMathLibrary::Cos(angle) - (y - Oy) * UKismetMathLibrary::Sin(angle) + Ox;
						float v = (x - Ox) * UKismetMathLibrary::Sin(angle) + (y - Oy) * UKismetMathLibrary::Cos(angle) + Oy;

						int a = (int)u;
						int b = (int)v;

						if (a < 0 || b < 0 || a >= TextureWidth || b >= TextureHeight)
						{
							continue;
						}

						if (FormatedImageData[FMath::Abs(b - TextureHeight + 1) * TextureWidth + a] == FColor::Black)
						{
							Set.Add(FIntPoint(X - OffsetX + i, Z - OffsetZ + j));
						}
					}
				}
				RawImageData->Unlock();
				DeleteGroupCollisionBlocks.Execute(Set);
			}
		}
	}
}

