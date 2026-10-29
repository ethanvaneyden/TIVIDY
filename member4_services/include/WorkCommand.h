#ifndef TIVIDY_WORKCOMMAND_H
#define TIVIDY_WORKCOMMAND_H

using namespace std;
#include <string>
#include "WorkComponent.h"

class WorkCommand
{
private:
    WorkComponent* component;   //dont know how the include will work from other people's classes
public:
    void execute();
    void undo();
    string getDescription();
};


#endif //TIVIDY_WORKCOMMAND_H
