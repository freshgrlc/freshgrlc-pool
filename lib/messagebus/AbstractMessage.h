#ifndef MESSAGEBUS_ABSTRACTMESSAGE_H
#define MESSAGEBUS_ABSTRACTMESSAGE_H

#include <string>
#include <memory>

#include <util/ByteString.h>


#ifndef INCLUDE_NLOHMANN_JSON_HPP_
namespace nlohmann {
    class json;
};
#endif

using json = nlohmann::json;


class AbstractMessage
{
    public:
        typedef std::unique_ptr<AbstractMessage> (*Instantiator)(std::string &&messageClass, std::string &&messageType, std::string &&content);

        static const int MAX_MESSAGE_SIZE = 32768;

        virtual ~AbstractMessage(void);

        ByteString encode(void) const;

        static std::unique_ptr<AbstractMessage> decode(const ConstByteStringRef &message);
        static std::unique_ptr<AbstractMessage> decodeOpaque(const ConstByteStringRef &message);

        template <class T>
        inline bool is(void) const
        {
            return _messageClass == T::CLASS && _messageType == T::TYPE;
        }

        template <class T>
        inline const T &as(void) const
        {
            if (!this->is<T>())
                throw std::runtime_error("Invalid runtime cast: Attempt to case message of '" +
                                         _messageClass + "::" + _messageType +
                                         "' to '" +
                                         T::CLASS + "::" + _messageType +
                                         "'!");

            return *((const T *) this);
        }

        std::string raw(void) const;

        inline const std::string &messageClass(void) const  { return _messageClass; }
        inline const std::string &messageType(void) const   { return _messageType; }

    protected:
        AbstractMessage(const std::string &messageClass, const std::string &messageType);
        AbstractMessage(std::string &&messageClass, std::string &&messageType);

        void setContent(const json &content) const;
        void setContent(json &&content) const;

    private:
        std::string _messageClass;
        std::string _messageType;

        mutable std::unique_ptr<json> _content;

        virtual void encodeContent(void) const = 0;

        static std::unique_ptr<AbstractMessage> doDecode(const ConstByteStringRef &message, Instantiator instantiator);
};

typedef std::unique_ptr<AbstractMessage> MessageRef;


#include "MessageFactory.h"

#define REGISTER_MESSAGE(definition, messageClass, messageType) \
    const std::string definition::CLASS = messageClass; \
    const std::string definition::TYPE = messageType; \
    static const MessageFactory &_dummy = MessageFactory::get().registerMessageBusMessageImplementation(definition);

#endif
