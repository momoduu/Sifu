// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BossFSM.h"
#include <Animation/AnimInstance.h>
#include "HJ_BossAnim.generated.h"



UCLASS()
class SIFU_API UHJ_BossAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeBeginPlay() override;

public:

	//상태머신 기억변수

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = FSM)
		EBossState banimState;

	// 		//공격 상태 재생할지 여부
	// 		UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = FSM)
	// 		bool bAttackPlay = false;

			//공격 애니메이션이 끝나는 이벤트 함수
	// 		UFUNCTION(BlueprintCallable, Category = FSMEvent)
	// 		void OnEndAttackAnimation();

			//피격 애니메이션 재생 함수 
	UFUNCTION(BlueprintImplementableEvent,Category = FSMEvent)
				void PlayBossAnim(FName sectionName);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSMComponent)
	class UBossFSM* bfsm;

// 	죽음 상태 애니메이션 종료 여부, BlueprintReadWrite
// 			UPROPERTY(EditDefaultsOnly,Category = FSM)
// 				bool bDieDone = false;

	UFUNCTION()
	void AnimNotify_EndAppear();
};
