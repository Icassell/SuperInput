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

	BindButtonActions();
}

TSharedRef<SWidget> UEnhancedButton::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MyButton = SNew(SButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, UEnhancedButton::SlateHandleClickedOverride))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, UEnhancedButton::SlateHandlePressedOverride))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, UEnhancedButton::SlateHandleReleasedOverride))
		.OnHovered_UObject(this, &UEnhancedButton::SlateHandleHoveredOverride)
		.OnUnhovered_UObject(this, &UEnhancedButton::SlateHandleUnhoveredOverride)
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

	RemoveButtonActions();
}

void UEnhancedButton::BindButtonActions()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			for (auto Event : this->FindEventsForAction(ClickedAction))
			{
				uint32 Index = EnhancedInputComponent->BindAction(ClickedAction, Event, this, &UEnhancedButton::OnClickedIATrigger).GetHandle();
				if (Index != 0)
				{
					ButtonActionHandleIndexArray.Add(Index);
				}
			}
			for (auto Event : this->FindEventsForAction(PressedAction))
			{
				uint32 Index = EnhancedInputComponent->BindAction(PressedAction, Event, this, &UEnhancedButton::OnPressedIATrigger).GetHandle();
				if (Index != 0)
				{
					ButtonActionHandleIndexArray.Add(Index);
				}
			}
			for (auto Event : this->FindEventsForAction(HoveredAction))
			{
				uint32 Index = EnhancedInputComponent->BindAction(HoveredAction, Event, this, &UEnhancedButton::OnHoveredIATrigger).GetHandle();
				if (Index != 0)
				{
					ButtonActionHandleIndexArray.Add(Index);
				}
			}
		}
	}
}

void UEnhancedButton::RemoveButtonActions()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			for (auto Index : ButtonActionHandleIndexArray)
			{
				EnhancedInputComponent->RemoveBindingByHandle(Index);
			}
		}
	}
}

void UEnhancedButton::OnClickedIATrigger()
{
	Super::SlateHandleClicked();
	
	this->StopInputInjectionForAction(ClickedAction);
}

void UEnhancedButton::OnPressedIATrigger()
{
	Super::SlateHandlePressed();
	
	this->StopInputInjectionForAction(PressedAction);
}

void UEnhancedButton::OnHoveredIATrigger()
{
	Super::SlateHandleHovered();

	this->StopInputInjectionForAction(HoveredAction);
}

FReply UEnhancedButton::SlateHandleClickedOverride()
{
	if (ClickedAction == nullptr)
	{
		return Super::SlateHandleClicked();
	}

	this->StartInputInjectionForAction(ClickedAction);

	return FReply::Handled();
}

void UEnhancedButton::SlateHandlePressedOverride()
{
	if (PressedAction == nullptr)
	{
		return Super::SlateHandlePressed();
	}

	this->StartInputInjectionForAction(PressedAction);
}

void UEnhancedButton::SlateHandleReleasedOverride()
{
	Super::SlateHandleReleased();

	this->StopInputInjectionForAction(PressedAction);
}

void UEnhancedButton::SlateHandleHoveredOverride()
{
	if (HoveredAction == nullptr)
	{
		return Super::SlateHandleHovered();
	}

	this->StartInputInjectionForAction(HoveredAction);
}

void UEnhancedButton::SlateHandleUnhoveredOverride()
{
	Super::SlateHandleUnhovered();

	this->StopInputInjectionForAction(HoveredAction);
}

TArray<ETriggerEvent> UEnhancedButton::FindEventsForAction(UInputAction* Action)
{
	TArray<ETriggerEvent> Events;
	if (Action == nullptr) return Events;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			for (auto& Binding : EnhancedInputComponent->GetActionEventBindings())
			{
				if (Binding->GetAction() == Action)
				{
					Events.Add(Binding->GetTriggerEvent());
				}
			}
		}
	}

	//AGamePlayerController::ReBindEvents()
	if(Events.IsEmpty())
		Events.Add(ETriggerEvent::Triggered);
	
	return Events;
}

void UEnhancedButton::StartInputInjectionForAction(UInputAction* Action)
{
	if (Action)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Sys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					Sys->StartContinuousInputInjectionForAction(Action, FInputActionValue(true), Action->Modifiers, Action->Triggers);
				}
			}
		}
	}
}

void UEnhancedButton::StopInputInjectionForAction(UInputAction* Action)
{
	if (Action)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Sys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					Sys->StopContinuousInputInjectionForAction(Action);
				}
			}
		}
	}
}
