// All rights reserved.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VRBaseCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class LAZERTAGX_API UVRBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:

	UVRBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PerformMovement(float DeltaSeconds) override;
	//virtual void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration) override;

	// Overriding this to run the seated logic
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	// Skip force updating position if we are seated.
	virtual bool ForcePositionUpdate(float DeltaTime) override;

	// Adding seated transition
	void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	// Called when a valid climbing step up movement is found, if bound to the default auto step up is not performed to let custom step up logic happen instead.
	//UPROPERTY(BlueprintAssignable, Category = "VRMovement")
	//	FVROnPerformClimbingStepUp OnPerformClimbingStepUp;

	//virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	// Can't be inline anymore
	FVector GetActorFeetLocationVR() const;

	FORCEINLINE bool HasRequestedVelocity()
	{
		return bHasRequestedVelocity;
	}

	void SetHasRequestedVelocity(bool bNewHasRequestedVelocity);
	bool IsClimbing() const;

	// Sets the crouching half height since it isn't exposed during runtime to blueprints
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		void SetCrouchedHalfHeight(float NewCrouchedHalfHeight);

	// Setting this higher will divide the wall slide effect by this value, to reduce collision sliding.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRMovement", meta = (ClampMin = "0.0", UIMin = "0", ClampMax = "5.0", UIMax = "5"))
		float VRWallSlideScaler;

	/** Custom version of SlideAlongSurface that handles different movement modes separately; namely during walking physics we might not want to slide up slopes. */
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;

	// Add in the custom replicated movement that climbing mode uses, this is a cutom vector that is applied to character movements
	// on the next tick as a movement input..
	UFUNCTION(BlueprintCallable, Category = "BaseVRCharacterMovementComponent|VRLocations")
		void AddCustomReplicatedMovement(FVector Movement);

	// Perform a snap turn in line with the move action system
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		void PerformMoveAction_SnapTurn(float SnapTurnDeltaYaw);

	// Perform a rotation set in line with the move actions system
	// This node specifically sets the FACING direction to a value, where your HMD is pointed
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		void PerformMoveAction_SetRotation(float NewYaw);

	// Perform a teleport in line with the move action system
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		void PerformMoveAction_Teleport(FVector TeleportLocation, FRotator TeleportRotation, bool bSkipEncroachmentCheck = false);

	// Perform StopAllMovementImmediately in line with the move action system
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		void PerformMoveAction_StopAllMovement();

	// Perform a custom moveaction that you define, will call the OnCustomMoveActionPerformed event in the character when processed so you can run your own logic
	// Be sure to set the minimum data replication requirements for your move action in order to save on replication.
	// Move actions are currently limited to 1 per frame.
	//UFUNCTION(BlueprintCallable, Category = "VRMovement")
	//	void PerformMoveAction_Custom(EVRMoveAction MoveActionToPerform, EVRMoveActionDataReq DataRequirementsForMoveAction, FVector MoveActionVector, FRotator MoveActionRotator);

	//FVRMoveActionArray MoveActionArray;

	//bool CheckForMoveAction();
	//bool DoMASnapTurn(FVRMoveActionContainer& MoveAction);
	//bool DoMASetRotation(FVRMoveActionContainer& MoveAction);
	//bool DoMATeleport(FVRMoveActionContainer& MoveAction);
	//bool DoMAStopAllMovement(FVRMoveActionContainer& MoveAction);

	FVector CustomVRInputVector;
	FVector AdditionalVRInputVector;
	FVector LastPreAdditiveVRVelocity;
	bool bApplyAdditionalVRInputVectorAsNegative;

	// Rewind the relative movement that we had with the HMD
	inline void RewindVRRelativeMovement()
	{
		if (bApplyAdditionalVRInputVectorAsNegative)
		{
			//FHitResult AHit;
			MoveUpdatedComponent(-AdditionalVRInputVector, UpdatedComponent->GetComponentQuat(), false);
		}
		//SafeMoveUpdatedComponent(-AdditionalVRInputVector, UpdatedComponent->GetComponentQuat(), false, AHit);
	}

	// Rewind the relative movement that we had with the HMD, this is exposed to Blueprint so that custom movement modes can use it to rewind prior to movement actions.
	// Returns the Vector required to get back to the original position (for custom movement modes)
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		FVector RewindVRMovement();

	// Gets the current CustomInputVector for use in custom movement modes
	UFUNCTION(BlueprintCallable, Category = "VRMovement")
		FVector GetCustomInputVector();

	bool bWasInPushBack;
	bool bIsInPushBack;
	void StartPushBackNotification(FHitResult HitResult);
	void EndPushBackNotification();

	//virtual void SendClientAdjustment() override;

	virtual bool VerifyClientTimeStamp(float TimeStamp, FNetworkPredictionData_Server_Character & ServerData) override;

	inline void ApplyVRMotionToVelocity(float deltaTime)
	{
		if (AdditionalVRInputVector.IsNearlyZero())
		{
			LastPreAdditiveVRVelocity = FVector::ZeroVector;
			return;
		}

		LastPreAdditiveVRVelocity = (AdditionalVRInputVector) / deltaTime;// Velocity; // Save off pre-additive Velocity for restoration next tick	
		Velocity += LastPreAdditiveVRVelocity;
	}

	inline void RestorePreAdditiveVRMotionVelocity()
	{
		Velocity -= LastPreAdditiveVRVelocity;
		LastPreAdditiveVRVelocity = FVector::ZeroVector;
	}

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void PhysCustom_Climbing(float deltaTime, int32 Iterations);
	virtual void PhysCustom_LowGrav(float deltaTime, int32 Iterations);

	// Skip updates with rotational differences
	virtual void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation, const FVector& NewLocation, const FQuat& NewRotation) override;

	/**
	* Smooth mesh location for network interpolation, based on values set up by SmoothCorrection.
	* Internally this simply calls SmoothClientPosition_Interpolate() then SmoothClientPosition_UpdateVisuals().
	* This function is not called when bNetworkSmoothingComplete is true.
	* @param DeltaSeconds Time since last update.
	*/
	virtual void SmoothClientPosition(float DeltaSeconds) override;

	/** Update mesh location based on interpolated values. */
	void SmoothClientPosition_UpdateVRVisuals();
	
};
