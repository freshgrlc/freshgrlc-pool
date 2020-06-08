#ifndef MESSAGEBUS_SETSUBSCRIPTIONSMESSAGE_H
#define MESSAGEBUS_SETSUBSCRIPTIONSMESSAGE_H

#include "AbstractMessage.h"

class SetSubscriptionsMessage : public AbstractMessage
{
    public:
        static const std::string CLASS;
        static const std::string TYPE;

        SetSubscriptionsMessage(const std::vector<std::string> &subscriptions);
        SetSubscriptionsMessage(std::vector<std::string> &&subscriptions);

        static MessageRef make(std::string &&messageClass, std::string &&messageType, std::string &&content);

        const std::vector<std::string> &subscriptions(void) const   { return _subscriptions; }

        inline bool operator!=(const SetSubscriptionsMessage &them) const
        {
            return this->subscriptions() != them.subscriptions();
        }

    private:
        std::vector<std::string> _subscriptions;

        void encodeContent() const override;
};

#endif
