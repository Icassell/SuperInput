// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EnhancedWheelWidget.h"

#include "EnhancedInputComponent.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UEnhancedWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(!PressedAction)
		PressedAction = CreateInputActionAsset(this);

	BindInputAction();
}

FReply UEnhancedWheelWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);

	XAxisMax = (WheelBackGround->GetBrush().GetImageSize().X * 0.4f);
	YAxisMax = (WheelBackGround->GetBrush().GetImageSize().Y * 0.4f);
	StickRange = FMath::Min(WheelBackGround->GetBrush().GetImageSize().X, WheelBackGround->GetBrush().GetImageSize().Y) * 0.4f;

	FirstMovePosition = InGestureEvent.GetScreenSpacePosition();

	FReply Reply = FReply::Handled();
	if (HasMouseCaptureByUser(InGestureEvent.GetUserIndex(), InGestureEvent.GetPointerIndex()))
	{
		Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	
	if(!PressedAction)
	{
		ReceivedWheelWidgetPressed();
	}
	else
	{
		if (!bStartPressed)
			bStartPressed = true;
	}
	
	bTouchFromStart = true;
	return Reply;
}

FReply UEnhancedWheelWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (!bTouchFromStart)
		return FReply::Unhandled();

	Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
	IsStopSimulation = false;
	HandleTouchDelta(InGeometry, InGestureEvent);
	FReply Reply = FReply::Handled();
	if (!HasMouseCaptureByUser(InGestureEvent.GetUserIndex(), InGestureEvent.GetPointerIndex()))
	{
		Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
	}

	if (LastStickMoveVector != FVector2D::ZeroVector)
		InputKeyValue2D(StickMoveVector - LastStickMoveVector);
	
	LastStickMoveVector = StickMoveVector;
	
	return Reply;
}

FReply UEnhancedWheelWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (!bTouchFromStart)
		return FReply::Unhandled();

	StopInputSimulation();
	ReceivedWheelWidgetReleased();
	return FReply::Handled().ReleaseMouseCapture();
}

void UEnhancedWheelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (WheelForeGround && WheelBackGround)
	{
		FVector2D RenderTranslation = (bNegateYAxis ? FVector2D(1.0f, -1.0f) : FVector2D(1.0f)) * StickRenderVector;
		RenderTranslation.Set(RenderTranslation.X * XAxisMax, RenderTranslation.Y * YAxisMax);
		WheelForeGround->SetRenderTranslation(RenderTranslation);
	}

	if(bStartPressed)
	{
		InjectBoolValueForIA(this,PressedAction,true);
	}
}

void UEnhancedWheelWidget::HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	const FVector2D& ScreenSpacePos = InGestureEvent.GetScreenSpacePosition();

	FVector2D MoveStick = GetWheelValue(ScreenSpacePos - FirstMovePosition);
	StickRenderVector = (MoveStick / StickRange) * FVector2D(1.0f, 1.0f);
	StickMoveVector = bUseForShoot ? ScreenSpacePos : MoveStick;
	LastMovePosition = ScreenSpacePos;
}

void UEnhancedWheelWidget::StopInputSimulation()
{
	FirstMovePosition = FVector2D::ZeroVector;
	FirstMovePosition = FVector2D::ZeroVector;
	StickMoveVector = FVector2D::ZeroVector;
	LastStickMoveVector = FVector2D::ZeroVector;
	StickRenderVector = FVector2D::ZeroVector;
	IsStopSimulation = true;
	bTouchFromStart = false;
	bStartPressed = false;

	InjectBoolValueForIA(this,PressedAction,false);
}

void UEnhancedWheelWidget::InputKeyValue2D(const FVector2D& Value)
{
	const FGamepadKeyNames::Type XAxis = (MainInputKey.IsValid() ? MainInputKey.GetFName() : FGamepadKeyNames::LeftAnalogX);
	const FGamepadKeyNames::Type YAxis = (AltInputKey.IsValid() ? AltInputKey.GetFName() : FGamepadKeyNames::LeftAnalogY);

	FInputDeviceId PrimaryInputDevice = IPlatformInputDeviceMapper::Get().GetPrimaryInputDeviceForUser(FSlateApplicationBase::SlateAppPrimaryPlatformUser);

	if (IsStopSimulation) //停止输入
	{
		FSlateApplication::Get().OnControllerAnalog(XAxis, FSlateApplicationBase::SlateAppPrimaryPlatformUser, PrimaryInputDevice, 0);
		FSlateApplication::Get().OnControllerAnalog(YAxis, FSlateApplicationBase::SlateAppPrimaryPlatformUser, PrimaryInputDevice, 0);
		return;
	}

	if (bUseForShoot)
	{
		// Shoot
		return;
	}

	if (FMath::Abs(Value.X) < FMath::Abs(DeadZone.X) && FMath::Abs(Value.Y) < FMath::Abs(DeadZone.Y)) //防止误触
	{
		FSlateApplication::Get().OnControllerAnalog(XAxis, FSlateApplicationBase::SlateAppPrimaryPlatformUser, PrimaryInputDevice, 0);
		FSlateApplication::Get().OnControllerAnalog(YAxis, FSlateApplicationBase::SlateAppPrimaryPlatformUser, PrimaryInputDevice, 0);
		return;
	}

	FSlateApplication::Get().OnControllerAnalog(XAxis, FSlateApplicationBase::SlateAppPrimaryPlatformUser, PrimaryInputDevice, Value.X);
	FSlateApplication::Get().OnControllerAnalog(YAxis, FSlateApplicationBase::SlateAppPrimaryPlatformUser, PrimaryInputDevice, Value.Y);
	//上面有反转Y轴的设置，这里与SVirtualStick相反
}

FVector2D UEnhancedWheelWidget::GetWheelValue(FVector2D OffsetPosition)
{
	FVector2D MoveStickOffset = OffsetPosition;

	if (bNegateYAxis)
	{
		MoveStickOffset *= FVector2D(1.0f, -1.0f);
	}

	FVector2D MoveStickDir = FVector2D::ZeroVector;
	float MoveStickLength = 0.0f;
	MoveStickOffset.ToDirectionAndLength(MoveStickDir, MoveStickLength);

	MoveStickLength = FMath::Min(MoveStickLength, StickRange);
	MoveStickOffset = MoveStickDir * MoveStickLength;

	return MoveStickOffset;
}

void UEnhancedWheelWidget::BindInputAction()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			PressedHandleIndex = EnhancedInputComponent->BindAction(PressedAction, PressedTriggerEvent, this, &UEnhancedWheelWidget::ReceivedWheelWidgetPressed).GetHandle();
		}
	}
}
