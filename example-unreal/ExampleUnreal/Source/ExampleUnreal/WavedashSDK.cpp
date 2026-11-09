#include "WavedashSDK.h"

#include "Containers/Ticker.h"

#if PLATFORM_WASM
#include <atomic>
#include <emscripten/em_asm.h>
#endif

namespace WavedashSDK
{
#if PLATFORM_WASM
	namespace
	{
		/** One answer from the page: the main thread writes Text, then sets Done. */
		struct FStringAnswer
		{
			std::atomic<int32> Done{0};
			ANSICHAR Text[512] = {};
		};

		/** Polls the answer on the game thread's ticker and hands it to OnResult once it is in. */
		void WhenAnswered(TSharedRef<FStringAnswer> Answer, TFunction<void(const FString&)> OnResult)
		{
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
				[Answer, OnResult = MoveTemp(OnResult)](float) {
					if (Answer->Done.load(std::memory_order_acquire) == 0)
					{
						return true; // not yet: ask again next tick
					}
					OnResult(UTF8_TO_TCHAR(Answer->Text));
					return false;
				}));
		}
	}
#endif

	void GetUsername(TFunction<void(const FString&)> OnResult)
	{
#if PLATFORM_WASM
		TSharedRef<FStringAnswer> Answer = MakeShared<FStringAnswer>();
		// Runs on the browser's main thread. Addresses are passed as doubles (the build is wasm64), and
		// HEAP32 is indexed with a division because >> would truncate an address above 2 GB. SDK names are
		// written in brackets, as the engine's own page code does, so a minifier can't rename them.
		MAIN_THREAD_ASYNC_EM_ASM({
			const sdk = window["Wavedash"];
			let name = "";
			try { name = (sdk && sdk["getUsername"]()) || ""; } catch (e) {}
			const bytes = new TextEncoder().encode(name).subarray(0, $1 - 1);
			HEAPU8.set(bytes, $0);
			HEAPU8[$0 + bytes.length] = 0;
			Atomics.store(HEAP32, $2 / 4, 1);
		}, (double)(UPTRINT)Answer->Text, (int32)sizeof(Answer->Text), (double)(UPTRINT)&Answer->Done);
		WhenAnswered(Answer, MoveTemp(OnResult));
#else
		OnResult(FString());
#endif
	}
}
