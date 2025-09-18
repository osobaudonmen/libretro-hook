#include "core_loader.h"

#ifdef __unix__
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#endif

#ifdef __ANDROID__
#include <stdio.h>
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
