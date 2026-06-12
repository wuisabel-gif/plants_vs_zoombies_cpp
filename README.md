# Plants vs. Zombies C++ Game

## English

### Project Description

A C++17/SFML desktop game inspired by Plants vs. Zombies, built as my first C++ game project. 
It includes grid-based plant placement, seed packets,
automatic sun collection, projectile combat, multiple zombie types, level
progression, saved progress, win/loss states, and looping background music.

The project uses SFML for rendering, window management, input, and audio. The
gameplay is organized around classic computer science ideas such as a 2D grid,
dynamic arrays, asset caching, inheritance, polymorphism, and finite state
machines.

### Asset Credit

The UI folder and related visual assets used by this project come from the
GitHub repository
[lixing-hust/plants_vs_zoombie](https://github.com/lixing-hust/plants_vs_zoombie).
Credit goes to that repository for providing those UI assets.

Some UI artwork and text are currently Chinese because they come from the
available asset set. Gameplay source code and upload-facing C++ files are kept
in English.

### Requirements

- CMake 3.16 or newer
- C++17 compiler
- SFML 3.0 or newer

On macOS with Homebrew:

```bash
brew install sfml
```

### Build

```bash
cmake -S . -B build
cmake --build build
```

If you move or clone the project into a new folder, run both commands again
from the project root. `cmake --build` by itself is incomplete because CMake
needs the build directory name:

```bash
cmake --build build
```

On Apple Silicon/Homebrew systems, this project defaults to `arm64` so it links
cleanly with `/opt/homebrew` SFML.

### Run

```bash
./build/Game
```

Or build and launch:

```bash
cmake --build build --target run
```

Running `cmake --build build` only compiles the executable; it does not open the
game window.

### Current Controls

- Click the Adventure button on the start menu to enter the lawn scene.
- Click a seed packet, then click a lawn grid cell to place it.
- Sun tokens are collected automatically after they appear.
- Shooter plants automatically fire at zombies in their lane.
- Press `M` to toggle music state and `N` to toggle sound-effect state.
- Press `R` on the start menu to reset saved progress.
- Press `Q` or `Esc` to quit.

### Soundtrack

The game automatically looks for a background music file in one of these
folders:

```text
soundtrack/
Soundtrack/
assets/soundtrack/
assets/Soundtrack/
```

It plays the first supported audio file it finds and loops it while music is
enabled. Supported extensions are `.ogg`, `.wav`, `.flac`, `.aiff`, and `.mp3`.
Use `M` in game to pause or resume the soundtrack.

### Plant Damage Summary

Current zombie health is `6` for the fast zombie, `7` for BasicZombie, `10`
for the tough zombie, and `14` for ConeheadZombie. Pea and snow-pea projectiles
each deal `1` damage. Repeater fires two peas per attack cycle, so it needs the
same total hits but fewer firing cycles. CherryBomb defeats any current zombie
type with one explosion if the zombie is inside its blast radius.

| Plant | Fast Zombie | BasicZombie | Tough Zombie | ConeheadZombie | Notes |
| --- | ---: | ---: | ---: | ---: | --- |
| SunFlower | N/A | N/A | N/A | N/A | Produces sun and does not attack. |
| Peashooter | 6 hits | 7 hits | 10 hits | 14 hits | One pea per firing cycle. |
| WallNut | N/A | N/A | N/A | N/A | Blocks zombies and does not attack. |
| CherryBomb | 1 explosion | 1 explosion | 1 explosion | 1 explosion | Must catch the zombie inside the blast radius. |
| SnowPea | 6 hits | 7 hits | 10 hits | 14 hits | Same damage as Peashooter, but slows the target. |
| Repeater | 6 hits / 3 volleys | 7 hits / 4 volleys | 10 hits / 5 volleys | 14 hits / 7 volleys | Fires two peas per firing cycle. |

### Data Structures Used

From a data structures perspective, this project can be viewed as a grid-based simulation. The lawn is represented by a fixed 2D array, active entities are stored in std::vector containers, and assets are organized using std::map for efficient lookup. The design also leverages inheritance and polymorphism to model different plant and zombie types, while a finite state machine controls the overall game flow and screen transitions.

Step 1: 2D Array / Grid

At the core of the game is a two-dimensional grid representing the lawn.  In `Game.hpp`, the lawn is
stored as a fixed `9 x 5` grid:

```cpp
std::array<std::array<std::unique_ptr<Plant>, rows>, columns>
```

Each grid cell stores either a null pointer or a pointer to a plant object, allowing constant-time access to any board location. This design is similar to many data structure problems involving matrices, grid traversal, board games, and simulation systems, where elements are accessed using row and column indices.

Step 2: Vector / Dynamic Array

The game uses `std::vector` for objects that appear and disappear during
gameplay:

```cpp
std::vector<Projectile> projectiles;
std::vector<SunToken> sunTokens;
std::vector<std::unique_ptr<Zombie>> zombies;
```

This is like an `ArrayList`: zombies spawn, projectiles are fired, and sun
tokens disappear after being collected.

Step 3: Map / Dictionary

The `ResourceManager` uses a map to cache loaded textures:

```cpp
std::map<std::string, sf::Texture>
```

The key is the asset path, and the value is the loaded texture. This prevents
the game from loading the same image many times. It is similar to memoization
or caching.

Step 4: Inheritance / Polymorphism

The game has base classes like `Plant` and `Zombie`, then specific subclasses
such as `SunFlower`, `Peashooter`, `WallNut`, `ZombieBasic`, and
`ZombieConehead`. This is polymorphism: the game can treat each object through
a shared interface while each type keeps its own behavior.

Step 5: Finite State Machine

The game scenes are controlled by an enum:

```cpp
enum class Scene {
    Start,
    Adventure,
    PlantsWon,
    ZombiesWon
};
```

This is a finite state machine. The game behaves differently depending on
whether it is showing the start menu, running the level, showing a win state,
or showing a game-over state.

Step 6: Searching / Traversal

The game’s update loop continuously iterates through plants, zombies, projectiles, 
and sun tokens to keep the game world synchronized. For example, projectiles check 
for zombies in their lane to detect collisions, while zombies continuously check 
for plants blocking their path. These repeated traversal operations form the core 
of the gameplay logic and are one of the first places where algorithmic efficiency 
becomes important.

From a data structures perspective, the project can be viewed as a grid-based simulation. 
A fixed 2D array represents the lawn, std::vector containers manage active game objects, 
std::map stores and reuses game assets, inheritance and polymorphism define entity behavior,
and a finite state machine controls transitions between menus, gameplay, and end-game screens.

### Project Notes

- Existing assets under `assets/` are preserved and reused by the game.
- The C++ code uses SFML for rendering, windowing, and input.
- Progress is saved locally in `save_progress.txt`, which is ignored by Git.
- Background music plays from the `soundtrack/` folder when a supported audio
  file is present.
- Generated folders such as `build/` are not required for upload.

### Next Development Targets

- Add individual sound-effect files for actions such as planting, shooting,
  collecting sun, and explosions.
- Continue tuning level balance from playtesting feedback.
- Add a browser version only when it can preserve the same gameplay, layout,
  and assets as the native SFML game.

## 中文

### 项目简介

这是一个受 Plants vs. Zombies 启发的 C++17/SFML 桌面游戏，也是我的第一个
较完整的 C++ 游戏项目。它包含基于网格的植物放置、种子卡片、自动收集阳光、
子弹战斗、多种僵尸、关卡推进、进度保存、胜负状态以及循环背景音乐。

项目使用 SFML 负责渲染、窗口、输入和音频。玩法结构中也用到了经典计算机科学
概念，例如二维网格、动态数组、素材缓存、继承、多态和有限状态机。

### 素材来源

本项目使用的 UI 文件夹和相关视觉素材来自 GitHub 仓库
[lixing-hust/plants_vs_zoombie](https://github.com/lixing-hust/plants_vs_zoombie)。
这些 UI 素材归功于该仓库。

目前有一部分 UI 图片和界面文字仍然是中文，因为它们来自当前可用的素材包。
游戏源码和准备上传的 C++ 文件会保持英文。

### 环境要求

- CMake 3.16 或更新版本
- 支持 C++17 的编译器
- SFML 3.0 或更新版本

如果你在 macOS 上使用 Homebrew，可以这样安装 SFML：

```bash
brew install sfml
```

### 构建

```bash
cmake -S . -B build
cmake --build build
```

如果你把项目移动到新的文件夹，或者重新 clone 了一份项目，需要在项目根目录
重新运行上面两条命令。单独运行 `cmake --build` 是不完整的，因为 CMake 需要
知道 build 目录名：

```bash
cmake --build build
```

在 Apple Silicon 和 Homebrew 环境下，本项目默认使用 `arm64`，这样可以和
`/opt/homebrew` 里的 SFML 正常链接。

### 运行

```bash
./build/Game
```

也可以一边构建一边启动：

```bash
cmake --build build --target run
```

注意：只运行 `cmake --build build` 只会编译程序，不会自动打开游戏窗口。

### 当前操作方式

- 在开始菜单点击 Adventure 按钮进入草坪场景。
- 点击种子卡片，然后点击草坪格子进行放置。
- 阳光出现后会自动收集。
- 攻击型植物会自动攻击同一行的僵尸。
- 按 `M` 切换音乐状态，按 `N` 切换音效状态。
- 在开始菜单按 `R` 可以重置保存的进度。
- 按 `Q` 或 `Esc` 退出游戏。

### 背景音乐

游戏会自动在下面这些文件夹里寻找背景音乐：

```text
soundtrack/
Soundtrack/
assets/soundtrack/
assets/Soundtrack/
```

程序会播放找到的第一个支持的音频文件，并在音乐开启时循环播放。支持的扩展名
包括 `.ogg`、`.wav`、`.flac`、`.aiff` 和 `.mp3`。游戏中可以按 `M` 暂停或
恢复背景音乐。

### 植物伤害总结

当前快速僵尸的生命值是 `6`，BasicZombie 是 `7`，厚血僵尸是 `10`，
ConeheadZombie 是 `14`。普通豌豆和寒冰豌豆每次命中造成 `1` 点伤害。
Repeater 每轮攻击会发射两颗豌豆，所以总命中次数相同，但需要的攻击轮数更少。
CherryBomb 只要把僵尸覆盖在爆炸范围内，就可以用一次爆炸击败当前所有僵尸。

| 植物 | 快速僵尸 | 普通僵尸 | 厚血僵尸 | 路障僵尸 | 说明 |
| --- | ---: | ---: | ---: | ---: | --- |
| 向日葵 | 不适用 | 不适用 | 不适用 | 不适用 | 生产阳光，不攻击。 |
| 豌豆射手 | 6 次命中 | 7 次命中 | 10 次命中 | 14 次命中 | 每轮攻击发射一颗豌豆。 |
| 坚果墙 | 不适用 | 不适用 | 不适用 | 不适用 | 阻挡僵尸，不攻击。 |
| 樱桃炸弹 | 1 次爆炸 | 1 次爆炸 | 1 次爆炸 | 1 次爆炸 | 僵尸必须在爆炸范围内。 |
| 寒冰射手 | 6 次命中 | 7 次命中 | 10 次命中 | 14 次命中 | 伤害和 Peashooter 相同，但会减速目标。 |
| 双发射手 | 6 次命中 / 3 轮攻击 | 7 次命中 / 4 轮攻击 | 10 次命中 / 5 轮攻击 | 14 次命中 / 7 轮攻击 | 每轮攻击发射两颗豌豆。 |

### 使用到的数据结构

虽然这是一个游戏项目，但底层实现涉及了不少数据结构和面向对象编程的知识。项目中使用了数组、
动态数组、映射表（Map）、继承、多态以及状态机等机制，用来管理植物、僵尸、子弹和不同的游戏状态。

第一步：二维数组 / 网格

最重要的数据结构是二维网格。在 `Game.hpp` 里，草坪被存成固定的 `9 x 5`
网格：

```cpp
std::array<std::array<std::unique_ptr<Plant>, rows>, columns>
```

每一个格子可以是空的，也可以放一个植物。这很像数据结构课程里用矩阵处理
网格遍历、棋盘游戏、寻路或者模拟问题。

第二步：Vector / 动态数组

游戏用 `std::vector` 保存会不断出现和消失的对象：

```cpp
std::vector<Projectile> projectiles;
std::vector<SunToken> sunTokens;
std::vector<std::unique_ptr<Zombie>> zombies;
```

这类似 Java 里的 `ArrayList`。僵尸会生成，子弹会发射，阳光会出现并被自动
收集，所以数量会随着游戏过程变化。

第三步：Map / 字典

`ResourceManager` 用 map 来缓存已经加载过的图片：

```cpp
std::map<std::string, sf::Texture>
```

key 是素材路径，value 是已经加载好的 texture。这样可以避免同一张图片被重复
加载很多次。这和 memoization 或 caching 的想法很像。

第四步：继承 / 多态

游戏里有 `Plant` 和 `Zombie` 这样的父类，然后有 `SunFlower`、`Peashooter`、
`WallNut`、`ZombieBasic`、`ZombieConehead` 这样的具体子类。这就是多态：
游戏可以用统一的接口处理对象，但每种植物或僵尸仍然有自己的行为。

第五步：有限状态机

游戏场景由 enum 控制：

```cpp
enum class Scene {
    Start,
    Adventure,
    PlantsWon,
    ZombiesWon
};
```

这就是有限状态机。游戏会根据当前状态决定要显示开始菜单、运行关卡、显示胜利
状态，还是显示失败状态。

第六步：搜索 / 遍历

在游戏运行过程中，update() 循环会不断遍历植物、僵尸、子弹和阳光等对象。例如，
每颗子弹都需要检查同一行中的僵尸，判断是否发生碰撞；僵尸也需要持续检测前方是
否有植物阻挡。这些操作本质上都属于搜索与遍历，也是游戏开发中开始需要关注时间
复杂度和性能开销的地方。

从数据结构的角度来看，这个项目可以看作一个基于网格（Grid）的实时模拟系统：
使用固定大小的二维数组管理草坪格子，使用 std::vector 存储动态游戏对象，
使用 std::map 缓存和管理游戏资源，通过继承和多态实现不同植物与僵尸的行为差异，
并利用有限状态机（Finite State Machine）控制菜单、游戏进行中以及胜负结算等
不同界面状态。

### 项目说明

- `assets/` 中的素材会被保留，并由游戏继续使用。
- C++ 代码使用 SFML 负责渲染、窗口和输入。
- 进度会保存在本地的 `save_progress.txt`，这个文件不会被 Git 上传。
- 如果 `soundtrack/` 中存在支持的音频文件，游戏会自动播放背景音乐。
- `build/` 这类生成目录不需要上传。

### 后续目标

- 为种植、发射、收集阳光、爆炸等动作加入单独的音效文件。
- 根据试玩反馈继续调整关卡平衡和演示节奏。
- 只有在能保持和原生 SFML 版本相同的玩法、布局和素材时，再加入网页版本。
