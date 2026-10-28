#include "Submission.h"

#include "EducationPolicies.h"
#include "InvalidTransition.h"

#include <stdexcept>
#include <utility>

namespace education
{

Submission::Submission(std::string id, std::string name, Component* parent)
    : Item(std::move(id), "submission", std::move(name), parent),
      mark(0), markRecorded(false)
{
}

void Submission::storeResult(int value)
{
    mark = value;
    markRecorded = true;
}

void Submission::complete(int mark)
{
    if (!EducationPolicies::isValidMark(mark))
        throw std::invalid_argument("A mark must be between 0 and 100");
    Item::complete(mark);
}

void Submission::resolveReview(Participant* reviewer, int mark)
{
    if (!EducationPolicies::isValidMark(mark))
        throw std::invalid_argument("A mark must be between 0 and 100");
    Item::resolveReview(reviewer, mark);
}

bool Submission::hasMark() const
{
    return markRecorded;
}

int Submission::getMark() const
{
    if (!markRecorded)
        throw InvalidTransition("This submission has no mark yet");
    return mark;
}

bool Submission::hasResult() const
{
    return markRecorded;
}

int Submission::getResult() const
{
    return getMark();
}

} // namespace education
