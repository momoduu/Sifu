// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HJ_Enemy.h"
#include "EnemyManager.generated.h"

UCLASS()
class SIFU_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// 	생성 시간
// 		float createTime = 0;
// 		//현재 시간 
// 		float currTime = 0;
	//스폰할 위치 정보 배열

	//적 생성 위치들
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TArray<class AActor*>  spawnPoints;

// 	enemy 탄창
// 		UPROPERTY(EditAnywhere, Category = SpawnSettings)
// 		TArray<class AHJ_Enemy*> enemyArray;

	//enemy 공장
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class AHJ_Enemy> enemyFactory;

	//랜덤 시간 간격 최소값
	UPROPERTY(EditAnywhere,Category = SpawnSettings)
	float minTime = 1;

	//랜덤 시간 간격 최대값 
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	float maxTime = 5;

	//스폰을 위한 알람 타이머
	FTimerHandle spawnTimerHandle;

	//적 생성 함수
	void CreateEnemy();

public:
	int32 spawnindex = 0;
	void FindSpawnPoint();

	int32 enemynum = 0;
};
