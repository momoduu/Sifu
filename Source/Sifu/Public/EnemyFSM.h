// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "EnemyFSM.generated.h"

//����� ���� ���� 
UENUM(BlueprintType)
enum class EEnemyState :uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
	ReturnPos
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIFU_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)// VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;

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

	//�ǰ� �� ��� �ִ��Լ�		
	UFUNCTION(BlueprintCallable)
	void DamageAnim(int32 attackIdx);

	//Ÿ���� �i�� �� �� �ִ�?
	bool IsTargetTrace();


	//���� �ð� ��ٷȴٰ� �̵� ���·� ��ȯ�ϰ� �ʹ�.
	//��� �ð�
	UPROPERTY(EditDefaultsOnly, Category = FSM)
		float idleDelayTime = 2;

	//��� �ð�
	float currentTime = 0;

	//�Ѿ� �� �� �ִ� ����
	float traceRange = 800;

	//�̵��� �� �ִ� �ݰ�
	float moveRange = 1000;

	//ó�� ��ġ�� ��� ���� ����
	FVector originPos;

	//�����Ҹ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* deathSound;

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
		void OnDamageProcess(float damage, int32 animIdx);

	//ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		float maxHP = 100;
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
		class UHJ_EnemyAnim* anim;

	//���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHJ_Enemy* me;

	// 	Enemy �� �����ϰ� �ִ� AIController
	UPROPERTY()
		class AAIController* ai;

	//�� ã�� ����� ���� ��ġ
	FVector randomPos;
	//���� ��ġ ��������
	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);
};