// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ_BossAnim.h"
#include "BossFSM.h"

void UHJ_BossAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();;
}

void UHJ_BossAnim::AnimNotify_EndAppear()
{
	UE_LOG(LogTemp, Warning, TEXT("1111111111111"));
	// ���� ��ġ ������ ��ġ�� ����

  	//bfsm->SetBoss();
	bfsm->FinishAppear();
}
