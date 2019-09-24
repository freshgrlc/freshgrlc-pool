#include <stratum/StratumServer.h>

#include <util/logger.h>

int main()
{
    __loglevel = DEBUG;

    StratumServer server(Listener(3032));

    return server.listen();
}
