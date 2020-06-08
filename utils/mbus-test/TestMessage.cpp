#include <nlohmann/json.hpp>

#include "TestMessage.h"

REGISTER_MESSAGE(TestMessage, "test", "helloWorld")

MessageRef TestMessage::make(std::string &&messageClass, std::string &&messageType, std::string &&content)
{
    return std::make_unique<TestMessage>(json::parse(content)["message"].get<std::string>());
}

void TestMessage::encodeContent() const
{
    this->setContent((json) {{ "message", this->message() }});
}
