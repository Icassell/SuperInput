// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnhancedUMGInterface.generated.h"


class UInputAction;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UEnhancedUMGInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ENHANCEDUMG_API IEnhancedUMGInterface
{
	GENERATED_BODY()

public:
	UInputAction* CreateInputActionAsset(UObject* Outer);

	virtual void BindInputAction() = 0;
	virtual void UnBindInputAction(const UObject* WorldContextObject, uint32 ActionHandleIndex);
	virtual void InjectBoolValueForIA(const UObject* WorldContextObject, UInputAction* InjectAction, bool InjectValue);
};
