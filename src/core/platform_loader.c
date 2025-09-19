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

    __android_log_print(ANDROID_LOG_INFO, "CoreLoader", "Loading core: %s with ROM: %s", core_path, rom_path);

    int ret = snprintf(command, sizeof(command),
        "am start -n com.retroarch/.browser.retroarch.RetroArchActivity -e ROM \"%s\" -e LIBRETRO \"%s\"",
        rom_path, core_path);
    if (ret >= sizeof(command)) {
        __android_log_print(ANDROID_LOG_ERROR, "CoreLoader", "Command line too long");
        return;
    }

    int result = system(command);
    if (result != 0) {
        __android_log_print(ANDROID_LOG_ERROR, "CoreLoader",
            "Failed to launch RetroArch: return code %d, command: %s", result, command);
    } else {
        cleanup_and_exit();
    }
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    if (access(script_path, X_OK) == 0) {
        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "%s '%s' '%s'", script_path, core_path, rom_path);
        int result = system(command);
        if (result != 0) {
            __android_log_print(ANDROID_LOG_ERROR, "CoreLoader", "Script execution failed: %d", result);
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
