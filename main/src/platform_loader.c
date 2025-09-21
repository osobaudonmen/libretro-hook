#include "platform_loader.h"
#include "platform_utils.h"
#include "hook_constants.h"
#include "hook_globals.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

#if defined(__unix__) && !defined(__ANDROID__)
void platform_load_core(const char *core_path, const char *rom_path) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/retroarch", "retroarch", "-L", core_path, rom_path, (char *)NULL);
        execlp("retroarch", "retroarch", "-L", core_path, rom_path, (char *)NULL);
        _exit(127);
    } else if (pid > 0) {
        cleanup_and_exit();
    }
}

void platform_run_script(const char *script_path, const char *core_path, const char *rom_path) {
    if (access(script_path, X_OK) == 0) {
        const char *system_dir = hook_get_system_directory();
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
            execl(script_path, script_path, system_dir, rom_path, (char *)NULL);
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

#elif defined(_WIN32)
void platform_load_core(const char *core_path, const char *rom_path) {
    char command[MAX_COMMAND_SIZE];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    int ret = snprintf(command, sizeof(command), "retroarch.exe -L \"%s\" \"%s\"", core_path, rom_path);
    if (ret >= sizeof(command)) {
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Command line too long\n");
        return;
    }

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
        const char *system_dir = hook_get_system_directory();
        if (!system_dir) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
            return;
        }

        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\"", script_path, system_dir, rom_path);
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

#elif defined(__ANDROID__)
void platform_load_core(const char *core_path, const char *rom_path) {
    log_cb(RETRO_LOG_INFO, "CoreLoader: Loading core: %s with ROM: %s\n", core_path, rom_path);

    if (environ_cb) {
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
        const char *system_dir = hook_get_system_directory();
        if (!system_dir) {
            log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
            return;
        }

        char command[MAX_COMMAND_SIZE];
        snprintf(command, sizeof(command), "%s '%s' '%s'", script_path, system_dir, rom_path);
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

#else
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

/* Common functions for enhanced script execution */
char *platform_parse_core_from_output(const char *output) {
    if (!output) return NULL;

    const char *start = strstr(output, "<core:");
    if (!start) return NULL;

    start += 6; /* Skip "<core:" */
    const char *end = strchr(start, '>');
    if (!end) return NULL;

    size_t len = end - start;
    if (len == 0) return NULL;

    char *result = malloc(len + 1);
    if (result) {
        strncpy(result, start, len);
        result[len] = '\0';
    }

    return result;
}

void platform_display_script_info(const char *rom_path, const char *output, const char *error, int exit_code) {
    char display_buffer[4096];
    int offset = 0;

    offset += snprintf(display_buffer + offset, sizeof(display_buffer) - offset,
                      "ROM: %s\n", rom_path ? rom_path : "NULL");

    offset += snprintf(display_buffer + offset, sizeof(display_buffer) - offset,
                      "Exit Code: %d\n", exit_code);

    if (output && strlen(output) > 0) {
        offset += snprintf(display_buffer + offset, sizeof(display_buffer) - offset,
                          "Output:\n%s\n", output);
    }

    if (error && strlen(error) > 0) {
        offset += snprintf(display_buffer + offset, sizeof(display_buffer) - offset,
                          "Error:\n%s\n", error);
    }

    /* Store for display in game screen */
    strncpy(retro_display_messages, display_buffer, sizeof(retro_display_messages) - 1);
    retro_display_messages[sizeof(retro_display_messages) - 1] = '\0';
}

#if defined(__unix__) && !defined(__ANDROID__)
int platform_run_script_with_output(const char *script_path, const char *rom_path, char **output, char **error) {
    const char *system_dir = hook_get_system_directory();
    if (!system_dir) {
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
        return -1;
    }

    int stdout_pipe[2], stderr_pipe[2];
    if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1) {
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to create pipes\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        execl(script_path, script_path, system_dir, rom_path, (char *)NULL);
        _exit(127);
    } else if (pid > 0) {
        /* Parent process */
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        /* Read stdout */
        char stdout_buffer[4096] = {0};
        ssize_t stdout_bytes = read(stdout_pipe[0], stdout_buffer, sizeof(stdout_buffer) - 1);
        if (stdout_bytes > 0) {
            *output = malloc(stdout_bytes + 1);
            if (*output) {
                memcpy(*output, stdout_buffer, stdout_bytes);
                (*output)[stdout_bytes] = '\0';
            }
        }

        /* Read stderr */
        char stderr_buffer[4096] = {0};
        ssize_t stderr_bytes = read(stderr_pipe[0], stderr_buffer, sizeof(stderr_buffer) - 1);
        if (stderr_bytes > 0) {
            *error = malloc(stderr_bytes + 1);
            if (*error) {
                memcpy(*error, stderr_buffer, stderr_bytes);
                (*error)[stderr_bytes] = '\0';
            }
        }

        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }

    return -1;
}

void platform_launch_retroarch_and_exit(const char *core_filename, const char *rom_path) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/retroarch", "retroarch", "-L", core_filename, rom_path, (char *)NULL);
        execlp("retroarch", "retroarch", "-L", core_filename, rom_path, (char *)NULL);
        _exit(127);
    } else if (pid > 0) {
        cleanup_and_exit();
    }
}

#elif defined(_WIN32)
int platform_run_script_with_output(const char *script_path, const char *rom_path, char **output, char **error) {
    SECURITY_ATTRIBUTES sa;
    HANDLE stdout_read, stdout_write;
    HANDLE stderr_read, stderr_write;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    const char *system_dir = hook_get_system_directory();
    if (!system_dir) {
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
        return -1;
    }

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&stdout_read, &stdout_write, &sa, 0) ||
        !CreatePipe(&stderr_read, &stderr_write, &sa, 0)) {
        return -1;
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = stdout_write;
    si.hStdError = stderr_write;
    si.dwFlags |= STARTF_USESTDHANDLES;

    char command[MAX_COMMAND_SIZE];
    snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\"", script_path, system_dir, rom_path);

    if (CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(stdout_write);
        CloseHandle(stderr_write);

        /* Read stdout */
        char stdout_buffer[4096] = {0};
        DWORD stdout_bytes;
        if (ReadFile(stdout_read, stdout_buffer, sizeof(stdout_buffer) - 1, &stdout_bytes, NULL) && stdout_bytes > 0) {
            *output = malloc(stdout_bytes + 1);
            if (*output) {
                memcpy(*output, stdout_buffer, stdout_bytes);
                (*output)[stdout_bytes] = '\0';
            }
        }

        /* Read stderr */
        char stderr_buffer[4096] = {0};
        DWORD stderr_bytes;
        if (ReadFile(stderr_read, stderr_buffer, sizeof(stderr_buffer) - 1, &stderr_bytes, NULL) && stderr_bytes > 0) {
            *error = malloc(stderr_bytes + 1);
            if (*error) {
                memcpy(*error, stderr_buffer, stderr_bytes);
                (*error)[stderr_bytes] = '\0';
            }
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exit_code;
        GetExitCodeProcess(pi.hProcess, &exit_code);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(stdout_read);
        CloseHandle(stderr_read);

        return exit_code;
    }

    CloseHandle(stdout_read);
    CloseHandle(stdout_write);
    CloseHandle(stderr_read);
    CloseHandle(stderr_write);
    return -1;
}

void platform_launch_retroarch_and_exit(const char *core_filename, const char *rom_path) {
    char command[MAX_COMMAND_SIZE];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    snprintf(command, sizeof(command), "retroarch.exe -L \"%s\" \"%s\"", core_filename, rom_path);

    if (CreateProcessA(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        cleanup_and_exit();
    }
}

#elif defined(__ANDROID__)
int platform_run_script_with_output(const char *script_path, const char *rom_path, char **output, char **error) {
    /* Android implementation with limited functionality */
    const char *system_dir = hook_get_system_directory();
    if (!system_dir) {
        log_cb(RETRO_LOG_ERROR, "CoreLoader: Failed to get system directory\n");
        return -1;
    }

    char command[MAX_COMMAND_SIZE];
    snprintf(command, sizeof(command), "%s '%s' '%s'", script_path, system_dir, rom_path);

    FILE *fp = popen(command, "r");
    if (fp) {
        char buffer[4096] = {0};
        size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, fp);
        if (bytes_read > 0) {
            *output = malloc(bytes_read + 1);
            if (*output) {
                memcpy(*output, buffer, bytes_read);
                (*output)[bytes_read] = '\0';
            }
        }

        int result = pclose(fp);
        return WEXITSTATUS(result);
    }

    return -1;
}

void platform_launch_retroarch_and_exit(const char *core_filename, const char *rom_path) {
    /* Android cannot launch child processes easily, fallback to error display */
    log_cb(RETRO_LOG_ERROR, "Android: Cannot launch retroarch as child process\n");
    platform_display_script_info(rom_path, "Android: Cannot launch RetroArch",
                                "Child process execution not supported on Android", -1);
}

#else
int platform_run_script_with_output(const char *script_path, const char *rom_path, char **output, char **error) {
    (void)script_path;
    (void)rom_path;
    (void)output;
    (void)error;
    return -1;
}

void platform_launch_retroarch_and_exit(const char *core_filename, const char *rom_path) {
    (void)core_filename;
    (void)rom_path;
}
#endif
