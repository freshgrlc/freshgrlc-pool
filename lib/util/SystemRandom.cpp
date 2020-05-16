#include "SystemRandom.h"

#include <stdexcept>
#include <cstdio>


SystemRandom::SystemRandom()
{
    if (!(this->urandom = fopen("/dev/urandom", "rb")))
        throw std::runtime_error("Error opening /dev/urandom: Error " + std::to_string(errno));
}

SystemRandom::~SystemRandom()
{
    fclose(this->urandom);
}

void SystemRandom::fill(void *buffer, size_t size)
{
    OBTAIN_LOCK(_lock);

    if (fread(buffer, 1, size, this->urandom) != size)
        throw std::runtime_error("Error reading from /dev/urandom: Error " + std::to_string(ferror(this->urandom)));
}

ByteString SystemRandom::get(size_t size)
{
    std::vector<uint8_t> buffer(size);

    this->fill(buffer.data(), buffer.size());
    return std::move(buffer);
}
