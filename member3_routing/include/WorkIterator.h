
class WorkComponent;

class WorkIterator
{
public:
    virtual ~WorkIterator();
    virtual bool hasNext() const = 0;
    virtual WorkComponent *first() = 0;
    virtual WorkComponent *next() = 0;
    virtual WorkComponent *isDone() = 0;
    virtual WorkComponent *Current() = 0;
};