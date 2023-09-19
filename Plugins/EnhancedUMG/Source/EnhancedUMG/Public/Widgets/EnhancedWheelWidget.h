// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedUMGInterface.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedWheelWidget.generated.h"

enum class ETriggerEvent : uint8;
class UImage;
/**
 * 
 */
UCLASS()
class ENHANCEDUMG_API UEnhancedWheelWidget : public UUserWidget, public IEnhancedUMGInterface
{
	GENERATED_BODY()

public:
	// Begin UUserWidget
	virtual void NativeConstruct() override;
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface

protected:
	void HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent);
	void StopInputSimulation();
	void InputKeyValue2D(const FVector2D& Value);

	UFUNCTION(BlueprintImplementableEvent)
	void ReceivedWheelWidgetPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceivedWheelWidgetReleased();

	FVector2D GetWheelValue(FVector2D OffsetPosition);

	virtual void BindInputAction() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FKey MainInputKey = EKeys::Gamepad_LeftX;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FKey AltInputKey = EKeys::Gamepad_LeftY;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* WheelBackGround;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* WheelForeGround;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bNegateYAxis = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseForShoot = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin="0", ClampMax="1"))
	FVector DeadZone;

	UPROPERTY(Transient)
	FVector2D FirstMovePosition = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	FVector2D LastMovePosition = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	FVector2D StickMoveVector = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	FVector2D LastStickMoveVector = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	FVector2D StickRenderVector = FVector2D::ZeroVector;

	float StickRange = 0.f;
	float XAxisMax = 0.f;
	float YAxisMax = 0.f;
	bool IsStopSimulation = true;
	bool bTouchFromStart = false;

	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite)
	TObjectPtr<UInputAction> PressedAction;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ETriggerEvent PressedTriggerEvent;

	uint32 PressedHandleIndex;
	FTimerHandle HoldHandle;
	bool bStartPressed;
};
