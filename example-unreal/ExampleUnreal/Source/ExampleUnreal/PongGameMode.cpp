#include "PongGameMode.h"

#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "PongGame.h"
#include "PongHUD.h"

APongGameMode::APongGameMode()
{
	DefaultPawnClass = APongCamera::StaticClass();
	PlayerControllerClass = APongPlayerController::StaticClass();
	HUDClass = APongHUD::StaticClass();
}

void APongGameMode::StartPlay()
{
	Super::StartPlay();
	GetWorld()->SpawnActor<APongGame>();
}

APongCamera::APongCamera()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	// In world space, wherever the pawn spawns (the map has no player start). The field is at the origin;
	// the camera is tilted toward the far wall, 2150 cm back from its center along the view direction
	Camera->SetAbsolute(true, true, false);
	const FRotator View(-62.f, -90.f, 0.f);
	Camera->SetRelativeLocationAndRotation(-View.Vector() * 2150.f, View);
	Camera->SetFieldOfView(58.f);
}

APongPlayerController::APongPlayerController()
{
	bShowMouseCursor = true;
}

void APongPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI Mode;
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
}
