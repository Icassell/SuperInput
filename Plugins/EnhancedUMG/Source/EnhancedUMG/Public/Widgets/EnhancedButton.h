// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedUMGInterface.h"
#include "InputTriggers.h"
#include "Components/Button.h"
#include "Slate/SEnhancedButton.h"
#include "EnhancedButton.generated.h"


/**
 * 
 */
UCLASS()
class ENHANCEDUMG_API UEnhancedButton : public UButton,public IEnhancedUMGInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void SlateHandlePressedOverride();

protected:
	// Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface 
	
	// UUserWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UUserWidget interface

	virtual FReply SlateHandleClickedOverride();
	virtual void SlateHandleReleasedOverride();

	virtual void BindInputAction() override;

	UFUNCTION(Blueprintable)
	void EnhancedTriggerPressed();

	UFUNCTION(Blueprintable)
	void InjectPressedDelegate();

	
protected:
	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite)
	TObjectPtr<UInputAction> PressedAction;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ETriggerEvent PressedTriggerEvent;
	
	uint32 PressedHandleIndex;
	TSharedPtr<SEnhancedButton> EnhancedButton;
	FTimerHandle HoldHandle;
	bool EndDelegate;
};
