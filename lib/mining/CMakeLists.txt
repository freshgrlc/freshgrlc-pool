
add_definitions(-DMODULE=mining -DPLUGINDIR="${PLUGINDIR}")

add_library(mining STATIC hashplugin.cpp
                          AbstractCoinbaseOutput.cpp CoinbaseOutput.cpp CoinbaseCommitment.cpp CoinbaseWitnessCommitment.cpp
                          CoinbaseOutputs.cpp CoinbaseTransaction.cpp
                          Hash.cpp
                          BlockHeader.cpp
                          MerkleBranch.cpp MerkleRoot.cpp
                          VarInt.cpp
                          txscript/PushData.cpp)

target_link_libraries(mining dl)
