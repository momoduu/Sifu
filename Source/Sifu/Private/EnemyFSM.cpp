// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "Player_KYI.h"
#include "HJ_Enemy.h"
#include <Components/CapsuleComponent.h>
#include "HJ_EnemyAnim.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <AIModule/Classes/AIController.h>
#include <NavigationSystem.h>
#include "EnemyManager.h"



// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundWave'/Game/Audio/MaleA/voice_male_grunt_pain_death_06.voice_male_grunt_pain_death_06'"));
	if (tempSound.Succeeded()) {
		deathSound = tempSound.Object;
	}
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	//���忡�� APlayer_KYI Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayer_KYI::StaticClass());

	//APlayer_KYI Ÿ������ ĳ����
	target = Cast<APlayer_KYI>(actor);

	//���� ��ü ��������
	me = Cast<AHJ_Enemy>(GetOwner());
	me->GetCharacterMovement()->MaxWalkSpeed = 300;
	//UEnemyAnim* �Ҵ�

	anim = Cast<UHJ_EnemyAnim>(me->GetMesh()->GetAnimInstance());

	//compProjectile=CreateDefaultSubobject< UProjectileMovementComponent>(TEXT("compProject"));

	//���� �ʱ� ü���� ��������
	currHP = maxHP;

	//ó�� ��ġ ����
	originPos = me->GetActorLocation();

	//AIController �Ҵ��ϱ�
	ai = Cast<AAIController>(me->GetController());

}



// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	case EEnemyState::ReturnPos:
		ReturnPosState();
		break;
	}
}
//��� ����
void UEnemyFSM::IdleState()
{

	// 	//1. �ð��� �귶���ϱ� 
	// 	currentTime += GetWorld()->DeltaTimeSeconds;
	// 	//2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	// 	if (currentTime > idleDelayTime)
	// 	{
	// 	
	// 		//3. �̵� ���·� ��ȯ�ϰ� �ʹ�.
	// 		mState = EEnemyState::Move;
	// 		//��� �ð� �ʱ�ȭ
	// 		currentTime = 0;
	// 
	// 		//Idle �ִϸ��̼� ���
	// 	    //int32 index = FMath::RandRange(0,1);
	// 		//anim->PlayDamageAnim(TEXT("Move0"));
	// 
	// 
	// 		//�ִϸ��̼� ���� ����ȭ 
	// 		anim->animState = mState;
	// 	}

	//���࿡ �÷��̾ �Ѿ� �� �� �ִ�?
	if (IsTargetTrace())
	{
		//���¸� Move �� ��ȯ
		mState = EEnemyState::Move;
	}
	//���࿡ Player �� �Ѿ� �� �� �ִ�? ( �� �þ߿� ���̸�)
	//currentTime += GetWorld()->DeltaTimeSeconds;
	else
	{

		//if (currentTime > idleDelayTime)
		if (IsTargetTrace())	
		{
			currentTime = 0;
			//���¸� Move �� ��ȯ
			mState = EEnemyState::Move;
			//�ִϸ��̼� ���� ����ȭ 
			anim->animState = mState;
			//���� ������ ��ġ �����ֱ�
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}
}
//�̵� ����
void UEnemyFSM::MoveState()
{
	//1. Ÿ���� ���ϴ� ������ ���ϰ�(target - me)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//1). ó�� ��ġ - ���� ��ġ �Ÿ��� ���Ѵ�.
	float dist = FVector::Distance(originPos, me->GetActorLocation());


	//2). ���࿡ dist �� moveRange ���� Ŀ���� (������ �� �ִ� �ݰ��� �Ѿ�ٸ�)
	if (dist > moveRange)
	{
		//3). ReturnPos ���·� ��ȯ 
		mState = EEnemyState::ReturnPos;
	}

	//Ÿ��� ��������� ���� ���·� ��ȯ�ϰ� �ʹ�.
	// 
	//1. ���� �Ÿ��� ���� ���� �ȿ� ������ 
	else if (dir.Length() < attackRange)
	{
		ai->StopMovement();

		//2. ���� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Attack;

		//�ִϸ��̼� ���� ����ȭ
		anim->animState = mState;

		//���� �ִϸ��̼� ��� Ȱ��ȭ 
		anim->PlayDamageAnim(TEXT("Idle"));

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
		ai->BuildPathfindingQuery(req, query);
		//�� ã�� ��� ��������
		FPathFindingResult r = ns->FindPathSync(query);
		//������ ������ ��ã�� ���� ���� Ȯ��
		if (r.Result == ENavigationQueryResult::Success && IsTargetTrace())
		{
			//Ÿ�� ������ �̵� 
			ai->MoveToLocation(destination);
		}
		else
		{
			//���� ��ġ�� �̵�
			auto result = ai->MoveToLocation(randomPos);
			//�������� �����ϸ�
			if (result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				//���¸� Move �� ��ȯ
				mState = EEnemyState::Idle;
				//�ִϸ��̼� ���� ����ȭ 
				anim->animState = mState;
				//���ο� ���� ��ġ ��������
				//GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
			}
		}
	}
}


//���� ����
void UEnemyFSM::AttackState()
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
		int32 index = FMath::RandRange(0, 3);
		FString sectionName = FString::Printf(TEXT("Attack%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));

		//anim->bAttackPlay = true;
	}
	//��ǥ: Ÿ���� ���� ������ ����� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
	//1. Ÿ����� �Ÿ��� �ʿ��ϴ�.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	//2. Ÿ����� �Ÿ��� ���ݹ����� ������ϱ�
	if (distance > attackRange)
	{
		//3. ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;
		//�ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

//�ǰ� �˸� �̺�Ʈ �Լ�
void UEnemyFSM::OnDamageProcess(float damage, int32 animIdx)
{
	if(mState == EEnemyState::Die) return;

	//ü�� ����
	currHP -= damage;
	//���� ü���� �����ִٸ� 
	if (currHP > 0)
	{

		//�÷��̾����� ������ �ڷ� �з�����
		//FVector s = me->GetActorLocation() + (-me->GetActorForwardVector() * 100);
		//me->SetActorLocation(s);
		
		//�÷��̾����� ������ �ڷ� �з�����
		FVector dir = me->GetActorLocation() + (-me->GetActorForwardVector() * 20);
		me->SetActorLocation(dir);
		
		currentTime = 0;
		
		FString s = FString::Printf(TEXT("Damage%d"), animIdx);

		//�ʻ�� �ִԸ��̼� ������ �ð�
		if (animIdx == 5) {
			FTimerHandle WaitHandle;
			float WaitTime = 5; //�ð��� �����ϰ�
			anim->PlayDamageAnim(FName(*s));
			GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]() {

				}), WaitTime, false);

		}
		else {
			anim->PlayDamageAnim(FName(*s));
		}
		//���¸� �ǰ����� ��ȯ
		mState = EEnemyState::Damage;

	}
	else
	{
		//���¸� �������� ��ȯ
		mState = EEnemyState::Die;
		//ĸ�� �浹ü ��Ȱ��ȭ
		me->GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

		//���� �ִϸ��̼� ���
		anim->PlayDamageAnim(TEXT("Die0"));
		UGameplayStatics::PlaySound2D(GetWorld(), deathSound);
	}
	UE_LOG(LogTemp, Warning, TEXT("curr HP : %f"), currHP);
	//�ִϸ��̼� ���� ����ȭ
	anim->animState = mState;
	ai->StopMovement();
}

//�ǰ� ����
void UEnemyFSM::DamageState()
{
	//1. �ð��� �귶���ϱ� 
	currentTime += GetWorld()->DeltaRealTimeSeconds;
	//2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > damageDelayTime)
	{
		//3. ��� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Idle;
		//��� �ð� �ʱ�ȭ
		currentTime = 0;

		anim->PlayDamageAnim(TEXT("Idle0"));
		//�ִϸ��̼� ���� ����ȭ
		//anim->animState = mState;
	}



}
//���� ����
void UEnemyFSM::DieState()
{

	//���� ���� �ִϸ��̼��� ������ �ʾҴٸ� 
	//�ٴڱ��� �������� �ʵ��� ó��
	if (anim->bDieDone == false)
	{
		//�÷��̾����� ������ �ڷ� �з�����
		FVector s = me->GetActorLocation() + (-me->GetActorForwardVector() * 3);
		me->SetActorLocation(s);
		return;
	}


	//��� �Ʒ��� �������� �ʹ�. p=p0+vt
	FVector p0 = me->GetActorLocation();
	FVector p = p0 + FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	me->SetActorLocation(p);

	//1. ���� 2���� �̻� �����Դٸ�
	if (p.Z < -200.0f)
	{
		//2. ���Ž�Ų��
		me->Destroy();
// 		���� ��Ȱ��ȭ
// 				me->SetActive(false);
// 				//enemyManager ã��
// 				AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass());
// 				AEnemyManager* am = Cast<AEnemyManager>(actor);
// 				//ã�� �𿡼�enemyArray �� ���� �ٽ� ����
// 				//am->spawnPoints.Add(me);
// 		
// 				//currHP = MaxHP ��CCC
// 				currHP = maxHP;
// 				//���¸� Idle
// 				mState = EEnemyState::Idle;
// 				// ��Ÿ�ָ� �����ش�
// 				me->StopAnimMontage();
// 				//bDieMove �� false �� ���ش�.
// 				anim->bDieDone = false;
	}
	//3. �׷��� ������ �ش� ��ġ�� �����Ѵ�
// 	else
// 	{
// 		me->SetActorLocation(p);
// 	}
}

void UEnemyFSM::ReturnPosState()
{
	//Ÿ�� ������ �̵� 
	EPathFollowingRequestResult::Type result = ai->MoveToLocation(originPos);
	//�������� �����ϸ�
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		//	//4. idle ���·� ��ȯ
		mState = EEnemyState::Idle;
		anim->animState = mState;
	}

	////1. �� ---> ó�� ��ġ�� ���ϴ� ������ ���Ѵ�.
	//FVector dir = originPos - me->GetActorLocation();

	//// 	3. ���࿡ ��--->ó�� ��ġ �Ÿ��� 10 ���� �۾�����
	//if (dir.Length() < 10)
	//{
	//	//4. idle ���·� ��ȯ
	//	mState = EEnemyState::Idle;
	//}
	////�׷��� ������
	//else
	//{
	//	//�� �������� �̵��Ѵ�.
	//	me->AddMovementInput(dir.GetSafeNormal());
	//}
}

void UEnemyFSM::DamageAnim(int32 attackIdx)
{
	float damage = target->legDamage;
	switch (attackIdx)
	{
		case 0: case 3:
		damage = target->handDamage;
		break;
		case 5:
		damage = target->finishDamage;
		break;
	}
	OnDamageProcess(damage, attackIdx);
}


bool UEnemyFSM::IsTargetTrace()
{
	//1. ��---->�÷��̾ ���ϴ� ������ ���Ѵ� (Normalize)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. ���� �չ���� 1���� ���� ������ ����
	float dotvalue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());
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
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(), target->GetActorLocation(),
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
bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;

}