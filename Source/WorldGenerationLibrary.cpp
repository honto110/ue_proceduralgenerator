#include "WorldGenerationLibrary.h"

float UWorldGenerationLibrary::LineInterpolate(float ValueA, float ValueB, float t)
{
	return ValueA + (ValueB - ValueA) * smootherstep(t);
}

float UWorldGenerationLibrary::smootherstep(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

FVector UWorldGenerationLibrary::RandomGradient(float x, float y)
{
	float random = 2920.0f * sin(x * 21942.0f + y * 171324.0f + 8912.0f) * cos(x * 23157.0f * y * 217832.0f + 9758.0f);
	return FVector(cos(random), sin(random), 0.0f);
}

float UWorldGenerationLibrary::DotGridGradient(float ix, float iy, float x, float y)
{
	FVector Gradient = RandomGradient(ix, iy);

	float dx = x - ix;
	float dy = y - iy;

	return dx * Gradient.X + dy * Gradient.Y;
}

float UWorldGenerationLibrary::PerlinNoise(float x, float y)
{
	int x0 = (int)x;
	int x1 = x0 + 1;
	int y0 = (int)y;
	int y1 = y0 + 1;

	float sx = x - (float)x0;
	float sy = y - (float)y0;

	float n0, n1, ix0, ix1, value;

	n0 = DotGridGradient(x0, y0, x, y);
	n1 = DotGridGradient(x1, y0, x, y);
	ix0 = LineInterpolate(n0, n1, sx);

	n0 = DotGridGradient(x0, y1, x, y);
	n1 = DotGridGradient(x1, y1, x, y);
	ix1 = LineInterpolate(n0, n1, sx);

	value = LineInterpolate(ix0, ix1, sy);
	return value;
}