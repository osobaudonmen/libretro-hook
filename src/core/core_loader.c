#include "core_loader.h"
#include <stdio.h>

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

void load_core(const char *core_path, const char *rom_path)
{
#ifdef __unix__
    execl(
        "/usr/bin/retroarch", "retroarch",
        "-L", core_path,
        rom_path,
        (char *)NULL
    );
#elif defined(_WIN32)
    char command[2048];  // Increased buffer size for safety
    STARTUPINFOA si;     // Use ANSI version explicitly
    PROCESS_INFORMATION pi;

    // Initialize structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Build command line with better path handling
    int ret = snprintf(command, sizeof(command),
        "retroarch.exe -L \"%s\" \"%s\"",
        core_path, rom_path);

    // Check for buffer overflow
    if (ret >= sizeof(command)) {
        MessageBoxA(NULL, "Command line too long", "Core Loader Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Create process
    if (CreateProcessA(NULL,     // No module name (use command line)
        command,                 // Command line
        NULL,                    // Process handle not inheritable
        NULL,                    // Thread handle not inheritable
        FALSE,                   // Set handle inheritance to FALSE
        0,                       // No creation flags
        NULL,                    // Use parent's environment block
        NULL,                    // Use parent's starting directory
        &si,                     // Pointer to STARTUPINFO structure
        &pi)                     // Pointer to PROCESS_INFORMATION structure
    ) {
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        // Handle error
        DWORD error = GetLastError();
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg),
            "Failed to start retroarch.exe: Error code %lu\nCommand: %s", error, command);
        MessageBoxA(NULL, error_msg, "Core Loader Error", MB_OK | MB_ICONERROR);
    }
#elif defined(__ANDROID__)
    // Android implementation using am (Activity Manager)
    char command[2048];  // Increased buffer size

    // Log the attempt
    __android_log_print(ANDROID_LOG_INFO, "CoreLoader",
        "Loading core: %s with ROM: %s", core_path, rom_path);

    // Build intent command to launch RetroArch with proper escaping
    int ret = snprintf(command, sizeof(command),
        "am start -n com.retroarch/.browser.retroarch.RetroArchActivity "
        "-e ROM \"%s\" -e LIBRETRO \"%s\"",
        rom_path, core_path);

    // Check for buffer overflow
    if (ret >= sizeof(command)) {
        __android_log_print(ANDROID_LOG_ERROR, "CoreLoader",
            "Command line too long");
        return;
    }

    // Execute the command
    int result = system(command);
    if (result != 0) {
        __android_log_print(ANDROID_LOG_ERROR, "CoreLoader",
            "Failed to launch RetroArch: return code %d, command: %s", result, command);
    }
#else
    // Unsupported platform
    #warning "load_core not implemented for this platform"
    (void)core_path;
    (void)rom_path;
#endif
}

void call_script_before_load(const char *core_file, const char *core_path, const char *rom_path)
{
    const char *system_dir = get_system_directory();
    if (!system_dir) {
        // システムディレクトリが取得できない場合は何もしない
        return;
    }

    #ifdef __unix__
        // UNIX/Linux: <system>/hook/before_load.sh
        char script_path[512];
        snprintf(script_path, sizeof(script_path), "%s/hook/before_load.sh", system_dir);
        if (access(script_path, X_OK) == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                execl(script_path, script_path, core_file, core_path, rom_path, (char *)NULL);
                _exit(127); // exec failed
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    #elif defined(_WIN32)
        // Windows: <system>\hook\before_load.bat
        char script_path[512];
        snprintf(script_path, sizeof(script_path), "%s\\hook\\before_load.bat", system_dir);
        DWORD attrib = GetFileAttributesA(script_path);
        if (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            char command[2048];
            snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\" \"%s\"", script_path, core_file, core_path, rom_path);
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
        // Android: <system>/hook/before_load.sh
        char script_path[512];
        snprintf(script_path, sizeof(script_path), "%s/hook/before_load.sh", system_dir);
        if (access(script_path, X_OK) == 0) {
            char command[2048];
            snprintf(command, sizeof(command), "%s '%s' '%s' '%s'", script_path, core_file, core_path, rom_path);
            int result = system(command);
            if (result != 0) {
                __android_log_print(ANDROID_LOG_ERROR, "CoreLoader", "before_load.sh failed: %d", result);
            }
        }
    #else
        // Unsupported platform
        (void)core_file;
        (void)core_path;
        (void)rom_path;
    #endif
}
