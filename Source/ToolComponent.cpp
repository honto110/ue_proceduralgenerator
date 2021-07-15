#include "ToolComponent.h"

void UToolComponent::OnClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UToolComponent::OnClick()"));
}