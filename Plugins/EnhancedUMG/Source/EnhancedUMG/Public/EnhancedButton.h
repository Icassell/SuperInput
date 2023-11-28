// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EnhancedButton.generated.h"

enum class ETriggerEvent : uint8;
class UInputAction;

/**
 * 
 */
UCLASS()
class ENHANCEDUMG_API UEnhancedButton : public UButton, public FTickableGameObject
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

	// FTickableGameObject interface
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// End of FTickableGameObject interface
	
protected:
	void BindPressedIA();
	void RemovePressedIA();
	void OnPressedIATrigger();

	void InjectInputValue_Bool(UInputAction* InjectAction, bool InjectValue);

	void SlateHandlePressedOverride();
	void SlateHandleReleasedOverride();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> PressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETriggerEvent PressedTriggerEvent;

private:
	uint32 PressedHandleIndex = -1;
	bool bPressedTick = false;
};
