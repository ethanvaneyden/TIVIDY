#include "Submission.h"

namespace education
{

const int Submission::NO_MARK;

Submission::Submission(const std::string& id,
                       const std::string& studentName,
                       const std::string& studentNumber)
    : Item(id, "submission", studentName), studentId(studentNumber), mark(NO_MARK)
{
}

Submission::~Submission() {}

std::map<std::string, std::string> Submission::getAttributes() const
{
    std::map<std::string, std::string> all = Item::getAttributes();
    if (!studentId.empty())
    {
        all["studentId"] = studentId;
    }
    return all;
}

const std::string& Submission::getStudentName() const
{
    return getName();
}

const std::string& Submission::getStudentId() const
{
    return studentId;
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
