#include "core_loader.h"

#ifdef __unix__
#include <unistd.h>
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
#endif
}
