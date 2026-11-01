#include "Submission.h"

namespace education
{

const int Submission::NO_MARK;

Submission::Submission(const std::string& id, const std::string& studentName)
    : Item(id, "submission", studentName), mark(NO_MARK)
{
}

Submission::~Submission() {}

const std::string& Submission::getStudentName() const
{
    return getName();
}

bool Submission::hasMark() const
{
    return mark != NO_MARK;
}

int Submission::getMark() const
{
    return mark;
}

void Submission::setMark(int value)
{
    mark = value;
}

}
