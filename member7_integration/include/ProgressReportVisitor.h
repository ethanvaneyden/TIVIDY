/**
 * @file ProgressReportVisitor.h
 * @brief Concrete Visitor that computes monitoring/progress figures.
 *
 * GoF Visitor - ConcreteVisitor. Demonstrates the point of the pattern: a
 * completely different "view" of the running workflow (counts per lifecycle
 * state and per container) is produced without adding a single method to
 * Root, Container or Item beyond accept().
 */
#pragma once

#include "ComponentVisitor.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

/** @brief Progress figures for one container (assignment / ward). */
struct ContainerProgress
{
    std::string id;
    std::string name;
    std::string phase;
    int total = 0;
    int completed = 0;
    int reviewed = 0;
    int rejected = 0;
    int percentComplete = 0;
};

class ProgressReportVisitor : public ComponentVisitor
{
public:
    ProgressReportVisitor();

    void visitRoot(const Root& root) override;
    void visitContainer(const Container& container) override;
    void visitItem(const Item& item) override;

    /** @brief Items per lifecycle state name over the whole visited tree. */
    const std::map<std::string, int>& stateCounts() const;

    /** @brief One entry per visited container, in tree order. */
    const std::vector<ContainerProgress>& containers() const;

    int totalItems() const;
    int completedItems() const;

    /** @brief Whole-tree completion percentage (0 when there are no items). */
    int percentComplete() const;

    /** @brief The report as a JSON string. */
    std::string toJson() const;

private:
    std::map<std::string, int> states;
    std::vector<ContainerProgress> perContainer;
    int total;
    int completed;
    std::size_t activeContainer;
    static const std::size_t NONE;
};
