#include "DepthFirstIterator.h"

class StateFilterIterator : public DepthFirstIterator
{
private:
    std::string state;

public:
    StateFilterIterator(Component *root, const std::string &state);
    virtual Component *first();
    virtual Component *next();
};
