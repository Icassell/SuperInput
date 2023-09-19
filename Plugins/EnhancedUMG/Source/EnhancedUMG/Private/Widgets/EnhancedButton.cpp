// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EnhancedButton.h"

#include "EnhancedInputComponent.h"
#include "Components/ButtonSlot.h"
#include "Kismet/GameplayStatics.h"

void UEnhancedButton::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	EndDelegate = false;
	UnBindInputAction(this,PressedHandleIndex);
	EnhancedButton.Reset();
}

TSharedRef<SWidget> UEnhancedButton::RebuildWidget()
{
	MyButton = EnhancedButton =
		SNew(SEnhancedButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClickedOverride))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressedOverride))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleasedOverride))
	    .ButtonStyle(&GetStyle())
		.ClickMethod(GetClickMethod())
		.PressMethod(GetPressMethod())
		.IsFocusable(false);

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	if (!PressedAction)
		PressedAction = CreateInputActionAsset(this);

	BindInputAction();
	
	return EnhancedButton.ToSharedRef();
}

FReply UEnhancedButton::SlateHandleClickedOverride()
{
	return Super::SlateHandleClicked();
}

void UEnhancedButton::SlateHandlePressedOverride()
{
	if (!PressedAction)
	{
		Super::SlateHandlePressed();
	}
	else
	{
		EndDelegate = false;
		GetWorld()->GetTimerManager().SetTimer(HoldHandle, FTimerDelegate::CreateUObject(this, &UEnhancedButton::InjectPressedDelegate), 0.01f, true);
	}
}

void UEnhancedButton::SlateHandleReleasedOverride()
{
	EndDelegate = true;
	GetWorld()->GetTimerManager().ClearTimer(HoldHandle);
	
	Super::SlateHandleReleased();

	InjectBoolValueForIA(this,PressedAction,false);
}

void UEnhancedButton::BindInputAction()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			PressedHandleIndex = EnhancedInputComponent->BindAction(PressedAction, PressedTriggerEvent, this, &UEnhancedButton::EnhancedTriggerPressed).GetHandle();
		}
	}
}

void UEnhancedButton::EnhancedTriggerPressed()
{
	EndDelegate = true;
	OnPressed.Broadcast();

	InjectBoolValueForIA(this,PressedAction,false);
}

void UEnhancedButton::InjectPressedDelegate()
{
	if (EndDelegate)
	{
		GetWorld()->GetTimerManager().ClearTimer(HoldHandle);
		return;
	}

	InjectBoolValueForIA(this,PressedAction,true);
}
