#include "platform_loader.h"
#include "platform_utils.h"
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
        const char *system_dir = get_system_directory();
        if (!system_dir) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
            return;
        }

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to create pipe\n");
            return;
        }
        pid_t pid = fork();
        if (pid == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            execl(script_path, script_path, system_dir, core_path, rom_path, (char *)NULL);
            _exit(127);
        } else if (pid > 0) {
            close(pipefd[1]);
            char buffer[1024];
            ssize_t bytes_read;
            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytes_read] = '\0';
                log_cb(RETRO_LOG_INFO, "CoreLoader: Script output: %s", buffer);
            }
            close(pipefd[0]);
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
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Error: %s\n", error_msg);
    }
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    DWORD attrib = GetFileAttributesA(script_path);
    if (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
        const char *system_dir = get_system_directory();
        if (!system_dir) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
            return;
        }

        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\" \"%s\"", script_path, system_dir, core_path, rom_path);
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        HANDLE hRead, hWrite;
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to create pipe\n");
            return;
        }
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        si.dwFlags |= STARTF_USESTDHANDLES;

        log_cb(RETRO_LOG_INFO, "CoreLoader: Command: %s\n", command);
        if (CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(hWrite);
            char buffer[1024];
            DWORD bytes_read;
            while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
                buffer[bytes_read] = '\0';
                log_cb(RETRO_LOG_INFO, "CoreLoader: Script output: %s", buffer);
            }
            CloseHandle(hRead);
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            log_cb(RETRO_LOG_INFO, "CoreLoader: Finished to run script\n");
        } else {
            DWORD error = GetLastError();
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to run script: Error code %lu\n", error);
        }
    }
}
#endif

#ifdef __ANDROID__
void platform_load_core(const char *core_path, const char *rom_path) {
    log_cb(RETRO_LOG_INFO, "CoreLoader: Loading core: %s with ROM: %s\n", core_path, rom_path);

    // On Android, we need to return control to RetroArch frontend
    // The frontend should handle loading the new core and ROM
    if (environ_cb) {
        // Shutdown the current core to return control to frontend
        if (!environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL)) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to shutdown current core\n");
            return;
        }

        log_cb(RETRO_LOG_INFO, "CoreLoader: Returned control to RetroArch frontend for core: %s, ROM: %s\n", core_path, rom_path);
        log_cb(RETRO_LOG_INFO, "CoreLoader: Please manually load the core and ROM in RetroArch\n");
        cleanup_and_exit();
    } else {
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Environment callback not available\n");
    }
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    if (access(script_path, X_OK) == 0) {
        const char *system_dir = get_system_directory();
        if (!system_dir) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
            return;
        }

        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "%s '%s' '%s' '%s'", script_path, system_dir, core_path, rom_path);
        FILE *fp = popen(command, "r");
        if (fp) {
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), fp)) {
                log_cb(RETRO_LOG_INFO, "CoreLoader: Script output: %s", buffer);
            }
            int result = pclose(fp);
            if (result != 0) {
                log_cb(RETRO_LOG_ERROR, "CoreLoader: Script execution failed: %d, command: %s\n", result, command);
            }
        } else {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to run script with popen\n");
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
