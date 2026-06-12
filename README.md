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

## 中文

这个项目是一个基于 C++17 和 SFML 的 Plants vs. Zombies 风格桌面游戏。
当前可上传版本只需要 C++ 源码和必要素材；`build/` 这类生成目录不需要上传。

当前版本已经实现了原生窗口、SFML 渲染、开始菜单、草坪背景、种子栏界面、
六种种子卡片的价格和冷却、SunFlower、Peashooter、WallNut、CherryBomb、
SnowPea、Repeater 的放置、自动收集的阳光、豌豆和寒冰豌豆子弹、
BasicZombie 和 ConeheadZombie、碰撞、生命值、关卡推进、波次爆发以及胜负
状态。

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
