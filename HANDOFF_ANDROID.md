# HANDOFF_ANDROID: Port CircleMoon para Android (APK Nativo)

Documento técnico de handoff atualizado com o estado atual do port Android / Samsung DeX de *Castlevania: Circle of the Moon*.

---

## 1. Repositório e Ambiente (Windows)

- **Diretório ativo do projeto:** `C:\workspace\recompgba\cicle of moon recomp`
- **Repositório GitHub:** `https://github.com/piecarlos/circle-of-moon-recomp` (autor único: `piecarlos <gamesavemanager@proton.me>`)
- **Android SDK:** `C:\Users\Jeronimo\AppData\Local\Android\Sdk`
- **Android NDK:** `28.2.13676358` (r28c)
- **CMake:** `3.31.6` (SDK) | **Ninja:** `1.13.2`
- **JDK obrigatório (Gradle 8.13 / AGP 8.7.3):** `C:\Users\Jeronimo\AppData\Local\Programs\jdk-17\jdk-17.0.21.1+2`
- **Variáveis de build:**
  ```powershell
  $env:JAVA_HOME = "C:\Users\Jeronimo\AppData\Local\Programs\jdk-17\jdk-17.0.21.1+2"
  $env:ANDROID_HOME = "C:\Users\Jeronimo\AppData\Local\Android\Sdk"
  ```

---

## 2. Estado Atual das Funcionalidades

### Build & Plataformas
- **Alvo:** Android 8.0+ (API 26+), `arm64-v8a`, `applicationId = "org.circlemoon.app"`, `-std=c++20 -O2` / `-O2`.
- **Samsung DeX e Tela:** `resizeableActivity="true"`, `sensorLandscape`, fullscreen imersivo + `FLAG_KEEP_SCREEN_ON` (`SDL_DisableScreenSaver()`) em `CircleMoonActivity.java`/`app.cpp` para impedir que a tela apague durante o uso com controle físico, e `SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS = "1"` em `src/main.cpp`.
- **ROM obrigatória:** Apenas **USA (`AGB-AAME`) rev 0** (SHA-1 `d661b01eb94186435723ac03344792c11c20c522`). Outras regiões (`AAMP`, `AAMJ`) ou dumps inválidos são recusados em `verify_rom()` e removidos caso tenham sido copiados.
- **Carregamento no Android (`App::load_configured_rom` / `accept_rom`):** cópia via SAF para `<pref_dir>/rom/circlemoon.gba`, fallback em `<pref_dir>/rom/*.gba` e suporte opcional a asset `rom/circlemoon.gba` (apenas em builds pessoais locais).

### Controles e Entrada
- **Menu do recomp:** **L2** exclusivo (`Hotkey::Menu = axis:lefttrigger+`, removido de `Button::L`), com debounce de 500 ms em `src/app/app.cpp`.
- **Atalhos em jogo:** **L3** = Rewind, **R3** = Fast Forward (`Start`/`Select` livres para o jogo).
- **Analógico esquerdo nos menus:** `src/options/menu_pad.cpp` converte `LEFTX`/`LEFTY` em navegação D-Pad do ImGui.
- **Touch Overlay (`src/options/touch_overlay.h/.cpp`):** controles virtuais multi-touch estilo Cyan Glass (D-Pad com diagonais, `A`, `B`, `L`, `R`, `SELECT`, `START` e `MENU`). **Desativado por padrão** (`input.touch_overlay = false`), configurável em **Settings → Controls → Touchscreen** (já acessível na tela inicial) com slider de opacidade (`20%`–`100%`).

### Interface, Áudio e Ciclo de Vida
- **Tema Cyan Glass Gothic (`src/app/theme_colors.cpp`, `theme.cpp`):** superfícies azul-noite translúcidas, borda ciano (`#22D3EE`), botões em pílula e engrenagem rápida no launcher.
- **Save States (`draw_states_tab`):** lista vertical navegável por controle (10 slots com miniatura 84×56, barra superior `Load` / `Save` / `Undo` / `Delete`).
- **Áudio e Background:** `SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES = "512"`, controle de taxa ajustado para Android, pausa/flush de áudio e auto-save em `SDL_EVENT_WILL_ENTER_BACKGROUND` / `DID_ENTER_FOREGROUND`.

---

## 3. Artefatos, Versionamento e Build de Release

- **Regra de versionamento:**
  - Novos projetos sempre iniciam em **`v0.1`**.
  - No CircleMoon, a versão atual é **`v1.2.4`**; a próxima recompilação será **`v1.2.5`**, incrementando a cada nova build.
  - **Sempre manter os APKs separados por versão** (nunca sobrescrever versões anteriores) e separar o APK público (sem ROM) do APK pessoal (com ROM).
- **Comando de build:**
  ```powershell
  Set-Location "C:\workspace\recompgba\cicle of moon recomp\android"
  .\gradlew.bat assembleRelease --console=plain
  ```
- **APKs atuais (em `C:\workspace\recompgba\`):**
  - Público (sem ROM, para GitHub Releases): `CircleMoon-v1.2.4-Android-arm64.apk` (mantendo `v1.2.0`, `v1.2.1`, `v1.2.2` e `v1.2.3` separados)
  - Pessoal (com ROM embutida, nunca distribuir/commitar): `CircleMoon-personal-with-rom.apk`
- **Documentação:** `README.md` (EN) e `README.pt-BR.md` (PT-BR) com banner `assets/banner/circlemoon-banner.png`, logo `assets/logo/circlemoon-logo.png`, print `assets/screenshots/home-screen.png` e badge `release-v1.2.4`.
- **Push Git:** `push-github.bat` (chama `push-github.ps1`, ambos no `.gitignore`).

---

## 4. Instalação via ADB (Galaxy S22+ / `SM-S906E`)

```powershell
# Verificar dispositivo conectado
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" devices

# Instalar / atualizar APK mantendo dados e ROM interna
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" install -r "C:\workspace\recompgba\CircleMoon-v1.2.4-Android-arm64.apk"

# Iniciar app e ver logs
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" shell am start -n org.circlemoon.app/.CircleMoonActivity
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" logcat -s SDL:V circlemoon:V
```

---

## 5. Pendências Conhecidas

- Validar em jogo no S22+ o Touch Overlay e a estabilidade final do L2.
- Limpar campos privados não utilizados `rename_slot_` / `rename_buf_` em `src/app/app.cpp` (geram warning de compilação).
- Publicar a Release `v1.2.4` no GitHub com `CircleMoon-v1.2.4-Android-arm64.apk` e fazer push das alterações via `push-github.bat`.
