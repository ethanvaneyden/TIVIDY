#include <gtest/gtest.h>
#include "json.h"

#include "Container.h"
#include "Item.h"
#include "JsonSerializer.h"
#include "Participant.h"
#include "Root.h"

#include <limits>
#include <stdexcept>
#include <string>

TEST(JsonSerializerTest, SerializesTreeAsValidJsonAndEscapesText)
{
    Root root("root", "workflow", "Workflow \"A\"");
    Container container("container", "group", "Group\nOne");
    Item item("item", "submission", "A\\B");
    root.add(&container);
    container.add(&item);

    const nlohmann::json serialized =
        nlohmann::json::parse(JsonSerializer::serializeTree(&root));

    EXPECT_EQ(serialized.at("name"), "Workflow \"A\"");
    EXPECT_EQ(serialized.at("children").at(0).at("name"), "Group\nOne");
    EXPECT_EQ(serialized.at("children").at(0).at("children").at(0).at("name"),
              "A\\B");
    EXPECT_EQ(serialized.at("children").at(0).at("children").at(0).at("assignee"),
              nullptr);
}

TEST(JsonSerializerTest, SerializesNullPointersAndParticipantFields)
{
    Participant participant("marker1", "Alex", "Marker");

    EXPECT_EQ(nlohmann::json::parse(JsonSerializer::serializeItem(nullptr)), nullptr);
    EXPECT_EQ(nlohmann::json::parse(JsonSerializer::serializeParticipant(nullptr)), nullptr);

    const nlohmann::json serialized =
        nlohmann::json::parse(JsonSerializer::serializeParticipant(&participant));
    EXPECT_EQ(serialized.at("id"), "marker1");
    EXPECT_EQ(serialized.at("name"), "Alex");
    EXPECT_EQ(serialized.at("role"), "Marker");
}

TEST(JsonSerializerTest, ParsesEscapedStringValuesAndUsesFallbackForWrongTypes)
{
    const std::string body = R"({"name":"A\"lice\\Smith","count":10})";

    EXPECT_EQ(JsonSerializer::getString(body, "name", ""), "A\"lice\\Smith");
    EXPECT_EQ(JsonSerializer::getString(body, "count", "fallback"), "fallback");
}

TEST(JsonSerializerTest, ParsesBoundedIntegerValuesAndUsesFallbackWhenOutOfRange)
{
    const std::string body =
        std::string("{\"value\":") + std::to_string(std::numeric_limits<int>::max()) +
        ",\"tooLarge\":2147483648,\"fraction\":1.5}";

    EXPECT_EQ(JsonSerializer::getInt(body, "value", 0),
              std::numeric_limits<int>::max());
    EXPECT_EQ(JsonSerializer::getInt(body, "tooLarge", 42), 42);
    EXPECT_EQ(JsonSerializer::getInt(body, "fraction", 42), 42);
    EXPECT_EQ(JsonSerializer::getInt(body, "missing", 42), 42);
}

TEST(JsonSerializerTest, InvalidJsonIsReported)
{
    EXPECT_THROW(JsonSerializer::getString("{invalid", "name", ""), std::exception);
}

TEST(JsonSerializerTest, SerializesHistoryAndErrorsAsJson)
{
    const nlohmann::json history = nlohmann::json::parse(
        JsonSerializer::serializeHistory({"started", "line\nquoted\""}));
    EXPECT_EQ(history, (nlohmann::json{"started", "line\nquoted\""}));

    const nlohmann::json error = nlohmann::json::parse(
        JsonSerializer::serializeError("NotFound", "instance"));
    EXPECT_EQ(error.at("error"), "NotFound");
    EXPECT_EQ(error.at("message"), "instance");
}
