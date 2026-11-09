#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PongGame.generated.h"

class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;

namespace PongRules
{
	/** First to this many points wins; the scores then reset */
	constexpr int32 WinScore = 7;
}

/**
 *  The whole game: field, paddles, ball, lights and rules, built from engine shapes in C++.
 *  Field units are meters (game x to the right, game y up the screen); one unit is 100 cm in the world.
 */
UCLASS()
class APongGame : public AActor
{
	GENERATED_BODY()

public:

	APongGame();

	virtual void Tick(float DeltaSeconds) override;

	int32 GetPlayerScore() const { return PlayerScore; }
	int32 GetAIScore() const { return AIScore; }

protected:

	virtual void BeginPlay() override;

private:

	UStaticMeshComponent* AddShape(const TCHAR* Name, UStaticMesh* Mesh, const FLinearColor& Color);
	void Place(UStaticMeshComponent* Shape, float X, float Y, float Z, float SizeX, float SizeY, float SizeZ) const;
	void Serve(float Direction);

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> ShapeMaterial;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> PlayerPaddle;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> AIPaddle;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Ball;

	float PlayerY = 0.f;
	float AIY = 0.f;
	FVector2D BallPosition = FVector2D::ZeroVector;
	FVector2D BallVelocity = FVector2D::ZeroVector;
	int32 PlayerScore = 0;
	int32 AIScore = 0;
};
