#ifndef TESTMESSAGE_H
#define TESTMESSAGE_H

#include <messagebus/AbstractMessage.h>

class TestMessage : public AbstractMessage
{
    public:
        static const std::string CLASS;
        static const std::string TYPE;

        inline TestMessage(const std::string &message) : AbstractMessage(CLASS, TYPE), _message(message) {}
        inline TestMessage(std::string &&message = "Hello world!") : AbstractMessage(CLASS, TYPE), _message(std::move(message)) {}

        static MessageRef make(std::string &&messageClass, std::string &&messageType, std::string &&content);

        void encodeContent() const override;

        inline const std::string &message(void) const   { return _message; }

    private:
        std::string _message;
};

#endif
