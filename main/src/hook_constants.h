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

#endif /* HOOK_CONSTANTS_H */
