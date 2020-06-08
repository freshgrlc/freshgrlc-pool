#include "ByteStringReader.h"
#include "ByteString.h"

ByteString ByteStringReader::read(size_t amount)
{
    ConstByteStringRef ptr = { this->buffer->data, amount };

    this->advance(amount);
    return ptr;
}

void ByteStringReader::advance(size_t amount)
{
    if (this->buffer->length < amount)
        throw std::runtime_error("ByteStringReader: Attempted to read beyond end of buffer (read " + std::to_string(amount) + ", size " + std::to_string(this->buffer->length) + ")");

    this->buffer = std::make_unique<ConstByteStringRef>(buffer->data + amount, buffer->length - amount);
}

template <>
ByteStringReader &ByteStringReader::operator>>(std::string &out)
{
    size_t size = (size_t) this->read<uint32_t>();
    char *ptr = (char *) this->buffer->data;

    this->advance(size);
    out = std::string(ptr, size);

    return *this;
}
