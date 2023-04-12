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

	//월드에서 APlayer_KYI 타깃 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayer_KYI::StaticClass());

	//APlayer_KYI 타입으로 캐스팅
	target = Cast<APlayer_KYI>(actor);

	//소유 객체 가져오기
	me = Cast<AHJ_Enemy>(GetOwner());
	me->GetCharacterMovement()->MaxWalkSpeed = 300;
	//UEnemyAnim* 할당

	anim = Cast<UHJ_EnemyAnim>(me->GetMesh()->GetAnimInstance());

	//compProjectile=CreateDefaultSubobject< UProjectileMovementComponent>(TEXT("compProject"));

	//나의 초기 체력을 셋팅하자
	currHP = maxHP;

	//처음 위치 셋팅
	originPos = me->GetActorLocation();

	//AIController 할당하기
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
//대기 상태
void UEnemyFSM::IdleState()
{

	// 	//1. 시간이 흘렀으니까 
	// 	currentTime += GetWorld()->DeltaTimeSeconds;
	// 	//2. 만약 경과 시간이 대기 시간을 초과했다면
	// 	if (currentTime > idleDelayTime)
	// 	{
	// 	
	// 		//3. 이동 상태로 전환하고 싶다.
	// 		mState = EEnemyState::Move;
	// 		//경과 시간 초기화
	// 		currentTime = 0;
	// 
	// 		//Idle 애니메이션 재생
	// 	    //int32 index = FMath::RandRange(0,1);
	// 		//anim->PlayDamageAnim(TEXT("Move0"));
	// 
	// 
	// 		//애니메이션 상태 동기화 
	// 		anim->animState = mState;
	// 	}

	//만약에 플레이어를 쫓아 갈 수 있니?
	if (IsTargetTrace())
	{
		//상태를 Move 로 전환
		mState = EEnemyState::Move;
	}
	//만약에 Player 를 쫓아 갈 수 있니? ( 내 시야에 보이면)
	//currentTime += GetWorld()->DeltaTimeSeconds;
	else
	{

		//if (currentTime > idleDelayTime)
		if (IsTargetTrace())	
		{
			currentTime = 0;
			//상태를 Move 로 전환
			mState = EEnemyState::Move;
			//애니메이션 상태 동기화 
			anim->animState = mState;
			//최초 랜덤한 위치 정해주기
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}
}
//이동 상태
void UEnemyFSM::MoveState()
{
	//1. 타겟을 향하는 방향을 구하고(target - me)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//1). 처음 위치 - 나의 위치 거리를 구한다.
	float dist = FVector::Distance(originPos, me->GetActorLocation());


	//2). 만약에 dist 가 moveRange 보다 커지면 (움직일 수 있는 반경을 넘어갔다면)
	if (dist > moveRange)
	{
		//3). ReturnPos 상태로 전환 
		mState = EEnemyState::ReturnPos;
	}

	//타깃과 가까워지면 공격 상태로 전환하고 싶다.
	// 
	//1. 만약 거리가 공격 범위 안에 들어오면 
	else if (dir.Length() < attackRange)
	{
		ai->StopMovement();

		//2. 공격 상태로 전환하고 싶다.
		mState = EEnemyState::Attack;

		//애니메이션 상태 동기화
		anim->animState = mState;

		//공격 애니메이션 재생 활성화 
		anim->PlayDamageAnim(TEXT("Idle"));

		//공격 상태 전환 시 대기 시간이 바로 끝나도록 처리
		currentTime = attackDelayTime;
	}
	else
	{
		//3. 그 방향으로 이동하고 싶다.+ (정규화)방향을 유지하되 크기가 1만큼의 방향?
			//me->AddMovementInput(dir.GetSafeNormal());
		FVector destination = target->GetActorLocation();

		//NavigationSystem 객체 얻어오기
		auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		//목적지 길 찾기 경로 데이터 검색
		FPathFindingQuery query;
		FAIMoveRequest req;
		//목적지에서 인지할 수 있는 범위
		req.SetAcceptanceRadius(3);
		req.SetGoalLocation(destination);
		//길 찾기 위한 쿼리 생성
		ai->BuildPathfindingQuery(req, query);
		//길 찾기 결과 가져오기
		FPathFindingResult r = ns->FindPathSync(query);
		//목적지 까지의 길찾기 성공 여부 확인
		if (r.Result == ENavigationQueryResult::Success && IsTargetTrace())
		{
			//타깃 쪽으로 이동 
			ai->MoveToLocation(destination);
		}
		else
		{
			//랜덤 위치로 이동
			auto result = ai->MoveToLocation(randomPos);
			//목적지에 도착하면
			if (result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				//상태를 Move 로 전환
				mState = EEnemyState::Idle;
				//애니메이션 상태 동기화 
				anim->animState = mState;
				//새로운 랜덤 위치 가져오기
				//GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
			}
		}
	}
}


//공격 상태
void UEnemyFSM::AttackState()
{
	//목표: 일정 시간에 한 번씩 공격하고 싶다.

	//1. 시간이 흘러야 한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. 공격 시간이 됐으니까
	if (currentTime > attackDelayTime)
	{

		//3. 공격하고 싶다.
		UE_LOG(LogTemp, Warning, TEXT("Attack!!!"));

		target->OnHitDamage(target->legDamage);

		// 경과 시간 초기화
		currentTime = 0;
		//공격 애니메이션 랜덤으로 재생하기
		int32 index = FMath::RandRange(0, 3);
		FString sectionName = FString::Printf(TEXT("Attack%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));

		//anim->bAttackPlay = true;
	}
	//목표: 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다.
	//1. 타깃과의 거리가 필요하다.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	//2. 타깃과의 거리가 공격범위를 벗어낫으니까
	if (distance > attackRange)
	{
		//3. 상태를 이동으로 전환하고 싶다.
		mState = EEnemyState::Move;
		//애니메이션 상태 동기화
		anim->animState = mState;
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

//피격 알림 이벤트 함수
void UEnemyFSM::OnDamageProcess(float damage, int32 animIdx)
{
	if(mState == EEnemyState::Die) return;

	//체력 감소
	currHP -= damage;
	//만약 체력이 남아있다면 
	if (currHP > 0)
	{

		//플레이어한테 맞으면 뒤로 밀려난다
		//FVector s = me->GetActorLocation() + (-me->GetActorForwardVector() * 100);
		//me->SetActorLocation(s);
		
		//플레이어한테 맞으면 뒤로 밀려난다
		FVector dir = me->GetActorLocation() + (-me->GetActorForwardVector() * 20);
		me->SetActorLocation(dir);
		
		currentTime = 0;
		
		FString s = FString::Printf(TEXT("Damage%d"), animIdx);

		//필살기 애님메이션 딜레이 시간
		if (animIdx == 5) {
			FTimerHandle WaitHandle;
			float WaitTime = 5; //시간을 설정하고
			anim->PlayDamageAnim(FName(*s));
			GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]() {

				}), WaitTime, false);

		}
		else {
			anim->PlayDamageAnim(FName(*s));
		}
		//상태를 피격으로 전환
		mState = EEnemyState::Damage;

	}
	else
	{
		//상태를 죽음으로 전환
		mState = EEnemyState::Die;
		//캡슐 충돌체 비활성화
		me->GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

		//죽음 애니메이션 재생
		anim->PlayDamageAnim(TEXT("Die0"));
		UGameplayStatics::PlaySound2D(GetWorld(), deathSound);
	}
	UE_LOG(LogTemp, Warning, TEXT("curr HP : %f"), currHP);
	//애니메이션 상태 동기화
	anim->animState = mState;
	ai->StopMovement();
}

//피격 상태
void UEnemyFSM::DamageState()
{
	//1. 시간이 흘렀으니까 
	currentTime += GetWorld()->DeltaRealTimeSeconds;
	//2. 만약 경과 시간이 대기 시간을 초과했다면
	if (currentTime > damageDelayTime)
	{
		//3. 대기 상태로 전환하고 싶다.
		mState = EEnemyState::Idle;
		//경과 시간 초기화
		currentTime = 0;

		anim->PlayDamageAnim(TEXT("Idle0"));
		//애니메이션 상태 동기화
		//anim->animState = mState;
	}



}
//죽음 상태
void UEnemyFSM::DieState()
{

	//아직 죽음 애니메이션이 끝나지 않았다면 
	//바닥까지 내려가지 않도록 처리
	if (anim->bDieDone == false)
	{
		//플레이어한테 맞으면 뒤로 밀려난다
		FVector s = me->GetActorLocation() + (-me->GetActorForwardVector() * 3);
		me->SetActorLocation(s);
		return;
	}


	//계속 아래로 내려가고 싶다. p=p0+vt
	FVector p0 = me->GetActorLocation();
	FVector p = p0 + FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	me->SetActorLocation(p);

	//1. 만약 2미터 이상 내려왔다면
	if (p.Z < -200.0f)
	{
		//2. 제거시킨다
		me->Destroy();
// 		나를 비활성화
// 				me->SetActive(false);
// 				//enemyManager 찾자
// 				AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass());
// 				AEnemyManager* am = Cast<AEnemyManager>(actor);
// 				//찾은 놈에서enemyArray 에 나를 다시 담자
// 				//am->spawnPoints.Add(me);
// 		
// 				//currHP = MaxHP 로CCC
// 				currHP = maxHP;
// 				//상태를 Idle
// 				mState = EEnemyState::Idle;
// 				// 몽타주를 멈춰준다
// 				me->StopAnimMontage();
// 				//bDieMove 를 false 로 해준다.
// 				anim->bDieDone = false;
	}
	//3. 그렇지 않으면 해당 위치로 셋팅한다
// 	else
// 	{
// 		me->SetActorLocation(p);
// 	}
}

void UEnemyFSM::ReturnPosState()
{
	//타깃 쪽으로 이동 
	EPathFollowingRequestResult::Type result = ai->MoveToLocation(originPos);
	//목적지에 도착하면
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		//	//4. idle 상태로 전환
		mState = EEnemyState::Idle;
		anim->animState = mState;
	}

	////1. 나 ---> 처음 위치를 향하는 방향을 구한다.
	//FVector dir = originPos - me->GetActorLocation();

	//// 	3. 만약에 나--->처음 위치 거리가 10 보다 작아지면
	//if (dir.Length() < 10)
	//{
	//	//4. idle 상태로 전환
	//	mState = EEnemyState::Idle;
	//}
	////그렇지 않으면
	//else
	//{
	//	//그 방향으로 이동한다.
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
	//1. 나---->플레이어를 향하는 방향을 구한다 (Normalize)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. 나의 앞방향과 1번에 구한 벡터의 내적
	float dotvalue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());
	//3. 2번에서 나온 값을 Acos 한다.---> 두 벡터의 사이각
	float degree = UKismetMathLibrary::DegAcos(dotvalue);

	//4. 만약에 3번에 나온 값이 30 보다 작고 (시야각 60)
	//나 - 타겟 과의 거리가 traceRange 보다 작으면
	if (degree < 30)// && dir.Length() < traceRange)
	{
		return true;

		//Enemy ----> targetLinTrace 쏘자 !
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(), target->GetActorLocation(),
			ECollisionChannel::ECC_Visibility, param);

		//만약에 부딪힌 곳이 있다면
		if (bHit)
		{
			//만약에 부딪힌 놈의 이름이 Player 를 포함하고 있다면
			FString str = hitInfo.GetActor()->GetName();
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Character")))
			{
				//true 반환
				return true;
			}
		}
	}
	//false 로 반환
	return false;
}

//랜덤 위치 가져오기
bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;

}