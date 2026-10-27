
class Component;

class WorkIterator
{
public:
    virtual ~WorkIterator();
    virtual bool hasNext() const = 0;
    virtual Component *first() = 0;
    virtual Component *next() = 0;
    virtual Component *isDone() = 0;
    virtual Component *current() = 0;
};