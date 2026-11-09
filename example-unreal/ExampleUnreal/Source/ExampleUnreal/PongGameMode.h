#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "PongGameMode.generated.h"

class UCameraComponent;

/** Spawns the game, and sets the camera, HUD and controller classes */
UCLASS()
class APongGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	APongGameMode();

	virtual void StartPlay() override;
};

/** The player's view: a fixed camera over the field */
UCLASS()
class APongCamera : public APawn
{
	GENERATED_BODY()

public:

	APongCamera();

private:

	UPROPERTY()
	TObjectPtr<UCameraComponent> Camera;
};

/** Keeps the mouse free: Pong only reads the keyboard, so the page never asks for pointer lock */
UCLASS()
class APongPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	APongPlayerController();

protected:

	virtual void BeginPlay() override;
};
