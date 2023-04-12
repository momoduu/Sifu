// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "HJ_EnemyAnim.generated.h"


UCLASS()
class SIFU_API UHJ_EnemyAnim : public UAnimInstance
{
	GENERATED_BODY()



public:

//���¸ӽ� ��ﺯ��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
		EEnemyState animState;

// 		//���� ���� ������� ����
// 		UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = FSM)
// 		bool bAttackPlay = false;

		//���� �ִϸ��̼��� ������ �̺�Ʈ �Լ�
// 		UFUNCTION(BlueprintCallable, Category = FSMEvent)
// 		void OnEndAttackAnimation();

 		//�ǰ� �ִϸ��̼� ��� �Լ�
		UFUNCTION(BlueprintImplementableEvent,Category=FSMEvent)
		void PlayDamageAnim(FName sectionName);
		
		//���� ���� �ִϸ��̼� ���� ����
		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category=FSM)
		bool bDieDone = false;
};
