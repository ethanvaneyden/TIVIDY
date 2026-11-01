/**
 * @file JsonSerializer.h
 * @brief Minimal JSON serializer for engine objects.
 *
 * Uses nlohmann/json for JSON generation and parsing.
*/

#pragma once

#include <string>
#include <vector>

class Item;
class Participant;
class Component;

/**
 * @class JsonSerializer
 * @brief Converts engine objects to JSON strings.
*/
class JsonSerializer
{
    public:
        JsonSerializer() = default;
        ~JsonSerializer() = default;

        /**
         * @brief Serialize a component tree.
        */
        static std::string serializeTree(const Component* root);

        /**
         * @brief Serialize a single item.
        */
        static std::string serializeItem(const Item* item);

        /**
         * @brief Serialize a participant.
        */
        static std::string serializeParticipant(const Participant* p);

        /**
         * @brief Serialize a flat list of history records.
         *        The history is stored as strings, so this just wraps them.
        */
        static std::string serializeHistory(const std::vector<std::string>& records);

        /**
         * @brief Serialize a simple error.
        */
        static std::string serializeError(const std::string& error,
                                          const std::string& message);

        /**
         * @brief Extract a string field from a JSON body.
         * @return The field's value, or the fallback if the field is absent.
        */
        static std::string getString(const std::string& json,
                                     const std::string& field,
                                     const std::string& fallback);

        /**
         * @brief Extract an integer field.
        */
        static int getInt(const std::string& json,
                          const std::string& field,
                          int fallback);

};
