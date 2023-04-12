// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ_Boss.h"
#include "BossFSM.h"
#include <GameFramework/Character.h>
#include "Components/PrimitiveComponent.h"
#include <Components/InputComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

// Sets default values
AHJ_Boss::AHJ_Boss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1.스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/HJ_other/Boss/goblin_d_shareyko.goblin_d_shareyko'"));;
	//1-1.데이터 로드 성공하면
	if (tempMesh.Succeeded())
	{
		//1-2. 데이터 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		//1-3.메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}
	//EnemyFSM 컴포넌트 추가
	bfsm = CreateDefaultSubobject<UBossFSM>(TEXT("BFSM"));

	//애니메이션 블루프린트 할당하기
	ConstructorHelpers::FClassFinder<UAnimInstance>tempClass(TEXT("AnimBlueprint'/Game/Blueprint/HJ_BossAnimBP.HJ_BossAnimBP_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}
	//월드에 배치되거나 스폰될 때 자동으로
	//AIController 부터 Possess 될 수 있도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AHJ_Boss::BeginPlay()
{
	Super::BeginPlay();
	//움직이는 방향으로 몸을 자동으로 회전하라는 옵션
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
}

// Called every frame
void AHJ_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHJ_Boss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHJ_Boss::SetActive(bool bActive)
{
	//만약에 bActive 가 true 라면 /활성화
	if (bActive)
	{
		//충돌활성
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//생성 위치 재설정
		bfsm->originPos = GetActorLocation();

	}
	//비활성화
	else
	{
		//충돌 안되게 셋팅
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//메쉬 비활성화
	GetMesh()->SetActive(bActive);
	//메쉬를 보이게  / 안보이게
	GetMesh()->SetVisibility(bActive);
	//캐릭터 무브먼트 활성 /비활성
	GetCharacterMovement()->SetActive(bActive);
	//fsm 활성 /비활성
	bfsm->SetActive(bActive);
}

