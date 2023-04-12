// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Player_KYI.generated.h"

UCLASS()
class SIFU_API APlayer_KYI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayer_KYI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//hj 가 한출 추가
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Components
	//Spring Arm
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		class USpringArmComponent* springArmComp;
	//Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		class UCameraComponent* camComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class USphereComponent* leftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float handDamage = 5;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class USphereComponent* leftLeg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float legDamage = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class USphereComponent* rightLeg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float finishDamage = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		float doOnce = true;


	////공격중 이동 중지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool movementEnabled = true;
	//이동속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float walkSpeed = 600;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isBlocking = false;
	//이동방향
	FVector direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* deathSound;
	//피격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* stomach;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* head2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* head3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* head4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* falldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* death;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* block;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* punch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* jab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* uppercut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* kick;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* highKick;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* lowKick;
	
	//피니시 타격
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* finish;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* finishVic;
	
	////playerUI 팩토리
	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<class UUserWidget> playerUIFactory;
	////플레이어 HP UI
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UUserWidget* playerUI;
	//좌우 이동입력 이벤트 처리함수
	void Turn(float value);
	void LookUp(float value);
	void InputHorizontal(float value);
	void InputVertical(float value);
	void InputJump();
	void InputRun(bool run);
	void AttackPunch();
	void AttackKick();
	void InputFinish();
	UFUNCTION(BlueprintCallable)
		void saveAttackCombo();
	UFUNCTION(BlueprintCallable)
		void punchCombo();
	UFUNCTION(BlueprintCallable)
		void kickCombo();
	UFUNCTION(BlueprintCallable)
		void ResetCombo();
	UFUNCTION(BlueprintCallable)
		void PlayerBlock(bool value);
	//UFUNCTION(BlueprintCallable)
	//	void setTarget();
	//공격받은 애니메이션 플레이 함수
	UFUNCTION(BlueprintCallable)
		void HurtAnim0();
	UFUNCTION(BlueprintCallable)
		void HurtAnim1();
	UFUNCTION(BlueprintCallable)
		void HurtAnim2();
	UFUNCTION(BlueprintCallable)
		void HurtAnim3();
	UFUNCTION(BlueprintCallable)
		void HurtAnim4();

	//HJ 가 수정
	UFUNCTION(BlueprintCallable)
		void OnHitDamage(float damage);
	UFUNCTION(BlueprintCallable)
		void PlayerDamage();
	UFUNCTION(BlueprintCallable)
		void PlayerDie();

	//타게팅 된 적
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHJ_Enemy* targetEnemy;
	//타게팅 된 보스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AHJ_Boss* targetBoss;
	//타겟 여부 확인
	UPROPERTY(EditAnywhere)
		bool targetLock = false;

	//체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float currHp = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float maxHp = 100;
	//죽음 확인
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isDead = false;
	//아래로 사라지는 속도
	UPROPERTY(EditAnywhere)
		float PlayerdieSpeed = 50.0f;

	//공격 콤보
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int punchCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool saveAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int kickCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool kickorPunch = true;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
