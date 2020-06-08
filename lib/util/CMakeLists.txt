
add_library(util SHARED logger.cpp Lock.cpp ByteString.cpp ConstByteStringRef.cpp CommandLineArguments.cpp Thread.cpp SystemRandom.cpp)
set_property(TARGET util PROPERTY POSITION_INDEPENDENT_CODE ON)
