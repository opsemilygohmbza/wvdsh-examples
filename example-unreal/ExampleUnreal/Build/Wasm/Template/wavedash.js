// Wavedash bridge for the UE 5.9 Wasm page.
//
// Staged through Build/Wasm/Template and loaded by the page's intro-shell slot
// (IntroShellScript=wavedash.js in Config/DefaultEngine.ini), after the page's own scripts.
// It registers no shell, so the engine's loading cover works as usual behind the Wavedash
// loading screen. It only mirrors that cover to the SDK:
//
// - the cover's progress bar (download, then level load) -> Wavedash.updateLoadProgressZeroToOne
// - the cover lifting (game visible), or stopping on an error or end card -> Wavedash.init
//
// Calling init on a failure matters: without it, a "this browser has no WebGPU" card would sit
// hidden behind the Wavedash loading screen.
(() => {
  const sdk = () => window.Wavedash;
  if (!sdk()) {
    console.log('WAVEDASH no SDK on this page (served outside Wavedash): bridge off');
    return;
  }

  let initialized = false;
  const init = (why) => {
    if (initialized) return;
    initialized = true;
    sdk().updateLoadProgressZeroToOne(1);
    sdk().init({ debug: false });
    console.log('WAVEDASH init (' + why + ')');
  };

  // Progress: the page writes the bar's value (0-100, never decreasing) to aria-valuenow.
  const bar = document.getElementById('progressbar');
  let last = -1;
  const report = () => {
    const value = Number(bar && bar.getAttribute('aria-valuenow'));
    if (initialized || !Number.isFinite(value) || value <= last) return;
    last = value;
    sdk().updateLoadProgressZeroToOne(Math.min(1, value / 100));
  };
  if (bar) {
    new MutationObserver(report).observe(bar, { attributes: true, attributeFilter: ['aria-valuenow'] });
    report();
  }

  // The cover's state is mirrored on body[data-cover]:
  // loading > engine > level > ready > (click) > lifted, or failed / ended from any state.
  const done = { lifted: 'game ready', click: 'waiting for a click', failed: 'page error', ended: 'game ended' };
  const watch = () => {
    const state = document.body.dataset.cover;
    if (done[state]) init(done[state]);
  };
  new MutationObserver(watch).observe(document.body, { attributes: true, attributeFilter: ['data-cover'] });
  watch();
})();
