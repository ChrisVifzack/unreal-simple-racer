#include "SR_BaseCharacter.h"
#include "Components/CapsuleComponent.h"

void ASR_BaseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bIsRagdoll)
    {
        return;
    }

    auto CharacterMesh = GetMesh();
    if (!ensure(CharacterMesh))
    {
        return;
    }
    const auto PelvisBI = CharacterMesh->GetBodyInstance(TEXT("pelvis"));
    if (!PelvisBI)
    {
        return;
    }
    const auto RagdollLocation = PelvisBI->GetUnrealWorldTransform().GetLocation();
    SetActorLocation(RagdollLocation);

    //const auto PelvisRot = CharacterMesh->GetSocketRotation(TEXT("pelvis"));
    //auto Rotator = FRotator(0.0f, PelvisRot.Yaw, 0.0f);
    //SetActorLocationAndRotation(RagdollLocation, Rotator);
}

void ASR_BaseCharacter::OnRagdollActivated()
{
    if (!ensure(GetMesh()) || !ensure(GetCapsuleComponent()))
    {
        return;
    }
    GetMesh()->SetAllBodiesSimulatePhysics(true);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"), false);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASR_BaseCharacter::OnRagdollDeactivated()
{
    if (!ensure(GetMesh()) || !ensure(GetCapsuleComponent()))
    {
        return;
    }

    if (auto AnimInstance = GetMesh()->GetAnimInstance())
    {
        // save snapshot for smooth animation blending
        AnimInstance->SavePoseSnapshot(FName(TEXT("RagdollPose")));
    }

    GetMesh()->SetAllBodiesSimulatePhysics(false);
    GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"), false);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // re-attach mesh to capsule
    GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
    GetMesh()->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
    GetMesh()->SetRelativeRotation(FRotator(0, 270, 0));
}
