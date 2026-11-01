#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "ActionHistory.h"
#include "WorkCommand.h"

namespace
{
class RecordingCommand : public WorkCommand
{
public:
    RecordingCommand(int& executions, int& destructions, std::string message)
        : executions(executions),
          destructions(destructions),
          message(std::move(message))
    {
    }

    ~RecordingCommand() override
    {
        ++destructions;
    }

    void execute() override
    {
        ++executions;
    }

    std::string description() const override
    {
        return message;
    }

private:
    int& executions;
    int& destructions;
    std::string message;
};
}

TEST(ActionHistoryTest, ExecutesAndOwnsCommandsInChronologicalOrder)
{
    int executions = 0;
    int destructions = 0;
    {
        ActionHistory history;
        history.execute(std::unique_ptr<WorkCommand>(
            new RecordingCommand(executions, destructions, "first")));
        history.execute(std::unique_ptr<WorkCommand>(
            new RecordingCommand(executions, destructions, "second")));

        EXPECT_EQ(executions, 2);
        EXPECT_EQ(destructions, 0);
        EXPECT_EQ(history.getHistoryAsStrings(),
                  (std::vector<std::string>{"first", "second"}));
    }

    EXPECT_EQ(destructions, 2);
}

TEST(ActionHistoryTest, IgnoresNullCommands)
{
    ActionHistory history;

    history.execute(std::unique_ptr<WorkCommand>());

    EXPECT_TRUE(history.getHistoryAsStrings().empty());
}