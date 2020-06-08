#ifndef MESSAGEBUS_OPAQUEMESSAGE_H
#define MESSAGEBUS_OPAQUEMESSAGE_H

#include "AbstractMessage.h"

class OpaqueMessage : public AbstractMessage
{
    public:
        const std::string content;

        OpaqueMessage(const std::string &messageClass, const std::string &messageType, const std::string &content);
        OpaqueMessage(std::string &&messageClass, std::string &&messageType, std::string &&content);
        OpaqueMessage(const OpaqueMessage &them);

        inline static MessageRef make(std::string &&messageClass, std::string &&messageType, std::string &&content)
        {
            return (MessageRef) std::make_unique<OpaqueMessage>(std::move(messageClass), std::move(messageType), std::move(content));
        }

    private:
        void encodeContent(void) const override;
};

#include <memory>

typedef std::unique_ptr<OpaqueMessage> OpaqueMessageRef;

#endif
