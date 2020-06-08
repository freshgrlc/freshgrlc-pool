#include <nlohmann/json.hpp>

#include "SetSubscriptionsMessage.h"


REGISTER_MESSAGE(SetSubscriptionsMessage, "__meta__", "setSubscriptions")


SetSubscriptionsMessage::SetSubscriptionsMessage(const std::vector<std::string> &subscriptions) : AbstractMessage(CLASS, TYPE),
    _subscriptions(subscriptions)
{
}

SetSubscriptionsMessage::SetSubscriptionsMessage(std::vector<std::string> &&subscriptions) : AbstractMessage(CLASS, TYPE),
    _subscriptions(std::move(subscriptions))
{
}

MessageRef SetSubscriptionsMessage::make(std::string &&messageClass, std::string &&messageType, std::string &&content)
{
    auto subscriptions = json::parse(content)["subscriptions"].get<std::vector<std::string>>();

    return std::make_unique<SetSubscriptionsMessage>(std::move(subscriptions));
}

void SetSubscriptionsMessage::encodeContent() const
{
    this->setContent((json) {{ "subscriptions", this->subscriptions() }});
}
