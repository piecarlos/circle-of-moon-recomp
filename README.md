<p align="center">
  <img src="assets/banner/circlemoon-banner.png" alt="CircleMoon Android Recomp" width="760">
</p>

<h1 align="center">CircleMoon — Android Recomp</h1>

<p align="center">
  <b>English</b> | <a href="README.pt-BR.md">Português (Brasil)</a>
</p>

<p align="center">
  Native <b>Android (arm64-v8a)</b> and <b>Samsung DeX</b> port of <i>Castlevania: Circle of the Moon</i>, featuring a redesigned <b>Cyan Glass Gothic</b> UI built for controller-first navigation.
</p>

<p align="center">
  <a href="https://github.com/piecarlos/circle-of-moon-recomp/releases/latest"><img alt="Latest release" src="https://img.shields.io/github/v/release/piecarlos/circle-of-moon-recomp?include_prereleases&label=release&color=22d3ee"></a>
  <a href="LICENSE"><img alt="License: MIT" src="https://img.shields.io/badge/license-MIT-0891b2"></a>
  <img alt="Platform" src="https://img.shields.io/badge/platform-Android%20(arm64--v8a)%20%7C%20Samsung%20DeX-0e7490">
</p>

> [!IMPORTANT]
> This repository **does not include any ROMs or APKs**. You must provide your own legally dumped ROM of **Castlevania: Circle of the Moon (USA)** (`AGB-AAME`, revision 0).
> Unofficial fan project, not affiliated with or endorsed by Konami.

## Download

Download the latest **Android APK (`arm64-v8a`, no ROM included)** from the [GitHub Releases page](https://github.com/piecarlos/circle-of-moon-recomp/releases/latest), install it on your device, and select your own `.gba` ROM on first launch.

---

## Key Features (Android)

- **Cyan Glass Gothic UI:** Redesigned home screen with pill-shaped buttons, quick top-right settings icon, and midnight-blue glass menus with cyan highlights.
- **100% Controller Navigation:** Full D-pad and **left analog stick** navigation across the home screen, settings menus, and a simplified controller-first **Save States** list (10 slots with thumbnails, `Load`, `Save`, `Undo`, and `Delete`).
- **Dedicated Menu Shortcut:** Press **L2** at any time in-game to open or close the recomp menu (keeping `Start` and `Select` free for the game).
- **Optimized Audio & Performance:** Native `-O2` optimization, Android-tuned audio buffering (`512` frames + dynamic rate control), and immersive fullscreen / windowed support on **Samsung DeX**.
- **Extras:** Widescreen (16:9 / 16:10), Rewind (`L3`), Fast Forward (`R3`), Quick Save/Load, Cheats, and RetroAchievements.

## Default Gamepad Controls

| Action | Gamepad |
| :--- | :--- |
| **Move** | D-Pad / Left Analog Stick |
| **A / B** | South / East (or North / West) |
| **L / R** | L1 / R1 (and R2 for R) |
| **Start / Select** | Start / Select |
| **Open / Close Recomp Menu** | **L2** (or Android Back button) |
| **Rewind (hold)** | **L3** |
| **Fast Forward (hold)** | **R3** |
| **Quick Save / Quick Load** | Select + R1 / Select + L1 |

## Getting Started

1. Install the APK on your Android device (`arm64-v8a`, Android 8.0+ / API 26+).
2. On first launch, tap **Choose ROM...** and select your valid *Castlevania: Circle of the Moon (USA)* `.gba` file (the app safely copies it to private internal storage).
3. *(Optional)* Side-load via ADB for testing:
   ```powershell
   adb push "Castlevania - Circle of the Moon (USA).gba" /sdcard/Android/data/org.circlemoon.app/files/rom/circlemoon.gba
   ```

## Building from Source (Android)

Requirements: **JDK 17**, **Android SDK 35**, **NDK 28.2.13676358**, and **CMake 3.31.6**.

```powershell
$env:JAVA_HOME = "C:\path\to\jdk-17"
$env:ANDROID_HOME = "C:\path\to\Android\Sdk"
cd android
.\gradlew.bat assembleDebug
```

The APK will be generated at `android/app/build/outputs/apk/debug/app-debug.apk`.
