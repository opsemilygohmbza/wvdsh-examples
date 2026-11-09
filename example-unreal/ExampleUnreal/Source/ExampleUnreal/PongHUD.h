#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PongHUD.generated.h"

class APongGame;

/** The score, the controls, and the Wavedash player's name (read from the SDK through WavedashSDK) */
UCLASS()
class APongHUD : public AHUD
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

private:

	void DrawCentered(const FString& Text, float Y, float Scale, const FLinearColor& Color);

	TWeakObjectPtr<APongGame> Game;

	/** Empty until the SDK answers, and when there is no SDK */
	FString Greeting;
};
