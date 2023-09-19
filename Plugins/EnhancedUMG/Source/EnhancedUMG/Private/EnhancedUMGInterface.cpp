// Fill out your copyright notice in the Description page of Project Settings.


#include "EnhancedUMGInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"


UInputAction* IEnhancedUMGInterface::CreateInputActionAsset(UObject* Outer)
{
	if (UInputAction* IA = NewObject<UInputAction>(Outer,TEXT("Enhanced UMG IA")))
	{
		return IA;
	}

	return nullptr;
}


void IEnhancedUMGInterface::UnBindInputAction(const UObject* WorldContextObject, uint32 ActionHandleIndex)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			EnhancedInputComponent->RemoveBindingByHandle(ActionHandleIndex);
		}
	}
}

void IEnhancedUMGInterface::InjectBoolValueForIA(const UObject* WorldContextObject, UInputAction* InjectAction, bool InjectValue)
{
	if (!WorldContextObject || !InjectAction)
		return;

	if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetLocalPlayer()))
	{
		const FInputActionValue ActionValue(InjectValue);
		LocalPlayerSubsystem->InjectInputForAction(InjectAction, ActionValue, InjectAction->Modifiers, InjectAction->Triggers);
	}
}
