/**
 * @file JsonSerializer.h
 * @brief Minimal JSON serializer for engine objects.
 *
 * [DEFAULT DECISION] Hand-rolled writer (no dependency on nlohmann/json).
 * Rationale: the shapes are simple, the code is small, and there is
 * nothing to fetch in CMake. If the team prefers nlohmann, this file
 * can be swapped out; only RestServer.cpp includes it.
*/

#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <string>
#include <vector>

class Container;
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
        static std::string serializeTree(const Container* root);

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

    private:
        static void serializeComponent(const Component* c, std::string& out);

        /**
         * @brief Escapes special characters in a string for JSON compatibility.
         * 
         * This method iterates through the input string and replaces control characters 
         * and quotation marks with their corresponding JSON-escaped equivalents.
         * 
         * @param s The input string containing text to be escaped.
         * @return A new `std::string` with all special characters properly escaped.
        */
        static std::string escape(const std::string& s);
};

#endif /*JSONSERIALIZER_H*/