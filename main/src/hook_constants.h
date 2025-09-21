#ifndef HOOK_CONSTANTS_H
#define HOOK_CONSTANTS_H

/* Path separator */
#ifdef _WIN32
#define HOOK_PATH_SEPARATOR "\\"
#else
#define HOOK_PATH_SEPARATOR "/"
#endif

/* Video constants */
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_PIXELS (VIDEO_WIDTH * VIDEO_HEIGHT)

/* Buffer sizes */
#define MAX_PATH_SIZE 4096
#define MAX_COMMAND_SIZE 2048
#define MAX_SCRIPT_PATH_SIZE 512
#define MAX_ERROR_MSG_SIZE 512

/* Core options */
#define MAX_CORE_OPTIONS 10
#define MAX_CORES_LIST_SIZE 8192
#define MAX_PATTERNS_LIST_SIZE 1024

/* Default path patterns */
#define DEFAULT_PATH_PATTERNS "/mame/|/fbneo/|/arcade/"

#endif /* HOOK_CONSTANTS_H */
