#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldGenerationLibrary.generated.h"

UCLASS()
class STARS_API UWorldGenerationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "WorldGeneration")
	static float LineInterpolate(float ValueA, float ValueB, float t);

	UFUNCTION(BlueprintPure, Category = "WorldGeneration")
	static float smootherstep(float t);

	UFUNCTION(BlueprintPure, Category = "WorldGeneration")
	static FVector RandomGradient(float x, float y);

	UFUNCTION(BlueprintPure, Category = "WorldGeneration")
	static float DotGridGradient(float ix, float iy, float x, float y);

	UFUNCTION(BlueprintPure, Category = "WorldGeneration")
	static float PerlinNoise(float x, float y);
	
};
