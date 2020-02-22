#include "Pool.h"

#include <stdexcept>

#include <mining/hashplugin.h>


Pool::Pool(DaemonConnectorRef &daemon, const DaemonConnector::StratumInitializer &stratumInitializer, const StratumConfiguration &configuration) :
    daemon(daemon),
    stratum(Listener(configuration.port), stratumInitializer, daemon->blockSubmitter(), get_hashplugin(configuration.hashingAlgorithm), configuration.coinbaseSignature, configuration.initialDifficulty),
    stratumListener(this->stratum)
{
    this->stratumListener.start();
}

void Pool::StratumListener::main()
{
    /* Should never return */
    int errorCode = this->stratumServer.listen();

    throw std::runtime_error(std::string("Failed to listen on stratum listener socket: Error ") + std::to_string(errorCode));
}
