#ifndef TIVIDY_WORKFLOWEVENT_H
#define TIVIDY_WORKFLOWEVENT_H

using namespace std;
#include <string>

struct WorkflowEvent {  //doxygen
    string type;
    string recipientId;
    string payload;
};


#endif //TIVIDY_WORKFLOWEVENT_H
