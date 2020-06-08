#ifndef UTIL_BYTESTRINGREADER_H
#define UTIL_BYTESTRINGREADER_H

#include <memory>
#include <stdexcept>


#include "ConstByteStringRef.h"


class ByteStringReader
{
    public:
        inline ByteStringReader(const ConstByteStringRef &buffer) : buffer(std::make_unique<ConstByteStringRef>(buffer)) {}

        template <typename T>
        ByteStringReader &operator>>(T &out)
        {
            T *ptr = (T *) this->buffer->data;

            this->advance(sizeof(T));

            out = *ptr;
            return *this;
        }

        template <>
        ByteStringReader &operator>>(std::string &out);

        template <typename T>
        inline T read(void)
        {
            T v;
            *this >> v;
            return v;
        }

        ByteString read(size_t amount);

        inline size_t remaining(void)                       { return this->buffer->length; }
        inline const ConstByteStringRef &remainder(void)    { return *this->buffer; }

    private:
        std::unique_ptr<ConstByteStringRef> buffer;

        void advance(size_t amount);
};

#endif
