#pragma once

#include "Item.h"

namespace education
{

/**
 * @brief An education workflow item that stores a validated assignment mark.
 */
class Submission : public Item
{
private:
    int mark;
    bool markRecorded;

    void storeResult(int value) override;

public:
    Submission(std::string id, std::string name, Component* parent = nullptr);

    void complete(int mark) override;
    void resolveReview(Participant* reviewer, int mark) override;

    bool hasResult() const override;
    int getResult() const override;
    bool hasMark() const;
    int getMark() const;
};

} // namespace education
