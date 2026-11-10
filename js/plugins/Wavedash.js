//=============================================================================
// Wavedash SDK
//=============================================================================
/*:
 * @target MZ
 * @plugindesc Initializes the Wavedash SDK, exposes name-keyed score/achievement helpers,
 *             and enables canvas stretch-to-fit in web browsers.
 * @author Wavedash
 */
(() => {
    // RPG Maker MZ only stretches the canvas for mobile or nwjs by default,
    // leaving desktop-web builds locked at the native 816x624 resolution.
    // Override _defaultStretchMode so the canvas scales to the viewport.
    Graphics._defaultStretchMode = function () {
        return true;
    };

    const _Scene_Boot_start = Scene_Boot.prototype.start;
    Scene_Boot.prototype.start = function () {
        window.Wavedash.updateLoadProgressZeroToOne(0.5);  // engine scripts loaded
        _Scene_Boot_start.call(this);
        try {
            window.Wavedash.updateLoadProgressZeroToOne(1);
            window.Wavedash.init({ debug: true });
        } catch (e) {
            console.warn("[wavedash] init failed:", e);
        }
    };

    const leaderboardIds = new Map();
    async function getLeaderboardId(name) {
        if (leaderboardIds.has(name)) return leaderboardIds.get(name);
        const res = await window.Wavedash.getLeaderboard(name);
        if (!res.success) throw new Error(`[wavedash] leaderboard "${name}" missing`);
        leaderboardIds.set(name, res.data.id);
        return res.data.id;
    }

    window.wavedashSubmitScore = async function (name, score) {
        const id = await getLeaderboardId(name);
        return window.Wavedash.uploadLeaderboardScore(id, score, true);
    };
    window.wavedashUnlockAchievement = function (id) {
        return window.Wavedash.setAchievement(id, true);
    };
})();
