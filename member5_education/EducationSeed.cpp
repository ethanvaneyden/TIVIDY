#include "EducationSeed.h"

#include "EducationPolicies.h"
#include "ParticipantRegistry.h"

namespace education
{

void EducationSeed::seedParticipants(ParticipantRegistry& registry)
{
    registry.add("Alice", EducationRoles::ta());
    registry.add("Bob", EducationRoles::ta());
    registry.add("Charlie", EducationRoles::ta());
    registry.add("Dr Dlamini", EducationRoles::lecturer());
    registry.add("Dr Mokoena", EducationRoles::moderator());
}

std::vector<DemoSubmission> EducationSeed::demoSubmissions()
{
    struct Row { const char* id; const char* student; int mark; };
    static const Row rows[] = {
        { "SUB-A", "Student A", 78 },  // Lecturer band
        { "SUB-B", "Student B", 65 },  // TA band
        { "SUB-C", "Student C", 45 },  // Moderator band
        { "SUB-D", "Student D", 50 },  // lowest mark of the TA band
        { "SUB-E", "Student E", 40 },  // lowest auto-approvable mark
        { "SUB-F", "Student F", 71 },  // lowest mark of the Lecturer band
        { "SUB-G", "Student G", 35 },  // Rejected: lecturer must resolve
        { "SUB-H", "Student H", 0 }    // Rejected: lecturer must resolve
    };

    std::vector<DemoSubmission> result;
    for (unsigned i = 0; i < sizeof(rows) / sizeof(rows[0]); ++i)
    {
        DemoSubmission s;
        s.itemId = rows[i].id;
        s.studentName = rows[i].student;
        s.mark = rows[i].mark;
        result.push_back(s);
    }
    return result;
}

} // namespace education
