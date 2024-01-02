// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EnhancedButton.generated.h"

enum class ETriggerEvent : uint8;
class UInputAction;

UCLASS()
class ENHANCEDUMG_API UEnhancedButton : public UButton
{
	GENERATED_BODY()

protected:
	// UObject interface
	virtual void PostInitProperties() override;
	// End of UObject interface

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void BeginDestroy() override;
	// End of UWidget interface
	
protected:
	void BindButtonActions();
	void RemoveButtonActions();

	void OnClickedIATrigger();
	void OnPressedIATrigger();
	void OnHoveredIATrigger();

	FReply SlateHandleClickedOverride();
	void SlateHandlePressedOverride();
	void SlateHandleReleasedOverride();
	void SlateHandleHoveredOverride();
	void SlateHandleUnhoveredOverride();
	
	TArray<ETriggerEvent> FindEventsForAction(UInputAction* Action);
	void StartInputInjectionForAction(UInputAction* Action);
	void StopInputInjectionForAction(UInputAction* Action);
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ClickedAction;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> PressedAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> HoveredAction;

private:
	TArray<uint32> ButtonActionHandleIndexArray;
};
