// All rights reserved.


#include "VRBaseCharacter.h"
//#include "NavigationSystem.h"
//#include "VRPathFollowingComponent.h"

//DEFINE_LOG_CATEGORY(LogBaseVRCharacter);

//FName AVRBaseCharacter::LeftMotionControllerComponentName(TEXT("Left Grip Motion Controller"));
//FName AVRBaseCharacter::RightMotionControllerComponentName(TEXT("Right Grip Motion Controller"));
//FName AVRBaseCharacter::ReplicatedCameraComponentName(TEXT("VR Replicated Camera"));
//FName AVRBaseCharacter::ParentRelativeAttachmentComponentName(TEXT("Parent Relative Attachment"));
//FName AVRBaseCharacter::SmoothingSceneParentComponentName(TEXT("NetSmoother"));

// Sets default values
AVRBaseCharacter::AVRBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVRBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

