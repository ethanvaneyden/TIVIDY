/**
 * @file JsonTreeVisitor.h
 * @brief Concrete Visitor that renders a workflow tree as JSON for the UI.
 *
 * GoF Visitor - ConcreteVisitor. The composite classes only know how to
 * accept a visitor; everything about the JSON shape lives here. Scenario
 * specific fields (diagnosis, age, student number ...) arrive through
 * Item::getAttributes(), so there is no type checking in the serializer.
 */
#pragma once

#include "ComponentVisitor.h"

#include <memory>
#include <string>

class JsonTreeVisitor : public ComponentVisitor
{
public:
    JsonTreeVisitor();
    ~JsonTreeVisitor() override;

    JsonTreeVisitor(const JsonTreeVisitor&) = delete;
    JsonTreeVisitor& operator=(const JsonTreeVisitor&) = delete;

    void visitRoot(const Root& root) override;
    void visitContainer(const Container& container) override;
    void visitItem(const Item& item) override;

    /** @brief JSON text of the most recently visited component (null if none). */
    std::string result() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
