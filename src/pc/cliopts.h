#ifndef _CLIOPTS_H
#define _CLIOPTS_H

#include "platform.h"

struct PCCLIOptions  {
    unsigned int SkipIntro;
    unsigned int FullScreen;
    unsigned int PoolSize;
    char ConfigFile[SYS_MAX_PATH];
    char SavePath[SYS_MAX_PATH];
    char GameDir[SYS_MAX_PATH];
};

extern struct PCCLIOptions gCLIOpts;

struct Tas {
    unsigned int controllerType;
    unsigned int stop;
    unsigned int progress;
    unsigned int onStart;
};

extern struct Tas gTas;

void parse_cli_opts(int argc, char* argv[]);

#endif // _CLIOPTS_H
