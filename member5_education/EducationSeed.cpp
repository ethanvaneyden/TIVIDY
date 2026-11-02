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
    struct Row { const char* id; const char* student; const char* studentId; int mark; };
    static const Row rows[] = {
        { "SUB-A", "Student A", "u1001", 78 },  // Lecturer band
        { "SUB-B", "Student B", "u1002", 65 },  // TA band
        { "SUB-C", "Student C", "u1003", 45 },  // Moderator band
        { "SUB-D", "Student D", "u1004", 50 },  // lowest mark of the TA band
        { "SUB-E", "Student E", "u1005", 40 },  // lowest auto-approvable mark
        { "SUB-F", "Student F", "u1006", 71 },  // lowest mark of the Lecturer band
        { "SUB-G", "Student G", "u1007", 35 },  // Rejected: lecturer must resolve
        { "SUB-H", "Student H", "u1008", 0 }    // Rejected: lecturer must resolve
    };

    std::vector<DemoSubmission> result;
    for (unsigned i = 0; i < sizeof(rows) / sizeof(rows[0]); ++i)
    {
        DemoSubmission s;
        s.itemId = rows[i].id;
        s.studentName = rows[i].student;
        s.studentId = rows[i].studentId;
        s.mark = rows[i].mark;
        result.push_back(s);
    }
    return result;
}

}
