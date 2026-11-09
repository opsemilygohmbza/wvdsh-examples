# Unreal Engine

A Pong game on Wavedash written in C++ for Unreal Engine 5.9, packaged with the engine's Wasm (WebAssembly + WebGPU) platform.

Play it: [playtest build](https://wavedash.com/playtest/unreal-example/cffaac50-d5e0-4a7a-8c01-559042f195cc). The packaged build isn't committed: its `.wasm` (about 140 MB) is over GitHub's file size limit.

## What it demonstrates

- **Load progress and `Wavedash.init()`** from a small page script, `ExampleUnreal/Build/Wasm/Template/wavedash.js`. UAT stages files in `Build/Wasm/Template/` next to the engine's page, and `IntroShellScript=wavedash.js` in `Config/DefaultEngine.ini` makes the page load it. It forwards the page's progress bar to `Wavedash.updateLoadProgressZeroToOne()` and calls `Wavedash.init()` when the loading cover lifts, or when the page shows an error card.
- **Calling the SDK from C++** with `WavedashSDK.h` / `WavedashSDK.cpp`. Unreal's web build runs the game thread in a pthread Worker, while `window.Wavedash` lives on the browser's main thread. `WavedashSDK::GetUsername()` proxies the call there with `MAIN_THREAD_ASYNC_EM_ASM`, without blocking, and hands the answer back to the game thread on a later tick. The HUD (`PongHUD.cpp`) shows it.
- **A game with no assets of its own.** The field, paddles, ball, light and camera are built in C++ from the engine's basic shapes (`PongGame.cpp`) on the engine's empty `Entry` map. The project is source and config only, so it opens and builds from a clone.

## Prerequisites

- Unreal Engine 5.9 Preview 1 or later, with the web libraries: the Launcher build, or a source build of [`5.9.0-preview-1`](https://github.com/EpicGames/UnrealEngine/releases/tag/5.9.0-preview-1). Tested with a source build on macOS (Apple silicon).
- [emscripten 6.0.10](https://github.com/emscripten-core/emsdk), exactly: `emsdk install 6.0.10 && emsdk activate 6.0.10`, then `export WASM_EMSDK_ROOT=/path/to/emsdk`.
- [Wavedash CLI](https://github.com/wvdsh/cli/releases)
- A browser with WebGPU (Chrome or Edge; Safari on macOS)

## Quick start

1. Package the game from the engine root (`Engine\Build\BatchFiles\RunUAT.bat` and `^` line breaks on Windows):

    ```
    Engine/Build/BatchFiles/RunUAT.sh BuildCookRun \
      -project=<this folder>/ExampleUnreal/ExampleUnreal.uproject -platform=Wasm -clientconfig=Shipping \
      -build -cook -stage -pak -iostore -package -archive -archivedirectory=<this folder>/build \
      -map=/Engine/Maps/Entry -AdditionalPackageOptions=-WasmNoBrotli -unattended -utf8output
    ```

    The first run also builds the editor, ShaderCompileWorker and UnrealPak for your machine. The staged site is `build/Wasm`; `build/Wasm-Debug` holds the symbol map and is never uploaded. `-WasmNoBrotli` skips the `.br` copies, since Wavedash's CDN compresses whole-file downloads itself.

2. Replace `game_id` in [`wavedash.toml`](./wavedash.toml) with your Wavedash game ID.
3. Run:

    ```
    wavedash dev
    ```

On a source build, don't pass `--exclude=IOS` to `Setup.sh`, even on a Mac: UAT always builds its iOS automation project, which needs files from those dependencies.

## Controls

- `W` / `S` or `↑` / `↓`: move your paddle (blue, left). The AI plays red. First to 7 wins.
