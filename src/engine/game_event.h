#pragma once

// GameEvent 表示已经发生的一次性全局游戏事件。
//
// 与 GameState 的区别：
// GameState = 游戏持续处于什么状态。
// GameEvent = 游戏刚刚发生了什么。
//
// 只有可能被多个高层模块关注的事件才应该放这里。
// CombatSystem / PlayerActionSystem 内部结果继续使用各自的 Result 类型。
enum class GameEvent {
    PlayerDied,
    PlayerRespawned,

    LevelStarted,
    LevelCompleted,
    LevelFailed,

    BossStarted,
    BossDefeated,

    GameCompleted
};
