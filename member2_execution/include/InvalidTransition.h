#ifndef INVALIDTRANSITION_H
#define INVALIDTRANSITION_H

#include <stdexcept>
#include <string>

class InvalidTransition : public std::logic_error
{
public:
    explicit InvalidTransition(const std::string& message)
        : std::logic_error(message) {}
};

#endif
