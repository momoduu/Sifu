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

	//tick �Լ��� �� ������ ȣ����� �ʵ��� ����
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

	//1. ���� ���� �ð� ���ϱ�
	float createTime = FMath::RandRange(minTime, maxTime);

	//2. Timer Manager ���� �ϸ� ���
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this,
	&AEnemyManager::CreateEnemy, createTime);

	//SpawnPos ����
	FindSpawnPoint();
	
	//���� �̸� 10�� ������
// 	for (int32 i = 0; i < 5; i++)
// 	{a
// 
// 		//� ��Ȳ�̴� ������ �����Ѵٴ� �ɼ�
// 		FActorSpawnParameters param;
// 		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// 
// 		//���ʹ̸� �����
// 		AHJ_Enemy* e = GetWorld()->SpawnActor<AHJ_Enemy>(enemyFactory, GetActorLocation(), GetActorRotation(), param);
// 		//���� ��Ȱ��ȭ ����
// 		e->SetActive(false);
// 
// 		//������ ���� źâ(�迭)�� ����
// 		enemyArray.Add(e);
// 
// 
// 
// 	}

	//for (int32 i = 0; i < 10; i++)
	//{
	//	//� ��Ȳ�̴� ������ �����Ѵٴ� �ɼ�
	//	FActorSpawnParameters param;
	//	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//	//���ʹ̸� �����
	//	AHJ_Enemy* e = GetWorld()->SpawnActor<AHJ_Enemy>(enemyFactory, GetActorLocation(), GetActorRotation(), param);
	//	//���� ��Ȱ��ȭ ����
	//	e->SetActive(false);

	//	//������ ���� źâ(�迭)�� ����
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
	//���࿡ �� źâ ������ 0�̸� �Լ��� ������
	//if (enemyArray.Num() > 0)



	if (enemynum == 2)
	{
		return;
	}
		
	else
	{

		//���� ��ġ ���ϱ�
		int32 index = FMath::RandRange(0, spawnPoints.Num() - 1);

		//1. ������ YAW �� ������ ( 0 ~ 360
		float yaw = FMath::RandRange(0.0f, 360.0f);
		//2. ���� ���� Spawn Pos Array �� yaw ���� ����
		FRotator rot = FRotator(0, yaw, 0);
		//spawnPoints[spawnindex]->SetActorRotation(FRotator(0, yaw, 0));
		//3. ������ ���� ������ ( 0 ~ 500)
		float length = FMath::RandRange(0.0f, 500.0f);
		//4. spawnPosArray �չ��� * ���� ����
		FVector pos = spawnPoints[spawnindex]->GetActorForwardVector() * length;

		//�� ���� �� ��ġ�ϱ�
		GetWorld()->SpawnActor<AHJ_Enemy>(enemyFactory, spawnPoints[index]->GetActorLocation() + pos, rot);


		//�� ��ġ,ȸ���� ����
		//spawnPoints[spawnindex]->SetActorLocation(spawnPoints[spawnindex]->GetActorLocation() + pos);
		//spawnPoints[spawnindex]->SetActorRotation(spawnPoints[spawnindex]->GetActorRotation());
		//�� źâ���� 0 ��° �� Ȱ��ȭ �� ��ġ ����
// 		enemyArray[0]->SetActive(true);
// 		//enemyArray ���� ����
// 		enemyArray.RemoveAt(0);

		//0 ~ ���� �������
		//���� �ڸ��� ����
		spawnindex++;
		spawnindex = spawnindex % spawnPoints.Num();
		enemynum++;
	}




	//�ٽ� ���� �ð��� creatEnemy �Լ��� ȣ��ǵ��� Ÿ�̸� ����
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
	//���� ���͸� ã�Ƽ� ��������
// 	TArray<AActor*> actors;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);


// 	for (int32 i = 0; i < actors.Num(); i++)
// 	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
// 	{
// 		//���࿡ �̸��� SpawnPos �� ���ԵǾ� �ִٸ�
// 		if (actors[i]->GetName().Contains(TEXT("SpawnPoint")))
// 		{
// 
// 			//spawnPosArray �� �߰��Ѵ�
// 			spawnPoints.Add(actors[i]);
// 		}
// 	}
// 
// 	//���� ���͸� ã�Ƽ� ��������
// 	TArray<AActor*>actors;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
// 	//���࿡ �̸��� spawnPoint �� ���ԵǾ� �ִٸ� 
// 	for (int32 i = 0; i < actors.Num(); i++)
// 		if (actors[i]->GetName().Contains(TEXT("SpawnPoint")))
// 		{	//spawnPoint �� �߰��Ѵ�.
// 			spawnPoints.Add(actors[i]);
// 		}
}