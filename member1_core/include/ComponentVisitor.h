/**
 * @file ComponentVisitor.h
 * @brief Visitor interface for walking the workflow composite.
 *
 * GoF Visitor. The composite (Root, Container, Item) only knows how to
 * accept a visitor. Every "view" of a running workflow (JSON for the UI,
 * a progress report, an audit export, ...) is a separate visitor, so new
 * reports never require edits to the core model classes.
 *
 * Participants:
 *  - Visitor:          ComponentVisitor
 *  - ConcreteVisitors: JsonTreeVisitor, ProgressReportVisitor (member7_integration)
 *  - Element:          Component (accept), implemented by Root, Container and Item
 *  - ConcreteElements: Root, Container, Item
 */
#pragma once

class Root;
class Container;
class Item;

class ComponentVisitor
{
public:
    virtual ~ComponentVisitor() = default;

    /** @brief Visit a workflow root. The visitor decides how to descend. */
    virtual void visitRoot(const Root& root) = 0;

    /** @brief Visit a container (assignment, ward, ...). */
    virtual void visitContainer(const Container& container) = 0;

    /** @brief Visit a leaf work item. */
    virtual void visitItem(const Item& item) = 0;
};
