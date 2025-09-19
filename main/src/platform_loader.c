#include "platform_loader.h"
#include "hook_constants.h"
#include "hook_globals.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __unix__
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __ANDROID__
#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>
#endif

static void cleanup_and_exit(void) {
    if (frame_buf) {
        free(frame_buf);
        frame_buf = NULL;
    }
    exit(0);
}

#ifdef __unix__
#ifndef __ANDROID__
void platform_load_core(const char *core_path, const char *rom_path) {
    pid_t pid = fork();
    if (pid == 0) {
        // まず標準的なインストール場所を試す
        execl("/usr/bin/retroarch", "retroarch", "-L", core_path, rom_path, (char *)NULL);

        // 見つからない場合はPATHから検索
        execlp("retroarch", "retroarch", "-L", core_path, rom_path, (char *)NULL);

        // それでも見つからない場合
        _exit(127);
    } else if (pid > 0) {
        cleanup_and_exit();
    }
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    if (access(script_path, X_OK) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            execl(script_path, script_path, core_path, rom_path, (char *)NULL);
            _exit(127);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
#endif
#endif

#ifdef _WIN32
void platform_load_core(const char *core_path, const char *rom_path) {
    char command[MAX_COMMAND_SIZE];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    int ret = snprintf(command, sizeof(command), "retroarch.exe -L \"%s\" \"%s\"", core_path, rom_path);
    if (ret >= sizeof(command)) {
        MessageBoxA(NULL, "Command line too long", "Core Loader Error", MB_OK | MB_ICONERROR);
        return;
    }

    // PATHからretroarch.exeを検索して実行
    if (CreateProcessA(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        cleanup_and_exit();
    } else {
        DWORD error = GetLastError();
        char error_msg[MAX_ERROR_MSG_SIZE];
        snprintf(error_msg, sizeof(error_msg), "Failed to start retroarch.exe: Error code %lu\nCommand: %s", error, command);
        MessageBoxA(NULL, error_msg, "Core Loader Error", MB_OK | MB_ICONERROR);
    }
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    DWORD attrib = GetFileAttributesA(script_path);
    if (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\"", script_path, core_path, rom_path);
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if (CreateProcessA(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}
#endif

#ifdef __ANDROID__
void platform_load_core(const char *core_path, const char *rom_path) {
    char command[MAX_COMMAND_SIZE];

    log_cb(RETRO_LOG_INFO, "CoreLoader: Loading core: %s with ROM: %s\n", core_path, rom_path);

    // Try different RetroArch package names (newer versions use architecture-specific names)
    const char *package_names[] = {
        "com.retroarch.aarch64",  // 64-bit
        "com.retroarch.arm",      // 32-bit
        "com.retroarch"           // legacy
    };
    const char *activity_name = ".MainActivity";

    int result = -1;
    for (size_t i = 0; i < sizeof(package_names) / sizeof(package_names[0]); i++) {
        int ret = snprintf(command, sizeof(command),
            "am start -n %s/%s -e LIBRETRO \"%s\" -e ROM \"%s\"",
            package_names[i], activity_name, core_path, rom_path);
        if (ret >= sizeof(command)) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Command line too long\n");
            return;
        }

        log_cb(RETRO_LOG_INFO, "CoreLoader: Trying command: %s\n", command);
        result = system(command);
        if (result == 0) {
            log_cb(RETRO_LOG_INFO, "CoreLoader: Successfully launched RetroArch with package: %s\n", package_names[i]);
            cleanup_and_exit();
            return;
        } else {
            log_cb(RETRO_LOG_WARN, "CoreLoader: Failed with package %s, return code: %d\n", package_names[i], result);
        }
    }

    log_cb(RETRO_LOG_ERROR, "CoreLoader: All RetroArch launch attempts failed, last command: %s\n", command);
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    if (access(script_path, X_OK) == 0) {
        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "%s '%s' '%s'", script_path, core_path, rom_path);
        int result = system(command);
        if (result != 0) {
            log_cb(RETRO_LOG_INFO, "CoreLoader: Script execution failed: %d, command: %s\n", result, command);
        }
    }
}
#endif

#ifndef __unix__
#ifndef _WIN32
#ifndef __ANDROID__
void platform_load_core(const char *core_path, const char *rom_path) {
    (void)core_path;
    (void)rom_path;
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    (void)script_path;
    (void)core_path;
    (void)rom_path;
}
#endif
#endif
#endif
