#ifndef REGISTRY_H
#define REGISTRY_H

/**
 * @brief Placeholder base type for workflow registries.
 *
 * This currently provides no registry operations; the default constructor and
 * virtual destructor exist to allow polymorphic ownership.
 */
class Registry
{
    public:
        /// @brief Constructs an empty placeholder registry.
        Registry() = default;

        /// @brief Destroys the registry polymorphically.
        virtual ~Registry() = default;
};

#endif