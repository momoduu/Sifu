// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSM.h"
#include <Kismet/GameplayStatics.h>
#include "Player_KYI.h"
#include "HJ_Boss.h"
#include <Components/CapsuleComponent.h>
#include "HJ_BossAnim.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <AIModule/Classes/AIController.h>
#include <NavigationSystem.h>

// #include <CollisionQueryParams.h>
// #include <Engine/EngineTypes.h>
// #include <AIModule/Classes/Navigation/PathFollowingComponent.h>
// 
// #include <NavigationSystemTypes.h>



// Sets default values for this component's properties
UBossFSM::UBossFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBossFSM::BeginPlay()
{
	Super::BeginPlay();

	//���忡�� APlayer_KYI Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayer_KYI::StaticClass());

	//APlayer_KYI Ÿ������ ĳ����
	target = Cast<APlayer_KYI>(actor);

	
	//���� ��ü ��������
	boss = Cast<class AHJ_Boss>(GetOwner());                 
	boss->GetCharacterMovement()->MaxWalkSpeed = 300;
	//UEnemyAnim* �Ҵ�

	banim = Cast<UHJ_BossAnim>(boss->GetMesh()->GetAnimInstance());

	//compProjectile=CreateDefaultSubobject< UProjectileMovementComponent>(TEXT("compProject"));

	//���� �ʱ� ü���� ��������
	currHP = maxHP;

	//ó�� ��ġ ����
	originPos = boss->GetActorLocation();

	//AIController �Ҵ��ϱ�
	bi = Cast<AAIController>(boss->GetController());

	
}

void UBossFSM::OnAppearStart()
{
	bAppear = true;
	banim->PlayBossAnim(TEXT("Attack0"));

//  	bState = EBossState::Appear;
//   	banim->banimState = bState;
}


// Called every frame
void UBossFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAppear == true)
	{
		switch (bState)
		{
		case EBossState::Appear:
			AppearState();
			break;
		case EBossState::Idle:
			IdleState();
			break;
		case EBossState::Move:
			MoveState();
			break;
		case EBossState::Attack:
			AttackState();
			break;
		case EBossState::Damage:
			DamageState();
			break;
		case EBossState::Die:
			DieState();
			break;
		case EBossState::ReturnPos:
			ReturnPosState();
			break;
		}
	}

}

void UBossFSM::AppearState()
{
	UE_LOG(LogTemp,Warning,TEXT("2222222222222"));

	//bi->MoveToLocation(target->GetActorLocation());
// 	������ �̵��ϴ� �ڵ�
//  		FVector Jumpdir = target->GetActorLocation() - boss->GetActorLocation();
//  		FVector destination = target->GetActorLocation();
// 		boss->SetActorLocation(Jumpdir);
// 	
// 		bState = EBossState::Idle;
// 		banim->banimState = bState;
}

void UBossFSM::SetBoss()
{
 	FVector pos = boss->GetActorLocation();
  	pos.X = -2160.000000;
 	pos.Y = -6812.000000;
	pos.Z = 2333.060439;
 	boss->SetActorLocation(pos);

	//FinishAppear();
}

void UBossFSM::FinishAppear()
{
	if (bState == EBossState::Appear)
	{
		// ����FSM �� Idle �� ���¸� �ٲ��
		bi->StopMovement();
		bState = EBossState::Idle;
		banim->StopAllMontages(0);
		SetBoss();
	}
}


//��� ����
void UBossFSM::IdleState()
{

	//���࿡ �÷��̾ �Ѿ� �� �� �ִ�?
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > createTime)
	{
		// 		if (IsTargetTrace())
		// 		{
		// 			//���¸� Move �� ��ȯ
		// 			bState = EBossState::Move;
		// 			banim->banimState = bState;
			// 
			// 
			// 		}


			//���࿡ Player �� �Ѿ� �� �� �ִ�? ( �� �þ߿� ���̸�)
			//currentTime += GetWorld()->DeltaTimeSeconds;
			//if (currentTime > idleDelayTime)
			if (IsTargetTrace())
			{
				
				currentTime = 0;

				//���¸� Move �� ��ȯ
				bState = EBossState::Move;
				//�ִϸ��̼� ���� ����ȭ 
				banim->banimState = bState;
				//���� ������ ��ġ �����ֱ�
				//GetRandomPositionInNavMesh(boss->GetActorLocation(), 500, randomPos);
			}
		}

}

//�̵� ����
void UBossFSM::MoveState()
{
	//1. Ÿ���� ���ϴ� ������ ���ϰ�(target - me)
	FVector dir = target->GetActorLocation() - boss->GetActorLocation();

	//1). ó�� ��ġ - ���� ��ġ �Ÿ��� ���Ѵ�.
	float dist = FVector::Distance(originPos, boss->GetActorLocation());



		//2). ���࿡ dist �� moveRange ���� Ŀ���� (������ �� �ִ� �ݰ��� �Ѿ�ٸ�)
		if (dist > moveRange)
		{
			bState = EBossState::Idle;
			//�ִϸ��̼� ���� ����ȭ 
			banim->banimState = bState;
			//3). ReturnPos ���·� ��ȯ 
			//bState = EBossState::ReturnPos;
		}

		//Ÿ��� ��������� ���� ���·� ��ȯ�ϰ� �ʹ�.
		// 
		//1. ���� �Ÿ��� ���� ���� �ȿ� ������ 
		else if (dir.Length() < attackRange)
		{
			bi->StopMovement();

			//2. ���� ���·� ��ȯ�ϰ� �ʹ�.
			bState = EBossState::Attack;

			//�ִϸ��̼� ���� ����ȭ
			banim->banimState = bState;

			//���� �ִϸ��̼� ��� Ȱ��ȭ 


			//���� ���� ��ȯ �� ��� �ð��� �ٷ� �������� ó��
			currentTime = attackDelayTime;
		}
		else
		{
			//3. �� �������� �̵��ϰ� �ʹ�.+ (����ȭ)������ �����ϵ� ũ�Ⱑ 1��ŭ�� ����?
				//me->AddMovementInput(dir.GetSafeNormal());
			FVector destination = target->GetActorLocation();

			//NavigationSystem ��ü ������
			auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			//������ �� ã�� ��� ������ �˻�
			FPathFindingQuery query;
			FAIMoveRequest req;
			//���������� ������ �� �ִ� ����
			req.SetAcceptanceRadius(3);
			req.SetGoalLocation(destination);
			//�� ã�� ���� ���� ����
			bi->BuildPathfindingQuery(req, query);
			//�� ã�� ��� ��������
			FPathFindingResult r = ns->FindPathSync(query);
			//������ ������ ��ã�� ���� ���� Ȯ��
			if (r.Result == ENavigationQueryResult::Success && IsTargetTrace())
			{
				//Ÿ�� ������ �̵� 
				bi->MoveToLocation(destination);
			}
			else
			{
				//���� ��ġ�� �̵�
				auto result = bi->MoveToLocation(randomPos);
				//�������� �����ϸ�
				if (result == EPathFollowingRequestResult::AlreadyAtGoal)
				{
					//���¸� Move �� ��ȯ
					bState = EBossState::Idle;
					//�ִϸ��̼� ���� ����ȭ 
					banim->banimState = bState;
					//���ο� ���� ��ġ ��������
					//GetRandomPositionInNavMesh(boss->GetActorLocation(), 500, randomPos);
				}
			}
		}
		
}

	
	//���ݻ���
	void UBossFSM::AttackState()
	{
		//��ǥ: ���� �ð��� �� ���� �����ϰ� �ʹ�.

		//1. �ð��� �귯�� �Ѵ�.
		currentTime += GetWorld()->DeltaTimeSeconds;
		//2. ���� �ð��� �����ϱ�
		if (currentTime > attackDelayTime)
		{

			//3. �����ϰ� �ʹ�.
			UE_LOG(LogTemp, Warning, TEXT("Attack!!!"));

			target->OnHitDamage(target->legDamage);

			// ��� �ð� �ʱ�ȭ
			currentTime = 0;
			//���� �ִϸ��̼� �������� ����ϱ�
			int32 index = FMath::RandRange(1, 5);
			FString sectionName = FString::Printf(TEXT("Attack%d"), index);
			banim->PlayBossAnim(FName(*sectionName));

		}
		//��ǥ: Ÿ���� ���� ������ ����� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
		//1. Ÿ����� �Ÿ��� �ʿ��ϴ�.
		float distance = FVector::Distance(target->GetActorLocation(), boss->GetActorLocation());

		//2. Ÿ����� �Ÿ��� ���ݹ����� ������ϱ�
		if (distance > attackRange)
		{
			//3. ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
			bState = EBossState::Move;
			//�ִϸ��̼� ���� ����ȭ
			banim->banimState = bState;
			GetRandomPositionInNavMesh(boss->GetActorLocation(), 500, randomPos);
		}
	}

	//�ǰ� �˸� �̺�Ʈ �Լ�
	void UBossFSM::OnBossProcess(float damage, int32 animIdx)
	{
		if (bState == EBossState::Die) return;

		//ü�� ����
		currHP -= damage;
		//���� ü���� �����ִٸ� 
		if (currHP > 0)
		{
			//���¸� �ǰ����� ��ȯ
			bState = EBossState::Damage;

			//�÷��̾����� ������ �ڷ� �з�����
			//FVector s = me->GetActorLocation() + (-me->GetActorForwardVector() * 100);
			//me->SetActorLocation(s);

			//�÷��̾����� ������ �ڷ� �з�����
			FVector dir = boss->GetActorLocation() + (-boss->GetActorForwardVector() * 20);
			boss->SetActorLocation(dir);

			currentTime = 0;

			FString s = FString::Printf(TEXT("Damage%d"), animIdx);
			banim->PlayBossAnim(FName(*s));
		}
		else
		{
			//���¸� �������� ��ȯ
			bState = EBossState::Die;
			//ĸ�� �浹ü ��Ȱ��ȭ
			boss->GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

			//���� �ִϸ��̼� ���
			banim->PlayBossAnim(TEXT("Die0"));
			//UGameplayStatics::PlaySound2D(GetWorld(), deathSound);
		}
		UE_LOG(LogTemp, Warning, TEXT("curr HP : %f"), currHP);
		//�ִϸ��̼� ���� ����ȭ
		banim->banimState = bState;
		bi->StopMovement();
	}

	//�ǰ� ����
	void UBossFSM::DamageState()
	{
		//1. �ð��� �귶���ϱ� 
		currentTime += GetWorld()->DeltaRealTimeSeconds;
		//2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
		if (currentTime > damageDelayTime)
		{
			//3. ��� ���·� ��ȯ�ϰ� �ʹ�.
			bState = EBossState::Idle;
			//��� �ð� �ʱ�ȭ
			currentTime = 0;

			//banim->PlayBossAnim(TEXT("Idle0"));
			//�ִϸ��̼� ���� ����ȭ
			banim->banimState = bState;
		}
	}
	//��������
	void UBossFSM::DieState()
	{
		//���� ���� �ִϸ��̼��� ������ �ʾҴٸ� 
		//�ٴڱ��� �������� �ʵ��� ó��
// 		if (banim->bDieDone == false)
// 		{
// 			//�÷��̾����� ������ �ڷ� �з�����
// 			FVector s = boss->GetActorLocation() + (-boss->GetActorForwardVector() * 3);
// 			boss->SetActorLocation(s);
// 			return;
// 		}
// 		//��� �Ʒ��� �������� �ʹ�. p=p0+vt
// 		FVector p0 = boss->GetActorLocation();
// 		FVector p = p0 + FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
// 		boss->SetActorLocation(p);
// 
// 		//1. ���� 2���� �̻� �����Դٸ�
// 		if (p.Z < -200.0f)
// 		{
// 			//2. ���Ž�Ų��
// 			boss->Destroy();
// 		}
	}


void UBossFSM::BossAnim(int32 attackIdx)
{
	float damage = target->legDamage;
	switch (attackIdx)
	{
	case 0: case 3:
		damage = target->handDamage;
		break;
	}
	OnBossProcess(damage, attackIdx);
}
	//����
	void UBossFSM::ReturnPosState()
	{
		//Ÿ�� ������ �̵� 
		EPathFollowingRequestResult::Type result = bi->MoveToLocation(originPos);
		//�������� �����ϸ�
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			//	//4. idle ���·� ��ȯ
			bState = EBossState::Idle;
			banim->banimState = bState;
		}
	}


	bool UBossFSM::IsTargetTrace()
	{

		//1. ��---->�÷��̾ ���ϴ� ������ ���Ѵ� (Normalize)
		FVector dir = target->GetActorLocation() - boss->GetActorLocation();

		//2. ���� �չ���� 1���� ���� ������ ����
		float dotvalue = FVector::DotProduct(boss->GetActorForwardVector(), dir.GetSafeNormal());
		//3. 2������ ���� ���� Acos �Ѵ�.---> �� ������ ���̰�
		float degree = UKismetMathLibrary::DegAcos(dotvalue);

		//4. ���࿡ 3���� ���� ���� 30 ���� �۰� (�þ߰� 60)
		//�� - Ÿ�� ���� �Ÿ��� traceRange ���� ������
		if (degree < 30)// && dir.Length() < traceRange)
		{
			return true;

			//Enemy ----> targetLinTrace ���� !
			FHitResult hitInfo;
			FCollisionQueryParams param;
			param.AddIgnoredActor(boss);

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				hitInfo,
				boss->GetActorLocation(), target->GetActorLocation(),
				ECollisionChannel::ECC_Visibility, param);

			//���࿡ �ε��� ���� �ִٸ�
			if (bHit)
			{
				//���࿡ �ε��� ���� �̸��� Player �� �����ϰ� �ִٸ�
				FString str = hitInfo.GetActor()->GetName();
				if (hitInfo.GetActor()->GetName().Contains(TEXT("Character")))
			{
				//true ��ȯ
				return true;
			}
		}
	}
	//false �� ��ȯ
	return false;
}

//���� ��ġ ��������
bool UBossFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;
}
