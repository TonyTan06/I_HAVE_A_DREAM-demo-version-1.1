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
- `systems/`：战斗、平台、弹道和影子生命周期规则。
- `input/`：把键盘等设备输入转换成统一操作状态。
- `rendering/`：角色素材、角色表现和 HUD 绘制。

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
