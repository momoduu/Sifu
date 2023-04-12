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

	//���¸ӽ� ��ﺯ��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = FSM)
		EBossState banimState;

	// 		//���� ���� ������� ����
	// 		UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = FSM)
	// 		bool bAttackPlay = false;

			//���� �ִϸ��̼��� ������ �̺�Ʈ �Լ�
	// 		UFUNCTION(BlueprintCallable, Category = FSMEvent)
	// 		void OnEndAttackAnimation();

			//�ǰ� �ִϸ��̼� ��� �Լ� 
	UFUNCTION(BlueprintImplementableEvent,Category = FSMEvent)
				void PlayBossAnim(FName sectionName);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSMComponent)
	class UBossFSM* bfsm;

// 	���� ���� �ִϸ��̼� ���� ����, BlueprintReadWrite
// 			UPROPERTY(EditDefaultsOnly,Category = FSM)
// 				bool bDieDone = false;

	UFUNCTION()
	void AnimNotify_EndAppear();
};
