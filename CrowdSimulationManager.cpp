#include "CrowdSimulationManager.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionQueryParams.h"

ACrowdSimulationManager::ACrowdSimulationManager()
{
	PrimaryActorTick.bCanEverTick = true;

	MasterAnimComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MasterAnimComponent"));
	RootComponent = MasterAnimComponent;
	
	MasterAnimComponent->SetVisibility(true);
	MasterAnimComponent->SetHiddenInGame(true); // Throttles primary rendering while keeping bone data alive
	MasterAnimComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones; 

	MaxAgentCount = 50;
	MoveSpeed = 400.0f;
}

void ACrowdSimulationManager::BeginPlay()
{
	Super::BeginPlay();

	// Fallback check to find the local player character as the movement objective target
	if (!TargetObjective)
	{
		TargetObjective = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	if (!MasterAnimComponent || !MasterAnimComponent->SkeletalMesh) return;
	USkeletalMesh* TargetSharedMesh = MasterAnimComponent->SkeletalMesh;

	for (int32 i = 0; i < MaxAgentCount; ++i)
	{
		FString ComponentName = FString::Printf(TEXT("SimAgent_%d"), i);
		USkeletalMeshComponent* NewAgent = NewObject<USkeletalMeshComponent>(this, *ComponentName);

		if (NewAgent)
		{
			NewAgent->RegisterComponent();
			NewAgent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			// Link to Master Pose Component for hardware instanced rendering performance
			NewAgent->SetSkeletalMesh(TargetSharedMesh);
			NewAgent->SetMasterPoseComponent(MasterAnimComponent); 

			FVector RandomOffset = FVector(FMath::RandRange(-3000.f, 3000.f), FMath::RandRange(-3000.f, 3000.f), 0.f);
			NewAgent->SetRelativeLocation(RandomOffset);
			NewAgent->SetCollisionProfileName(TEXT("NoCollision"));

			// ==========================================================
			// LOW-LEVEL GPU RENDERING AND THREAD COUPLING OPTIMIZATIONS
			// ==========================================================
			NewAgent->bDisableClothSimulation = true;
			NewAgent->SetPhysicsLinearVelocity(FVector::ZeroVector);
			NewAgent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered; 
			NewAgent->bComponentUseFixedSkelBounds = true;
			NewAgent->SetCastShadow(false); // Disabling shadows saves massive GPU Draw Calls

			AgentMeshArray.Add(NewAgent);
		}
	}
}

void ACrowdSimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetObjective || AgentMeshArray.Num() == 0) return;

	FVector ObjectiveLocation = TargetObjective->GetActorLocation();
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	int32 TotalActiveAgents = AgentMeshArray.Num();

	for (int32 i = 0; i < TotalActiveAgents; ++i)
	{
		USkeletalMeshComponent* AgentComp = AgentMeshArray[i];
		if (!AgentComp) continue;

		FVector CurrentLoc = AgentComp->GetComponentLocation();
		FVector Direction = (ObjectiveLocation - CurrentLoc).GetSafeNormal();
		
		// 1. Compute Base Velocity Translation Vector
		FVector NewLoc = CurrentLoc + (Direction * MoveSpeed * DeltaTime);

		// 2. Spatial Cache Neighbor Check: Lightweight Inter-Agent Push Forces
		int32 NeighborIndex = (i + 1) % TotalActiveAgents;
		if (AgentMeshArray[NeighborIndex])
		{
			FVector OtherLoc = AgentMeshArray[NeighborIndex]->GetComponentLocation();
			float DistanceSq = FVector::DistSquared(NewLoc, OtherLoc);
			if (DistanceSq < 14400.0f) // Clamped to 120 units proximity matrix (120^2)
			{
				FVector PushForce = (NewLoc - OtherLoc).GetSafeNormal();
				NewLoc += PushForce * 120.0f * DeltaTime; 
			}
		}

		// 3. Raycast Ground-Snapping Vector Channel Logic
		FHitResult FloorHit;
		FVector TraceStart = FVector(NewLoc.X, NewLoc.Y, NewLoc.Z + 300.0f);
		FVector TraceEnd = FVector(NewLoc.X, NewLoc.Y, NewLoc.Z - 600.0f);

		if (GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
		{
			NewLoc.Z = FloorHit.ImpactPoint.Z;
		}

		// 4. Orientation Yaw-Only Transform Calculations
		FRotator BaseRotation = Direction.Rotation();
		FRotator CorrectedRotation = FRotator(0.0f, BaseRotation.Yaw - 90.0f, 0.0f);

		AgentComp->SetWorldLocationAndRotation(NewLoc, CorrectedRotation);
	}
}
