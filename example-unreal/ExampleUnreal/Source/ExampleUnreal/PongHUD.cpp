#include "PongHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "EngineUtils.h"
#include "PongGame.h"
#include "WavedashSDK.h"

void APongHUD::BeginPlay()
{
	Super::BeginPlay();

	WavedashSDK::GetUsername([WeakThis = TWeakObjectPtr<APongHUD>(this)](const FString& Username) {
		if (WeakThis.IsValid() && !Username.IsEmpty())
		{
			WeakThis->Greeting = FString::Printf(TEXT("Playing on Wavedash as %s"), *Username);
		}
	});
}

void APongHUD::DrawCentered(const FString& Text, float Y, float Scale, const FLinearColor& Color)
{
	UFont* Font = GEngine->GetLargeFont();
	float Width = 0.f, Height = 0.f;
	GetTextSize(Text, Width, Height, Font, Scale);
	DrawText(Text, Color, (Canvas->ClipX - Width) / 2.f, Y, Font, Scale);
}

void APongHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas)
	{
		return;
	}

	if (!Game.IsValid())
	{
		for (TActorIterator<APongGame> It(GetWorld()); It; ++It)
		{
			Game = *It;
		}
	}
	if (Game.IsValid())
	{
		DrawCentered(FString::Printf(TEXT("%d   %d"), Game->GetPlayerScore(), Game->GetAIScore()), Canvas->ClipY * 0.03f, 2.5f, FLinearColor::White);
	}

	const float Scale = FMath::Max(1.f, Canvas->ClipY / 720.f);
	DrawCentered(FString::Printf(TEXT("W / S or Up / Down to move.  First to %d."), PongRules::WinScore), Canvas->ClipY - 40.f * Scale, Scale, FLinearColor(0.7f, 0.7f, 0.75f));
	if (!Greeting.IsEmpty())
	{
		DrawText(Greeting, FLinearColor(0.55f, 0.8f, 1.f), 20.f * Scale, 18.f * Scale, GEngine->GetLargeFont(), Scale);
	}
}
