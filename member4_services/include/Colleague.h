#ifndef TIVIDY_COLLEAGUE_H
#define TIVIDY_COLLEAGUE_H

using namespace std;
#include <string>
#include "Mediator.h"

class Colleague {
public:
    virtual void notify(string event);
    void setMediator(Mediator* mediator);
    virtual ~Colleague();
};


#endif //TIVIDY_COLLEAGUE_H
