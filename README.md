# I_HAVE_A_DREAM-demo-version-1.0

I have a dream 群里的东西。

## 代码结构

程序入口只负责组合并启动游戏：

```text
main
  → GameEngine（窗口、主循环、场景生命周期）
      → Scene 接口
          → GameScene（当前关卡的内容与执行顺序）
              → entities / systems / input / rendering
```

`src/` 按职责划分：

- `engine/`：游戏运行框架和通用场景接口。
- `scenes/`：具体场景；当前游戏内容位于 `GameScene`。
- `entities/`：玩家、敌人、影子和物品等实体状态。
- `systems/`：玩家动作、战斗、平台、弹道和影子生命周期规则。
- `input/`：把键盘、手柄等设备输入转换成统一操作状态，不直接控制实体。
- `rendering/`：角色素材、角色表现和 HUD 绘制。

玩家控制遵循单向依赖：

```text
键盘 / 后续手柄
  → PlayerController
      → PlayerInputState（设备无关的本帧操作意图）
          → PlayerActionSystem
              → Player / PlayerShadow 的移动、跳跃、攻击、防御与闪避方法
```

`Player` 与 `PlayerShadow` 不引用输入控制器、按键或输入状态，也没有 `applyInput()`。左右输入冲突、无方向闪避和影子技能同步等“如何解释操作”的规则集中在 `PlayerActionSystem`；角色实体只保存自身状态并执行明确的领域动作。因此后续接入手柄、AI 或操作回放时，只需生成 `PlayerInputState`，无需修改角色。

`GameScene` 的帧更新只负责按固定顺序编排功能阶段：玩家动作、主要角色物理、短时表现、友方弹道生成、敌军战斗、弹道结算、玩家生命周期、友方近战、击杀经验、影子生命周期。每个阶段由一个具名 `update...()` 或 `spawn...()` 函数负责，具体规则留在对应 System 或阶段 helper 中。`updateWithInput()` 允许手柄、AI、回放和测试直接提交设备无关输入，同时复用完全相同的帧管线。

## 编码约定

项目自有的 C++ 代码（`src/` 与 `tests/`）不使用 lambda。可复用的局部逻辑应提取为具名成员 helper 或静态 helper；需要携带上下文的回调使用具名 callable helper，并明确其引用对象的生命周期。第三方依赖保持上游实现，不纳入该约定。

## 游戏流程状态

`GameEngine` 持有全局 `GameStateManager`。`GameState` 表示一个会持续一段时间的游戏阶段；角色移动、攻击、受伤等瞬时行为不属于全局游戏状态。

| 状态 | 用途 |
| --- | --- |
| `Booting` | 引擎刚创建、窗口和资源尚未准备完成 |
| `MainMenu` | 主菜单 |
| `Loading` | 加载或切换场景 |
| `Playing` | 正常游玩 |
| `Paused` | 暂停 |
| `Victory` | 胜利结算 |
| `Defeat` | 失败结算 |
| `Exiting` | 正在退出 |

当前版本启动时经历 `Booting → Loading → Playing`。游玩中按 `Esc` 会切换到 `Paused`，暂停期间角色、敌人、弹道、技能冷却和影子生命周期全部停止更新；继续游戏后回到 `Playing`。主菜单和胜负结算状态仍为后续场景预留。

状态真正改变时，`GameStateManager` 会发送包含前后状态的 `GameStateChangedEvent`。音乐、UI 等系统以后可以从 `GameEngine` 取得只读的状态管理器，通过 `addStateChangedListener()` 监听，并在不再需要时用返回的监听 ID 调用 `removeStateChangedListener()`；状态转换本身由引擎和后续场景管理流程负责。

## 暂停菜单与存档

游玩中按 `Esc` 打开暂停菜单。菜单提供四个选项：

- `RESUME`：继续游戏；在菜单主页再次按 `Esc` 也可以继续。
- `SETTINGS`：进入设置页；当前支持窗口与全屏切换。
- `SAVE`：保存后继续停留在暂停菜单。
- `SAVE & QUIT`：保存成功后退出；如果保存失败，会留在菜单并显示错误，不会丢弃进度后强制退出。

菜单支持 `W/S`、上下方向键、Enter、空格和鼠标。由于项目目前还没有中文字库，界面标签暂用英文，避免中文显示成方框。

当前存档格式版本为 `1`，保存场景 ID 及玩家位置、生命、等级、经验、金币和朝向。敌人复活计时、影子、弹道和技能短冷却属于瞬时战斗状态，不写入存档；读取存档入口将在后续主菜单中接入。

默认存档位置：

- macOS：`~/Library/Application Support/I_HAVE_A_DREAM/savegame.sav`
- Windows：`%APPDATA%\I_HAVE_A_DREAM\savegame.sav`
- Linux：`$XDG_DATA_HOME/I_HAVE_A_DREAM/savegame.sav`；未设置时使用 `~/.local/share/I_HAVE_A_DREAM/savegame.sav`

# 背景：
背景是中式朋克和赛博朋克的世界观下，人们考道兵作战。道兵是一群有科技支持的统一同质化的军队，使用符咒，铜钱和唐刀为武器，可以使用出一些类似于修仙里的能力，比如符文定神，开立场，回血，铜钱远程攻击，埋伏陷阱，唐刀劈砍格挡。

#### 运行debug若有raylib报错，在终端运行以下代码：
- rm -rf build
- cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=OFF

#### 在运行时请耐心等待，并保持网络良好，第一次启动慢是正常的（5-10分钟左右），debug完成后运行以下代码：
- cmake -S . -B build
- cmake --build build
进度条到100%后接着下一步
- ./build/I_HAVE_A_DREAM （Mac/Linux）
- .\build\Debug\I_HAVE_A_DREAM.exe （Win）
就可以启动游戏窗口了
##### 第二次之后写完代码调试后就只用输入这两行：
- cmake --build build
- ./build/I_HAVE_A_DREAM （Mac/Linux）
- .\build\Debug\I_HAVE_A_DREAM.exe （Win）

### 运行docker镜像

如果只是想快速构建并运行项目，可以直接使用 Docker。这样不需要在本机手动配置 CMake、raylib 或 Linux 图形环境。

首先确保已经安装 Docker Desktop 或 Docker Engine。

在项目根目录下运行：

```bash
docker compose up --build
```

构建完成后，在浏览器中打开：

```
http://localhost:6080/vnc.html?autoconnect=true&resize=scale
```

#### 创建一个branch，先：
- git switch main
- git pull origin main
- git fetch

- 再，git switch -c

#### 然后在你push之前，做一个检查：

- git log --oneline main..（你的branch，例：feature/player）
##### 还有，push前，pull一下确保和main一致
