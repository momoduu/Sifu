// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "EnemyFSM.generated.h"

//사용자 상태 정의 
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
	//상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)// VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;

	//대기 상태
	void IdleState();
	//이동 상태
	void MoveState();
	
	//공격상태
	void AttackState();
	//피격 상태
	void DamageState();
	//죽음상태
	UFUNCTION(BlueprintCallable)
		void DieState();
	//리턴
	void ReturnPosState();

	//피격 시 재생 애님함수		
	UFUNCTION(BlueprintCallable)
	void DamageAnim(int32 attackIdx);

	//타겟을 쫒아 갈 수 있니?
	bool IsTargetTrace();


	//일정 시간 기다렸다가 이동 상태로 전환하고 싶다.
	//대기 시간
	UPROPERTY(EditDefaultsOnly, Category = FSM)
		float idleDelayTime = 2;

	//경과 시간
	float currentTime = 0;

	//쫓아 갈 수 있는 범위
	float traceRange = 800;

	//이동할 수 있는 반경
	float moveRange = 1000;

	//처음 위치를 담아 놓을 변수
	FVector originPos;

	//죽음소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* deathSound;

	//타깃
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class APlayer_KYI* target;

	//공격 범위
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 100.0f;
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDamage = 10;

	//공격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;

	UFUNCTION(BlueprintCallable)
		void OnDamageProcess(float damage, int32 animIdx);

	//체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		float maxHP = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		float currHP;
	//피격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
		float damageDelayTime = 1.0f;

	//아래로 사라지는 속도
	UPROPERTY(EditAnywhere, Category = FSM)
		float dieSpeed = 50.0f;

	//사용 중인 애니메이션 블루프린트
	UPROPERTY()
		class UHJ_EnemyAnim* anim;

	//소유 액터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHJ_Enemy* me;

	// 	Enemy 를 소유하고 있는 AIController
	UPROPERTY()
		class AAIController* ai;

	//길 찾기 수행시 랜덤 위치
	FVector randomPos;
	//랜덤 위치 가져오기
	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);
};