#include "platform_loader.h"
#include "hook_constants.h"
#include <stdio.h>
#include <stdint.h>

extern uint8_t *frame_buf;

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

void platform_load_core(const char *core_path, const char *rom_path)
{
#ifdef __unix__
    pid_t pid = fork();
    if (pid == 0) {
        /* Child process: launch RetroArch */
        execl(
            "/usr/bin/retroarch", "retroarch",
            "-L", core_path,
            rom_path,
            (char *)NULL
        );
        /* If execl fails */
        _exit(127);
    } else if (pid > 0) {
        /* Parent process: perform cleanup and exit */
        if (frame_buf) {
            free(frame_buf);
            frame_buf = NULL;
        }
        exit(0);
    } else {
        /* Fork failed */
        /* Handle error if needed */
    }
#elif defined(_WIN32)
    char command[2048];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    /* Initialize structures */
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    /* Build command line with better path handling */
    int ret = snprintf(command, sizeof(command),
        "retroarch.exe -L \"%s\" \"%s\"",
        core_path, rom_path);

    /* Check for buffer overflow */
    if (ret >= sizeof(command)) {
        MessageBoxA(NULL, "Command line too long", "Core Loader Error", MB_OK | MB_ICONERROR);
        return;
    }

    /* Create process */
    if (CreateProcessA(NULL,     /* No module name (use command line) */
        command,                 /* Command line */
        NULL,                    /* Process handle not inheritable */
        NULL,                    /* Thread handle not inheritable */
        FALSE,                   /* Set handle inheritance to FALSE */
        0,                       /* No creation flags */
        NULL,                    /* Use parent's environment block */
        NULL,                    /* Use parent's starting directory */
        &si,                     /* Pointer to STARTUPINFO structure */
        &pi)                     /* Pointer to PROCESS_INFORMATION structure */
    ) {
        /* Close process and thread handles */
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        /* Perform cleanup before exit */
        if (frame_buf) {
            free(frame_buf);
            frame_buf = NULL;
        }
        /* Exit this core after launching RetroArch */
        exit(0);
    } else {
        /* Handle error */
        DWORD error = GetLastError();
        char error_msg[MAX_ERROR_MSG_SIZE];
        snprintf(error_msg, sizeof(error_msg),
            "Failed to start retroarch.exe: Error code %lu\nCommand: %s", error, command);
        MessageBoxA(NULL, error_msg, "Core Loader Error", MB_OK | MB_ICONERROR);
    }
#elif defined(__ANDROID__)
    /* Android implementation using am (Activity Manager) */
    char command[MAX_COMMAND_SIZE];

    /* Log the attempt */
    __android_log_print(ANDROID_LOG_INFO, "CoreLoader",
        "Loading core: %s with ROM: %s", core_path, rom_path);

    /* Build intent command to launch RetroArch with proper escaping */
    int ret = snprintf(command, sizeof(command),
        "am start -n com.retroarch/.browser.retroarch.RetroArchActivity "
        "-e ROM \"%s\" -e LIBRETRO \"%s\"",
        rom_path, core_path);

    /* Check for buffer overflow */
    if (ret >= sizeof(command)) {
        __android_log_print(ANDROID_LOG_ERROR, "CoreLoader",
            "Command line too long");
        return;
    }

    /* Execute the command */
    int result = system(command);
    if (result != 0) {
        __android_log_print(ANDROID_LOG_ERROR, "CoreLoader",
            "Failed to launch RetroArch: return code %d, command: %s", result, command);
    } else {
        /* Perform cleanup before exit */
        if (frame_buf) {
            free(frame_buf);
            frame_buf = NULL;
        }
        /* Exit this core after launching RetroArch */
        exit(0);
    }
#else
    /* Unsupported platform */
    #warning "platform_load_core not implemented for this platform"
    (void)core_path;
    (void)rom_path;
#endif
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path)
{
    #ifdef __unix__
        /* UNIX/Linux: execute shell script */
        if (access(script_path, X_OK) == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                execl(script_path, script_path, core_path, rom_path, (char *)NULL);
                _exit(127); /* exec failed */
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    #elif defined(_WIN32)
        /* Windows: execute batch file */
        DWORD attrib = GetFileAttributesA(script_path);
        if (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            char command[MAX_COMMAND_SIZE];
            snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\"",
                    script_path, core_path, rom_path);
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
    #elif defined(__ANDROID__)
        /* Android: execute shell script */
        if (access(script_path, X_OK) == 0) {
            char command[MAX_COMMAND_SIZE];
            snprintf(command, sizeof(command), "%s '%s' '%s'",
                    script_path, core_path, rom_path);
            int result = system(command);
            if (result != 0) {
                __android_log_print(ANDROID_LOG_ERROR, "CoreLoader",
                                   "Script execution failed: %d", result);
            }
        }
    #else
        /* Unsupported platform */
        (void)script_path;
        (void)core_path;
        (void)rom_path;
    #endif
}
