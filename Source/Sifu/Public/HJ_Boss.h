// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HJ_Boss.generated.h"

UCLASS()
class SIFU_API AHJ_Boss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHJ_Boss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
		class UBossFSM* bfsm;

	void SetActive(bool bActive);
};
