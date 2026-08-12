#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "CrowdSimulationManager.generated.h"

UCLASS()
class HORDECPP_API ACrowdSimulationManager : public AActor
{
	GENERATED_BODY()

public:	
	ACrowdSimulationManager();

protected:
	// Native engine lifespan cycles overrides
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crowd Simulation | Core")
	USkeletalMeshComponent* MasterAnimComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd Simulation | Settings")
	int32 MaxAgentCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd Simulation | Settings")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd Simulation | Settings")
	AActor* TargetObjective;

private:
	// Low-level array tracking individual skeletal components in memory
	UPROPERTY()
	TArray<USkeletalMeshComponent*> AgentMeshArray;
};
