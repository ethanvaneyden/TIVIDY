/**
 * @file AssignmentMarkingBuilder.h
 * @brief Concrete Builder for the Education scenario.
 */

#ifndef ASSIGNMENTMARKINGBUILDER_H
#define ASSIGNMENTMARKINGBUILDER_H

#include "WorkflowBuilder.h"

#include <memory>
#include <string>

class Root;
class Container;

namespace education
{

class AssignmentMarkingBuilder : public WorkflowBuilder
{
    private:
        std::unique_ptr<Root> root;
        Container* currentContainer;

    public:
        AssignmentMarkingBuilder();
        virtual ~AssignmentMarkingBuilder();

        /** @brief Start a new module. Any earlier unfinished tree is discarded. */
        virtual void buildRoot(const std::string& rootName);

        /**
         * @brief Add an assignment under the module and make it current.
         * @throws std::logic_error if buildRoot() has not been called.
         */
        virtual void buildContainer(const std::string& containerId,
                                    const std::string& name);

        /**
         * @brief Add a student submission under the current assignment.
         * @throws std::logic_error if there is no current assignment.
         */
        virtual void buildItem(const std::string& itemId, const std::string& name);

        /** @brief Hand over the finished tree; the builder is left empty. */
        virtual std::unique_ptr<Root> getResult();
};

} // namespace education

#endif /*ASSIGNMENTMARKINGBUILDER_H*/
