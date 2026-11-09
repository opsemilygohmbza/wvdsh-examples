#pragma once

#include "CoreMinimal.h"

/**
 * Calls the Wavedash JavaScript SDK (window.Wavedash) from game code.
 *
 * Unreal's web build runs the game thread in a pthread Worker, but window.Wavedash lives on the
 * browser's main thread. Each call is proxied there without blocking the game thread, and an answer
 * comes back to the game thread on a later tick. Outside a Wasm build, or on a page without the SDK
 * (a plain web server), the calls do nothing and answers are empty.
 */
namespace WavedashSDK
{
	/** The signed-in player's Wavedash username. OnResult runs on the game thread ("" without the SDK). */
	void GetUsername(TFunction<void(const FString&)> OnResult);
}
