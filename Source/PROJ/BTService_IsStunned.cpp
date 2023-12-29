// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsStunned.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PawnMovementComponent.h"

UBTService_IsStunned::UBTService_IsStunned()
{
	NodeName = TEXT("IsStunned");
}

void UBTService_IsStunned::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void UBTService_IsStunned::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (OwnerComp.GetAIOwner() == nullptr) return;

	TreeComponent = &OwnerComp;

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsStunned", OwnerCharacter->bIsStunned);
	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("StunnedDuration", OwnerCharacter->StunnedDuration);

	// Repositioning only relevant for flying enemy 
	if(!OwnerCharacter->IsA(AFlyingEnemyCharacter::StaticClass()))
		return; 
	
	if (OwnerCharacter->bIsStunned && /*OwnerCharacter->StunnedDuration > OwnerCharacter->StaggeredDuration OwnerCharacter->StunnedDuration > 0 &&*/
		!OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsRepositioning"))
	{
		OwnerCharacter->GetMovementComponent()->Velocity = FVector::ZeroVector;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsRepositioning", true);
		GetWorld()->GetTimerManager().SetTimer(StopRepositioningTimerHandle, this, &UBTService_IsStunned::StopRepositioning,
			OwnerCharacter->RepositioningDuration + OwnerCharacter->StunnedDuration, false);
	}
	
}

void UBTService_IsStunned::StopRepositioning()
{
	if(!IsValid(TreeComponent))
		return; 
	
	TreeComponent->GetBlackboardComponent()->SetValueAsBool("bIsRepositioning", false);
	TreeComponent->GetBlackboardComponent()->ClearValue("bIsRepositioning");

	GetWorld()->GetTimerManager().ClearTimer(StopRepositioningTimerHandle);
}
