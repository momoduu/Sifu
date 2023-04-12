// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>
// #include "HJ_Boss.h"
// #include "HJ_BossAnim.h"
// #include <AIModule/Classes/AIController.h>
// #include <Engine/EngineBaseTypes.h>
// #include "Player_KYI.h"
#include "BossFSM.generated.h"

//����� ���� ���� 
UENUM(BlueprintType)
enum class EBossState :uint8
{
	Appear,
	Idle,
	Move,
	Attack,
	Damage,
	Die,
	ReturnPos
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIFU_API UBossFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBossFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
public:
	//���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)// VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EBossState bState = EBossState::Appear;
	//���� ����
	void AppearState();
	//��� ����
	void IdleState();
	//�̵� ����
	void MoveState();
	//���ݻ���
	void AttackState();
	//�ǰ� ����
	void DamageState();
	//��������
	UFUNCTION(BlueprintCallable)
	void DieState();
	//����
	void ReturnPosState();
	//��Ÿ�� ��� �ִ��Լ�		
	UFUNCTION(BlueprintCallable)
	void BossAnim(int32 attackIdx);
	//Ÿ���� �i�� �� �� �ִ�?
	bool IsTargetTrace();
	

	//��� �ð�
	UPROPERTY(EditDefaultsOnly, Category = FSM)
		float idleDelayTime = 2;

	//��� �ð�
	float currentTime = 0;
	//���� �ð�
	float createTime = 4;
	//�Ѿ� �� �� �ִ� ����
	float traceRange = 800;

	//�̵��� �� �ִ� �ݰ�
	float moveRange = 1000;

	//ó�� ��ġ�� ��� ���� ����
	FVector originPos;

	//Ÿ��
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class APlayer_KYI* target;

	//���� ����
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 100.0f;
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDamage = 10;

	//���� ��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;
	UFUNCTION(BlueprintCallable)
		void OnBossProcess(float damage, int32 animIdx);
		
	//ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		float maxHP = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		float currHP;
	//�ǰ� ��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
		float damageDelayTime = 1.0f;

	//�Ʒ��� ������� �ӵ�
	UPROPERTY(EditAnywhere, Category = FSM)
		float dieSpeed = 50.0f;

	//��� ���� �ִϸ��̼� �������Ʈ
	UPROPERTY()
		class UHJ_BossAnim* banim;

	//���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHJ_Boss* boss;

	// 	Enemy �� �����ϰ� �ִ� AIController
	UPROPERTY()
		class AAIController* bi;

	//�� ã�� ����� ���� ��ġ
	FVector randomPos;
	//���� ��ġ ��������
	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);

	UFUNCTION(BlueprintCallable)
	void OnAppearStart();
	bool bAppear = false;
	void SetBoss();
	void FinishAppear();
};
