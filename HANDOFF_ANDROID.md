# HANDOFF_ANDROID: Port CircleMoon para Android (APK Nativo)

Documento técnico de handoff para continuidade imediata do projeto sem perda de contexto.

---

## 1. Ambiente Configurado (Windows)

- **Android SDK:** `C:\Users\Jeronimo\AppData\Local\Android\Sdk`
  - Platforms: `android-35`, `android-36`
  - Build-tools: `36.0.0`, `35.0.0`, `34.0.0`
- **Android NDK:** `28.2.13676358` (r28c) em `C:\Users\Jeronimo\AppData\Local\Android\Sdk\ndk\28.2.13676358`
  - Toolchain CMake: `C:\Users\Jeronimo\AppData\Local\Android\Sdk\ndk\28.2.13676358\build\cmake\android.toolchain.cmake`
  - `llvm-readelf` / `nm`: `C:\Users\Jeronimo\AppData\Local\Android\Sdk\ndk\28.2.13676358\toolchains\llvm\prebuilt\windows-x86_64\bin\llvm-readelf.exe`
- **CMake:** `3.31.6` em `C:\Users\Jeronimo\AppData\Local\Android\Sdk\cmake\3.31.6\bin\cmake.exe` (sistema possui `4.4.4` em `C:\Program Files\CMake\bin\cmake.exe`)
- **Ninja:** `1.13.2` em `C:\Users\Jeronimo\AppData\Local\Microsoft\WinGet\Links\ninja.exe`
- **JDK (obrigatório para o Gradle/AGP deste projeto):** `C:\Users\Jeronimo\AppData\Local\Programs\jdk-17\jdk-17.0.21.1+2` (JDK 17).
  *Atenção:* O Windows possui Zulu 11 e JDK 25 registrados; o AGP 8.7.3 rejeita JDK 11 e o Gradle 8.13 rejeita JDK 25. O build deve sempre rodar com `JAVA_HOME` apontando para o JDK 17 acima.
- **Variáveis de ambiente requeridas para o build:**
  ```powershell
  $env:JAVA_HOME = "C:\Users\Jeronimo\AppData\Local\Programs\jdk-17\jdk-17.0.21.1+2"
  $env:ANDROID_HOME = "C:\Users\Jeronimo\AppData\Local\Android\Sdk"
  ```

---

## 2. Alterações Feitas

### Arquivos modificados no repositório:
- `CMakeLists.txt`: Quando `ANDROID` está ativo, compila `circlemoon` como biblioteca `SHARED` (`libmain.so`), guarda o binário host `circlemoon_icon` com `if(NOT CMAKE_CROSSCOMPILING)` usando fallback para ícone embutido, e faz link com `android`, `log`, `GLESv2`, `GLESv3`, `vulkan`.
- `src/app/app.cpp`: No Android, copia atômica da ROM via SAF para `<pref_dir>/rom/circlemoon.gba` para reter acesso após fechar o app; fallback de leitura automática em `<pref_dir>/rom/*.gba` para testes via `adb push`; e inicialização com `ImGuiConfigFlags_NavEnableGamepad`.
- `src/app/http.cpp`: Adicionado `#elif defined(__ANDROID__)` retornando `backend_ready() == false` (evita falha de `dlopen("libcurl.so")`).
- `src/app/platform.cpp`: Adicionados stubs `#ifdef __ANDROID__` para funções de atalho de desktop (retornando não suportado de forma limpa).
- `src/update/http.cpp`: Adicionados stubs `#ifdef __ANDROID__` em `http_download` e `http_get`.
- `src/update/updater.cpp`: Adicionados stubs `#ifdef __ANDROID__` no updater do jogo (evita crash ao tentar auto-atualizar binário no mobile).

### Arquivos criados (`CircleMoon/android/`):
- `android/build.gradle` & `android/settings.gradle` & `android/gradle.properties`: Configuração raiz do Gradle (AGP 8.7.3).
- `android/gradlew` & `android/gradlew.bat` & `android/gradle/wrapper/*`: Gradle Wrapper 8.13.
- `android/app/build.gradle`: Módulo Android (`compileSdk 35`, `minSdk 26`, `targetSdk 35`, NDK `28.2.13676358`, CMake `3.31.6`, abiFilters `arm64-v8a`), hook que copia os fontes Java da SDL3 baixados via FetchContent para o sourceSet de compilação.
- `android/app/src/main/AndroidManifest.xml`: Configurações de permissões (`VIBRATE`, `INTERNET`), `glEsVersion="0x00020000"`, `resizeableActivity="true"`, `sensorLandscape`, e `CircleMoonActivity`.
- `android/app/src/main/java/org/circlemoon/app/CircleMoonActivity.java`: Atividade herdando de `org.libsdl.app.SDLActivity`, configurando fullscreen imersivo para Samsung DeX e registrando `getLibraries() = {"main"}`.
- `android/app/src/main/res/`: Strings e ícone de launcher adaptados.

> **CONFIRMAÇÃO EXPLÍCITA:** O núcleo de emulação/hardware GBA em `src/gba/` (`apu.cpp`, `bios.cpp`, `bus.cpp`, `cpu.cpp`, `ppu.cpp`, `state.cpp`, `system.cpp`, `wide_level.cpp`, `gba.h`) permaneceu **100% INTACTO**, sem nenhuma alteração.

---

## 3. Status do Build

- **Status:** `BUILD SUCCESSFUL` (passou no `assembleDebug`).
- **APK gerado:**
  - Caminho: `c:\Recomp Library\circlemoon\CircleMoon\android\app\build\outputs\apk\debug\app-debug.apk`
  - Tamanho: `8.674.178 bytes` (~8,27 MB)
- **Validação do símbolo `SDL_main` na `libmain.so` (arm64-v8a):**
  - Binário dentro do APK: `lib/arm64-v8a/libmain.so` (7,55 MB stripped)
  - Binário não-stripped: `android/app/build/intermediates/cxx/Debug/5s3s5j1s/obj/arm64-v8a/libmain.so`
  - Símbolo verificado via `llvm-readelf -s`:
    `5192: 0000000000261840 13396 FUNC GLOBAL DEFAULT 14 SDL_main`
    (Exportado como função global visível para a `SDLActivity` da SDL3 carregar).

---

## 4. Status dos 4 Requisitos Android

- **[OK] Cópia da ROM escolhida via SAF para `SDL_GetPrefPath()` + fallback em `SDL_GetPrefPath()/rom/`:**
  Implementado em `src/app/app.cpp` (`App::accept_rom` e `App::load_configured_rom`). Salva em `rom/circlemoon.gba` interno e faz busca por qualquer `.gba` válido na pasta `rom/`.
- **[OK] `ImGuiConfigFlags_NavEnableGamepad` ativo para navegar no launcher 100% com controle físico:**
  Implementado em `src/app/app.cpp:262` (`io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad`). O ImGui e o `menu_pad` recebem eventos de controle via SDL3.
- **[OK] `AndroidManifest.xml` configurado com `resizeableActivity="true"`, `sensorLandscape` e modo fullscreen imersivo (Samsung DeX):**
  Implementado em `AndroidManifest.xml` (na tag `<application>` e `<activity>`) e em `CircleMoonActivity.java` usando `WindowInsetsController` (API 30+) e `SYSTEM_UI_FLAG_IMMERSIVE_STICKY` para ocultar barras e suportar redimensionamento limpo no Samsung DeX.
- **[OK] `libmain.so` (arm64-v8a) compilando e exportando `SDL_main` para a `SDLActivity` da SDL3:**
  Compilação nativa C++ concluída com NDK r28c e validada com `llvm-readelf`.

---

## 5. Próximo Passo Imediato

Instalar o APK no dispositivo/emulador Android conectado via USB/Wi-Fi e testar com uma ROM válida de *Castlevania: Circle of the Moon (USA)*:

```powershell
# 1. Verificar aparelho conectado via adb
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" devices

# 2. Instalar o APK gerado
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" install -r "c:\Recomp Library\circlemoon\CircleMoon\android\app\build\outputs\apk\debug\app-debug.apk"

# 3. (Opcional) Enviar a ROM diretamente para a pasta interna do app para teste sem file picker:
# O pref path no Android geralmente é: /sdcard/Android/data/org.circlemoon.app/files/rom/
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" shell mkdir -p /sdcard/Android/data/org.circlemoon.app/files/rom/
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" push "<caminho_da_rom>\Circle of the Moon.gba" /sdcard/Android/data/org.circlemoon.app/files/rom/circlemoon.gba

# 4. Iniciar o app e monitorar os logs da SDL3 / ImGui
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" shell am start -n org.circlemoon.app/.CircleMoonActivity
& "C:\Users\Jeronimo\AppData\Local\Android\Sdk\platform-tools\adb.exe" logcat -s SDL:V circlemoon:V
```
