// Fill out your copyright notice in the Description page of Project Settings.


#include "EnhancedButton.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/ButtonSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Input/SButton.h"

void UEnhancedButton::PostInitProperties()
{
	Super::PostInitProperties();

	BindPressedIA();
}

TSharedRef<SWidget> UEnhancedButton::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MyButton = SNew(SButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, UEnhancedButton::SlateHandlePressedOverride)) //替换基类slate绑定回调
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, UEnhancedButton::SlateHandleReleasedOverride)) //替换基类slate绑定回调
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	return MyButton.ToSharedRef();
}

void UEnhancedButton::BeginDestroy()
{
	Super::BeginDestroy();

	RemovePressedIA();
}

bool UEnhancedButton::IsTickable() const
{
	if (MyButton.IsValid() && IsVisible())
	{
		return true;
	}
	
	return false;
}

void UEnhancedButton::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Log, TEXT("Button Ticking!"));
	InjectInputValue_Bool(PressedAction, bPressedTick);
}

TStatId UEnhancedButton::GetStatId() const
{
	return Super::GetStatID();
}

void UEnhancedButton::BindPressedIA()
{
	if (!PressedAction) return;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			PressedHandleIndex = EnhancedInputComponent->BindAction(PressedAction, PressedTriggerEvent, this, &UEnhancedButton::OnPressedIATrigger).GetHandle();
		}
	}
}

void UEnhancedButton::RemovePressedIA()
{
	if (!PressedAction) return;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			EnhancedInputComponent->RemoveBindingByHandle(PressedHandleIndex);
		}
	}
}

void UEnhancedButton::OnPressedIATrigger()
{
	//真正调用回调的部分
	OnPressed.Broadcast();

	//手动触发IA的值变化
	bPressedTick = false;
	InjectInputValue_Bool(PressedAction, bPressedTick);
}

void UEnhancedButton::InjectInputValue_Bool(UInputAction* InjectAction, bool InjectValue)
{
	if (!InjectAction) return;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Sys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				const FInputActionValue ActionValue(InjectValue);
				Sys->InjectInputForAction(InjectAction, ActionValue, InjectAction->Modifiers, InjectAction->Triggers);
			}
		}
	}
}

void UEnhancedButton::SlateHandlePressedOverride()
{
	if (!PressedAction || PressedHandleIndex == -1)
	{
		Super::SlateHandlePressed();
	}
	else
	{
		//Begin Tick
		bPressedTick = true;
		InjectInputValue_Bool(PressedAction, bPressedTick);
	}
}

void UEnhancedButton::SlateHandleReleasedOverride()
{
	Super::SlateHandleReleased();

	if (PressedAction)
	{
		//End Tick
		bPressedTick = false;
		InjectInputValue_Bool(PressedAction, bPressedTick);
	}
}
