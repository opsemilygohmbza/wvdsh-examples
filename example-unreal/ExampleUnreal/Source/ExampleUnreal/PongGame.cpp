#include "PongGame.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace Pong
{
	// The same rules as the other Wavedash pong examples (example-js)
	constexpr float FieldW = 16.f;
	constexpr float FieldH = 9.f;
	constexpr float PaddleW = 0.35f;
	constexpr float PaddleH = 2.f;
	constexpr float BallSize = 0.35f;
	constexpr float PlayerSpeed = 9.f;
	constexpr float AISpeed = 5.5f;
	constexpr float BallSpeedX = 6.5f;
	constexpr float BallSpeedY = 2.8f;
	constexpr float MaxVX = 12.f;
	constexpr float MaxVY = 8.f;
	constexpr float PaddleX = FieldW / 2.f - 0.9f;

	/** Engine shapes are 100 cm across */
	constexpr float CmPerUnit = 100.f;
}

APongGame::APongGame()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Engine content only: these assets ship with every engine and are cooked because they are referenced here
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	CubeMesh = Cube.Object;
	SphereMesh = Sphere.Object;
	ShapeMaterial = Material.Object;

	// One movable sun from behind the camera, so the faces the player sees are lit and cast shadows
	UDirectionalLightComponent* Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	Sun->SetupAttachment(RootComponent);
	Sun->SetMobility(EComponentMobility::Movable);
	Sun->SetRelativeRotation(FRotator(-55.f, -70.f, 0.f));
	Sun->SetIntensity(6.f);
	Sun->SetCastShadows(true);
}

UStaticMeshComponent* APongGame::AddShape(const TCHAR* Name, UStaticMesh* Mesh, const FLinearColor& Color)
{
	UStaticMeshComponent* Shape = NewObject<UStaticMeshComponent>(this, Name ? FName(Name) : NAME_None);
	Shape->SetupAttachment(RootComponent);
	Shape->SetMobility(EComponentMobility::Movable);
	Shape->SetStaticMesh(Mesh);
	Shape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* Paint = UMaterialInstanceDynamic::Create(ShapeMaterial, this);
	Paint->SetVectorParameterValue(TEXT("Color"), Color);
	Shape->SetMaterial(0, Paint);
	Shape->RegisterComponent();
	return Shape;
}

void APongGame::Place(UStaticMeshComponent* Shape, float X, float Y, float Z, float SizeX, float SizeY, float SizeZ) const
{
	// Game y points up the screen; the camera looks down the world's -Y axis, so it maps to -Y
	Shape->SetRelativeLocation(FVector(X, -Y, Z) * Pong::CmPerUnit);
	Shape->SetRelativeScale3D(FVector(SizeX, SizeY, SizeZ));
}

void APongGame::BeginPlay()
{
	Super::BeginPlay();
	using namespace Pong;

	const FLinearColor Floor(0.02f, 0.025f, 0.035f);
	const FLinearColor Line(0.25f, 0.25f, 0.3f);
	const FLinearColor Player(0.1f, 0.55f, 1.f);
	const FLinearColor Opponent(1.f, 0.3f, 0.2f);
	const FLinearColor White(0.9f, 0.9f, 0.9f);

	Place(AddShape(TEXT("Floor"), CubeMesh, Floor), 0.f, 0.f, -0.55f, FieldW + 2.f, FieldH + 1.f, 0.1f);
	Place(AddShape(TEXT("WallTop"), CubeMesh, Line), 0.f, FieldH / 2.f + 0.15f, -0.25f, FieldW + 2.f, 0.3f, 0.5f);
	Place(AddShape(TEXT("WallBottom"), CubeMesh, Line), 0.f, -FieldH / 2.f - 0.15f, -0.25f, FieldW + 2.f, 0.3f, 0.5f);
	for (float DashY = -FieldH / 2.f + 0.5f; DashY <= FieldH / 2.f - 0.4f; DashY += 0.9f)
	{
		Place(AddShape(nullptr, CubeMesh, Line), 0.f, DashY, -0.49f, 0.08f, 0.4f, 0.02f);
	}

	PlayerPaddle = AddShape(TEXT("PlayerPaddle"), CubeMesh, Player);
	AIPaddle = AddShape(TEXT("AIPaddle"), CubeMesh, Opponent);
	Ball = AddShape(TEXT("Ball"), SphereMesh, White);

	Serve(FMath::RandBool() ? 1.f : -1.f);
}

void APongGame::Serve(float Direction)
{
	using namespace Pong;
	PlayerY = 0.f;
	AIY = 0.f;
	BallPosition = FVector2D::ZeroVector;
	float VY = FMath::FRandRange(-BallSpeedY, BallSpeedY);
	if (FMath::Abs(VY) < 1.2f)
	{
		VY = VY < 0.f ? -1.5f : 1.5f;
	}
	BallVelocity = FVector2D(Direction * BallSpeedX, VY);
}

void APongGame::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	using namespace Pong;
	const float Dt = FMath::Min(DeltaSeconds, 0.05f);

	// Player: W/S or the arrow keys (read straight from the player controller, so no input assets are needed)
	float Dir = 0.f;
	if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		Dir += (PC->IsInputKeyDown(EKeys::W) || PC->IsInputKeyDown(EKeys::Up)) ? 1.f : 0.f;
		Dir -= (PC->IsInputKeyDown(EKeys::S) || PC->IsInputKeyDown(EKeys::Down)) ? 1.f : 0.f;
	}
	const float PadMax = FieldH / 2.f - PaddleH / 2.f;
	PlayerY = FMath::Clamp(PlayerY + Dir * PlayerSpeed * Dt, -PadMax, PadMax);

	// AI: follows the ball at a capped speed
	const float AIStep = AISpeed * Dt;
	AIY = FMath::Clamp(AIY + FMath::Clamp(BallPosition.Y - AIY, -AIStep, AIStep), -PadMax, PadMax);

	// Ball and walls
	BallPosition += BallVelocity * Dt;
	const float Half = BallSize / 2.f;
	if (BallPosition.Y + Half > FieldH / 2.f) { BallPosition.Y = FieldH / 2.f - Half; BallVelocity.Y = -FMath::Abs(BallVelocity.Y); }
	if (BallPosition.Y - Half < -FieldH / 2.f) { BallPosition.Y = -FieldH / 2.f + Half; BallVelocity.Y = FMath::Abs(BallVelocity.Y); }

	// Paddles: the hit point steers the ball, and every hit speeds it up
	auto Hits = [&](float PaddleCenterX, float PaddleY) {
		return BallPosition.X + Half >= PaddleCenterX - PaddleW / 2.f && BallPosition.X - Half <= PaddleCenterX + PaddleW / 2.f
			&& BallPosition.Y + Half >= PaddleY - PaddleH / 2.f && BallPosition.Y - Half <= PaddleY + PaddleH / 2.f;
	};
	auto Bounce = [&](float PaddleY, float Sign) {
		const float Impact = FMath::Clamp((BallPosition.Y - PaddleY) / (PaddleH / 2.f), -1.f, 1.f);
		BallVelocity.X = Sign * FMath::Min(FMath::Abs(BallVelocity.X) * 1.05f + 0.4f, MaxVX);
		BallVelocity.Y = FMath::Clamp(BallVelocity.Y + Impact * 3.5f, -MaxVY, MaxVY);
	};
	if (BallVelocity.X < 0.f && Hits(-PaddleX, PlayerY))
	{
		Bounce(PlayerY, 1.f);
		BallPosition.X = -PaddleX + PaddleW / 2.f + Half;
	}
	if (BallVelocity.X > 0.f && Hits(PaddleX, AIY))
	{
		Bounce(AIY, -1.f);
		BallPosition.X = PaddleX - PaddleW / 2.f - Half;
	}

	// Scoring
	if (BallPosition.X < -FieldW / 2.f - 1.f || BallPosition.X > FieldW / 2.f + 1.f)
	{
		const bool bPlayerScored = BallPosition.X > 0.f;
		int32& Score = bPlayerScored ? PlayerScore : AIScore;
		if (++Score >= PongRules::WinScore)
		{
			PlayerScore = 0;
			AIScore = 0;
		}
		Serve(bPlayerScored ? 1.f : -1.f);
	}

	Place(PlayerPaddle, -PaddleX, PlayerY, -0.25f, PaddleW, PaddleH, 0.5f);
	Place(AIPaddle, PaddleX, AIY, -0.25f, PaddleW, PaddleH, 0.5f);
	Place(Ball, BallPosition.X, BallPosition.Y, -0.32f, BallSize, BallSize, BallSize);
}
