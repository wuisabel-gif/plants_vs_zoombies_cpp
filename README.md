# Plants vs. Zombies C++ Game

## English

This repository is a C++17/SFML desktop Plants vs. Zombies-style game. The
uploadable project is C++ source plus the required assets; generated build
folders are not needed in the repository.

The current version includes native window creation, SFML rendering, start menu
UI, lawn background, seed bank UI, SunFlower, Peashooter, WallNut, CherryBomb,
SnowPea, and Repeater placement, all six seed packet card costs/cooldowns,
automatically collected sun tokens, pea and snow-pea projectiles, BasicZombie
and ConeheadZombie behavior, collision, health, level progression, wave bursts,
and win/loss states.

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
- Press `Q` or `Esc` to quit.

### Plant Damage Summary

Current zombie health is `7` for BasicZombie and `14` for ConeheadZombie.
Pea and snow-pea projectiles each deal `1` damage. Repeater fires two peas per
attack cycle, so it needs the same total hits but fewer firing cycles.
CherryBomb defeats either zombie type with one explosion if the zombie is
inside its blast radius.

| Plant | BasicZombie | ConeheadZombie | Notes |
| --- | ---: | ---: | --- |
| SunFlower | N/A | N/A | Produces sun and does not attack. |
| Peashooter | 7 hits | 14 hits | One pea per firing cycle. |
| WallNut | N/A | N/A | Blocks zombies and does not attack. |
| CherryBomb | 1 explosion | 1 explosion | Must catch the zombie inside the blast radius. |
| SnowPea | 7 hits | 14 hits | Same damage as Peashooter, but slows the target. |
| Repeater | 7 hits / 4 volleys | 14 hits / 7 volleys | Fires two peas per firing cycle. |

### Project Notes

- Existing assets under `assets/` are preserved and reused by the game.
- The C++ code uses SFML for rendering, windowing, and input.
- Generated folders such as `build/` are not required for upload.

### Next Development Targets

- Add audio and music controls.
- Add more zombie variants beyond BasicZombie and ConeheadZombie.
- Polish level balancing and demo pacing.
- Add a browser version only when it can preserve the same gameplay, layout,
  and assets as the native SFML game.
- Add save/progress behavior if needed.

# 中文

这是一个使用 **C++17** 和 **SFML** 开发的 Plants vs. Zombies 风格桌面游戏项目。

提交作业或上传项目时，只需要包含源码和必要素材即可，像 `build/` 这类编译生成目录不需要上传。

## 当前功能

目前已经实现：

- 原生窗口与 SFML 渲染
- 开始菜单和主界面
- 草坪场景与种子栏 UI
- 六种植物卡片（包含阳光消耗与冷却时间）
- SunFlower
- Peashooter
- WallNut
- CherryBomb
- SnowPea
- Repeater
- 自动生成并收集阳光
- 普通豌豆与寒冰豌豆子弹
- BasicZombie 与 ConeheadZombie
- 碰撞检测与生命值系统
- 僵尸波次生成与关卡推进
- 游戏胜利与失败判定

## 素材来源

项目中的部分 UI 界面和美术资源来自 GitHub 项目：

https://github.com/lixing-hust/plants_vs_zoombie

感谢原作者提供的素材资源。

由于目前使用的素材包本身包含中文资源，因此游戏中仍然可以看到部分中文按钮、图片和界面文字。后续如果有合适的英文素材，会逐步替换。

项目源码全部采用英文命名，提交和上传的 C++ 文件也保持英文风格。

## 环境要求

- CMake 3.16 或更高版本
- 支持 C++17 的编译器
- SFML 3.0 或更高版本

如果你使用 macOS 和 Homebrew，可以通过以下命令安装 SFML：

```bash
brew install sfml
```

## 构建项目

```bash
cmake -S . -B build
cmake --build build
```

## 项目结构

```text
.
├── assets/
├── include/
├── src/
├── CMakeLists.txt
└── README.md
```

## 项目状态

当前项目仍在持续开发中，后续计划完善更多植物、僵尸、关卡机制以及 UI 细节，并逐步替换临时素材资源。