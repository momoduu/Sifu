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

// 	���� �ð�
// 		float createTime = 0;
// 		//���� �ð� 
// 		float currTime = 0;
	//������ ��ġ ���� �迭

	//�� ���� ��ġ��
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TArray<class AActor*>  spawnPoints;

// 	enemy źâ
// 		UPROPERTY(EditAnywhere, Category = SpawnSettings)
// 		TArray<class AHJ_Enemy*> enemyArray;

	//enemy ����
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class AHJ_Enemy> enemyFactory;

	//���� �ð� ���� �ּҰ�
	UPROPERTY(EditAnywhere,Category = SpawnSettings)
	float minTime = 1;

	//���� �ð� ���� �ִ밪 
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	float maxTime = 5;

	//������ ���� �˶� Ÿ�̸�
	FTimerHandle spawnTimerHandle;

	//�� ���� �Լ�
	void CreateEnemy();

public:
	int32 spawnindex = 0;
	void FindSpawnPoint();

	int32 enemynum = 0;
};
