// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsInCameraFOV.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

UBTService_IsInCameraFOV::UBTService_IsInCameraFOV()
{
	NodeName = TEXT("IsInCameraFOV");
}

void UBTService_IsInCameraFOV::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_IsInCameraFOV::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_IsInCameraFOV::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	if (OwnerCharacter == nullptr)
	{
		return;
	}

	const APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(OwnerCharacter->GetWorld(), 0);

	if (PlayerCam == nullptr)
	{
		return;
	}

	
	// Get the camera loc and rot, calculate direction of cam
	CameraLocation = PlayerCam->GetCameraLocation();
	CameraForwardVector = PlayerCam->GetActorForwardVector();
	
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector DirectionToOwner = (OwnerLocation - CameraLocation).GetSafeNormal();

	const float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CameraForwardVector, DirectionToOwner)));

	// Check if the angle is within the field of view, set true if yes
	if (Angle <= FieldOfViewAngle /2 )
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsWithinCameraFOV", true);
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsWithinCameraFOV", false);
}
