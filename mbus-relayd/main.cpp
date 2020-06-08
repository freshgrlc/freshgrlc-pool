#include "MessageBusServer.h"

#include <util/CommandLineArguments.h>
#include <util/logger.h>


static void enableDebugLogging()
{
    __loglevel = DEBUG;

    MODULE_LOGLEVEL = DEBUG;
}

int main(int argc, char **argv)
{
    int port = 9094;
    bool debug = false;

    CommandLineArguments(argc, argv, {
        { port,                 "port",                 'p',        "Port to listen on." },
        { debug,                "debug",                'D',        "Enable debug logging." }
    }).processOrPrintUsageAndExit();

    if (debug)
        enableDebugLogging();

    return MessageBusServer(port).run();
}
