#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TurnInPlaceToPlayer.generated.h"

class AEnemy_Boss;
class APlayerCharacter;

UCLASS()
class ISREAL_API UTurnInPlaceToPlayer : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UTurnInPlaceToPlayer();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    UPROPERTY(EditAnywhere, Category = "Montage")
    TObjectPtr<UAnimMontage> TurnLeftMontage;

    UPROPERTY(EditAnywhere, Category = "Montage")
    TObjectPtr<UAnimMontage> TurnRightMontage;

    UPROPERTY(EditAnywhere, Category = "Turn")
    float StopTolerance = 5.0f; // Margin

    UPROPERTY()
    TObjectPtr<AEnemy_Boss> CachedEnemy;

    UPROPERTY()
    TObjectPtr<UBlackboardComponent> CachedBlackboard;

    UPROPERTY()
    TObjectPtr<UAnimInstance> CachedAnimInst;

    UPROPERTY()
    TObjectPtr<UAnimMontage> PlayingMontage;

    UPROPERTY()
    TObjectPtr<APlayerCharacter> CachedPlayer;
};