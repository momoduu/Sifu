// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "HJ_Enemy.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Actor.h>
#include <EngineUtils.h>
#include <Engine/World.h>

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//tick 함수가 매 프레임 호출되지 않도록 설정
	PrimaryActorTick.bCanEverTick = false;

// 	ConstructorHelpers::FClassFinder<class AHJ_Enemy>tempEnemy(TEXT("Blueprint'/Game/Blueprint/BP_HJ_Enemy.BP_HJ_Enemy_C'"));
// 		if (tempEnemy.Succeeded())
// 		{
// 			enemyFactory = tempEnemy.Class;
// 		}


}
	



// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	//1. 랜덤 생성 시간 구하기
	float createTime = FMath::RandRange(minTime, maxTime);

	//2. Timer Manager 한테 일림 등록
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this,
	&AEnemyManager::CreateEnemy, createTime);

	//SpawnPos 셋팅
	FindSpawnPoint();
	
	//적을 미리 10개 만들자
// 	for (int32 i = 0; i < 5; i++)
// 	{a
// 
// 		//어떤 상황이던 무조건 생성한다는 옵션
// 		FActorSpawnParameters param;
// 		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// 
// 		//에너미를 만든다
// 		AHJ_Enemy* e = GetWorld()->SpawnActor<AHJ_Enemy>(enemyFactory, GetActorLocation(), GetActorRotation(), param);
// 		//적을 비활성화 하자
// 		e->SetActive(false);
// 
// 		//생성된 적을 탄창(배열)에 담자
// 		enemyArray.Add(e);
// 
// 
// 
// 	}

	//for (int32 i = 0; i < 10; i++)
	//{
	//	//어떤 상황이던 무조건 생성한다는 옵션
	//	FActorSpawnParameters param;
	//	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//	//에너미를 만든다
	//	AHJ_Enemy* e = GetWorld()->SpawnActor<AHJ_Enemy>(enemyFactory, GetActorLocation(), GetActorRotation(), param);
	//	//적을 비활성화 하자
	//	e->SetActive(false);

	//	//생성된 적을 탄창(배열)에 담자
	//	enemyArray.Add(e);
	//}

	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

void  AEnemyManager::CreateEnemy()
{
	//만약에 적 탄창 갯수가 0이면 함수를 나가자
	//if (enemyArray.Num() > 0)



	if (enemynum == 2)
	{
		return;
	}
		
	else
	{

		//랜덤 위치 구하기
		int32 index = FMath::RandRange(0, spawnPoints.Num() - 1);

		//1. 랜덤한 YAW 값 구하자 ( 0 ~ 360
		float yaw = FMath::RandRange(0.0f, 360.0f);
		//2. 구한 값을 Spawn Pos Array 의 yaw 값에 셋팅
		FRotator rot = FRotator(0, yaw, 0);
		//spawnPoints[spawnindex]->SetActorRotation(FRotator(0, yaw, 0));
		//3. 랜덤한 길이 구하자 ( 0 ~ 500)
		float length = FMath::RandRange(0.0f, 500.0f);
		//4. spawnPosArray 앞방향 * 랜덤 길이
		FVector pos = spawnPoints[spawnindex]->GetActorForwardVector() * length;

		//적 생성 및 배치하기
		GetWorld()->SpawnActor<AHJ_Enemy>(enemyFactory, spawnPoints[index]->GetActorLocation() + pos, rot);


		//적 위치,회전값 셋팅
		//spawnPoints[spawnindex]->SetActorLocation(spawnPoints[spawnindex]->GetActorLocation() + pos);
		//spawnPoints[spawnindex]->SetActorRotation(spawnPoints[spawnindex]->GetActorRotation());
		//적 탄창에서 0 번째 놈 활성화 및 위치 셋팅
// 		enemyArray[0]->SetActive(true);
// 		//enemyArray 에서 빼자
// 		enemyArray.RemoveAt(0);

		//0 ~ 갯수 순서대로
		//다음 자리로 설정
		spawnindex++;
		spawnindex = spawnindex % spawnPoints.Num();
		enemynum++;
	}




	//다시 랜덤 시간에 creatEnemy 함수가 호출되도록 타이머 설정
	float createTime = FMath::RandRange(minTime, maxTime);

	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this,
		&AEnemyManager::CreateEnemy, createTime);

}

void  AEnemyManager::FindSpawnPoint()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* spawn = *It;
		if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
		{
			spawnPoints.Add(spawn);
		}
	}
	//스폰 액터를 찾아서 가져오자
// 	TArray<AActor*> actors;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);


// 	for (int32 i = 0; i < actors.Num(); i++)
// 	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
// 	{
// 		//만약에 이름에 SpawnPos 가 포함되어 있다면
// 		if (actors[i]->GetName().Contains(TEXT("SpawnPoint")))
// 		{
// 
// 			//spawnPosArray 에 추가한다
// 			spawnPoints.Add(actors[i]);
// 		}
// 	}
// 
// 	//스폰 액터를 찾아서 가져오자
// 	TArray<AActor*>actors;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
// 	//만약에 이름에 spawnPoint 가 포함되어 있다면 
// 	for (int32 i = 0; i < actors.Num(); i++)
// 		if (actors[i]->GetName().Contains(TEXT("SpawnPoint")))
// 		{	//spawnPoint 에 추가한다.
// 			spawnPoints.Add(actors[i]);
// 		}
}