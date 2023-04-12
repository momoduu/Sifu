// Fiout your copyright notice in the Description page of Project Settings.


#include "Player_KYI.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/Character.h>
#include <Camera/CameraComponent.h>
#include <Components/SphereComponent.h>
#include "HJ_Enemy.h"
#include "EnemyFSM.h"
#include <Engine/SkeletalMesh.h>
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>

#include "Components/PrimitiveComponent.h"
#include <Components/SphereComponent.h>
#include "HJ_Enemy.h"
#include "EnemyFSM.h"
#include <Engine/SkeletalMesh.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include <Animation/AnimMontage.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerAnim.h"
#include <Sound/SoundBase.h>
#include <Blueprint/UserWidget.h>
#include "HJ_Boss.h"


// Sets default values
APlayer_KYI::APlayer_KYI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/UE4_Mannequin.UE4_Mannequin'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	//왼손 Collision
	leftHand = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHand"));
	leftHand->SetupAttachment(GetMesh(), TEXT("middle_03_l"));
	leftHand->SetRelativeScale3D(FVector(0.5f));
	leftHand->SetGenerateOverlapEvents(true);
	leftHand->SetCollisionProfileName(TEXT("DamagePreset"));

	//왼발 Collision
	leftLeg = CreateDefaultSubobject<USphereComponent>(TEXT("LeftLeg"));
	leftLeg->SetupAttachment(GetMesh(), TEXT("ball_l"));
	leftLeg->SetRelativeScale3D(FVector(0.5f));
	leftLeg->SetRelativeLocation(FVector(0));
	leftLeg->SetGenerateOverlapEvents(true);
	leftLeg->SetCollisionProfileName(TEXT("DamagePreset"));

	//오른발 Collision
	rightLeg = CreateDefaultSubobject<USphereComponent>(TEXT("RightLeg"));
	rightLeg->SetupAttachment(GetMesh(), TEXT("ball_r"));
	rightLeg->SetRelativeScale3D(FVector(0.5f));
	rightLeg->SetRelativeLocation(FVector(0));
	rightLeg->SetGenerateOverlapEvents(true);
	rightLeg->SetCollisionProfileName(TEXT("DamagePreset"));

	//springArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 0, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	//Camera 컴포넌트 붙이기
	camComp = CreateDefaultSubobject<UCameraComponent>(TEXT("camComp"));
	camComp->SetupAttachment(springArmComp);
	camComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;

	//애니메이션 블루프린트 가져오자
	ConstructorHelpers::FClassFinder<UPlayerAnim> tempAnim(TEXT("AnimBlueprint'/Game/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C'"));
	if (tempAnim.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundWave'/Game/Audio/MaleA/voice_male_grunt_pain_death_03.voice_male_grunt_pain_death_03'"));
	if (tempSound.Succeeded()) {
		deathSound = tempSound.Object;
	}

	//피격 애니메이션
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempStomach(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Big_Stomach_Hit_Montage.Big_Stomach_Hit_Montage'"));
	if (tempStomach.Succeeded()) {
		stomach = tempStomach.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempHead2(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Head_Hit_2_Montage.Head_Hit_2_Montage'"));
	if (tempHead2.Succeeded()) {
		head2 = tempHead2.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempHead3(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Head_Hit_3_Montage.Head_Hit_3_Montage'"));
	if (tempHead3.Succeeded()) {
		head3 = tempHead3.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempHead4(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Head_Hit_4_Montage.Head_Hit_4_Montage'"));
	if (tempHead4.Succeeded()) {
		head4 = tempHead4.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempFallDown(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Sweep_Fall_Montage.Sweep_Fall_Montage'"));
	if (tempFallDown.Succeeded()) {
		falldown = tempFallDown.Object;
	}
	//죽음
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempDeath(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Death.Death'"));
	if (tempDeath.Succeeded()) {
		death = tempDeath.Object;
	}
	//방어
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempBlock(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Block_Hit_Montage.Block_Hit_Montage'"));
	if (tempBlock.Succeeded()) {
		block = tempBlock.Object;
	}
	//주먹 공격 애니메이션
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempPunch(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/regular-punch.regular-punch'"));
	if (tempPunch.Succeeded()) {
		punch = tempPunch.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempJab(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Lead_Jab_Montage.Lead_Jab_Montage'"));
	if (tempJab.Succeeded()) {
		jab = tempJab.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempUpper(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Uppercut_Montage.Uppercut_Montage'"));
	if (tempUpper.Succeeded()) {
		uppercut = tempUpper.Object;
	}
	//발차기 공격 애니메이션
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempKick(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/kick-regular_Montage.kick-regular_Montage'"));
	if (tempKick.Succeeded()) {
		kick = tempKick.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempHighKick(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/kick-up_Montage.kick-up_Montage'"));
	if (tempHighKick.Succeeded()) {
		highKick = tempHighKick.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempLowKick(TEXT("AnimMontage'/Game/Mannequin/Animations/h2H_Anim/Leg_Sweep_Montage.Leg_Sweep_Montage'"));
	if (tempLowKick.Succeeded()) {
		lowKick = tempLowKick.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempFinish(TEXT("AnimMontage'/Game/Mannequin/Animations/H2H_Paired/H2H_Paired_ChokeSlam_Att_Montage.H2H_Paired_ChokeSlam_Att_Montage'"));
	if (tempFinish.Succeeded()) {
		finish = tempFinish.Object;
	}

	////playerUI 클래스를 가져온다
	//ConstructorHelpers::FClassFinder<UUserWidget> tempUI(TEXT("WidgetBlueprint'/Game/SideScrollerBP/UI/PlayerUI.PlayerUI'"));
	//if (tempUI.Succeeded()) {
	//	playerUIFactory = tempUI.Class;
	//}
	////플레이어 UI 만들기
	//playerUI = CreateWidget<UUserWidget>(GetWorld(), playerUIFactory);
}

// Called when the game starts or when spawned
void APlayer_KYI::BeginPlay() {
	Super::BeginPlay();
	currHp = maxHp;
	leftHand->OnComponentBeginOverlap.AddDynamic(this, &APlayer_KYI::BeginOverlap);
	leftLeg->OnComponentBeginOverlap.AddDynamic(this, &APlayer_KYI::BeginOverlap);
	rightLeg->OnComponentBeginOverlap.AddDynamic(this, &APlayer_KYI::BeginOverlap);
	
}

// Called every frame
void APlayer_KYI::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (movementEnabled) {
		direction = FTransform(GetControlRotation()).TransformVector(direction);
		AddMovementInput(direction);
		direction = FVector::ZeroVector;
	}
	leftHand->SetActive(false);
	leftLeg->SetActive(false);
	rightLeg->SetActive(false);
}

void APlayer_KYI::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);
}

void APlayer_KYI::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	targetEnemy = Cast<AHJ_Enemy>(OtherActor);
	targetBoss = Cast<AHJ_Boss>(OtherActor);
	//if (targetEnemy) {
	//	if (OverlappedComponent->GetName().Contains(TEXT("hand"))) {
	//		//targetEnemy->fsm->OnDamageProcess(handDamage);
	//	}
	//	else if (OverlappedComponent->GetName().Contains(TEXT("leg"))) {
	//		//targetEnemy->fsm->OnDamageProcess(legDamage);
	//	}
	//}
}

// Called to bind functionality to input
void APlayer_KYI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &APlayer_KYI::InputJump);
	PlayerInputComponent->BindAction(TEXT("Punch"), IE_Pressed, this, &APlayer_KYI::AttackPunch);
	PlayerInputComponent->BindAction(TEXT("Kick"), IE_Pressed, this, &APlayer_KYI::AttackKick);
	PlayerInputComponent->BindAction(TEXT("Finish"), IE_Pressed, this, &APlayer_KYI::InputFinish);
	DECLARE_DELEGATE_OneParam(FBlock, bool);
	PlayerInputComponent->BindAction<FBlock>(TEXT("Block"), IE_Pressed, this, &APlayer_KYI::PlayerBlock, true);
	PlayerInputComponent->BindAction<FBlock>(TEXT("Block"), IE_Released, this, &APlayer_KYI::PlayerBlock, false);
	DECLARE_DELEGATE_OneParam(FRun, bool);
	PlayerInputComponent->BindAction<FRun>(TEXT("Run"), IE_Pressed, this, &APlayer_KYI::InputRun, true);
	PlayerInputComponent->BindAction<FRun>(TEXT("Run"), IE_Released, this, &APlayer_KYI::InputRun, false);

	if (GetWorld()->GetCurrentLevel()->GetFullName().Contains("FinalBossMap")) {
		PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &APlayer_KYI::InputVertical);
	}
	else {
		PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &APlayer_KYI::InputHorizontal);
		PlayerInputComponent->BindAxis(TEXT("Move Forward / Backward"), this, &APlayer_KYI::InputVertical);
		PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Mouse"), this, &APlayer_KYI::Turn);
		PlayerInputComponent->BindAxis(TEXT("Look Up / Down Mouse"), this, &APlayer_KYI::LookUp);
	}
}

void APlayer_KYI::Turn(float value) {
	AddControllerYawInput(value);
}
void APlayer_KYI::LookUp(float value) {
	AddControllerPitchInput(value);
}
void APlayer_KYI::InputHorizontal(float value) {
	if (GetWorld()->GetCurrentLevel()->GetFullName().Contains("FinalBossMap")) {
		direction.Y = -value;
	}
	else {
		direction.Y = value;
	}
}
void APlayer_KYI::InputVertical(float value) {
	direction.X = value;
}
void APlayer_KYI::InputJump() {
	Jump();
}
void APlayer_KYI::InputRun(bool run) {
	if (run)
		GetCharacterMovement()->MaxWalkSpeed = 1000;
	else
		GetCharacterMovement()->MaxWalkSpeed = 600;
}
//void APlayer_KYI::setTarget() {
//	if (!targetLock) {
//		FVector startPos = camComp->GetComponentLocation();
//		FVector endPos = startPos + camComp->GetForwardVector() * 5000;
//		FHitResult hitinfo;
//		FCollisionQueryParams params;
//		params.AddIgnoredActor(this);
//		bool bHit = GetWorld()->LineTraceSingleByChannel(hitinfo, startPos, endPos, ECC_Pawn, params);
//		if (bHit) {
//			AHJ_Enemy* enem = Cast<AHJ_Enemy>(hitinfo.GetActor());
//			if (enem) {
//				targetEnemy = enem;
//				targetLock = true;
//				UE_LOG(LogTemp, Warning, TEXT("%s"), *targetEnemy->GetName());
//			}
//		}
//	}
//	else if (targetEnemy->fsm->currHP == 0) {
//		targetLock = false;
//	}
//}

//공격 방어
void APlayer_KYI::PlayerBlock(bool value) {
	if (!isDead) {
		isBlocking = value;
		movementEnabled = !value;
	}
}

//플레이어가 공격을 받았다
void APlayer_KYI::OnHitDamage(float damage) {
	if (!isDead && !isBlocking) {
		//체력 감소
		currHp -= damage;
		//만약에 체력이 없다면
		if (currHp <= 0) {
			isDead = true;
			//상태를 죽음으로 전환
			PlayerDie();
			//캡슐 충돌체 비활성화
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//GetComponentByClass(UCapsuleComponent::StaticClass())
		}
	}
}

//플레이어가 적을 타격
void APlayer_KYI::PlayerDamage() {
	if (targetLock) {
		//거리 계산 (Player - enemy)
		FVector v = GetActorLocation() - targetEnemy->GetActorLocation();
		if (v.Length() <= 300) {
			//targetEnemy->fsm->OnDamageProcess();
		}
	}
	//TArray<AActor*> enemys;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHJ_Enemy::StaticClass(), enemys);
	//for (int i = 0; i < enemys.Num(); i++)
	//{
	//	//거리 계산 (Player - enemy)
	//	FVector v = GetActorLocation() - enemys[i]->GetActorLocation();
	//	float dist = v.Length();
	//	if (dist < 300) {
	//		AHJ_Enemy* e = Cast<AHJ_Enemy>(enemys[i]);
	//		e->fsm->OnDamageProcess();
	//	}
	//}
}

//플레이어 죽음
void APlayer_KYI::PlayerDie()
{
	PlayAnimMontage(death);
	UGameplayStatics::PlaySound2D(GetWorld(), deathSound);
}

void APlayer_KYI::AttackPunch() {
	leftHand->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	leftLeg->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	rightLeg->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	movementEnabled = false;
	kickorPunch = true;
	if (IsAttacking) {
		saveAttack = true;
	}
	else {
		IsAttacking = true;
		punchCombo();
	}
}

void APlayer_KYI::AttackKick() {
	leftHand->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	leftLeg->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	rightLeg->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	movementEnabled = false;
	kickorPunch = false;
	if (IsAttacking) {
		saveAttack = true;
	}
	else {
		IsAttacking = true;
		kickCombo();
	}
}

//플레이어 공격 콤보
void APlayer_KYI::saveAttackCombo() {
	if (saveAttack) {
		saveAttack = false;
		if (kickorPunch) {
			punchCombo();
		}
		else {
			kickCombo();
		}
	}
	leftHand->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	leftLeg->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	rightLeg->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

//플레이어 콤보 애니메이션 스위치
void APlayer_KYI::punchCombo() {
	switch (punchCount)
	{
	case 0:
		punchCount = 1;
		PlayAnimMontage(jab);
		break;
	case 1:
		punchCount = 2;
		PlayAnimMontage(punch);
		break;
	case 2:
		punchCount = 0;
		PlayAnimMontage(uppercut);
		break;
	}
}
void APlayer_KYI::kickCombo() {
	switch (kickCount)
	{
	case 0:
		kickCount = 1;
		PlayAnimMontage(kick);
		break;
	case 1:
		kickCount = 2;
		PlayAnimMontage(highKick);
		break;
	case 2:
		kickCount = 0;
		PlayAnimMontage(lowKick);
		break;
	}
}

void APlayer_KYI::InputFinish() {
	PlayAnimMontage(finish);	
}

//플레이어 공격콤보 재설정
void APlayer_KYI::ResetCombo() {
	IsAttacking = false;
	punchCount = 0;
	saveAttack = false;
	kickCount = 0;
	movementEnabled = true;
}

//Stomach hit
void APlayer_KYI::HurtAnim0() {
	if (!isDead && !isBlocking) {
		PlayAnimMontage(stomach);
		//OnHitDamage();
	}
	ResetCombo();
}
//head hit2
void APlayer_KYI::HurtAnim1() {
	if (!isDead && !isBlocking) {
		PlayAnimMontage(head2);
		//OnHitDamage();
	}
	ResetCombo();
}
//head hit3
void APlayer_KYI::HurtAnim2() {
	if (!isDead && !isBlocking) {
		PlayAnimMontage(head3);
		//OnHitDamage();
	}
	ResetCombo();
}
//head hit4
void APlayer_KYI::HurtAnim3() {
	if (!isDead && !isBlocking) {
		PlayAnimMontage(head4);
		//OnHitDamage();
	}
	ResetCombo();
}
//fall down 
void APlayer_KYI::HurtAnim4() {
	if (!isDead && !isBlocking) {
		PlayAnimMontage(falldown);
		//OnHitDamage();
	}
	ResetCombo();
}